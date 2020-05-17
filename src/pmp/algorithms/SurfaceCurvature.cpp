// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/SurfaceCurvature.h"
#include "pmp/algorithms/SurfaceNormals.h"
#include "pmp/algorithms/DifferentialGeometry.h"

namespace pmp {

SurfaceCurvature::SurfaceCurvature(SurfaceMesh& mesh) : mesh_(mesh)
{
    min_curvature_ = mesh_.add_vertex_property<Scalar>("curv:min");
    max_curvature_ = mesh_.add_vertex_property<Scalar>("curv:max");
}

SurfaceCurvature::~SurfaceCurvature()
{
    mesh_.remove_vertex_property(min_curvature_);
    mesh_.remove_vertex_property(max_curvature_);
}

void SurfaceCurvature::analyze(unsigned int post_smoothing_steps)
{
    Scalar kmin, kmax, mean, gauss;
    Scalar area, sum_angles;
    Scalar weight, sum_weights;
    Point p0, p1, p2, laplace;

    // cotan weight per edge
    auto cotan = mesh_.add_edge_property<double>("curv:cotan");
    for (auto e : mesh_.edges())
        cotan[e] = cotan_weight(mesh_, e);

    // Voronoi area per vertex
    // Laplace per vertex
    // angle sum per vertex
    // -> mean, Gauss -> min, max curvature
    for (auto v : mesh_.vertices())
    {
        kmin = kmax = 0.0;

        if (!mesh_.is_isolated(v) && !mesh_.is_boundary(v))
        {
            laplace = Point(0.0);
            sum_weights = 0.0;
            sum_angles = 0.0;
            p0 = mesh_.position(v);

            // Voronoi area
            area = voronoi_area(mesh_, v);

            // Laplace & angle sum
            for (auto vh : mesh_.halfedges(v))
            {
                p1 = mesh_.position(mesh_.to_vertex(vh));
                p2 = mesh_.position(
                    mesh_.to_vertex(mesh_.ccw_rotated_halfedge(vh)));

                weight = cotan[mesh_.edge(vh)];
                sum_weights += weight;
                laplace += weight * p1;

                p1 -= p0;
                p1.normalize();
                p2 -= p0;
                p2.normalize();
                sum_angles += acos(clamp_cos(dot(p1, p2)));
            }
            laplace -= sum_weights * mesh_.position(v);
            laplace /= Scalar(2.0) * area;

            mean = Scalar(0.5) * norm(laplace);
            gauss = (2.0 * M_PI - sum_angles) / area;

            const Scalar s = sqrt(std::max(Scalar(0.0), mean * mean - gauss));
            kmin = mean - s;
            kmax = mean + s;
        }

        min_curvature_[v] = kmin;
        max_curvature_[v] = kmax;
    }

    // boundary vertices: interpolate from interior neighbors
    for (auto v : mesh_.vertices())
    {
        if (mesh_.is_boundary(v))
        {
            kmin = kmax = sum_weights = 0.0;

            for (auto vh : mesh_.halfedges(v))
            {
                v = mesh_.to_vertex(vh);
                if (!mesh_.is_boundary(v))
                {
                    weight = cotan[mesh_.edge(vh)];
                    sum_weights += weight;
                    kmin += weight * min_curvature_[v];
                    kmax += weight * max_curvature_[v];
                }
            }

            if (sum_weights)
            {
                kmin /= sum_weights;
                kmax /= sum_weights;
            }

            min_curvature_[v] = kmin;
            max_curvature_[v] = kmax;
        }
    }

    // clean-up properties
    mesh_.remove_edge_property(cotan);

    // smooth curvature values
    smooth_curvatures(post_smoothing_steps);
}

void SurfaceCurvature::analyze_tensor(unsigned int post_smoothing_steps,
                                      bool two_ring_neighborhood)
{
    auto area = mesh_.add_vertex_property<double>("curv:area", 0.0);
    auto normal = mesh_.add_face_property<dvec3>("curv:normal");
    auto evec = mesh_.add_edge_property<dvec3>("curv:evec", dvec3(0, 0, 0));
    auto angle = mesh_.add_edge_property<double>("curv:angle", 0.0);

    dvec3 p0, p1, n0, n1, ev;
    double l, A, beta, a1, a2, a3;
    dmat3 tensor;

    double eval1, eval2, eval3, kmin, kmax;
    dvec3 evec1, evec2, evec3;

    std::vector<Vertex> neighborhood;
    neighborhood.reserve(15);

    // precompute Voronoi area per vertex
    for (auto v : mesh_.vertices())
    {
        area[v] = voronoi_area(mesh_, v);
    }

    // precompute face normals
    for (auto f : mesh_.faces())
    {
        normal[f] = (dvec3)SurfaceNormals::compute_face_normal(mesh_, f);
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
            l *= 0.5; // only consider half of the edge (matchig Voronoi area)
            angle[e] = atan2(dot(cross(n0, n1), ev), dot(n0, n1));
            evec[e] = sqrt(l) * ev;
        }
    }

    // compute curvature tensor for each vertex
    for (auto v : mesh_.vertices())
    {
        kmin = 0.0;
        kmax = 0.0;

        if (!mesh_.is_isolated(v))
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
                // accumulate tensor from dihedral angles around vertices
                for (auto hv : mesh_.halfedges(nit))
                {
                    auto ee = mesh_.edge(hv);
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

    // smooth curvature values
    smooth_curvatures(post_smoothing_steps);
}

void SurfaceCurvature::smooth_curvatures(unsigned int iterations)
{
    Scalar kmin, kmax;
    Scalar weight, sum_weights;

    // properties
    auto vfeature = mesh_.get_vertex_property<bool>("v:feature");
    auto cotan = mesh_.add_edge_property<double>("curv:cotan");

    // cotan weight per edge
    for (auto e : mesh_.edges())
    {
        cotan[e] = cotan_weight(mesh_, e);
    }

    for (unsigned int i = 0; i < iterations; ++i)
    {
        for (auto v : mesh_.vertices())
        {
            // don't smooth feature vertices
            if (vfeature && vfeature[v])
                continue;

            kmin = kmax = sum_weights = 0.0;

            for (auto vh : mesh_.halfedges(v))
            {
                auto tv = mesh_.to_vertex(vh);

                // don't consider feature vertices (high curvature)
                if (vfeature && vfeature[tv])
                    continue;

                weight = std::max(0.0, cotan_weight(mesh_, mesh_.edge(vh)));
                sum_weights += weight;
                kmin += weight * min_curvature_[tv];
                kmax += weight * max_curvature_[tv];
            }

            if (sum_weights)
            {
                min_curvature_[v] = kmin / sum_weights;
                max_curvature_[v] = kmax / sum_weights;
            }
        }
    }

    // remove property
    mesh_.remove_edge_property(cotan);
}

void SurfaceCurvature::mean_curvature_to_texture_coordinates() const
{
    auto curvatures = mesh_.add_vertex_property<Scalar>("v:curv");
    for (auto v : mesh_.vertices())
    {
        curvatures[v] = fabs(mean_curvature(v));
    }
    curvature_to_texture_coordinates();
    mesh_.remove_vertex_property<Scalar>(curvatures);
}

void SurfaceCurvature::gauss_curvature_to_texture_coordinates() const
{
    auto curvatures = mesh_.add_vertex_property<Scalar>("v:curv");
    for (auto v : mesh_.vertices())
    {
        curvatures[v] = gauss_curvature(v);
    }
    curvature_to_texture_coordinates();
    mesh_.remove_vertex_property<Scalar>(curvatures);
}

void SurfaceCurvature::max_curvature_to_texture_coordinates() const
{
    auto curvatures = mesh_.add_vertex_property<Scalar>("v:curv");
    for (auto v : mesh_.vertices())
    {
        curvatures[v] = max_abs_curvature(v);
    }
    curvature_to_texture_coordinates();
    mesh_.remove_vertex_property<Scalar>(curvatures);
}

void SurfaceCurvature::curvature_to_texture_coordinates() const
{
    auto curvatures = mesh_.get_vertex_property<Scalar>("v:curv");
    assert(curvatures);

    // sort curvature values
    std::vector<Scalar> values;
    values.reserve(mesh_.n_vertices());
    for (auto v : mesh_.vertices())
    {
        values.push_back(curvatures[v]);
    }
    std::sort(values.begin(), values.end());
    unsigned int n = values.size() - 1;

    // clamp upper/lower 5%
    unsigned int i = n / 20;
    Scalar kmin = values[i];
    Scalar kmax = values[n - 1 - i];

    // generate 1D texture coordiantes
    auto tex = mesh_.vertex_property<TexCoord>("v:tex");
    if (kmin < 0.0) // signed
    {
        kmax = std::max(fabs(kmin), fabs(kmax));
        for (auto v : mesh_.vertices())
        {
            tex[v] = TexCoord((0.5f * curvatures[v] / kmax) + 0.5f, 0.0);
        }
    }
    else // unsigned
    {
        for (auto v : mesh_.vertices())
        {
            tex[v] = TexCoord((curvatures[v] - kmin) / (kmax - kmin), 0.0);
        }
    }
}

} // namespace pmp
