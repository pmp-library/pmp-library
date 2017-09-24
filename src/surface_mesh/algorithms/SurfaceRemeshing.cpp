//=============================================================================
// Copyright (C) 2011-2016 by Graphics & Geometry Group, Bielefeld University
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

#include <surface_mesh/algorithms/SurfaceRemeshing.h>
#include <surface_mesh/algorithms/distancePointTriangle.h>
#include <surface_mesh/algorithms/SurfaceCurvature.h>
#include <surface_mesh/algorithms/barycentricCoordinates.h>
#include <surface_mesh/Timer.h>

#include <cfloat>
#include <cmath>

//=============================================================================

namespace surface_mesh {

//=============================================================================

SurfaceRemeshing::SurfaceRemeshing(SurfaceMesh& mesh)
    : m_mesh(mesh), m_refmesh(NULL), m_kDTree(NULL)
{
    m_points  = m_mesh.vertexProperty<Point>("v:point");
    m_vnormal = m_mesh.vertexProperty<Point>("v:normal");
}

//-----------------------------------------------------------------------------

SurfaceRemeshing::~SurfaceRemeshing()
{
}

//-----------------------------------------------------------------------------

void SurfaceRemeshing::uniformRemeshing(Scalar       edgeLength,
                                        unsigned int iterations,
                                        bool         useProjection)
{
    if (!m_mesh.isTriangleMesh())
    {
        std::cerr << "Not a triangle mesh!" << std::endl;
        return;
    }

    m_uniform          = true;
    m_useProjection    = useProjection;
    m_targetEdgeLength = edgeLength;

    double tPre(0);
    double tSplit(0);
    double tCollapse(0);
    double tFlip(0);
    double tSmooth(0);
    double tCaps(0);
    Timer  timer;

    timer.start();
    preprocessing();
    timer.stop();
    tPre = timer.elapsed();

    for (unsigned int i = 0; i < iterations; ++i)
    {
        timer.start();
        splitLongEdges();
        timer.stop();
        tSplit += timer.elapsed();

        m_mesh.updateVertexNormals();

        timer.start();
        collapseShortEdges();
        timer.stop();
        tCollapse += timer.elapsed();

        timer.start();
        flipEdges();
        timer.stop();
        tFlip += timer.elapsed();

        timer.start();
        tangentialSmoothing(5);
        timer.stop();
        tSmooth += timer.elapsed();
    }

    timer.start();
    removeCaps();
    timer.stop();
    tCaps += timer.elapsed();

    postprocessing();

    // std::cout << "Remeshing times:\n";
    // std::cout << "preproc:  " << tPre << "ms\n";
    // std::cout << "split:    " << tSplit << "ms\n";
    // std::cout << "collapse: " << tCollapse << "ms\n";
    // std::cout << "flip:     " << tFlip << "ms\n";
    // std::cout << "smooth:   " << tSmooth << "ms\n";
    // std::cout << "caps:     " << tCaps << "ms\n";
    // std::cout << std::endl;
}

//-----------------------------------------------------------------------------

void SurfaceRemeshing::adaptiveRemeshing(Scalar       minEdgeLength,
                                         Scalar       maxEdgeLength,
                                         Scalar       approxError,
                                         unsigned int iterations,
                                         bool         useProjection)
{
    if (!m_mesh.isTriangleMesh())
    {
        std::cerr << "Not a triangle mesh!" << std::endl;
        return;
    }

    m_uniform       = false;
    m_minEdgeLength = minEdgeLength;
    m_maxEdgeLength = maxEdgeLength;
    m_approxError   = approxError;
    m_useProjection = useProjection;

    double tPre(0);
    double tSplit(0);
    double tCollapse(0);
    double tFlip(0);
    double tSmooth(0);
    double tCaps(0);
    Timer  timer;

    timer.start();
    preprocessing();
    timer.stop();
    tPre = timer.elapsed();

    for (unsigned int i = 0; i < iterations; ++i)
    {
        timer.start();
        splitLongEdges();
        timer.stop();
        tSplit += timer.elapsed();

        m_mesh.updateVertexNormals();

        timer.start();
        collapseShortEdges();
        timer.stop();
        tCollapse += timer.elapsed();

        timer.start();
        flipEdges();
        timer.stop();
        tFlip += timer.elapsed();

        timer.start();
        tangentialSmoothing(5);
        timer.stop();
        tSmooth += timer.elapsed();
    }

    timer.start();
    removeCaps();
    timer.stop();
    tCaps += timer.elapsed();

    postprocessing();

    // std::cout << "Remeshing times:\n";
    // std::cout << "preproc:  " << tPre << "ms\n";
    // std::cout << "split:    " << tSplit << "ms\n";
    // std::cout << "collapse: " << tCollapse << "ms\n";
    // std::cout << "flip:     " << tFlip << "ms\n";
    // std::cout << "smooth:   " << tSmooth << "ms\n";
    // std::cout << "caps:     " << tCaps << "ms\n";
    // std::cout << std::endl;
}

//-----------------------------------------------------------------------------

void SurfaceRemeshing::preprocessing()
{
    // properties
    m_vfeature = m_mesh.vertexProperty<bool>("v:feature", false);
    m_efeature = m_mesh.edgeProperty<bool>("e:feature", false);
    m_vlocked  = m_mesh.addVertexProperty<bool>("v:locked", false);
    m_elocked  = m_mesh.addEdgeProperty<bool>("e:locked", false);
    m_vsizing  = m_mesh.getVertexProperty<Scalar>("v:sizing");

    // re-use an existing sizing field. used for remeshing a cage in the
    // adaptive refinement benchmark.
    bool useSizingField(false);

    if (m_vsizing)
        useSizingField = true;
    else
        m_vsizing = m_mesh.addVertexProperty<Scalar>("v:sizing");

    // lock unselected vertices if some vertices are selected
    SurfaceMesh::VertexProperty<bool> vselected =
        m_mesh.getVertexProperty<bool>("v:selected");
    if (vselected)
    {
        bool hasSelection = false;
        for (SurfaceMesh::VertexIterator vIt = m_mesh.verticesBegin();
             vIt != m_mesh.verticesEnd(); ++vIt)
        {
            if (vselected[*vIt])
            {
                hasSelection = true;
                break;
            }
        }

        if (hasSelection)
        {
            for (SurfaceMesh::VertexIterator vIt = m_mesh.verticesBegin();
                 vIt != m_mesh.verticesEnd(); ++vIt)
            {
                m_vlocked[*vIt] = !vselected[*vIt];
            }

            // lock an edge if one of its vertices is locked
            for (SurfaceMesh::EdgeIterator eIt = m_mesh.edgesBegin();
                 eIt != m_mesh.edgesEnd(); ++eIt)
            {
                m_elocked[*eIt] = (m_vlocked[m_mesh.vertex(*eIt, 0)] ||
                                   m_vlocked[m_mesh.vertex(*eIt, 1)]);
            }
        }
    }

    // lock feature corners
    SurfaceMesh::HalfedgeAroundVertexCirculator vhIt, vhEnd;
    int                                         c;
    for (SurfaceMesh::VertexIterator vIt = m_mesh.verticesBegin();
         vIt != m_mesh.verticesEnd(); ++vIt)
    {
        if (m_vfeature[*vIt])
        {
            c    = 0;
            vhIt = vhEnd = m_mesh.halfedges(*vIt);
            do
            {
                if (m_efeature[m_mesh.edge(*vhIt)])
                    ++c;
            } while (++vhIt != vhEnd);

            if (c != 2)
                m_vlocked[*vIt] = true;
        }
    }

    // compute sizing field
    if (m_uniform)
    {
        for (SurfaceMesh::VertexIterator vit = m_mesh.verticesBegin();
             vit != m_mesh.verticesEnd(); ++vit)
        {
            m_vsizing[*vit] = m_targetEdgeLength;
        }
    }
    else if (!useSizingField)
    {
        // compute curvature for all mesh vertices, using cotan or Cohen-Steiner
        // do 2 post-smoothing steps to get a smoother sizing field
        SurfaceCurvature curv(m_mesh);
        //curv.analyze(1);
        curv.analyzeTensor(1, true);

        for (SurfaceMesh::VertexIterator vit = m_mesh.verticesBegin();
             vit != m_mesh.verticesEnd(); ++vit)
        {
            // maximum absolute curvature
            Scalar c = curv.maxAbsCurvature(*vit);

            // curvature of feature vertices: average of non-feature neighbors
            if (m_vfeature[*vit])
            {
                SurfaceMesh::HalfedgeAroundVertexCirculator vhit, vhend;
                SurfaceMesh::Vertex                         v;
                Scalar                                      w, ww = 0.0;
                c = 0.0;

                vhit = vhend = m_mesh.halfedges(*vit);
                if (vhit)
                    do
                    {
                        v = m_mesh.toVertex(*vhit);
                        if (!m_vfeature[v])
                        {
                            w = std::max(
                                0.0, cotanWeight(m_mesh, m_mesh.edge(*vhit)));
                            ww += w;
                            c += w * curv.maxAbsCurvature(v);
                        }
                    } while (++vhit != vhend);

                c /= ww;
            }

            // get edge length from curvature
            const Scalar r = 1.0 / c;
            const Scalar e = m_approxError;
            Scalar       h;
            if (e < r)
            {
                // see mathworld: "circle segment" and "equilateral triangle"
                //h = sqrt(2.0*r*e-e*e) * 3.0 / sqrt(3.0);
                h = sqrt(6.0 * e * r - 3.0 * e * e); // simplified...
            }
            else
            {
                // this does not really make sense
                h = e * 3.0 / sqrt(3.0);
            }

            // clamp to min. and max. edge length
            if (h < m_minEdgeLength)
                h = m_minEdgeLength;
            else if (h > m_maxEdgeLength)
                h = m_maxEdgeLength;

            // store target edge length
            m_vsizing[*vit] = h;
        }
    }

    if (m_useProjection)
    {
        // build reference mesh
        m_refmesh = new SurfaceMesh();
        m_refmesh->assign(m_mesh);
        m_refmesh->updateVertexNormals();
        m_refpoints  = m_refmesh->vertexProperty<Point>("v:point");
        m_refnormals = m_refmesh->vertexProperty<Point>("v:normal");

        // copy sizing field from m_mesh
        m_refsizing = m_refmesh->addVertexProperty<Scalar>("v:sizing");
        for (SurfaceMesh::VertexIterator vit = m_refmesh->verticesBegin();
             vit != m_refmesh->verticesEnd(); ++vit)
        {
            m_refsizing[*vit] = m_vsizing[*vit];
        }

        // build BSP
        m_kDTree = new TriangleKdTree(*m_refmesh, 0);
    }
}

//-----------------------------------------------------------------------------

void SurfaceRemeshing::postprocessing()
{
    // delete BSP and reference mesh
    if (m_useProjection)
    {
        delete m_kDTree;
        delete m_refmesh;
    }

    // remove properties
    m_mesh.removeVertexProperty(m_vlocked);
    m_mesh.removeEdgeProperty(m_elocked);
    m_mesh.removeVertexProperty(m_vsizing);
}

//-----------------------------------------------------------------------------

void SurfaceRemeshing::projectToReference(SurfaceMesh::Vertex v)
{
    if (!m_useProjection)
    {
        return;
    }

    // find closest triangle of reference mesh
    TriangleKdTree::NearestNeighbor nn = m_kDTree->nearest(m_points[v]);
    const Point                     p  = nn.nearest;
    const SurfaceMesh::Face         f  = nn.face;

    // get face data
    SurfaceMesh::VertexAroundFaceCirculator fvIt = m_refmesh->vertices(f);
    const Point                             p0   = m_refpoints[*fvIt];
    const Point                             n0   = m_refnormals[*fvIt];
    const Scalar                            s0   = m_refsizing[*fvIt];
    ++fvIt;
    const Point  p1 = m_refpoints[*fvIt];
    const Point  n1 = m_refnormals[*fvIt];
    const Scalar s1 = m_refsizing[*fvIt];
    ++fvIt;
    const Point  p2 = m_refpoints[*fvIt];
    const Point  n2 = m_refnormals[*fvIt];
    const Scalar s2 = m_refsizing[*fvIt];

    // get barycentric coordinates
    Point b = barycentricCoordinates(p, p0, p1, p2);

    // interpolate normal
    Point n;
    n = (n0 * b[0]);
    n += (n1 * b[1]);
    n += (n2 * b[2]);
    n.normalize();
#ifdef _WIN32
    assert(Finite(n[0]));
#else
    assert(!std::isnan(n[0]));
#endif

    // interpolate sizing field
    Scalar s;
    s = (s0 * b[0]);
    s += (s1 * b[1]);
    s += (s2 * b[2]);

    // set result
    m_points[v]  = p;
    m_vnormal[v] = n;
    m_vsizing[v] = s;
}

//-----------------------------------------------------------------------------

void SurfaceRemeshing::splitLongEdges()
{
    SurfaceMesh::VertexIterator vIt, vEnd;
    SurfaceMesh::EdgeIterator   eIt, eEnd;
    SurfaceMesh::Vertex         v, v0, v1;
    SurfaceMesh::Edge           e, e0, e1;
    SurfaceMesh::Face           f0, f1, f2, f3;
    bool                        ok, isFeature, isBoundary;
    int                         i;

    for (ok = false, i = 0; !ok && i < 10; ++i)
    {
        ok = true;

        for (eIt = m_mesh.edgesBegin(), eEnd = m_mesh.edgesEnd(); eIt != eEnd;
             ++eIt)
        {
            v0 = m_mesh.vertex(*eIt, 0);
            v1 = m_mesh.vertex(*eIt, 1);

            if (!m_elocked[*eIt] && isTooLong(v0, v1))
            {
                const Point& p0 = m_points[v0];
                const Point& p1 = m_points[v1];

                isFeature  = m_efeature[*eIt];
                isBoundary = m_mesh.isBoundary(*eIt);

                v = m_mesh.addVertex((p0 + p1) * 0.5);
                m_mesh.split(*eIt, v);

                // need normal or sizing for adaptive refinement
                //m_vnormal[v] = normalize(m_vnormal[v0] + m_vnormal[v1]);
                m_vnormal[v] = m_mesh.computeVertexNormal(v);
                m_vsizing[v] = 0.5f * (m_vsizing[v0] + m_vsizing[v1]);

                if (isFeature)
                {
                    e = isBoundary ? SurfaceMesh::Edge(m_mesh.nEdges() - 2)
                                   : SurfaceMesh::Edge(m_mesh.nEdges() - 3);
                    m_efeature[e] = true;
                    m_vfeature[v] = true;
                }
                else
                {
                    projectToReference(v);
                }

                ok = false;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SurfaceRemeshing::collapseShortEdges()
{
    SurfaceMesh::EdgeIterator                 eIt, eEnd;
    SurfaceMesh::VertexAroundVertexCirculator vvIt, vvEnd;
    SurfaceMesh::Vertex                       v0, v1;
    SurfaceMesh::Halfedge                     h0, h1, h01, h10;
    bool                                      ok, b0, b1, l0, l1, f0, f1;
    int                                       i;
    bool                                      hcol01, hcol10;

    for (ok = false, i = 0; !ok && i < 10; ++i)
    {
        ok = true;

        for (eIt = m_mesh.edgesBegin(), eEnd = m_mesh.edgesEnd(); eIt != eEnd;
             ++eIt)
        {
            if (!m_mesh.isDeleted(*eIt) && !m_elocked[*eIt])
            {
                h10 = m_mesh.halfedge(*eIt, 0);
                h01 = m_mesh.halfedge(*eIt, 1);
                v0  = m_mesh.toVertex(h10);
                v1  = m_mesh.toVertex(h01);

                if (isTooShort(v0, v1))
                {
                    // get status
                    b0     = m_mesh.isBoundary(v0);
                    b1     = m_mesh.isBoundary(v1);
                    l0     = m_vlocked[v0];
                    l1     = m_vlocked[v1];
                    f0     = m_vfeature[v0];
                    f1     = m_vfeature[v1];
                    hcol01 = hcol10 = true;

                    // boundary rules
                    if (b0 && b1)
                    {
                        if (!m_mesh.isBoundary(*eIt))
                            continue;
                    }
                    else if (b0)
                        hcol01 = false;
                    else if (b1)
                        hcol10 = false;

                    // locked rules
                    if (l0 && l1)
                        continue;
                    else if (l0)
                        hcol01 = false;
                    else if (l1)
                        hcol10 = false;

                    // feature rules
                    if (f0 && f1)
                    {
                        // edge must be feature
                        if (!m_efeature[*eIt])
                            continue;

                        // the other two edges removed by collapse must not be features
                        h0 = m_mesh.prevHalfedge(h01);
                        h1 = m_mesh.nextHalfedge(h10);
                        if (m_efeature[m_mesh.edge(h0)] ||
                            m_efeature[m_mesh.edge(h1)])
                            hcol01 = false;
                        h0         = m_mesh.prevHalfedge(h10);
                        h1         = m_mesh.nextHalfedge(h01);
                        if (m_efeature[m_mesh.edge(h0)] ||
                            m_efeature[m_mesh.edge(h1)])
                            hcol10 = false;
                    }
                    else if (f0)
                        hcol01 = false;
                    else if (f1)
                        hcol10 = false;

                    // topological rules
                    bool collapseOk = m_mesh.isCollapseOk(h01);

                    if (hcol01)
                        hcol01 = collapseOk;
                    if (hcol10)
                        hcol10 = collapseOk;

                    // both collapses possible: collapse into vertex w/ higher valence
                    if (hcol01 && hcol10)
                    {
                        if (m_mesh.valence(v0) < m_mesh.valence(v1))
                            hcol10 = false;
                        else
                            hcol01 = false;
                    }

                    // try v1 -> v0
                    if (hcol10)
                    {
                        // don't create too long edges
                        vvIt = vvEnd = m_mesh.vertices(v1);
                        do
                        {
                            if (isTooLong(v0, *vvIt))
                            {
                                hcol10 = false;
                                break;
                            }
                        } while (++vvIt != vvEnd);

                        if (hcol10)
                        {
                            m_mesh.collapse(h10);
                            ok = false;
                        }
                    }

                    // try v0 -> v1
                    else if (hcol01)
                    {
                        // don't create too long edges
                        vvIt = vvEnd = m_mesh.vertices(v0);
                        do
                        {
                            if (isTooLong(v1, *vvIt))
                            {
                                hcol01 = false;
                                break;
                            }
                        } while (++vvIt != vvEnd);

                        if (hcol01)
                        {
                            m_mesh.collapse(h01);
                            ok = false;
                        }
                    }
                }
            }
        }
    }

    m_mesh.garbageCollection();
}

//-----------------------------------------------------------------------------

void SurfaceRemeshing::flipEdges()
{
    SurfaceMesh::EdgeIterator   eIt, eEnd;
    SurfaceMesh::VertexIterator vIt, vEnd;
    SurfaceMesh::Vertex         v0, v1, v2, v3;
    SurfaceMesh::Halfedge       h;
    int                         val0, val1, val2, val3;
    int                         valOpt0, valOpt1, valOpt2, valOpt3;
    int                         ve0, ve1, ve2, ve3, veBefore, veAfter;
    bool                        ok;
    int                         i;

    // precompute valences
    SurfaceMesh::VertexProperty<int> valence =
        m_mesh.addVertexProperty<int>("valence");
    SurfaceMesh::VertexIterator vit, vend = m_mesh.verticesEnd();
    for (vit = m_mesh.verticesBegin(); vit != vend; ++vit)
    {
        valence[*vit] = m_mesh.valence(*vit);
    }

    for (ok = false, i = 0; !ok && i < 10; ++i)
    {
        ok = true;

        for (eIt = m_mesh.edgesBegin(), eEnd = m_mesh.edgesEnd(); eIt != eEnd;
             ++eIt)
        {
            if (!m_elocked[*eIt] && !m_efeature[*eIt])
            {
                h  = m_mesh.halfedge(*eIt, 0);
                v0 = m_mesh.toVertex(h);
                v2 = m_mesh.toVertex(m_mesh.nextHalfedge(h));
                h  = m_mesh.halfedge(*eIt, 1);
                v1 = m_mesh.toVertex(h);
                v3 = m_mesh.toVertex(m_mesh.nextHalfedge(h));

                if (!m_vlocked[v0] && !m_vlocked[v1] && !m_vlocked[v2] &&
                    !m_vlocked[v3])
                {
                    val0 = valence[v0];
                    val1 = valence[v1];
                    val2 = valence[v2];
                    val3 = valence[v3];

                    valOpt0 = (m_mesh.isBoundary(v0) ? 4 : 6);
                    valOpt1 = (m_mesh.isBoundary(v1) ? 4 : 6);
                    valOpt2 = (m_mesh.isBoundary(v2) ? 4 : 6);
                    valOpt3 = (m_mesh.isBoundary(v3) ? 4 : 6);

                    ve0 = (val0 - valOpt0);
                    ve1 = (val1 - valOpt1);
                    ve2 = (val2 - valOpt2);
                    ve3 = (val3 - valOpt3);

                    ve0 *= ve0;
                    ve1 *= ve1;
                    ve2 *= ve2;
                    ve3 *= ve3;

                    veBefore = ve0 + ve1 + ve2 + ve3;

                    --val0;
                    --val1;
                    ++val2;
                    ++val3;

                    ve0 = (val0 - valOpt0);
                    ve1 = (val1 - valOpt1);
                    ve2 = (val2 - valOpt2);
                    ve3 = (val3 - valOpt3);

                    ve0 *= ve0;
                    ve1 *= ve1;
                    ve2 *= ve2;
                    ve3 *= ve3;

                    veAfter = ve0 + ve1 + ve2 + ve3;

                    if (veBefore > veAfter && m_mesh.isFlipOk(*eIt))
                    {
                        m_mesh.flip(*eIt);
                        --valence[v0];
                        --valence[v1];
                        ++valence[v2];
                        ++valence[v3];
                        ok = false;
                    }
                }
            }
        }
    }

    m_mesh.removeVertexProperty(valence);
}

//-----------------------------------------------------------------------------

void SurfaceRemeshing::tangentialSmoothing(unsigned int iterations)
{
    SurfaceMesh::Vertex v1, v2, v3, vv;
    SurfaceMesh::Edge   e;
    Scalar              w, ww, area;
    Point               u, n, t, b;

    SurfaceMesh::VertexIterator                 vit, vend(m_mesh.verticesEnd());
    SurfaceMesh::HalfedgeAroundVertexCirculator hit, hend;

    // add property
    SurfaceMesh::VertexProperty<Point> update =
        m_mesh.addVertexProperty<Point>("v:update");

    // project at the beginning to get valid sizing values and normal vectors
    // for vertices introduced by splitting
    if (m_useProjection)
    {
        for (vit = m_mesh.verticesBegin(); vit != vend; ++vit)
        {
            SurfaceMesh::Vertex v(*vit);

            if (!m_mesh.isBoundary(v) && !m_vlocked[v])
            {
                projectToReference(v);
            }
        }
    }

    for (unsigned int iters = 0; iters < iterations; ++iters)
    {
        for (vit = m_mesh.verticesBegin(); vit != vend; ++vit)
        {
            SurfaceMesh::Vertex v(*vit);

            if (!m_mesh.isBoundary(v) && !m_vlocked[v])
            {
                if (m_vfeature[v])
                {
                    u     = 0.0;
                    t     = 0.0;
                    ww    = 0;
                    int c = 0;

                    hit = hend = m_mesh.halfedges(v);
                    do
                    {
                        SurfaceMesh::Halfedge h = *hit;

                        if (m_efeature[m_mesh.edge(h)])
                        {
                            vv = m_mesh.toVertex(h);

                            b = m_points[v];
                            b += m_points[vv];
                            b *= 0.5;

                            w = distance(m_points[v], m_points[vv]) /
                                (0.5 * (m_vsizing[v] + m_vsizing[vv]));
                            ww += w;
                            u += w * b;

                            if (c == 0)
                            {
                                t += normalize(m_points[vv] - m_points[v]);
                                ++c;
                            }
                            else
                            {
                                ++c;
                                t -= normalize(m_points[vv] - m_points[v]);
                            }
                        }
                    } while (++hit != hend);

                    assert(c == 2);

                    u *= (1.0 / ww);
                    u -= m_points[v];
                    t = normalize(t);
                    u = t * dot(u, t);
                }
                else
                {
                    u  = 0.0;
                    t  = 0.0;
                    ww = 0;

                    hit = hend = m_mesh.halfedges(v);
                    do
                    {
                        SurfaceMesh::Halfedge h = *hit;

                        v1 = v;
                        v2 = m_mesh.toVertex(h);
                        v3 = m_mesh.toVertex(m_mesh.nextHalfedge(h));

                        b = m_points[v1];
                        b += m_points[v2];
                        b += m_points[v3];
                        b *= (1.0 / 3.0);

                        area = norm(cross(m_points[v2] - m_points[v1],
                                          m_points[v3] - m_points[v1]));
                        w = area / pow((m_vsizing[v1] + m_vsizing[v2] +
                                        m_vsizing[v3]) /
                                           3.0,
                                       2.0);

                        u += w * b;
                        ww += w;
                    } while (++hit != hend);

                    u /= ww;
                    u -= m_points[v];
                    n = m_vnormal[v];
                    u -= n * dot(u, n);

                    update[v] = u;
                }
            }
        }

        // update vertex positions
        for (vit = m_mesh.verticesBegin(); vit != vend; ++vit)
        {
            SurfaceMesh::Vertex v(*vit);
            if (!m_mesh.isBoundary(v) && !m_vlocked[v])
            {
                m_points[v] += update[v];
            }
        }

        // update normal vectors (if not done so through projection
        m_mesh.updateVertexNormals();
    }

    // project at the end
    if (m_useProjection)
    {
        for (vit = m_mesh.verticesBegin(); vit != vend; ++vit)
        {
            SurfaceMesh::Vertex v(*vit);
            if (!m_mesh.isBoundary(v) && !m_vlocked[v])
            {
                projectToReference(v);
            }
        }
    }

    // remove property
    m_mesh.removeVertexProperty(update);
}

//-----------------------------------------------------------------------------

void SurfaceRemeshing::removeCaps()
{
    SurfaceMesh::EdgeIterator eIt, eEnd(m_mesh.edgesEnd());
    SurfaceMesh::Halfedge     h;
    SurfaceMesh::Vertex       v, vb, vd;
    SurfaceMesh::Face         fb, fd;
    Scalar                    a0, a1, amin, aa(::cos(170.0 * M_PI / 180.0));
    Point                     a, b, c, d;

    for (eIt = m_mesh.edgesBegin(); eIt != eEnd; ++eIt)
    {
        if (!m_elocked[*eIt] && m_mesh.isFlipOk(*eIt))
        {
            h = m_mesh.halfedge(*eIt, 0);
            a = m_points[m_mesh.toVertex(h)];

            h = m_mesh.nextHalfedge(h);
            b = m_points[vb = m_mesh.toVertex(h)];

            h = m_mesh.halfedge(*eIt, 1);
            c = m_points[m_mesh.toVertex(h)];

            h = m_mesh.nextHalfedge(h);
            d = m_points[vd = m_mesh.toVertex(h)];

            a0 = dot((a - b).normalize(), (c - b).normalize());
            a1 = dot((a - d).normalize(), (c - d).normalize());

            if (a0 < a1)
            {
                amin = a0;
                v    = vb;
            }
            else
            {
                amin = a1;
                v    = vd;
            }

            // is it a cap?
            if (amin < aa)
            {
                // feature edge and feature vertex -> seems to be intended
                if (m_efeature[*eIt] && m_vfeature[v])
                    continue;

                // project v onto feature edge
                if (m_efeature[*eIt])
                    m_points[v] = (a + c) * 0.5;

                // flip
                m_mesh.flip(*eIt);
            }
        }
    }
}

//=============================================================================
} // namespace surface_mesh
//=============================================================================
