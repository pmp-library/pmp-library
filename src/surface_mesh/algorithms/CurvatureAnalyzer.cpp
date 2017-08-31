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

#include <surface_mesh/algorithms/CurvatureAnalyzer.h>
#include <surface_mesh/Matrix.h>

//=============================================================================

namespace surface_mesh {

//=============================================================================

CurvatureAnalyzer::CurvatureAnalyzer(SurfaceMesh& mesh) : m_mesh(mesh)
{
    m_minCurvature = m_mesh.addVertexProperty<Scalar>("curv:min");
    m_maxCurvature = m_mesh.addVertexProperty<Scalar>("curv:max");
}

//-----------------------------------------------------------------------------

CurvatureAnalyzer::~CurvatureAnalyzer()
{
    m_mesh.removeVertexProperty(m_minCurvature);
    m_mesh.removeVertexProperty(m_maxCurvature);
}

//-----------------------------------------------------------------------------

void CurvatureAnalyzer::analyze(unsigned int smoothingSteps)
{
    Scalar kmin, kmax, mean, gauss;
    Scalar area, sumAngles;
    Scalar weight, sumWeights;
    Point  p0, p1, p2, laplace;

    // cotan weight per edge
    auto cotan = m_mesh.addEdgeProperty<double>("curv:cotan");
    for (auto e : m_mesh.edges())
        cotan[e] = cotanWeight(m_mesh, e);

    // Voronoi area per vertex
    // Laplace per vertex
    // angle sum per vertex
    // -> mean, Gauss -> min, max curvature
    for (auto v : m_mesh.vertices())
    {
        kmin = kmax = 0.0;

        if (!m_mesh.isIsolated(v) && !m_mesh.isBoundary(v))
        {
            laplace    = 0.0;
            sumWeights = 0.0;
            sumAngles  = 0.0;
            p0         = m_mesh.position(v);

            // Voronoi area
            area = voronoiArea(m_mesh, v);

            // Laplace & angle sum
            for (auto vh : m_mesh.halfedges(v))
            {
                p1 = m_mesh.position(m_mesh.toVertex(vh));
                p2 = m_mesh.position(m_mesh.toVertex(m_mesh.ccwRotatedHalfedge(vh)));

                weight = cotan[m_mesh.edge(vh)];
                sumWeights += weight;
                laplace += weight * p1;

                p1 -= p0;
                p1.normalize();
                p2 -= p0;
                p2.normalize();
                sumAngles += acos(clampCos(dot(p1, p2)));
            }
            laplace -= sumWeights * m_mesh.position(v);
            laplace /= Scalar(2.0) * area;

            mean  = Scalar(0.5) * norm(laplace);
            gauss = (2.0 * M_PI - sumAngles) / area;

            const Scalar s = sqrt(std::max(Scalar(0.0), mean * mean - gauss));
            kmin           = mean - s;
            kmax           = mean + s;
        }

        m_minCurvature[v] = kmin;
        m_maxCurvature[v] = kmax;
    }

    // boundary vertices: interpolate from interior neighbors
    for (auto v : m_mesh.vertices())
    {
        if (m_mesh.isBoundary(v))
        {
            kmin = kmax = sumWeights = 0.0;

            for (auto vh : m_mesh.halfedges(v))
            {
                v = m_mesh.toVertex(vh);
                if (!m_mesh.isBoundary(v))
                {
                    weight = cotan[m_mesh.edge(vh)];
                    sumWeights += weight;
                    kmin += weight * m_minCurvature[v];
                    kmax += weight * m_maxCurvature[v];
                }
            }

            if (sumWeights)
            {
                kmin /= sumWeights;
                kmax /= sumWeights;
            }

            m_minCurvature[v] = kmin;
            m_maxCurvature[v] = kmax;
        }
    }

    // clean-up properties
    m_mesh.removeEdgeProperty(cotan);

    // smooth curvature values
    smoothCurvatures(smoothingSteps);
}

//-----------------------------------------------------------------------------

void CurvatureAnalyzer::analyzeTensor(unsigned int smoothingSteps,
                                      bool         twoRingNeighborhood)
{
    SurfaceMesh::VertexProperty<double> area =
        m_mesh.addVertexProperty<double>("curv:area", 0.0);
    SurfaceMesh::FaceProperty<dvec3> normal =
        m_mesh.addFaceProperty<dvec3>("curv:normal");
    SurfaceMesh::EdgeProperty<dvec3> evec =
        m_mesh.addEdgeProperty<dvec3>("curv:evec", dvec3(0, 0, 0));
    SurfaceMesh::EdgeProperty<double> angle =
        m_mesh.addEdgeProperty<double>("curv:angle", 0.0);

    SurfaceMesh::VertexIterator vit, vend = m_mesh.verticesEnd();
    SurfaceMesh::EdgeIterator   eit, eend = m_mesh.edgesEnd();
    SurfaceMesh::FaceIterator   fit, fend = m_mesh.facesEnd();

    SurfaceMesh::VertexAroundVertexCirculator   vvit, vvend;
    SurfaceMesh::VertexAroundFaceCirculator     vfit;
    SurfaceMesh::HalfedgeAroundVertexCirculator hvit, hvend;

    SurfaceMesh::Vertex   v;
    SurfaceMesh::Halfedge h0, h1;
    SurfaceMesh::Edge     ee;
    SurfaceMesh::Face     f0, f1;
    dvec3                 p0, p1, n0, n1, e;
    double                l, A, beta, a1, a2, a3;
    dmat3                 tensor;

    double eval1, eval2, eval3, kmin, kmax;
    dvec3  evec1, evec2, evec3;

    std::vector<SurfaceMesh::Vertex>                 neighborhood;
    std::vector<SurfaceMesh::Vertex>::const_iterator nit, nend;
    neighborhood.reserve(15);

    // precompute Voronoi area per vertex
    for (vit = m_mesh.verticesBegin(); vit != vend; ++vit)
    {
        area[*vit] = voronoiArea(m_mesh, *vit);
    }

    // precompute face normals
    for (fit = m_mesh.facesBegin(); fit != fend; ++fit)
    {
        normal[*fit] = (dvec3)m_mesh.computeFaceNormal(*fit);
    }

    // precompute dihedralAngle*edgeLength*edge per edge
    for (eit = m_mesh.edgesBegin(); eit != eend; ++eit)
    {
        h0 = m_mesh.halfedge(*eit, 0);
        h1 = m_mesh.halfedge(*eit, 1);
        f0 = m_mesh.face(h0);
        f1 = m_mesh.face(h1);
        if (f0.isValid() && f1.isValid())
        {
            n0 = normal[f0];
            n1 = normal[f1];
            e  = m_mesh.position(m_mesh.toVertex(h0));
            e -= m_mesh.position(m_mesh.toVertex(h1));
            l = norm(e);
            e /= l;
            l *= 0.5; // only consider half of the edge (matchig Voronoi area)
            angle[*eit] = atan2(dot(cross(n0, n1), e), dot(n0, n1));
            evec[*eit]  = sqrt(l) * e;
        }
    }

    // compute curvature tensor for each vertex
    for (vit = m_mesh.verticesBegin(); vit != vend; ++vit)
    {
        kmin = 0.0;
        kmax = 0.0;

        if (!m_mesh.isIsolated(*vit))
        {
            // one-ring or two-ring neighborhood?
            neighborhood.clear();
            neighborhood.push_back(*vit);
            if (twoRingNeighborhood)
            {
                vvit = vvend = m_mesh.vertices(*vit);
                do
                {
                    neighborhood.push_back(*vvit);
                } while (++vvit != vvend);
            }

            A      = 0.0;
            tensor = dmat3::zero();

            // compute tensor over vertex neighborhood stored in vertices
            for (nit = neighborhood.begin(), nend = neighborhood.end();
                 nit != nend; ++nit)
            {
                // accumulate tensor from dihedral angles around vertices
                hvit = hvend = m_mesh.halfedges(*nit);
                do
                {
                    ee   = m_mesh.edge(*hvit);
                    e    = evec[ee];
                    beta = angle[ee];
                    for (int i = 0; i < 3; ++i)
                        for (int j = 0; j < 3; ++j)
                            tensor(i, j) += beta * e[i] * e[j];
                } while (++hvit != hvend);

                // accumulate area
                A += area[*nit];
            }

            // normalize tensor by accumulated
            tensor /= A;

            // Eigen-decomposition
            bool ok = symmetricEigendecomposition(tensor, eval1, eval2, eval3,
                                                  evec1, evec2, evec3);
            if (ok)
            {
                // curvature values:
                //   normal vector -> eval with smallest absolute value
                //   evals are sorted in decreasing order
                a1 = fabs(eval1);
                a2 = fabs(eval2);
                a3 = fabs(eval3);
                if (a1 < a2)
                {
                    if (a1 < a3)
                    {
                        // e1 is normal
                        kmax = eval2;
                        kmin = eval3;
                    }
                    else
                    {
                        // e3 is normal
                        kmax = eval1;
                        kmin = eval2;
                    }
                }
                else
                {
                    if (a2 < a3)
                    {
                        // e2 is normal
                        kmax = eval1;
                        kmin = eval3;
                    }
                    else
                    {
                        // e3 is normal
                        kmax = eval1;
                        kmin = eval2;
                    }
                }
            }
        }

        assert(kmin <= kmax);

        m_minCurvature[*vit] = kmin;
        m_maxCurvature[*vit] = kmax;
    }

    // clean-up properties
    m_mesh.removeVertexProperty(area);
    m_mesh.removeEdgeProperty(evec);
    m_mesh.removeEdgeProperty(angle);
    m_mesh.removeFaceProperty(normal);

    // smooth curvature values
    smoothCurvatures(smoothingSteps);
}

//-----------------------------------------------------------------------------

void CurvatureAnalyzer::smoothCurvatures(unsigned int iterations)
{
    SurfaceMesh::VertexIterator                 vit, vend = m_mesh.verticesEnd();
    SurfaceMesh::EdgeIterator                   eit, eend = m_mesh.edgesEnd();
    SurfaceMesh::HalfedgeAroundVertexCirculator vhit, vhend;

    SurfaceMesh::Vertex v;
    Scalar              kmin, kmax;
    Scalar              weight, sumWeights;

    // properties
    auto vfeature = m_mesh.getVertexProperty<bool>("v:feature");
    auto cotan    = m_mesh.addEdgeProperty<double>("curv:cotan");

    // cotan weight per edge
    for (eit = m_mesh.edgesBegin(); eit != eend; ++eit)
    {
        cotan[*eit] = cotanWeight(m_mesh, *eit);
    }

    for (unsigned int i = 0; i < iterations; ++i)
    {
        for (vit = m_mesh.verticesBegin(); vit != vend; ++vit)
        {
            // don't smooth feature vertices
            if (vfeature && vfeature[*vit])
                continue;

            kmin = kmax = sumWeights = 0.0;

            vhit = vhend = m_mesh.halfedges(*vit);
            if (vhit)
                do
                {
                    v = m_mesh.toVertex(*vhit);

                    // don't consider feature vertices (high curvature)
                    if (vfeature && vfeature[v])
                        continue;

                    weight =
                        std::max(0.0, cotanWeight(m_mesh, m_mesh.edge(*vhit)));
                    sumWeights += weight;
                    kmin += weight * m_minCurvature[v];
                    kmax += weight * m_maxCurvature[v];
                } while (++vhit != vhend);

            if (sumWeights)
            {
                m_minCurvature[*vit] = kmin / sumWeights;
                m_maxCurvature[*vit] = kmax / sumWeights;
            }
        }
    }

    // remove property
    m_mesh.removeEdgeProperty(cotan);
}

//=============================================================================
} // namespace surface_mesh
//=============================================================================
