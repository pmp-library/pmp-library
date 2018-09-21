//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
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

#include <pmp/algorithms/SurfaceCurvature.h>
#include <pmp/algorithms/SurfaceNormals.h>
#include <pmp/MatVec.h>

//=============================================================================

namespace pmp {

//=============================================================================

SurfaceCurvature::SurfaceCurvature(SurfaceMesh& mesh) : m_mesh(mesh)
{
    m_minCurvature = m_mesh.addVertexProperty<Scalar>("curv:min");
    m_maxCurvature = m_mesh.addVertexProperty<Scalar>("curv:max");
}

//-----------------------------------------------------------------------------

SurfaceCurvature::~SurfaceCurvature()
{
    m_mesh.removeVertexProperty(m_minCurvature);
    m_mesh.removeVertexProperty(m_maxCurvature);
}

//-----------------------------------------------------------------------------

void SurfaceCurvature::analyze(unsigned int postSmoothingSteps)
{
    Scalar kmin, kmax, mean, gauss;
    Scalar area, sumAngles;
    Scalar weight, sumWeights;
    Point p0, p1, p2, laplace;

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

        if (!m_mesh.isIsolated(v) && !m_mesh.isSurfaceBoundary(v))
        {
            laplace = Point(0.0);
            sumWeights = 0.0;
            sumAngles = 0.0;
            p0 = m_mesh.position(v);

            // Voronoi area
            area = voronoiArea(m_mesh, v);

            // Laplace & angle sum
            for (auto vh : m_mesh.halfedges(v))
            {
                p1 = m_mesh.position(m_mesh.toVertex(vh));
                p2 = m_mesh.position(
                    m_mesh.toVertex(m_mesh.ccwRotatedHalfedge(vh)));

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

            mean = Scalar(0.5) * norm(laplace);
            gauss = (2.0 * M_PI - sumAngles) / area;

            const Scalar s = sqrt(std::max(Scalar(0.0), mean * mean - gauss));
            kmin = mean - s;
            kmax = mean + s;
        }

        m_minCurvature[v] = kmin;
        m_maxCurvature[v] = kmax;
    }

    // boundary vertices: interpolate from interior neighbors
    for (auto v : m_mesh.vertices())
    {
        if (m_mesh.isSurfaceBoundary(v))
        {
            kmin = kmax = sumWeights = 0.0;

            for (auto vh : m_mesh.halfedges(v))
            {
                v = m_mesh.toVertex(vh);
                if (!m_mesh.isSurfaceBoundary(v))
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
    smoothCurvatures(postSmoothingSteps);
}

//-----------------------------------------------------------------------------

void SurfaceCurvature::analyzeTensor(unsigned int postSmoothingSteps,
                                     bool twoRingNeighborhood)
{
    auto area = m_mesh.addVertexProperty<double>("curv:area", 0.0);
    auto normal = m_mesh.addFaceProperty<dvec3>("curv:normal");
    auto evec = m_mesh.addEdgeProperty<dvec3>("curv:evec", dvec3(0, 0, 0));
    auto angle = m_mesh.addEdgeProperty<double>("curv:angle", 0.0);

    dvec3 p0, p1, n0, n1, ev;
    double l, A, beta, a1, a2, a3;
    dmat3 tensor;

    double eval1, eval2, eval3, kmin, kmax;
    dvec3 evec1, evec2, evec3;

    std::vector<SurfaceMesh::Vertex> neighborhood;
    neighborhood.reserve(15);

    // precompute Voronoi area per vertex
    for (auto v : m_mesh.vertices())
    {
        area[v] = voronoiArea(m_mesh, v);
    }

    // precompute face normals
    for (auto f : m_mesh.faces())
    {
        normal[f] = (dvec3)SurfaceNormals::computeFaceNormal(m_mesh, f);
    }

    // precompute dihedralAngle*edgeLength*edge per edge
    for (auto e : m_mesh.edges())
    {
        auto h0 = m_mesh.halfedge(e, 0);
        auto h1 = m_mesh.halfedge(e, 1);
        auto f0 = m_mesh.face(h0);
        auto f1 = m_mesh.face(h1);
        if (f0.isValid() && f1.isValid())
        {
            n0 = normal[f0];
            n1 = normal[f1];
            ev = (dvec3)m_mesh.position(m_mesh.toVertex(h0));
            ev -= (dvec3)m_mesh.position(m_mesh.toVertex(h1));
            l = norm(ev);
            ev /= l;
            l *= 0.5; // only consider half of the edge (matchig Voronoi area)
            angle[e] = atan2(dot(cross(n0, n1), ev), dot(n0, n1));
            evec[e] = sqrt(l) * ev;
        }
    }

    // compute curvature tensor for each vertex
    for (auto v : m_mesh.vertices())
    {
        kmin = 0.0;
        kmax = 0.0;

        if (!m_mesh.isIsolated(v))
        {
            // one-ring or two-ring neighborhood?
            neighborhood.clear();
            neighborhood.push_back(v);
            if (twoRingNeighborhood)
            {
                for (auto vv : m_mesh.vertices(v))
                    neighborhood.push_back(vv);
            }

            A = 0.0;
            tensor = dmat3(0.0);

            // compute tensor over vertex neighborhood stored in vertices
            for (auto nit : neighborhood)
            {
                // accumulate tensor from dihedral angles around vertices
                for (auto hv : m_mesh.halfedges(nit))
                {
                    auto ee = m_mesh.edge(hv);
                    ev = evec[ee];
                    beta = angle[ee];
                    for (int i = 0; i < 3; ++i)
                        for (int j = 0; j < 3; ++j)
                            tensor(i, j) += beta * ev[i] * ev[j];
                }

                // accumulate area
                A += area[nit];
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

        m_minCurvature[v] = kmin;
        m_maxCurvature[v] = kmax;
    }

    // clean-up properties
    m_mesh.removeVertexProperty(area);
    m_mesh.removeEdgeProperty(evec);
    m_mesh.removeEdgeProperty(angle);
    m_mesh.removeFaceProperty(normal);

    // smooth curvature values
    smoothCurvatures(postSmoothingSteps);
}

//-----------------------------------------------------------------------------

void SurfaceCurvature::smoothCurvatures(unsigned int iterations)
{
    Scalar kmin, kmax;
    Scalar weight, sumWeights;

    // properties
    auto vfeature = m_mesh.getVertexProperty<bool>("v:feature");
    auto cotan = m_mesh.addEdgeProperty<double>("curv:cotan");

    // cotan weight per edge
    for (auto e : m_mesh.edges())
    {
        cotan[e] = cotanWeight(m_mesh, e);
    }

    for (unsigned int i = 0; i < iterations; ++i)
    {
        for (auto v : m_mesh.vertices())
        {
            // don't smooth feature vertices
            if (vfeature && vfeature[v])
                continue;

            kmin = kmax = sumWeights = 0.0;

            for (auto vh : m_mesh.halfedges(v))
            {
                auto tv = m_mesh.toVertex(vh);

                // don't consider feature vertices (high curvature)
                if (vfeature && vfeature[tv])
                    continue;

                weight = std::max(0.0, cotanWeight(m_mesh, m_mesh.edge(vh)));
                sumWeights += weight;
                kmin += weight * m_minCurvature[tv];
                kmax += weight * m_maxCurvature[tv];
            }

            if (sumWeights)
            {
                m_minCurvature[v] = kmin / sumWeights;
                m_maxCurvature[v] = kmax / sumWeights;
            }
        }
    }

    // remove property
    m_mesh.removeEdgeProperty(cotan);
}

//-----------------------------------------------------------------------------

void SurfaceCurvature::meanCurvatureToTextureCoordinates() const
{
    auto curvatures = m_mesh.addVertexProperty<Scalar>("v:curv");
    for (auto v : m_mesh.vertices())
    {
        curvatures[v] = fabs(meanCurvature(v));
    }
    curvatureToTextureCoordinates();
    m_mesh.removeVertexProperty<Scalar>(curvatures);
}

//-----------------------------------------------------------------------------

void SurfaceCurvature::gaussCurvatureToTextureCoordinates() const
{
    auto curvatures = m_mesh.addVertexProperty<Scalar>("v:curv");
    for (auto v : m_mesh.vertices())
    {
        curvatures[v] = gaussCurvature(v);
    }
    curvatureToTextureCoordinates();
    m_mesh.removeVertexProperty<Scalar>(curvatures);
}

//-----------------------------------------------------------------------------

void SurfaceCurvature::maxCurvatureToTextureCoordinates() const
{
    auto curvatures = m_mesh.addVertexProperty<Scalar>("v:curv");
    for (auto v : m_mesh.vertices())
    {
        curvatures[v] = maxAbsCurvature(v);
    }
    curvatureToTextureCoordinates();
    m_mesh.removeVertexProperty<Scalar>(curvatures);
}

//-----------------------------------------------------------------------------

void SurfaceCurvature::curvatureToTextureCoordinates() const
{
    auto curvatures = m_mesh.getVertexProperty<Scalar>("v:curv");
    assert(curvatures);

    // sort curvature values
    std::vector<Scalar> values;
    values.reserve(m_mesh.nVertices());
    for (auto v : m_mesh.vertices())
    {
        values.push_back(curvatures[v]);
    }
    std::sort(values.begin(), values.end());
    unsigned int n = values.size() - 1;
    std::cout << "curvature: [" << values[0] << ", " << values[n - 1] << "]\n";

    // clamp upper/lower 5%
    unsigned int i = n / 20;
    Scalar kmin = values[i];
    Scalar kmax = values[n - 1 - i];

    // generate 1D texture coordiantes
    auto tex = m_mesh.vertexProperty<TextureCoordinate>("v:tex");
    if (kmin < 0.0) // signed
    {
        kmax = std::max(fabs(kmin), fabs(kmax));
        for (auto v : m_mesh.vertices())
        {
            tex[v] =
                TextureCoordinate((0.5f * curvatures[v] / kmax) + 0.5f, 0.0);
        }
    }
    else // unsigned
    {
        for (auto v : m_mesh.vertices())
        {
            tex[v] =
                TextureCoordinate((curvatures[v] - kmin) / (kmax - kmin), 0.0);
        }
    }
}

//=============================================================================
} // namespace pmp
//=============================================================================
