// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>

#include "pmp/io/io_flags.h"
#include "pmp/surface_mesh.h"

namespace pmp {

void write_off(const SurfaceMesh& mesh, const std::filesystem::path& file,
               const IOFlags& flags);

} // namespace pmp
