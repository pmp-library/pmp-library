// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "pmp/algorithms/analysis.h"
#include "pmp/algorithms/utilities.h"
#include "pmp/algorithms/differential_geometry.h"
#include "pmp/tolerances.h"

#include <queue>

namespace pmp {

namespace {

// const version of connected_components, count only
int count_connected_components(const SurfaceMesh& mesh)
{
    std::vector<bool> visited(mesh.n_vertices(), false);

    int count = 0;

    for (auto v : mesh.vertices())
    {
        if (visited[v.idx()])
            continue;

        std::queue<Vertex> queue;
        queue.push(v);
        visited[v.idx()] = true;

        while (!queue.empty())
        {
            auto vv = queue.front();
            queue.pop();

            for (auto vc : mesh.vertices(vv))
            {
                if (!visited[vc.idx()])
                {
                    visited[vc.idx()] = true;
                    queue.push(vc);
                }
            }
        }

        ++count;
    }

    return count;
}

bool is_degenerate(const SurfaceMesh& mesh, Face f, Scalar eps)
{
    bool degenerate = false;

    // check for zero area
    if (face_area(mesh, f) < eps * eps)
        degenerate = true;

    // check for short edges
    for (auto h : mesh.halfedges(f))
        if (edge_length(mesh, mesh.edge(h)) < eps)
            degenerate = true;

    return degenerate;
}

} // namespace

std::vector<std::pair<Vertex, Vertex>> find_duplicate_vertices(
    const SurfaceMesh& mesh, Scalar eps)
{
    const size_t n_vertices = mesh.n_vertices();
    if (n_vertices == 0)
        return {};

    using Cell = std::tuple<int, int, int>;

    const auto hasher = [](const Cell& c) {
        const auto& [x, y, z] = c;
        return ((x * 73856093) ^ (y * 19349663) ^ (z * 83492791));
    };

    const auto cell_of = [&](const Point& p) -> Cell {
        return {int(std::floor(p[0] / eps)), //
                int(std::floor(p[1] / eps)), //
                int(std::floor(p[2] / eps))};
    };

    std::vector<std::pair<Vertex, Vertex>> duplicates;

    // Flat hash table with chaining for collision resolution.
    const size_t table_capacity = n_vertices * 2;
    std::vector<int> hash_table(table_capacity, -1);
    std::vector<int> next_node(n_vertices, -1);

    // Precompute keys for cache locality during lookup.
    std::vector<Cell> vertex_cells(n_vertices);
    for (auto v : mesh.vertices())
    {
        vertex_cells[v.idx()] = cell_of(mesh.position(v));
    }

    const Scalar sqr_eps = eps * eps;

    for (auto v : mesh.vertices())
    {
        int v_idx = v.idx();
        const Point& p = mesh.position(v);
        const Cell& c = vertex_cells[v_idx];

        // Search 27 cell neighborhood.
        for (int dx = -1; dx <= 1; ++dx)
            for (int dy = -1; dy <= 1; ++dy)
                for (int dz = -1; dz <= 1; ++dz)
                {
                    const auto& [x, y, z] = c;
                    Cell neighbor{x + dx, y + dy, z + dz};

                    size_t bucket = hasher(neighbor) % table_capacity;
                    int current_entry = hash_table[bucket];

                    while (current_entry != -1)
                    {
                        // Ensure the stored entry actually belongs to the cell we are searching
                        if (vertex_cells[current_entry] == neighbor)
                        {
                            if (sqrnorm(mesh.position(Vertex(current_entry)) -
                                        p) < sqr_eps)
                            {
                                duplicates.emplace_back(Vertex(current_entry),
                                                        v);
                            }
                        }
                        current_entry = next_node[current_entry];
                    }
                }

        // Insert vertex into hash table.
        size_t current_bucket = hasher(c) % table_capacity;
        next_node[v_idx] = hash_table[current_bucket];
        hash_table[current_bucket] = v_idx;
    }
    return duplicates;
}

MeshAnalysis analyze(const SurfaceMesh& mesh)
{
    MeshAnalysis analysis;

    // basic stats
    analysis.n_vertices = mesh.n_vertices();
    analysis.n_edges = mesh.n_edges();
    analysis.n_faces = mesh.n_faces();

    // mesh type
    analysis.is_triangle_mesh = mesh.is_triangle_mesh();
    analysis.is_quad_mesh = mesh.is_quad_mesh();

    // manifoldness and boundary
    for (auto v : mesh.vertices())
    {
        if (!mesh.is_manifold(v))
            analysis.is_manifold = false;
        if (mesh.is_boundary(v))
            analysis.has_boundary = true;
        if (mesh.is_isolated(v))
            ++analysis.n_isolated_vertices;
    }

    // eps according to bounding box diagonal
    constexpr Scalar relative_eps = detail::relative_epsilon<Scalar>();
    constexpr Scalar absolute_eps = detail::absolute_epsilon<Scalar>();
    const Scalar diagonal = bounds(mesh).size();
    const Scalar eps = std::max(relative_eps * diagonal, absolute_eps);

    for (auto f : mesh.faces())
        if (is_degenerate(mesh, f, eps))
            ++analysis.n_degenerate_faces;

    analysis.n_components = count_connected_components(mesh);

    const auto duplicate_pairs = find_duplicate_vertices(mesh, eps);
    std::vector<bool> duplicates(mesh.n_vertices(), false);
    for (const auto& [v0, v1] : duplicate_pairs)
    {
        duplicates[v0.idx()] = true;
        duplicates[v1.idx()] = true;
    }
    analysis.n_duplicate_vertices =
        std::count(duplicates.begin(), duplicates.end(), true);

    return analysis;
}

std::ostream& operator<<(std::ostream& os, const MeshAnalysis& analysis)
{
    os << "Mesh analysis:\n";
    os << "  vertices: " << analysis.n_vertices << "\n";
    os << "  edges: " << analysis.n_edges << "\n";
    os << "  faces: " << analysis.n_faces << "\n";
    os << "  boundary: " << (analysis.has_boundary ? "yes" : "no") << "\n";
    os << "  manifold: " << (analysis.is_manifold ? "yes" : "no") << "\n";
    os << "  triangle mesh: " << (analysis.is_triangle_mesh ? "yes" : "no")
       << "\n";
    os << "  quad mesh: " << (analysis.is_quad_mesh ? "yes" : "no") << "\n";
    os << "  components: " << analysis.n_components << "\n";
    os << "  isolated vertices: " << analysis.n_isolated_vertices << "\n";
    os << "  degenerate faces: " << analysis.n_degenerate_faces << "\n";
    os << "  duplicate vertices: " << analysis.n_duplicate_vertices << "\n";
    return os;
}

} // namespace pmp
