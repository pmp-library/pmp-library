//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================

#include <pmp/algorithms/SurfaceSimplification.h>
#include <pmp/algorithms/distancePointTriangle.h>
#include <pmp/algorithms/SurfaceNormals.h>

#include <cfloat>
#include <iterator> // for back_inserter on Windows

//=============================================================================

namespace pmp {

//=============================================================================

SurfaceSimplification::SurfaceSimplification(SurfaceMesh& mesh)
    : m_mesh(mesh), m_initialized(false), m_queue(NULL)

{
    m_aspectRatio     = 0;
    m_edgeLength      = 0;
    m_maxValence      = 0;
    m_normalDeviation = 0;
    m_hausdorffError  = 0;

    // add properties
    m_vquadric = m_mesh.addVertexProperty<Quadric>("v:quadric");

    // get properties
    m_vpoint = m_mesh.vertexProperty<Point>("v:point");

    // compute face normals
    SurfaceNormals::computeFaceNormals(m_mesh);
    m_fnormal = m_mesh.faceProperty<Normal>("f:normal");
}

//-----------------------------------------------------------------------------

SurfaceSimplification::~SurfaceSimplification()
{
    // remove added properties
    m_mesh.removeVertexProperty(m_vquadric);
    m_mesh.removeFaceProperty(m_normalCone);
    m_mesh.removeFaceProperty(m_facePoints);
}

//-----------------------------------------------------------------------------

void SurfaceSimplification::initialize(Scalar aspectRatio, Scalar edgeLength,
                                       unsigned int maxValence,
                                       Scalar       normalDeviation,
                                       Scalar       hausdorffError)
{
    if (!m_mesh.isTriangleMesh())
        return;

    // store parameters
    m_aspectRatio     = aspectRatio;
    m_maxValence      = maxValence;
    m_edgeLength      = edgeLength;
    m_normalDeviation = normalDeviation / 180.0 * M_PI;
    m_hausdorffError  = hausdorffError;

    // properties
    if (m_normalDeviation > 0.0)
        m_normalCone = m_mesh.faceProperty<NormalCone>("f:normalCone");
    else
        m_mesh.removeFaceProperty(m_normalCone);
    if (hausdorffError > 0.0)
        m_facePoints = m_mesh.faceProperty<Points>("f:points");
    else
        m_mesh.removeFaceProperty(m_facePoints);

    // vertex selection
    m_hasSelection = false;
    m_vselected    = m_mesh.getVertexProperty<bool>("v:selected");
    if (m_vselected)
    {
        for (auto v : m_mesh.vertices())
        {
            if (m_vselected[v])
            {
                m_hasSelection = true;
                break;
            }
        }
    }

    // feature vertices/edges
    m_hasFeatures = false;
    m_vfeature    = m_mesh.getVertexProperty<bool>("v:feature");
    m_efeature    = m_mesh.getEdgeProperty<bool>("e:feature");
    if (m_vfeature && m_efeature)
    {
        for (auto v : m_mesh.vertices())
        {
            if (m_vfeature[v])
            {
                m_hasFeatures = true;
                break;
            }
        }
    }

    // initialize quadrics
    for (auto v : m_mesh.vertices())
    {
        m_vquadric[v].clear();

        if (!m_mesh.isIsolated(v))
        {
            for (auto f : m_mesh.faces(v))
            {
                m_vquadric[v] += Quadric(m_fnormal[f], m_vpoint[v]);
            }
        }
    }

    // initialize normal cones
    if (m_normalDeviation)
    {
        for (auto f : m_mesh.faces())
        {
            m_normalCone[f] = NormalCone(m_fnormal[f]);
        }
    }

    // initialize faces' point list
    if (m_hausdorffError)
    {
        for (auto f : m_mesh.faces())
        {
            Points().swap(m_facePoints[f]); // free mem
        }
    }

    m_initialized = true;
}

//-----------------------------------------------------------------------------

void SurfaceSimplification::simplify(unsigned int NVertices)
{
    if (!m_mesh.isTriangleMesh())
    {
        std::cerr << "Not a triangle mesh!" << std::endl;
        return;
    }

    // make sure the decimater is initialized
    if (!m_initialized)
        initialize();

    unsigned int nv(m_mesh.nVertices());

    std::vector<SurfaceMesh::Vertex>           oneRing;
    std::vector<SurfaceMesh::Vertex>::iterator orIt, orEnd;
    SurfaceMesh::Halfedge                      h;
    SurfaceMesh::Vertex                        v;

    // add properties for priority queue
    m_vpriority = m_mesh.addVertexProperty<float>("v:prio");
    m_heapPos   = m_mesh.addVertexProperty<int>("v:heap");
    m_vtarget   = m_mesh.addVertexProperty<SurfaceMesh::Halfedge>("v:target");

    // build priority queue
    HeapInterface hi(m_vpriority, m_heapPos);
    m_queue = new PriorityQueue(hi);
    m_queue->reserve(m_mesh.nVertices());
    for (auto v : m_mesh.vertices())
    {
        m_queue->resetHeapPosition(v);
        enqueueVertex(v);
    }

    while (nv > NVertices && !m_queue->empty())
    {
        // get 1st element
        v = m_queue->front();
        m_queue->popFront();
        h = m_vtarget[v];
        CollapseData cd(m_mesh, h);

        // check this (again)
        if (!m_mesh.isCollapseOk(h))
            continue;

        // store one-ring
        oneRing.clear();
        for (auto vv : m_mesh.vertices(cd.v0))
        {
            oneRing.push_back(vv);
        }

        // perform collapse
        m_mesh.collapse(h);
        --nv;
        //if (nv % 1000 == 0) std::cerr << nv << "\r";

        // postprocessing, e.g., update quadrics
        postprocessCollapse(cd);

        // update queue
        for (orIt = oneRing.begin(), orEnd = oneRing.end(); orIt != orEnd;
             ++orIt)
            enqueueVertex(*orIt);
    }

    // clean up
    delete m_queue;
    m_mesh.garbageCollection();
    m_mesh.removeVertexProperty(m_vpriority);
    m_mesh.removeVertexProperty(m_heapPos);
    m_mesh.removeVertexProperty(m_vtarget);
}

//-----------------------------------------------------------------------------

void SurfaceSimplification::enqueueVertex(SurfaceMesh::Vertex v)
{
    float                 prio, minPrio(FLT_MAX);
    SurfaceMesh::Halfedge minH;

    // find best out-going halfedge
    for (auto h : m_mesh.halfedges(v))
    {
        CollapseData cd(m_mesh, h);
        if (isCollapseLegal(cd))
        {
            prio = priority(cd);
            if (prio != -1.0 && prio < minPrio)
            {
                minPrio = prio;
                minH    = h;
            }
        }
    }

    // target found -> put vertex on heap
    if (minH.isValid())
    {
        m_vpriority[v] = minPrio;
        m_vtarget[v]   = minH;

        if (m_queue->isStored(v))
            m_queue->update(v);
        else
            m_queue->insert(v);
    }

    // not valid -> remove from heap
    else
    {
        if (m_queue->isStored(v))
            m_queue->remove(v);

        m_vpriority[v] = -1;
        m_vtarget[v]   = minH;
    }
}

//-----------------------------------------------------------------------------

bool SurfaceSimplification::isCollapseLegal(const CollapseData& cd)
{
    // test selected vertices
    if (m_hasSelection)
    {
        if (!m_vselected[cd.v0])
            return false;
    }

    // test features
    if (m_hasFeatures)
    {
        if (m_vfeature[cd.v0] && !m_efeature[m_mesh.edge(cd.v0v1)])
            return false;

        if (cd.vl.isValid() && m_efeature[m_mesh.edge(cd.vlv0)])
            return false;

        if (cd.vr.isValid() && m_efeature[m_mesh.edge(cd.v0vr)])
            return false;
    }

    // do not collapse boundary vertices to interior vertices
    if (m_mesh.isSurfaceBoundary(cd.v0) && !m_mesh.isSurfaceBoundary(cd.v1))
        return false;

    // there should be at least 2 incident faces at v0
    if (m_mesh.cwRotatedHalfedge(m_mesh.cwRotatedHalfedge(cd.v0v1)) == cd.v0v1)
        return false;

    // topological check
    if (!m_mesh.isCollapseOk(cd.v0v1))
        return false;

    // check maximal valence
    if (m_maxValence > 0)
    {
        unsigned int val0 = m_mesh.valence(cd.v0);
        unsigned int val1 = m_mesh.valence(cd.v1);
        unsigned int val  = val0 + val1 - 1;
        if (cd.fl.isValid())
            --val;
        if (cd.fr.isValid())
            --val;
        if (val > m_maxValence && !(val < std::max(val0, val1)))
            return false;
    }

    // remember the positions of the endpoints
    const Point p0 = m_vpoint[cd.v0];
    const Point p1 = m_vpoint[cd.v1];

    // check for maximum edge length
    if (m_edgeLength)
    {
        for (auto v : m_mesh.vertices(cd.v0))
        {
            if (v != cd.v1 && v != cd.vl && v != cd.vr)
            {
                if (norm(m_vpoint[v] - p1) > m_edgeLength)
                    return false;
            }
        }
    }

    // check for flipping normals
    if (m_normalDeviation == 0.0)
    {
        m_vpoint[cd.v0] = p1;
        for (auto f : m_mesh.faces(cd.v0))
        {
            if (f != cd.fl && f != cd.fr)
            {
                Normal n0 = m_fnormal[f];
                Normal n1 = SurfaceNormals::computeFaceNormal(m_mesh, f);
                if (dot(n0, n1) < 0.0)
                {
                    m_vpoint[cd.v0] = p0;
                    return false;
                }
            }
        }
        m_vpoint[cd.v0] = p0;
    }

    // check normal cone
    else
    {
        m_vpoint[cd.v0] = p1;

        SurfaceMesh::Face fll, frr;
        if (cd.vl.isValid())
            fll = m_mesh.face(
                m_mesh.oppositeHalfedge(m_mesh.prevHalfedge(cd.v0v1)));
        if (cd.vr.isValid())
            frr = m_mesh.face(
                m_mesh.oppositeHalfedge(m_mesh.nextHalfedge(cd.v1v0)));

        for (auto f : m_mesh.faces(cd.v0))
        {
            if (f != cd.fl && f != cd.fr)
            {
                NormalCone nc = m_normalCone[f];
                nc.merge(SurfaceNormals::computeFaceNormal(m_mesh, f));

                if (f == fll)
                    nc.merge(m_normalCone[cd.fl]);
                if (f == frr)
                    nc.merge(m_normalCone[cd.fr]);

                if (nc.angle() > 0.5 * m_normalDeviation)
                {
                    m_vpoint[cd.v0] = p0;
                    return false;
                }
            }
        }

        m_vpoint[cd.v0] = p0;
    }

    // check aspect ratio
    if (m_aspectRatio)
    {
        Scalar ar0(0), ar1(0);

        for (auto f : m_mesh.faces(cd.v0))
        {
            if (f != cd.fl && f != cd.fr)
            {
                // worst aspect ratio after collapse
                m_vpoint[cd.v0] = p1;
                ar1             = std::max(ar1, aspectRatio(f));
                // worst aspect ratio before collapse
                m_vpoint[cd.v0] = p0;
                ar0             = std::max(ar0, aspectRatio(f));
            }
        }

        // aspect ratio is too bad, and it does also not improve
        if (ar1 > m_aspectRatio && ar1 > ar0)
            return false;
    }

    // check Hausdorff error
    if (m_hausdorffError)
    {
        Points points;
        bool   ok;

        // collect points to be tested
        for (auto f : m_mesh.faces(cd.v0))
        {
            std::copy(m_facePoints[f].begin(), m_facePoints[f].end(),
                      std::back_inserter(points));
        }
        points.push_back(m_vpoint[cd.v0]);

        // test points against all faces
        m_vpoint[cd.v0] = p1;
        for (unsigned int i = 0; i < points.size(); ++i)
        {
            ok = false;

            for (auto f : m_mesh.faces(cd.v0))
            {
                if (f != cd.fl && f != cd.fr)
                {
                    if (distance(f, points[i]) < m_hausdorffError)
                    {
                        ok = true;
                        break;
                    }
                }
            }

            if (!ok)
            {
                m_vpoint[cd.v0] = p0;
                return false;
            }
        }
        m_vpoint[cd.v0] = p0;
    }

    // collapse passed all tests -> ok
    return true;
}

//-----------------------------------------------------------------------------

float SurfaceSimplification::priority(const CollapseData& cd)
{
    // computer quadric error metric
    Quadric Q = m_vquadric[cd.v0];
    Q += m_vquadric[cd.v1];
    return Q(m_vpoint[cd.v1]);
}

//-----------------------------------------------------------------------------

void SurfaceSimplification::postprocessCollapse(const CollapseData& cd)
{
    // update error quadrics
    m_vquadric[cd.v1] += m_vquadric[cd.v0];

    // update normal cones
    if (m_normalDeviation)
    {
        for (auto f : m_mesh.faces(cd.v1))
        {
            m_normalCone[f].merge(SurfaceNormals::computeFaceNormal(m_mesh, f));
        }

        if (cd.vl.isValid())
        {
            SurfaceMesh::Face f = m_mesh.face(cd.v1vl);
            if (f.isValid())
                m_normalCone[f].merge(m_normalCone[cd.fl]);
        }

        if (cd.vr.isValid())
        {
            SurfaceMesh::Face f = m_mesh.face(cd.vrv1);
            if (f.isValid())
                m_normalCone[f].merge(m_normalCone[cd.fr]);
        }
    }

    // update Hausdorff error
    if (m_hausdorffError)
    {
        Points                                  points;
        Scalar                                  d, dd;
        SurfaceMesh::Face                       f, ff;
        SurfaceMesh::FaceAroundVertexCirculator vfIt, vfEnd;

        // collect points to be distributed

        // points of v1's one-ring
        for (auto f : m_mesh.faces(cd.v1))
        {
            std::copy(m_facePoints[f].begin(), m_facePoints[f].end(),
                      std::back_inserter(points));
            m_facePoints[f].clear();
        }

        // points of the 2 removed triangles
        if (cd.fl.isValid())
        {
            std::copy(m_facePoints[cd.fl].begin(), m_facePoints[cd.fl].end(),
                      std::back_inserter(points));
            Points().swap(m_facePoints[cd.fl]); // free mem
        }
        if (cd.fr.isValid())
        {
            std::copy(m_facePoints[cd.fr].begin(), m_facePoints[cd.fr].end(),
                      std::back_inserter(points));
            Points().swap(m_facePoints[cd.fr]); // free mem
        }

        // the removed vertex
        points.push_back(m_vpoint[cd.v0]);

        // test points against all faces
        for (unsigned int i = 0; i < points.size(); ++i)
        {
            dd = FLT_MAX;

            for (auto f : m_mesh.faces(cd.v1))
            {
                d = distance(f, points[i]);
                if (d < dd)
                {
                    ff = f;
                    dd = d;
                }
            }

            m_facePoints[ff].push_back(points[i]);
        }
    }
}

//-----------------------------------------------------------------------------

Scalar SurfaceSimplification::aspectRatio(SurfaceMesh::Face f) const
{
    // min height is area/maxLength
    // aspect ratio = length / height
    //              = length * length / area

    SurfaceMesh::VertexAroundFaceCirculator fvit = m_mesh.vertices(f);

    const Point p0 = m_vpoint[*fvit];
    const Point p1 = m_vpoint[*(++fvit)];
    const Point p2 = m_vpoint[*(++fvit)];

    const Point d0 = p0 - p1;
    const Point d1 = p1 - p2;
    const Point d2 = p2 - p0;

    const Scalar l0 = sqrnorm(d0);
    const Scalar l1 = sqrnorm(d1);
    const Scalar l2 = sqrnorm(d2);

    // max squared edge length
    const Scalar l = std::max(l0, std::max(l1, l2));

    // triangle area
    Scalar a = norm(cross(d0, d1));

    return l / a;
}

//-----------------------------------------------------------------------------

Scalar SurfaceSimplification::distance(SurfaceMesh::Face f,
                                       const Point&      p) const
{
    SurfaceMesh::VertexAroundFaceCirculator fvit = m_mesh.vertices(f);

    const Point p0 = m_vpoint[*fvit];
    const Point p1 = m_vpoint[*(++fvit)];
    const Point p2 = m_vpoint[*(++fvit)];

    Point n;

    return distPointTriangle(p, p0, p1, p2, n);
}

//-----------------------------------------------------------------------------

SurfaceSimplification::CollapseData::CollapseData(SurfaceMesh&          m,
                                                  SurfaceMesh::Halfedge h)
    : mesh(m)
{
    v0v1 = h;
    v1v0 = mesh.oppositeHalfedge(v0v1);
    v0   = mesh.toVertex(v1v0);
    v1   = mesh.toVertex(v0v1);
    fl   = mesh.face(v0v1);
    fr   = mesh.face(v1v0);

    // get vl
    if (fl.isValid())
    {
        v1vl = mesh.nextHalfedge(v0v1);
        vlv0 = mesh.nextHalfedge(v1vl);
        vl   = mesh.toVertex(v1vl);
    }

    // get vr
    if (fr.isValid())
    {
        v0vr = mesh.nextHalfedge(v1v0);
        vrv1 = mesh.prevHalfedge(v0vr);
        vr   = mesh.fromVertex(vrv1);
    }
}

//=============================================================================
} // namespace pmp
//=============================================================================
