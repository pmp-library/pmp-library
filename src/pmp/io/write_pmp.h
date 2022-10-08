// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <filesystem>

#include "pmp/io/IOFlags.h"
#include "pmp/SurfaceMesh.h"

namespace pmp {

void write_pmp(SurfaceMesh& mesh, const std::filesystem::path& file,
               const IOFlags& flags);

} // namespace pmp