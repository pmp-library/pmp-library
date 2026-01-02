// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>

#include "pmp/surface_mesh.h"

namespace pmp {

void read_stl(SurfaceMesh& mesh, const std::filesystem::path& file);

} // namespace pmp