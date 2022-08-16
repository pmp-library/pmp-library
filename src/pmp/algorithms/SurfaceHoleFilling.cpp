// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/SurfaceHoleFilling.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "pmp/algorithms/SurfaceFairing.h"

#include <unordered_map>

using SparseMatrix = Eigen::SparseMatrix<double>;
using Triplet = Eigen::Triplet<double>;

namespace pmp {

HolePatchMesh::HolePatchMesh(const pmp::SurfaceMesh& origin_mesh, size_t n_prev_faces) {
    for(size_t i = n_prev_faces; i < origin_mesh.n_faces(); i++) {
        std::vector<pmp::Vertex> face_vertices;
        for(auto v: origin_mesh.vertices(pmp::Face(i))) {
            if(new_v_index.find(v.idx()) == new_v_index.end()) {
                new_v_index[v.idx()] = pmp::Vertex(new_v_index.size());
                patch2origin[mesh_.n_vertices()] = v;
                mesh_.add_vertex(origin_mesh.position(v));
            }
            face_vertices.push_back(new_v_index[v.idx()]);
        }
        mesh_.add_face(face_vertices);
    }
}

HolePatchMesh::~HolePatchMesh() {
    if(mesh_.n_vertices() == 0) return;
    mesh_.remove_vertex_property(vlocked_);
    mesh_.remove_edge_property(elocked_);
}

SurfaceHoleFilling::SurfaceHoleFilling(SurfaceMesh& _mesh) : mesh_(_mesh)
{
    points_ = mesh_.vertex_property<Point>("v:point");
}

bool SurfaceHoleFilling::is_interior_edge(Vertex _a, Vertex _b) const
{
    Halfedge h = mesh_.find_halfedge(_a, _b);
    if (!h.is_valid())
        return false; // edge does not exist
    return (!mesh_.is_boundary(h) &&
            !mesh_.is_boundary(mesh_.opposite_halfedge(h)));
}

Scalar SurfaceHoleFilling::compute_area(Vertex _a, Vertex _b, Vertex _c) const
{
    return sqrnorm(cross(points_[_b] - points_[_a], points_[_c] - points_[_a]));
}

Point SurfaceHoleFilling::compute_normal(Vertex _a, Vertex _b, Vertex _c) const
{
    return normalize(
        cross(points_[_b] - points_[_a], points_[_c] - points_[_a]));
}

Scalar SurfaceHoleFilling::compute_angle(const Point& _n1,
                                         const Point& _n2) const
{
    return (1.0 - dot(_n1, _n2));
}

void SurfaceHoleFilling::fill_hole(Halfedge h)
{
    if (!h.is_valid())
    {
        throw InvalidInputException("SurfaceHoleFilling: Invalid halfedge.");
    }

    // is it really a hole?
    if (!mesh_.is_boundary(h))
    {
        auto what = "SurfaceHoleFilling: Not a boundary halfedge.";
        throw InvalidInputException(what);
    }

    // lock vertices/edge that already exist, to be later able to
    // identify the filled-in vertices/edges

    try
    {
        size_t n_pre_faces = mesh_.n_faces();
        triangulate_hole(h); // do minimal triangulation
        filled_patch_ = std::move(HolePatchMesh(mesh_, n_pre_faces));
        filled_patch_.refine();            // refine filled-in edges
        for(auto f: mesh_.faces())
            if(f.idx() >= n_pre_faces) mesh_.delete_face(f);
        for(auto f: filled_patch_.mesh_.faces()) {
            std::vector<pmp::Vertex> new_face;
            for(auto v: filled_patch_.mesh_.vertices(f)) {
                if(filled_patch_.patch2origin.find(v.idx()) == filled_patch_.patch2origin.end()) {
                    auto new_v = mesh_.add_vertex(filled_patch_.mesh_.position(v));
                    filled_patch_.patch2origin[v.idx()] = new_v;
                }
                new_face.push_back(filled_patch_.patch2origin[v.idx()]);
            }
            mesh_.add_face(new_face);
        }
        mesh_.garbage_collection();
    }
    catch (InvalidInputException& e)
    {
        // clean up
        hole_.clear();
        throw e;
    }

    // clean up
    hole_.clear();
}

void SurfaceHoleFilling::triangulate_hole(Halfedge _h)
{
    // trace hole
    hole_.clear();
    Halfedge h = _h;
    do
    {
        // check for manifoldness
        if (!mesh_.is_manifold(mesh_.to_vertex(h)))
        {
            auto what = "SurfaceHoleFilling: Non-manifold hole.";
            throw InvalidInputException(what);
        }

        hole_.emplace_back(h);
    } while ((h = mesh_.next_halfedge(h)) != _h);
    const int n = hole_.size();

    // compute minimal triangulation by dynamic programming
    weight_.clear();
    weight_.resize(n, std::vector<Weight>(n, Weight()));
    index_.clear();
    index_.resize(n, std::vector<int>(n, 0));

    int i, j, m, k, imin;
    Weight w, wmin;

    // initialize 2-gons
    for (i = 0; i < n - 1; ++i)
    {
        weight_[i][i + 1] = Weight(0, 0);
        index_[i][i + 1] = -1;
    }

    // n-gons with n>2
    for (j = 2; j < n; ++j)
    {
        // for all n-gons [i,i+j]
        for (i = 0; i < n - j; ++i)
        {
            k = i + j;
            wmin = Weight();
            imin = -1;

            // find best split i < m < i+j
            for (m = i + 1; m < k; ++m)
            {
                w = weight_[i][m] + compute_weight(i, m, k) + weight_[m][k];
                if (w < wmin)
                {
                    wmin = w;
                    imin = m;
                }
            }

            weight_[i][k] = wmin;
            index_[i][k] = imin;
        }
    }

    // now add triangles to mesh
    std::vector<ivec2> todo;
    todo.reserve(n);
    todo.emplace_back(0, n - 1);
    while (!todo.empty())
    {
        ivec2 tri = todo.back();
        todo.pop_back();
        int start = tri[0];
        int end = tri[1];
        if (end - start < 2)
            continue;
        int split = index_[start][end];

        mesh_.add_triangle(hole_vertex(start), hole_vertex(split),
                           hole_vertex(end));

        todo.emplace_back(start, split);
        todo.emplace_back(split, end);
    }

    // clean up
    weight_.clear();
    index_.clear();
}

SurfaceHoleFilling::Weight SurfaceHoleFilling::compute_weight(int _i, int _j,
                                                              int _k) const
{
    const Vertex a = hole_vertex(_i);
    const Vertex b = hole_vertex(_j);
    const Vertex c = hole_vertex(_k);
    Vertex d;

    // if one of the potential edges already exists, this would result
    // in an invalid triangulation -> prevent by giving infinite weight
    if (is_interior_edge(a, b) || is_interior_edge(b, c) ||
        is_interior_edge(c, a))
    {
        return {};
    }

    // compute area
    const Scalar area = compute_area(a, b, c);

    // compute dihedral angles with...
    Scalar angle(0);
    const Point n = compute_normal(a, b, c);

    // ...neighbor to (i,j)
    d = (_i + 1 == _j) ? opposite_vertex(_j) : hole_vertex(index_[_i][_j]);
    angle = std::max(angle, compute_angle(n, compute_normal(a, d, b)));

    // ...neighbor to (j,k)
    d = (_j + 1 == _k) ? opposite_vertex(_k) : hole_vertex(index_[_j][_k]);
    angle = std::max(angle, compute_angle(n, compute_normal(b, d, c)));

    // ...neighbor to (k,i) if (k,i)==(n-1, 0)
    if (_i == 0 && _k + 1 == (int)hole_.size())
    {
        d = opposite_vertex(0);
        angle = std::max(angle, compute_angle(n, compute_normal(c, d, a)));
    }

    return {angle, area};
}

void HolePatchMesh::refine()
{
    Scalar l, lmin, lmax, n_bnd_edges;

    // compute target edge length
    l = 0.0, n_bnd_edges = 0;
    for (auto e: mesh_.edges())
    {
        if(mesh_.is_boundary(e)) {
            n_bnd_edges++;
            l += mesh_.edge_length(e);
        }
    }
    l /= (Scalar)n_bnd_edges;
    lmin = 0.7 * l;
    lmax = 1.5 * l;
    points_ = mesh_.vertex_property<pmp::Point>("v:point");
    vlocked_ = mesh_.vertex_property<bool>("v:vlocked", false);
    elocked_ = mesh_.edge_property<bool>("e:elocked", false);
    for (auto v : mesh_.vertices()) vlocked_[v] = true;
    for (auto e : mesh_.edges()) elocked_[e] = mesh_.is_boundary(e);
    // do some iterations
    for (int iter = 0; iter < 10; ++iter)
    {
        split_long_edges(lmax);
        collapse_short_edges(lmin);
        flip_edges();
        relaxation();
    }
    fairing();
}

void HolePatchMesh::split_long_edges(const Scalar _lmax)
{
    bool ok;
    int i;

    for (ok = false, i = 0; !ok && i < 10; ++i)
    {
        ok = true;

        for (auto e : mesh_.edges())
        {
            if (!elocked_[e])
            {
                if (mesh_.edge_length(e) > _lmax)
                {
                    mesh_.split(e, 0.5 * (mesh_.position(mesh_.vertex(e, 0)) + mesh_.position(mesh_.vertex(e, 1))));
                    ok = false;
                }
            }
        }
    }
}

void HolePatchMesh::collapse_short_edges(const Scalar _lmin)
{
    bool ok;
    int i;

    for (ok = false, i = 0; !ok && i < 10; ++i)
    {
        ok = true;

        for (auto e : mesh_.edges())
        {
            if (!mesh_.is_deleted(e) && !elocked_[e])
            {
                // edge too short?
                if (mesh_.edge_length(e) < _lmin)
                {
                    Halfedge h;
                    if (!vlocked_[mesh_.vertex(e, 0)])
                        h = mesh_.halfedge(e, 1);
                    else if (!vlocked_[mesh_.vertex(e, 1)])
                        h = mesh_.halfedge(e, 0);

                    if (h.is_valid() && mesh_.is_collapse_ok(h))
                    {
                        mesh_.collapse(h);
                        ok = false;
                    }
                }
            }
        }
    }

    mesh_.garbage_collection();
}

void HolePatchMesh::flip_edges()
{
    Vertex v0, v1, v2, v3;
    Halfedge h;
    int val0, val1, val2, val3;
    int val_opt0, val_opt1, val_opt2, val_opt3;
    int ve0, ve1, ve2, ve3, ve_before, ve_after;
    bool ok;
    int i;

    for (ok = false, i = 0; !ok && i < 10; ++i)
    {
        ok = true;

        for (auto e : mesh_.edges())
        {
            if (!elocked_[e])
            {
                h = mesh_.halfedge(e, 0);
                v0 = mesh_.to_vertex(h);
                v2 = mesh_.to_vertex(mesh_.next_halfedge(h));
                h = mesh_.halfedge(e, 1);
                v1 = mesh_.to_vertex(h);
                v3 = mesh_.to_vertex(mesh_.next_halfedge(h));

                val0 = mesh_.valence(v0);
                val1 = mesh_.valence(v1);
                val2 = mesh_.valence(v2);
                val3 = mesh_.valence(v3);

                val_opt0 = (mesh_.is_boundary(v0) ? 4 : 6);
                val_opt1 = (mesh_.is_boundary(v1) ? 4 : 6);
                val_opt2 = (mesh_.is_boundary(v2) ? 4 : 6);
                val_opt3 = (mesh_.is_boundary(v3) ? 4 : 6);

                ve0 = (val0 - val_opt0);
                ve1 = (val1 - val_opt1);
                ve2 = (val2 - val_opt2);
                ve3 = (val3 - val_opt3);

                ve_before = ve0 * ve0 + ve1 * ve1 + ve2 * ve2 + ve3 * ve3;

                --val0;
                --val1;
                ++val2;
                ++val3;

                ve0 = (val0 - val_opt0);
                ve1 = (val1 - val_opt1);
                ve2 = (val2 - val_opt2);
                ve3 = (val3 - val_opt3);

                ve_after = ve0 * ve0 + ve1 * ve1 + ve2 * ve2 + ve3 * ve3;

                if (ve_before > ve_after && mesh_.is_flip_ok(e))
                {
                    mesh_.flip(e);
                    ok = false;
                }
            }
        }
    }
}

void HolePatchMesh::relaxation()
{
    // properties
    VertexProperty<int> idx =
        mesh_.add_vertex_property<int>("SurfaceHoleFilling:idx", -1);

    // collect free vertices
    std::vector<Vertex> vertices;
    vertices.reserve(mesh_.n_vertices());
    for (auto v : mesh_.vertices())
    {
        if (!vlocked_[v])
        {
            idx[v] = vertices.size();
            vertices.emplace_back(v);
        }
    }
    const int n = vertices.size();

    // setup matrix & rhs
    Eigen::MatrixXd B(n, 3);
    std::vector<Triplet> triplets;
    for (int i = 0; i < n; ++i)
    {
        Vertex v = vertices[i];
        Point b(0, 0, 0);
        Scalar c(0);

        for (auto vv : mesh_.vertices(v))
        {
            if (vlocked_[vv])
                b += points_[vv];
            else
                triplets.emplace_back(i, idx[vv], -1.0);
            ++c;
        }

        if (vlocked_[v])
            b -= c * points_[v];
        else
            triplets.emplace_back(i, idx[v], c);

        B.row(i) = (Eigen::Vector3d)b;
    }

    // solve least squares system
    SparseMatrix A(n, n);
    A.setFromTriplets(triplets.begin(), triplets.end());
    Eigen::SimplicialLDLT<SparseMatrix> solver(A);
    Eigen::MatrixXd X = solver.solve(B);

    if (solver.info() != Eigen::Success)
    {
        // clean up
        mesh_.remove_vertex_property(idx);
        auto what = "SurfaceHoleFilling: Failed to solve linear system.";
        throw SolverException(what);
    }

    // copy solution to mesh vertices
    for (int i = 0; i < n; ++i)
    {
        points_[vertices[i]] = X.row(i);
    }

    // clean up
    mesh_.remove_vertex_property(idx);
}

void HolePatchMesh::fairing()
{
    // did the refinement insert new vertices?
    // if yes, then trigger fairing; otherwise don't.
    bool new_vertices = false;
    for (auto v : mesh_.vertices())
        if (!vlocked_[v])
            new_vertices = true;
    if (!new_vertices)
        return;

    // convert non-locked into selection
    auto vsel = mesh_.vertex_property<bool>("v:selected");
    for (auto v : mesh_.vertices())
        vsel[v] = !vlocked_[v];

    try
    {
        // fair new vertices
        SurfaceFairing fairing(mesh_);
        fairing.minimize_curvature();
    }
    catch (SolverException& e)
    {
        // clean up
        mesh_.remove_vertex_property(vsel);
        throw e;
    }

    // clean up
    mesh_.remove_vertex_property(vsel);
}

} // namespace pmp
