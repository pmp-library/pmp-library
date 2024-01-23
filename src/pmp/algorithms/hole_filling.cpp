// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/hole_filling.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <limits>
#include <vector>

#include "pmp/algorithms/fairing.h"
#include "pmp/algorithms/normals.h"

namespace pmp {
namespace {

class HoleFilling
{
public:
    explicit HoleFilling(SurfaceMesh& mesh);
    void fill_hole(Halfedge h);

private:
    struct Weight
    {
        Weight(Scalar angle = std::numeric_limits<Scalar>::max(),
               Scalar area = std::numeric_limits<Scalar>::max())
            : angle_(angle), area_(area)
        {
        }

        Weight operator+(const Weight& rhs) const
        {
            return Weight(std::max(angle_, rhs.angle_), area_ + rhs.area_);
        }

        bool operator<(const Weight& rhs) const
        {
            return (angle_ < rhs.angle_ ||
                    (angle_ == rhs.angle_ && area_ < rhs.area_));
        }

        Scalar angle_;
        Scalar area_;
    };

    // compute optimal triangulation of hole
    // throws InvalidInputException in case of a non-manifold hole.
    void triangulate_hole(Halfedge h);

    // compute the weight of the triangle (i,j,k).
    Weight compute_weight(int i, int j, int k) const;

    // refine triangulation (isotropic remeshing)
    void refine();
    void split_long_edges(const Scalar lmax);
    void collapse_short_edges(const Scalar lmin);
    void flip_edges();
    void relaxation();
    void fairing();

    // return i'th vertex of hole
    Vertex hole_vertex(unsigned int i) const
    {
        assert(i < hole_.size());
        return mesh_.to_vertex(hole_[i]);
    }

    // return normal of face opposite to edge (i-1,i)
    Normal opposite_normal(unsigned int i) const
    {
        assert(i < hole_.size());
        return face_normal(mesh_,
                           mesh_.face(mesh_.opposite_halfedge(hole_[i])));
    }

    // does interior edge (_a,_b) exist already?
    bool is_interior_edge(Vertex a, Vertex b) const;

    // triangle area
    Scalar compute_area(Vertex a, Vertex b, Vertex c) const;

    // triangle normal
    Point compute_normal(Vertex a, Vertex b, Vertex c) const;

    // dihedral angle
    Scalar compute_angle(const Point& n1, const Point& n2) const;

    // mesh and properties
    SurfaceMesh& mesh_;
    VertexProperty<Point> points_;
    VertexProperty<bool> vlocked_;
    EdgeProperty<bool> elocked_;

    std::vector<Halfedge> hole_;

    // data for computing optimal triangulation
    std::vector<std::vector<Weight>> weight_;
    std::vector<std::vector<int>> index_;
};

HoleFilling::HoleFilling(SurfaceMesh& mesh) : mesh_(mesh)
{
    points_ = mesh_.vertex_property<Point>("v:point");
}

bool HoleFilling::is_interior_edge(Vertex a, Vertex b) const
{
    Halfedge h = mesh_.find_halfedge(a, b);
    if (!h.is_valid())
        return false; // edge does not exist
    return (!mesh_.is_boundary(h) &&
            !mesh_.is_boundary(mesh_.opposite_halfedge(h)));
}

Scalar HoleFilling::compute_area(Vertex a, Vertex b, Vertex c) const
{
    return sqrnorm(cross(points_[b] - points_[a], points_[c] - points_[a]));
}

Point HoleFilling::compute_normal(Vertex a, Vertex b, Vertex c) const
{
    return normalize(cross(points_[b] - points_[a], points_[c] - points_[a]));
}

Scalar HoleFilling::compute_angle(const Point& n1, const Point& n2) const
{
    return (1.0 - dot(n1, n2));
}

void HoleFilling::fill_hole(Halfedge h)
{
    if (!h.is_valid())
    {
        throw InvalidInputException("HoleFilling: Invalid halfedge.");
    }

    // is it really a hole?
    if (!mesh_.is_boundary(h))
    {
        auto what = std::string{__func__} + ": Not a boundary halfedge.";
        throw InvalidInputException(what);
    }

    // lock vertices/edge that already exist, to be later able to
    // identify the filled-in vertices/edges
    vlocked_ = mesh_.add_vertex_property<bool>("HoleFilling:vlocked", false);
    elocked_ = mesh_.add_edge_property<bool>("HoleFilling:elocked", false);
    for (auto v : mesh_.vertices())
        vlocked_[v] = true;
    for (auto e : mesh_.edges())
        elocked_[e] = true;

    try
    {
        triangulate_hole(h); // do minimal triangulation
        refine();            // refine filled-in edges
    }
    catch (InvalidInputException& e)
    {
        // clean up
        hole_.clear();
        mesh_.remove_vertex_property(vlocked_);
        mesh_.remove_edge_property(elocked_);

        throw e;
    }

    // clean up
    hole_.clear();
    mesh_.remove_vertex_property(vlocked_);
    mesh_.remove_edge_property(elocked_);
}

void HoleFilling::triangulate_hole(Halfedge h)
{
    // trace hole
    hole_.clear();
    Halfedge hit = h;
    do
    {
        // check for manifoldness
        if (!mesh_.is_manifold(mesh_.to_vertex(hit)))
        {
            auto what = std::string{__func__} + ": Non-manifold hole.";
            throw InvalidInputException(what);
        }

        hole_.emplace_back(hit);
    } while ((hit = mesh_.next_halfedge(hit)) != h);
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

HoleFilling::Weight HoleFilling::compute_weight(int i, int j, int k) const
{
    const Vertex a = hole_vertex(i);
    const Vertex b = hole_vertex(j);
    const Vertex c = hole_vertex(k);
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
    Point n2;

    // ...neighbor to (i,j)
    n2 = (i + 1 == j) ? opposite_normal(j)
                      : compute_normal(a, hole_vertex(index_[i][j]), b);
    angle = std::max(angle, compute_angle(n, n2));

    // ...neighbor to (j,k)
    n2 = (j + 1 == k) ? opposite_normal(k)
                      : compute_normal(b, hole_vertex(index_[j][k]), c);
    angle = std::max(angle, compute_angle(n, n2));

    // ...neighbor to (k,i) if (k,i)==(n-1, 0)
    if (i == 0 && k + 1 == (int)hole_.size())
    {
        n2 = opposite_normal(0);
        angle = std::max(angle, compute_angle(n, n2));
    }

    return {angle, area};
}

void HoleFilling::refine()
{
    const int n = hole_.size();
    Scalar l, lmin, lmax;

    // compute target edge length
    l = 0.0;
    for (int i = 0; i < n; ++i)
    {
        l += distance(points_[hole_vertex(i)],
                      points_[hole_vertex((i + 1) % n)]);
    }
    l /= (Scalar)n;
    lmin = 0.7 * l;
    lmax = 1.5 * l;

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

void HoleFilling::split_long_edges(const Scalar lmax)
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
                Halfedge h10 = mesh_.halfedge(e, 0);
                Halfedge h01 = mesh_.halfedge(e, 1);
                const Point& p0 = points_[mesh_.to_vertex(h10)];
                const Point& p1 = points_[mesh_.to_vertex(h01)];

                if (distance(p0, p1) > lmax)
                {
                    mesh_.split(e, 0.5 * (p0 + p1));
                    ok = false;
                }
            }
        }
    }
}

void HoleFilling::collapse_short_edges(const Scalar _lmin)
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
                Halfedge h10 = mesh_.halfedge(e, 0);
                Halfedge h01 = mesh_.halfedge(e, 1);
                Vertex v0 = mesh_.to_vertex(h10);
                Vertex v1 = mesh_.to_vertex(h01);
                const Point& p0 = points_[v0];
                const Point& p1 = points_[v1];

                // edge too short?
                if (distance(p0, p1) < _lmin)
                {
                    Halfedge h;
                    if (!vlocked_[v0])
                        h = h01;
                    else if (!vlocked_[v1])
                        h = h10;

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

void HoleFilling::flip_edges()
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

void HoleFilling::relaxation()
{
    // properties
    VertexProperty<int> idx =
        mesh_.add_vertex_property<int>("HoleFilling:idx", -1);

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
    using Triplet = Eigen::Triplet<double>;
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
    using SparseMatrix = Eigen::SparseMatrix<double>;
    SparseMatrix A(n, n);
    A.setFromTriplets(triplets.begin(), triplets.end());
    Eigen::SimplicialLDLT<SparseMatrix> solver(A);
    Eigen::MatrixXd X = solver.solve(B);

    if (solver.info() != Eigen::Success)
    {
        // clean up
        mesh_.remove_vertex_property(idx);
        auto what = std::string{__func__} + ": Failed to solve linear system.";
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

void HoleFilling::fairing()
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
        minimize_curvature(mesh_);
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
} // namespace

void fill_hole(SurfaceMesh& mesh, Halfedge h)
{
    HoleFilling(mesh).fill_hole(h);
}

} // namespace pmp
