// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "pmp/algorithms/analysis.h"

namespace pmp {

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
    return os;
}

} // namespace pmp
