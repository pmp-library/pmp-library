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

// grid-based spatial hashing for duplicate vertex detection
// see Teschner et al., "Optimized Spatial Hashing for Collision Detection of Deformable Objects", VMV, 2003.
using Cell = std::tuple<int, int, int>;

struct CellHash
{
    size_t operator()(const Cell& c) const
    {
        const auto& [x, y, z] = c;
        size_t hx = std::hash<int>{}(x);
        size_t hy = std::hash<int>{}(y);
        size_t hz = std::hash<int>{}(z);
        return hx ^ (hy << 1) ^ (hz << 2);
    }
};

Cell cell_of(const Point& p, Scalar eps)
{
    return {int(std::floor(p[0] / eps)), //
            int(std::floor(p[1] / eps)), //
            int(std::floor(p[2] / eps))};
};

int count_duplicate_vertices(const SurfaceMesh& mesh, Scalar eps)
{
    std::unordered_map<Cell, std::vector<Vertex>, CellHash> grid;
    std::vector<bool> duplicate(mesh.n_vertices(), false);

    for (auto v : mesh.vertices())
    {
        const Point& p = mesh.position(v);

        const Cell c = cell_of(p, eps);

        for (int dx = -1; dx <= 1; ++dx)
            for (int dy = -1; dy <= 1; ++dy)
                for (int dz = -1; dz <= 1; ++dz)
                {
                    const auto& [x, y, z] = c;
                    Cell neighbor{x + dx, y + dy, z + dz};

                    auto it = grid.find(neighbor);

                    if (it == grid.end())
                        continue;

                    for (auto vv : it->second)
                        if (sqrnorm(mesh.position(vv) - p) < eps * eps)
                        {
                            duplicate[vv.idx()] = true;
                            duplicate[v.idx()] = true;
                        }
                }

        grid[c].push_back(v);
    }
    return std::count(duplicate.begin(), duplicate.end(), true);
}

} // namespace

AnalysisReport analyze(const SurfaceMesh& mesh)
{
    AnalysisReport report;

    // basic stats
    report.n_vertices = mesh.n_vertices();
    report.n_edges = mesh.n_edges();
    report.n_faces = mesh.n_faces();

    // mesh type
    report.is_triangle_mesh = mesh.is_triangle_mesh();
    report.is_quad_mesh = mesh.is_quad_mesh();

    // manifoldness and boundary
    for (auto v : mesh.vertices())
    {
        if (!mesh.is_manifold(v))
            report.is_manifold = false;
        if (mesh.is_boundary(v))
            report.has_boundary = true;
        if (mesh.is_isolated(v))
            ++report.n_isolated_vertices;
    }

    // eps according to bounding box diagonal
    constexpr Scalar relative_eps = detail::relative_epsilon<Scalar>();
    constexpr Scalar absolute_eps = detail::absolute_epsilon<Scalar>();
    const Scalar diagonal = bounds(mesh).size();
    const Scalar eps = std::max(relative_eps * diagonal, absolute_eps);

    for (auto f : mesh.faces())
        if (is_degenerate(mesh, f, eps))
            ++report.n_degenerate_faces;

    report.n_components = count_connected_components(mesh);

    report.n_duplicate_vertices = count_duplicate_vertices(mesh, eps);

    return report;
}

std::ostream& operator<<(std::ostream& os, const AnalysisReport& report)
{
    os << "Analysis Report:\n";
    os << "  vertices: " << report.n_vertices << "\n";
    os << "  edges: " << report.n_edges << "\n";
    os << "  faces: " << report.n_faces << "\n";
    os << "  boundary: " << (report.has_boundary ? "yes" : "no") << "\n";
    os << "  manifold: " << (report.is_manifold ? "yes" : "no") << "\n";
    os << "  triangle mesh: " << (report.is_triangle_mesh ? "yes" : "no")
       << "\n";
    os << "  quad mesh: " << (report.is_quad_mesh ? "yes" : "no") << "\n";
    os << "  components: " << report.n_components << "\n";
    os << "  isolated vertices: " << report.n_isolated_vertices << "\n";
    os << "  degenerate faces: " << report.n_degenerate_faces << "\n";
    os << "  duplicate vertices: " << report.n_duplicate_vertices << "\n";
    return os;
}

} // namespace pmp
