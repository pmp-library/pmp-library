// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <filesystem>

#include "pmp/surface_mesh.h"

namespace pmp {

void read_pmp(SurfaceMesh& mesh, const std::filesystem::path& file);

} // namespace pmp