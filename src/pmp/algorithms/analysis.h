// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include "pmp/surface_mesh.h"
#include <iostream>

namespace pmp {

//! \addtogroup algorithms
//! @{

//! Report with basic mesh information.
struct AnalysisReport
{
    int n_vertices;
    int n_edges;
    int n_faces;
    bool has_boundary;
    bool is_manifold;
    bool is_triangle_mesh;
    bool is_quad_mesh;
    int n_components;
};

//! Output operator for AnalysisReport.
std::ostream& operator<<(std::ostream& os, const AnalysisReport& report);

//! Analyze mesh and return report with basic stats.
AnalysisReport analyze(const SurfaceMesh& mesh);

//! @}

} // namespace pmp
