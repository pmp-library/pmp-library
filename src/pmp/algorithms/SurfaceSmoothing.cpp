// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/SurfaceSmoothing.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "pmp/algorithms/DifferentialGeometry.h"

namespace pmp {

using SparseMatrix = Eigen::SparseMatrix<double>;
using Triplet = Eigen::Triplet<double>;

SurfaceSmoothing::SurfaceSmoothing(SurfaceMesh& mesh) : mesh_(mesh)
{
    how_many_edge_weights_ = 0;
    how_many_vertex_weights_ = 0;
}

SurfaceSmoothing::~SurfaceSmoothing()
{
    auto vweight = mesh_.get_vertex_property<Scalar>("v:area");
    if (vweight)
        mesh_.remove_vertex_property(vweight);

    auto eweight = mesh_.get_edge_property<Scalar>("e:cotan");
    if (eweight)
        mesh_.remove_edge_property(eweight);
}

void SurfaceSmoothing::compute_edge_weights(bool use_uniform_laplace)
{
    auto eweight = mesh_.edge_property<Scalar>("e:cotan");

    if (use_uniform_laplace)
    {
        for (auto e : mesh_.edges())
            eweight[e] = 1.0;
    }
    else
    {
        for (auto e : mesh_.edges())
            eweight[e] = std::max(0.0, cotan_weight(mesh_, e));
    }

    how_many_edge_weights_ = mesh_.n_edges();
}

void SurfaceSmoothing::compute_vertex_weights(bool use_uniform_laplace)
{
    auto vweight = mesh_.vertex_property<Scalar>("v:area");

    if (use_uniform_laplace)
    {
        for (auto v : mesh_.vertices())
            vweight[v] = 1.0 / mesh_.valence(v);
    }
    else
    {
        for (auto v : mesh_.vertices())
            vweight[v] = 0.5 / voronoi_area(mesh_, v);
    }

    how_many_vertex_weights_ = mesh_.n_vertices();
}

void SurfaceSmoothing::explicit_smoothing(unsigned int iters,
                                          bool use_uniform_laplace)
{
    if (!mesh_.n_vertices())
        return;

    // compute Laplace weight per edge: cotan or uniform
    if (!mesh_.has_edge_property("e:cotan") ||
        how_many_edge_weights_ != mesh_.n_edges())
        compute_edge_weights(use_uniform_laplace);

    auto points = mesh_.get_vertex_property<Point>("v:point");
    auto eweight = mesh_.get_edge_property<Scalar>("e:cotan");
    auto laplace = mesh_.add_vertex_property<Point>("v:laplace");

    // smoothing iterations
    Vertex vv;
    Edge e;
    for (unsigned int i = 0; i < iters; ++i)
    {
        // step 1: compute Laplace for each vertex
        for (auto v : mesh_.vertices())
        {
            Point l(0, 0, 0);

            if (!mesh_.is_boundary(v))
            {
                Scalar w(0);

                for (auto h : mesh_.halfedges(v))
                {
                    vv = mesh_.to_vertex(h);
                    e = mesh_.edge(h);
                    l += eweight[e] * (points[vv] - points[v]);
                    w += eweight[e];
                }

                l /= w;
            }

            laplace[v] = l;
        }

        // step 2: move each vertex by its (damped) Laplacian
        for (auto v : mesh_.vertices())
        {
            points[v] += 0.5f * laplace[v];
        }
    }

    // clean-up custom properties
    mesh_.remove_vertex_property(laplace);
}

void SurfaceSmoothing::implicit_smoothing(Scalar timestep,
                                          bool use_uniform_laplace,
                                          bool rescale)
{
    if (!mesh_.n_vertices())
        return;

    // compute edge weights if they don't exist or if the mesh changed
    if (!mesh_.has_edge_property("e:cotan") ||
        how_many_edge_weights_ != mesh_.n_edges())
        compute_edge_weights(use_uniform_laplace);

    // compute vertex weights
    compute_vertex_weights(use_uniform_laplace);

    // store center and area
    Point center_before(0, 0, 0);
    Scalar area_before(0);
    if (rescale)
    {
        center_before = centroid(mesh_);
        area_before = surface_area(mesh_);
    }

    // properties
    auto points = mesh_.get_vertex_property<Point>("v:point");
    auto vweight = mesh_.get_vertex_property<Scalar>("v:area");
    auto eweight = mesh_.get_edge_property<Scalar>("e:cotan");
    auto idx = mesh_.add_vertex_property<int>("v:idx", -1);

    // collect free (non-boundary) vertices in array free_vertices[]
    // assign indices such that idx[ free_vertices[i] ] == i
    unsigned i = 0;
    std::vector<Vertex> free_vertices;
    free_vertices.reserve(mesh_.n_vertices());
    for (auto v : mesh_.vertices())
    {
        if (!mesh_.is_boundary(v))
        {
            idx[v] = i++;
            free_vertices.push_back(v);
        }
    }
    const unsigned int n = free_vertices.size();

    // A*X = B
    SparseMatrix A(n, n);
    Eigen::MatrixXd B(n, 3);

    // nonzero elements of A as triplets: (row, column, value)
    std::vector<Triplet> triplets;

    // setup matrix A and rhs B
    dvec3 b;
    double ww;
    Vertex v, vv;
    Edge e;
    for (unsigned int i = 0; i < n; ++i)
    {
        v = free_vertices[i];

        // rhs row
        b = static_cast<dvec3>(points[v]) / vweight[v];

        // lhs row
        ww = 0.0;
        for (auto h : mesh_.halfedges(v))
        {
            vv = mesh_.to_vertex(h);
            e = mesh_.edge(h);
            ww += eweight[e];

            // fixed boundary vertex -> right hand side
            if (mesh_.is_boundary(vv))
            {
                b -= -timestep * eweight[e] * static_cast<dvec3>(points[vv]);
            }
            // free interior vertex -> matrix
            else
            {
                triplets.emplace_back(i, idx[vv], -timestep * eweight[e]);
            }

            B.row(i) = (Eigen::Vector3d)b;
        }

        // center vertex -> matrix
        triplets.emplace_back(i, i, 1.0 / vweight[v] + timestep * ww);
    }

    // build sparse matrix from triplets
    A.setFromTriplets(triplets.begin(), triplets.end());

    // solve A*X = B
    Eigen::SimplicialLDLT<SparseMatrix> solver(A);
    Eigen::MatrixXd X = solver.solve(B);
    if (solver.info() != Eigen::Success)
    {
        // clean-up
        mesh_.remove_vertex_property(idx);
        auto what = "SurfaceSmoothing: Failed to solve linear system.";
        throw SolverException(what);
    }
    else
    {
        // copy solution
        for (unsigned int i = 0; i < n; ++i)
        {
            points[free_vertices[i]] = X.row(i);
        }
    }

    if (rescale)
    {
        // restore original surface area
        Scalar area_after = surface_area(mesh_);
        Scalar scale = sqrt(area_before / area_after);
        for (auto v : mesh_.vertices())
            mesh_.position(v) *= scale;

        // restore original center
        Point center_after = centroid(mesh_);
        Point trans = center_before - center_after;
        for (auto v : mesh_.vertices())
            mesh_.position(v) += trans;
    }

    // clean-up
    mesh_.remove_vertex_property(idx);
}

} // namespace pmp
// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/SurfaceSmoothing.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "pmp/algorithms/DifferentialGeometry.h"
#include "pmp/algorithms/SurfaceNormals.h"

namespace pmp {

using SparseMatrix = Eigen::SparseMatrix<double>;
using Triplet = Eigen::Triplet<double>;

SurfaceSmoothing::SurfaceSmoothing(SurfaceMesh& mesh) : mesh_(mesh)
{
    how_many_edge_weights_ = 0;
    how_many_vertex_weights_ = 0;
}

SurfaceSmoothing::~SurfaceSmoothing()
{
    auto vweight = mesh_.get_vertex_property<Scalar>("v:area");
    if (vweight)
        mesh_.remove_vertex_property(vweight);

    auto eweight = mesh_.get_edge_property<Scalar>("e:cotan");
    if (eweight)
        mesh_.remove_edge_property(eweight);
}

void SurfaceSmoothing::compute_edge_weights(bool use_uniform_laplace)
{
    auto eweight = mesh_.edge_property<Scalar>("e:cotan");

    if (use_uniform_laplace)
    {
        for (auto e : mesh_.edges())
            eweight[e] = 1.0;
    }
    else
    {
        for (auto e : mesh_.edges())
            eweight[e] = std::max(0.0, cotan_weight(mesh_, e));
    }

    how_many_edge_weights_ = mesh_.n_edges();
}

void SurfaceSmoothing::compute_vertex_weights(bool use_uniform_laplace)
{
    auto vweight = mesh_.vertex_property<Scalar>("v:area");

    if (use_uniform_laplace)
    {
        for (auto v : mesh_.vertices())
            vweight[v] = 1.0 / mesh_.valence(v);
    }
    else
    {
        for (auto v : mesh_.vertices())
            vweight[v] = 0.5 / voronoi_area(mesh_, v);
    }

    how_many_vertex_weights_ = mesh_.n_vertices();
}

void SurfaceSmoothing::explicit_smoothing(unsigned int iters,
                                          bool use_uniform_laplace)
{
    if (!mesh_.n_vertices())
        return;

    // compute Laplace weight per edge: cotan or uniform
    if (!mesh_.has_edge_property("e:cotan") ||
        how_many_edge_weights_ != mesh_.n_edges())
        compute_edge_weights(use_uniform_laplace);

    auto points = mesh_.get_vertex_property<Point>("v:point");
    auto eweight = mesh_.get_edge_property<Scalar>("e:cotan");
    auto laplace = mesh_.add_vertex_property<Point>("v:laplace");

    // smoothing iterations
    Vertex vv;
    Edge e;
    for (unsigned int i = 0; i < iters; ++i)
    {
        // step 1: compute Laplace for each vertex
        for (auto v : mesh_.vertices())
        {
            Point l(0, 0, 0);

            if (!mesh_.is_boundary(v))
            {
                Scalar w(0);

                for (auto h : mesh_.halfedges(v))
                {
                    vv = mesh_.to_vertex(h);
                    e = mesh_.edge(h);
                    l += eweight[e] * (points[vv] - points[v]);
                    w += eweight[e];
                }

                l /= w;
            }

            laplace[v] = l;
        }

        // step 2: move each vertex by its (damped) Laplacian
        for (auto v : mesh_.vertices())
        {
            points[v] += 0.5f * laplace[v];
        }
    }

    // clean-up custom properties
    mesh_.remove_vertex_property(laplace);
}

void SurfaceSmoothing::implicit_smoothing(Scalar timestep,
                                          bool use_uniform_laplace,
                                          bool rescale)
{
    if (!mesh_.n_vertices())
        return;

    // compute edge weights if they don't exist or if the mesh changed
    if (!mesh_.has_edge_property("e:cotan") ||
        how_many_edge_weights_ != mesh_.n_edges())
        compute_edge_weights(use_uniform_laplace);

    // compute vertex weights
    compute_vertex_weights(use_uniform_laplace);

    // store center and area
    Point center_before(0, 0, 0);
    Scalar area_before(0);
    if (rescale)
    {
        center_before = centroid(mesh_);
        area_before = surface_area(mesh_);
    }

    // properties
    auto points = mesh_.get_vertex_property<Point>("v:point");
    auto vweight = mesh_.get_vertex_property<Scalar>("v:area");
    auto eweight = mesh_.get_edge_property<Scalar>("e:cotan");
    auto idx = mesh_.add_vertex_property<int>("v:idx", -1);

    // collect free (non-boundary) vertices in array free_vertices[]
    // assign indices such that idx[ free_vertices[i] ] == i
    unsigned i = 0;
    std::vector<Vertex> free_vertices;
    free_vertices.reserve(mesh_.n_vertices());
    for (auto v : mesh_.vertices())
    {
        if (!mesh_.is_boundary(v))
        {
            idx[v] = i++;
            free_vertices.push_back(v);
        }
    }
    const unsigned int n = free_vertices.size();

    // A*X = B
    SparseMatrix A(n, n);
    Eigen::MatrixXd B(n, 3);

    // nonzero elements of A as triplets: (row, column, value)
    std::vector<Triplet> triplets;

    // setup matrix A and rhs B
    dvec3 b;
    double ww;
    Vertex v, vv;
    Edge e;
    for (unsigned int i = 0; i < n; ++i)
    {
        v = free_vertices[i];

        // rhs row
        b = static_cast<dvec3>(points[v]) / vweight[v];

        // lhs row
        ww = 0.0;
        for (auto h : mesh_.halfedges(v))
        {
            vv = mesh_.to_vertex(h);
            e = mesh_.edge(h);
            ww += eweight[e];

            // fixed boundary vertex -> right hand side
            if (mesh_.is_boundary(vv))
            {
                b -= -timestep * eweight[e] * static_cast<dvec3>(points[vv]);
            }
            // free interior vertex -> matrix
            else
            {
                triplets.emplace_back(i, idx[vv], -timestep * eweight[e]);
            }

            B.row(i) = (Eigen::Vector3d)b;
        }

        // center vertex -> matrix
        triplets.emplace_back(i, i, 1.0 / vweight[v] + timestep * ww);
    }

    // build sparse matrix from triplets
    A.setFromTriplets(triplets.begin(), triplets.end());

    // solve A*X = B
    Eigen::SimplicialLDLT<SparseMatrix> solver(A);
    Eigen::MatrixXd X = solver.solve(B);
    if (solver.info() != Eigen::Success)
    {
        // clean-up
        mesh_.remove_vertex_property(idx);
        auto what = "SurfaceSmoothing: Failed to solve linear system.";
        throw SolverException(what);
    }
    else
    {
        // copy solution
        for (unsigned int i = 0; i < n; ++i)
        {
            points[free_vertices[i]] = X.row(i);
        }
    }

    if (rescale)
    {
        // restore original surface area
        Scalar area_after = surface_area(mesh_);
        Scalar scale = sqrt(area_before / area_after);
        for (auto v : mesh_.vertices())
            mesh_.position(v) *= scale;

        // restore original center
        Point center_after = centroid(mesh_);
        Point trans = center_before - center_after;
        for (auto v : mesh_.vertices())
            mesh_.position(v) += trans;
    }

    // clean-up
    mesh_.remove_vertex_property(idx);
}

void SurfaceSmoothing::laplace_optimized_smoothing(double smoothness) {
    size_t n = mesh_.n_vertices();
    Eigen::SparseMatrix<double> L(n, n);
    Eigen::SparseMatrix<double> M(n, n);
    Eigen::MatrixX3d B(n, 3);
    std::vector<Eigen::Triplet<double> > L_triplets, M_triplets;
    for(auto v: mesh_.vertices()) {
        double w = 0;
        double ww = 0;
        for(auto h: mesh_.halfedges(v)) {
            w = std::max(0.0, pmp::cotan_weight(mesh_, mesh_.edge(h)));
            ww  += w;
            L_triplets.push_back({v.idx(), mesh_.to_vertex(h).idx(), w});
        }
        B.row(v.idx()) = Eigen::Vector3d(mesh_.position(v));
        L_triplets.push_back({v.idx(), v.idx(), -ww});
        M_triplets.push_back({v.idx(), v.idx(), 1/pmp::voronoi_area(mesh_, v)});
    }

    L.setFromTriplets(L_triplets.begin(), L_triplets.end());
    M.setFromTriplets(M_triplets.begin(), M_triplets.end());

    Eigen::SparseMatrix<double> A = smoothness * L.transpose() * M * L + (1-smoothness)*M;
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver(A);
    Eigen::MatrixXd X = solver.solve( M * B);
    if (solver.info() != Eigen::Success){
        std::cerr << "SurfaceSmoothing: Could not solve linear system\n";
    } else {
        // copy solution
        for (unsigned int i = 0; i < n; ++i)
            mesh_.position(pmp::Vertex(i)) = X.row(i);
    }
}

void SurfaceSmoothing::bilateral_smoothing(unsigned int iters, bool rescale) {
    // store center and area
    Point center_before = centroid(mesh_);
    Scalar area_before = surface_area(mesh_);

    std::vector<pmp::Point> X(mesh_.n_vertices());
    std::vector<double> offsets;
    for(size_t i = 0; i < iters; i++) {
        for(auto v: mesh_.vertices()) {
            pmp::Normal n = pmp::SurfaceNormals::compute_vertex_normal(mesh_, v);
            double sigma_c = 0, sigma_s = 0;
            for(auto vv: mesh_.vertices(v))
                sigma_c += norm(mesh_.position(v) - mesh_.position(vv) );
            sigma_c /= mesh_.valence(v);

            offsets.clear();
            offsets.resize(mesh_.valence(v));
            for(auto vv: mesh_.vertices(v))
                offsets.push_back(std::abs(pmp::dot(n, mesh_.position(v) - mesh_.position(vv) )));
            double avg = std::accumulate(offsets.begin(), offsets.end(), 0.f)/offsets.size();
            for(auto d: offsets)
                sigma_s += std::pow(d - avg, 2);
            sigma_s = std::max(1e-12, std::sqrt(sigma_s/offsets.size()));

            double sum = 0, normalizer = 0, t, h, w_c, w_s;
            for(auto vv: mesh_.vertices(v)) {
                t = norm(mesh_.position(v) - mesh_.position(vv) );
                h = pmp::dot(n, mesh_.position(v) - mesh_.position(vv) );
                w_c = std::exp(-0.5*t*t/(sigma_c *sigma_c));
                w_s = std::exp(-0.5*h*h/(sigma_s *sigma_s));
                sum += w_c * w_s * h;
                normalizer += w_c * w_s;
            }
            X[v.idx()] = n * sum / normalizer;
        }
        for(auto v: mesh_.vertices())
            mesh_.position(v) -= X[v.idx()];
    }

    if (rescale) {
        // restore original surface area
        Scalar area_after = surface_area(mesh_);
        Scalar scale = sqrt(area_before / area_after);
        for (auto v : mesh_.vertices())
            mesh_.position(v) *= scale;

        // restore original center
        Point center_after = centroid(mesh_);
        Point trans = center_before - center_after;
        for (auto v : mesh_.vertices())
            mesh_.position(v) += trans;
    }
}


void SurfaceSmoothing::bilateral_normal_smoothing(double sigma_s,
                              double smoothness) {
    // bilateral normal smoothing in normal domain
    auto f_n = mesh_.face_property<pmp::Normal>("f:normal");
    pmp::SurfaceNormals::compute_face_normals(mesh_);
    auto f_area  = mesh_.face_property<pmp::Scalar>("f:area");
    auto f_centroid = mesh_.face_property<pmp::Normal>("f:centroid");
    Eigen::MatrixX3d B(mesh_.n_faces(), 3);
    for(auto f: mesh_.faces()) {
        f_area[f] = triangle_area(mesh_, f);
        B.row(f.idx()) = (Eigen::Vector3d)f_n[f];
        f_centroid[f] = centroid(mesh_, f);
    }

    double sigma_c = 0, sigma_c_sum = 0;
    for(auto fi: mesh_.faces()) {
        for(auto h: mesh_.halfedges(fi)) {
            auto fj = mesh_.face(mesh_.opposite_halfedge(h));
            if(!fj.is_valid()) continue;
            sigma_c += norm(f_centroid[fi] - f_centroid[fj]);
            sigma_c_sum ++;
        }
    }
    sigma_c /= sigma_c_sum;

    std::vector<Eigen::Triplet<double> > coeff_triple, weight_triple;
    for(auto fi: mesh_.faces()) {
        double w , ww = 0;
        for(auto h: mesh_.halfedges(fi)) {
            auto fj = mesh_.face(mesh_.opposite_halfedge(h));
            if(!fj.is_valid()) continue;
            double _s = norm(f_n[fi] - f_n[fj]);
            double _c = norm(f_centroid[fi] - f_centroid[fj]);
            double w_c = exp(-0.5*_c*_c/(sigma_c *sigma_c));
            double w_s = exp(-0.5*_s*_s/(sigma_s *sigma_s));
            w = f_area[fj] * w_c * w_s;
            coeff_triple.push_back({fi.idx(), fj.idx(), w});
            ww += w;
        }
        weight_triple.push_back({fi.idx(), fi.idx(), 1.0/ww});
    }

    Eigen::SparseMatrix<double> L(mesh_.n_faces(), mesh_.n_faces());
    Eigen::SparseMatrix<double> I(mesh_.n_faces(), mesh_.n_faces());
    Eigen::SparseMatrix<double> M_inv(mesh_.n_faces(), mesh_.n_faces());

    I.setIdentity();

    L.setFromTriplets(coeff_triple.begin(), coeff_triple.end());
    M_inv.setFromTriplets(weight_triple.begin(), weight_triple.end());
    L = I - M_inv * L;

    Eigen::SparseMatrix<double> left_term = (1 - smoothness) * L.transpose() * L + smoothness * I;
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver(left_term);
    Eigen::MatrixXd X = solver.solve( smoothness * B);
    if (solver.info() != Eigen::Success){
        std::cerr << "SurfaceSmoothing: Could not solve linear system\n";
    } else {
        X.rowwise().normalized();
        for (unsigned int i = 0; i < mesh_.n_faces(); ++i)
            f_n[pmp::Face(i)] = X.row(i);
    }


    // update vertex position from new face normals
    auto v_new = mesh_.vertex_property<pmp::Normal>("v:position_");
    for(size_t iter = 0; iter < 20; iter++) {
        for(auto v: mesh_.vertices()) {
            v_new[v] *= 0;
            for(auto f: mesh_.faces(v))
                v_new[v] += f_n[f] * pmp::dot(f_n[f], f_centroid[f]-mesh_.position(v));
            v_new[v] = mesh_.position(v) + v_new[v]/mesh_.valence(v);
        }

        for(auto v: mesh_.vertices())
            mesh_.position(v) = v_new[v];
        for(auto f: mesh_.faces())
            f_centroid[f] = centroid(mesh_, f);
    }

    mesh_.remove_face_property(f_area);
    mesh_.remove_face_property(f_centroid);
    mesh_.remove_vertex_property(v_new);
}

} // namespace pmp
