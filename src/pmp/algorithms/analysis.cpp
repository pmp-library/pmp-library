// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "pmp/algorithms/analysis.h"

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
    report.is_manifold = true;
    report.has_boundary = false;
    for (auto v : mesh.vertices())
    {
        if (!mesh.is_manifold(v))
            report.is_manifold = false;
        if (mesh.is_boundary(v))
            report.has_boundary = true;
    }

    report.n_components = count_connected_components(mesh);

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
    return os;
}

} // namespace pmp
