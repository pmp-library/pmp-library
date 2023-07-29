// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <filesystem>

#include "pmp/io/io_flags.h"
#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Read into \p mesh from \p file
//! \details File extension determines file type. Supported formats and
//! vertex attributes (a=ASCII, b=binary):
//!
//! Format | ASCII | Binary | Normals | Colors | Texcoords
//! -------|-------|--------|---------|--------|----------
//! OBJ    | yes   | no     | a       | no     | no
//! OFF    | yes   | yes    | a / b   | a      | a / b
//! PMP    | no    | yes    | no      | no     | no
//! STL    | yes   | yes    | no      | no     | no
//!
//! In addition, the OBJ and PMP formats support reading per-halfedge
//! texture coordinates.
//! \ingroup io
void read(SurfaceMesh& mesh, const std::filesystem::path& file);

//! \brief Write \p mesh to \p file controlled by \p flags
//! \details File extension determines file type. Supported formats and
//! vertex attributes (a=ASCII, b=binary):
//!
//! Format | ASCII | Binary | Normals | Colors | Texcoords
//! -------|-------|--------|---------|--------|----------
//! OBJ    | yes   | no     | a       | no     | no
//! OFF    | yes   | yes    | a       | a      | a
//! PMP    | no    | yes    | no      | no     | no
//! STL    | yes   | yes    | no      | no     | no
//!
//! In addition, the OBJ and PMP formats support writing per-halfedge
//! texture coordinates.
//! \ingroup io
void write(const SurfaceMesh& mesh, const std::filesystem::path& file,
           const IOFlags& flags = IOFlags());

} // namespace pmp
