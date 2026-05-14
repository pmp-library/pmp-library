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
    int n_vertices = 0;
    int n_edges = 0;
    int n_faces = 0;
    bool has_boundary = false;
    bool is_manifold = true;
    bool is_triangle_mesh = false;
    bool is_quad_mesh = false;
    int n_components = 0;
    int n_isolated_vertices = 0;
    int n_degenerate_faces = 0;
    int n_duplicate_vertices = 0;
};

//! Output operator for AnalysisReport.
std::ostream& operator<<(std::ostream& os, const AnalysisReport& report);

//! Analyze mesh and return report with basic stats.
AnalysisReport analyze(const SurfaceMesh& mesh);

//! @}

} // namespace pmp
