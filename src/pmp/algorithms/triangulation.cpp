// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/triangulation.h"

#include <limits>
#include <vector>

namespace pmp {
namespace {

class Triangulation
{
public:
    explicit Triangulation(SurfaceMesh& mesh);

    void triangulate(Face f);

private:
    // Compute the weight of the triangle (i,j,k).
    Scalar compute_weight(int i, int j, int k) const;

    // Does edge (a,b) exist?
    bool is_edge(Vertex a, Vertex b) const;

    // Add edge from vertex i to j.
    bool insert_edge(int i, int j);

    // mesh and properties
    SurfaceMesh& mesh_;
    VertexProperty<Point> points_;
    std::vector<Halfedge> halfedges_;
    std::vector<Vertex> vertices_;

    // data for computing optimal triangulation
    std::vector<std::vector<Scalar>> weight_;
    std::vector<std::vector<int>> index_;
};

Triangulation::Triangulation(SurfaceMesh& mesh) : mesh_(mesh)
{
    points_ = mesh_.vertex_property<Point>("v:point");
}

void Triangulation::triangulate(Face f)
{
    // collect polygon halfedges
    const Halfedge h0 = mesh_.halfedge(f);
    halfedges_.clear();
    vertices_.clear();
    Halfedge h = h0;
    do
    {
        if (!mesh_.is_manifold(mesh_.to_vertex(h)))
        {
            auto what = std::string{__func__} + ": Non-manifold polygon";
            throw InvalidInputException(what);
        }

        halfedges_.emplace_back(h);
        vertices_.emplace_back(mesh_.to_vertex(h));
    } while ((h = mesh_.next_halfedge(h)) != h0);

    // do we have at least four vertices?
    const auto n = halfedges_.size();
    if (n <= 3)
        return;

    // compute minimal triangulation by dynamic programming
    weight_.clear();
    weight_.resize(n,
                   std::vector<Scalar>(n, std::numeric_limits<Scalar>::max()));
    index_.clear();
    index_.resize(n, std::vector<int>(n, 0));

    // initialize 2-gons
    for (size_t i = 0; i < n - 1; ++i)
    {
        weight_[i][i + 1] = 0.0;
        index_[i][i + 1] = -1;
    }

    // n-gons with n>2
    for (size_t j = 2; j < n; ++j)
    {
        // for all n-gons [i,i+j]
        for (size_t i = 0; i < n - j; ++i)
        {
            auto k = i + j;
            auto wmin = std::numeric_limits<Scalar>::max();
            auto imin = -1;

            // find best split i < m < i+j
            for (size_t m = i + 1; m < k; ++m)
            {
                const Scalar w =
                    weight_[i][m] + compute_weight(i, m, k) + weight_[m][k];

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
        const int start = tri[0];
        const int end = tri[1];
        if (end - start < 2)
            continue;
        const int split = index_[start][end];

        insert_edge(start, split);
        insert_edge(split, end);

        todo.emplace_back(start, split);
        todo.emplace_back(split, end);
    }

    // clean up
    weight_.clear();
    index_.clear();
    halfedges_.clear();
    vertices_.clear();
}

Scalar Triangulation::compute_weight(int i, int j, int k) const
{
    const Vertex a = vertices_[i];
    const Vertex b = vertices_[j];
    const Vertex c = vertices_[k];

    // If one of the potential edges already exists this would result in an
    // invalid triangulation. This happens for suzanne.obj. Prevent this by
    // giving infinite weight.
    if (is_edge(a, b) && is_edge(b, c) && is_edge(c, a))
        return std::numeric_limits<Scalar>::max();

    const Point& pa = points_[a];
    const Point& pb = points_[b];
    const Point& pc = points_[c];

    return sqrnorm(cross(pb - pa, pc - pa));
}

bool Triangulation::is_edge(Vertex a, Vertex b) const
{
    return mesh_.find_halfedge(a, b).is_valid();
}

bool Triangulation::insert_edge(int i, int j)
{
    const Halfedge h0 = halfedges_[i];
    const Halfedge h1 = halfedges_[j];
    const Vertex v0 = vertices_[i];
    const Vertex v1 = vertices_[j];

    // does edge already exist?
    if (mesh_.find_halfedge(v0, v1).is_valid())
    {
        return false;
    }

    // can we reach v1 from h0?
    {
        Halfedge h = h0;
        do
        {
            h = mesh_.next_halfedge(h);
            if (mesh_.to_vertex(h) == v1)
            {
                mesh_.insert_edge(h0, h);
                return true;
            }
        } while (h != h0);
    }

    // can we reach v0 from h1?
    {
        Halfedge h = h1;
        do
        {
            h = mesh_.next_halfedge(h);
            if (mesh_.to_vertex(h) == v0)
            {
                mesh_.insert_edge(h1, h);
                return true;
            }
        } while (h != h1);
    }

    return false;
}
} // namespace

void triangulate(SurfaceMesh& mesh)
{
    Triangulation tr(mesh);
    for (auto f : mesh.faces())
        tr.triangulate(f);
}

void triangulate(SurfaceMesh& mesh, Face f)
{
    Triangulation(mesh).triangulate(f);
}

} // namespace pmp
