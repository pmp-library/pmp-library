// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/curvature.h"
#include "pmp/exceptions.h"
#include "pmp/algorithms/normals.h"
#include "pmp/algorithms/differential_geometry.h"
#include "pmp/algorithms/laplace.h"

namespace pmp {

class CurvatureAnalyzer
{
public:
    //! construct with mesh to be analyzed
    CurvatureAnalyzer(SurfaceMesh& mesh);

    ~CurvatureAnalyzer()
    {
        mesh_.remove_vertex_property(min_curvature_);
        mesh_.remove_vertex_property(max_curvature_);
    }

    //! compute curvature information for each vertex, optionally followed
    //! by some smoothing iterations of the curvature values
    void analyze(unsigned int post_smoothing_steps = 0);

    //! compute curvature information for each vertex, optionally followed
    //! by some smoothing iterations of the curvature values
    void analyze_tensor(unsigned int post_smoothing_steps = 0,
                        bool two_ring_neighborhood = false);

    //! return mean curvature
    Scalar mean_curvature(Vertex v) const
    {
        return Scalar(0.5) * (min_curvature_[v] + max_curvature_[v]);
    }

    //! return Gaussian curvature
    Scalar gauss_curvature(Vertex v) const
    {
        return min_curvature_[v] * max_curvature_[v];
    }

    //! return minimum (signed) curvature
    Scalar min_curvature(Vertex v) const { return min_curvature_[v]; }

    //! return maximum (signed) curvature
    Scalar max_curvature(Vertex v) const { return max_curvature_[v]; }

    //! return maximum absolute curvature
    Scalar max_abs_curvature(Vertex v) const
    {
        return std::max(fabs(min_curvature_[v]), fabs(max_curvature_[v]));
    }

private:
    // determine curvature values on boundary from non-boundary neighbors
    void set_boundary_curvatures();

    // smooth curvature values
    void smooth_curvatures(unsigned int iterations);

    SurfaceMesh& mesh_;
    VertexProperty<Scalar> min_curvature_;
    VertexProperty<Scalar> max_curvature_;
};

CurvatureAnalyzer::CurvatureAnalyzer(SurfaceMesh& mesh) : mesh_(mesh)
{
    min_curvature_ = mesh_.add_vertex_property<Scalar>("curv:min");
    max_curvature_ = mesh_.add_vertex_property<Scalar>("curv:max");
}

void CurvatureAnalyzer::analyze(unsigned int post_smoothing_steps)
{
    Scalar kmin, kmax, mean, gauss;
    Scalar area, sum_angles;
    Point p0, p1, p2;

    // compute area-normalized Laplace
    SparseMatrix L;
    laplace_matrix(mesh_, L);
    DiagonalMatrix M;
    mass_matrix(mesh_, M);
    DenseMatrix X;
    coordinates_to_matrix(mesh_, X);
    DenseMatrix LX = L * X;

    // mean curvature as norm of Laplace
    // Gauss curvatures as angle deficit
    // min/max from mean/gauss
    for (auto v : mesh_.vertices())
    {
        kmin = kmax = 0.0;

        if (!mesh_.is_isolated(v) && !mesh_.is_boundary(v))
        {
            p0 = mesh_.position(v);

            // Voronoi area
            area = M.diagonal()[v.idx()];

            // angle sum
            sum_angles = 0.0;
            for (auto vh : mesh_.halfedges(v))
            {
                p1 = mesh_.position(mesh_.to_vertex(vh));
                p2 = mesh_.position(
                    mesh_.to_vertex(mesh_.ccw_rotated_halfedge(vh)));
                sum_angles += angle(p1 - p0, p2 - p0);
            }

            mean = 0.5 * LX.row(v.idx()).norm() / area;
            gauss = (2.0 * M_PI - sum_angles) / area;

            const Scalar s = sqrt(std::max(Scalar(0.0), mean * mean - gauss));
            kmin = mean - s;
            kmax = mean + s;
        }

        min_curvature_[v] = kmin;
        max_curvature_[v] = kmax;
    }

    // boundary vertices: interpolate from interior neighbors
    set_boundary_curvatures();

    // smooth curvatures values
    smooth_curvatures(post_smoothing_steps);
}

void CurvatureAnalyzer::analyze_tensor(unsigned int post_smoothing_steps,
                                       bool two_ring_neighborhood)
{
    auto area = mesh_.add_vertex_property<double>("curv:area", 0.0);
    auto normal = mesh_.add_face_property<dvec3>("curv:normal");
    auto evec = mesh_.add_edge_property<dvec3>("curv:evec", dvec3(0, 0, 0));
    auto angle = mesh_.add_edge_property<double>("curv:angle", 0.0);

    dvec3 n0, n1, ev;
    double l, A, beta, a1, a2, a3;
    dmat3 tensor;

    double eval1, eval2, eval3, kmin, kmax;
    dvec3 evec1, evec2, evec3;

    std::vector<Vertex> neighborhood;
    neighborhood.reserve(15);

    // precompute Voronoi area per vertex
    DiagonalMatrix M;
    mass_matrix(mesh_, M);
    for (auto v : mesh_.vertices())
    {
        area[v] = M.diagonal()[v.idx()];
    }

    // precompute face normals
    for (auto f : mesh_.faces())
    {
        normal[f] = (dvec3)face_normal(mesh_, f);
    }

    // precompute dihedralAngle*edge_length*edge per edge
    for (auto e : mesh_.edges())
    {
        auto h0 = mesh_.halfedge(e, 0);
        auto h1 = mesh_.halfedge(e, 1);
        auto f0 = mesh_.face(h0);
        auto f1 = mesh_.face(h1);
        if (f0.is_valid() && f1.is_valid())
        {
            n0 = normal[f0];
            n1 = normal[f1];
            ev = (dvec3)mesh_.position(mesh_.to_vertex(h0));
            ev -= (dvec3)mesh_.position(mesh_.to_vertex(h1));
            l = norm(ev);
            ev /= l;
            l *= 0.5; // only consider half of the edge (matching Voronoi area)
            angle[e] = atan2(dot(cross(n0, n1), ev), dot(n0, n1));
            evec[e] = sqrt(l) * ev;
        }
    }

    // compute curvature tensor for each vertex
    for (auto v : mesh_.vertices())
    {
        kmin = 0.0;
        kmax = 0.0;

        if (!mesh_.is_isolated(v) && !mesh_.is_boundary(v))
        {
            // one-ring or two-ring neighborhood?
            neighborhood.clear();
            neighborhood.push_back(v);
            if (two_ring_neighborhood)
            {
                for (auto vv : mesh_.vertices(v))
                    neighborhood.push_back(vv);
            }

            A = 0.0;
            tensor = dmat3(0.0);

            // compute tensor over vertex neighborhood stored in vertices
            for (auto nit : neighborhood)
            {
                if (mesh_.is_boundary(nit))
                    continue;

                // accumulate tensor from dihedral angles around vertices
                for (auto e : mesh_.edges(nit))
                {
                    ev = evec[e];
                    beta = angle[e];
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
            bool ok = symmetric_eigendecomposition(tensor, eval1, eval2, eval3,
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

        min_curvature_[v] = kmin;
        max_curvature_[v] = kmax;
    }

    // clean-up properties
    mesh_.remove_vertex_property(area);
    mesh_.remove_edge_property(evec);
    mesh_.remove_edge_property(angle);
    mesh_.remove_face_property(normal);

    // boundary vertices: interpolate from interior neighbors
    set_boundary_curvatures();

    // smooth curvature values
    smooth_curvatures(post_smoothing_steps);
}

void CurvatureAnalyzer::set_boundary_curvatures()
{
    for (auto v : mesh_.vertices())
    {
        if (mesh_.is_boundary(v))
        {
            Scalar kmin(0.0), kmax(0.0), sum(0.0);
            for (auto vv : mesh_.vertices(v))
            {
                if (!mesh_.is_boundary(vv))
                {
                    sum += 1.0;
                    kmin += min_curvature_[vv];
                    kmax += max_curvature_[vv];
                }
            }

            if (sum)
            {
                kmin /= sum;
                kmax /= sum;
            }

            min_curvature_[v] = kmin;
            max_curvature_[v] = kmax;
        }
    }
}

void CurvatureAnalyzer::smooth_curvatures(unsigned int iterations)
{
    // Laplace matrix (clamp negative cotan weights to zero)
    SparseMatrix L;
    laplace_matrix(mesh_, L, true);

    // normalize each row by sum of weights
    // scale by 0.5 to make it more robust
    // multiply by -1 to make it neg. definite again
    DiagonalMatrix D = L.diagonal().asDiagonal().inverse();
    L = -0.5 * D * L;

    // copy vertex curvatures to matrix
    const int n = mesh_.n_vertices();
    DenseMatrix curv(n, 2);
    for (auto v : mesh_.vertices())
    {
        curv(v.idx(), 0) = min_curvature_[v];
        curv(v.idx(), 1) = max_curvature_[v];
    }

    // perform smoothing iterations
    for (unsigned int i = 0; i < iterations; ++i)
    {
        curv += L * curv;
    }

    // copy result to curvatures
    for (auto v : mesh_.vertices())
    {
        min_curvature_[v] = curv(v.idx(), 0);
        max_curvature_[v] = curv(v.idx(), 1);
    }
}

void curvature_to_texture_coordinates(SurfaceMesh& mesh)
{
    auto curvatures = mesh.get_vertex_property<Scalar>("v:curv");
    assert(curvatures);

    // sort curvature values
    std::vector<Scalar> values;
    values.reserve(mesh.n_vertices());
    for (auto v : mesh.vertices())
    {
        values.push_back(curvatures[v]);
    }
    std::sort(values.begin(), values.end());
    unsigned int n = values.size() - 1;
    // std::cout << "curvatures in [" << values[0] << ", " << values[n] << "]\n";

    // clamp upper/lower 5%
    unsigned int i = n / 20;
    Scalar kmin = values[i];
    Scalar kmax = values[n - 1 - i];

    // generate 1D texture coordinates
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    if (kmin < 0.0) // signed
    {
        kmax = std::max(fabs(kmin), fabs(kmax));
        for (auto v : mesh.vertices())
        {
            tex[v] = TexCoord((0.5f * curvatures[v] / kmax) + 0.5f, 0.0);
        }
    }
    else // unsigned
    {
        for (auto v : mesh.vertices())
        {
            tex[v] = TexCoord((curvatures[v] - kmin) / (kmax - kmin), 0.0);
        }
    }
}

void curvature(SurfaceMesh& mesh, Curvature c, int smoothing_steps,
               bool use_tensor, bool use_two_ring)
{
    CurvatureAnalyzer analyzer(mesh);
    if (use_tensor)
        analyzer.analyze_tensor(smoothing_steps, use_two_ring);
    else
        analyzer.analyze(smoothing_steps);

    auto curvatures = mesh.vertex_property<Scalar>("v:curv");

    switch (c)
    {
        case Curvature::min:
        {
            for (auto v : mesh.vertices())
                curvatures[v] = analyzer.min_curvature(v);
            break;
        }
        case Curvature::max:
        {
            for (auto v : mesh.vertices())
                curvatures[v] = analyzer.max_curvature(v);
            break;
        }
        case Curvature::mean:
        {
            for (auto v : mesh.vertices())
                curvatures[v] = fabs(analyzer.mean_curvature(v));
            break;
        }
        case Curvature::gauss:
        {
            for (auto v : mesh.vertices())
                curvatures[v] = analyzer.gauss_curvature(v);
            break;
        }
        case Curvature::max_abs:
        {
            for (auto v : mesh.vertices())
                curvatures[v] = analyzer.max_abs_curvature(v);
            break;
        }
        default:
            throw InvalidInputException("Unknown Curvature type");
    }
}

} // namespace pmp
