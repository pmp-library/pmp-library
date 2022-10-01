// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <string>
#include <utility>

#include "pmp/Types.h"
#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief Read into \p mesh from file \p filename controlled by \p flags
//! \details File extension determines file type. Supported formats and
//! vertex attributes (a=ASCII, b=binary):
//!
//! Format | ASCII | Binary | Normals | Colors | Texcoords
//! -------|-------|--------|---------|--------|----------
//! OFF    | yes   | yes    | a / b   | a      | a / b
//! OBJ    | yes   | no     | a       | no     | no
//! STL    | yes   | yes    | no      | no     | no
//!
//! In addition, the OBJ supports reading per-halfedge
//! texture coordinates.
void read(SurfaceMesh& mesh, const std::string& filename,
          const IOFlags& flags = IOFlags());

//! \brief Write \p mesh to file \p filename controlled by \p flags
//! \details File extension determines file type. Supported formats and
//! vertex attributes (a=ASCII, b=binary):
//!
//! Format | ASCII | Binary | Normals | Colors | Texcoords
//! -------|-------|--------|---------|--------|----------
//! OFF    | yes   | yes    | a       | a      | a
//! OBJ    | yes   | no     | a       | no     | no
//! STL    | yes   | no     | no      | no     | no
//!
//! In addition, the OBJ format supports writing per-halfedge
//! texture coordinates.
void write(const SurfaceMesh& mesh, const std::string& filename,
           const IOFlags& flags = IOFlags());

class SurfaceMeshIO
{
public:
    SurfaceMeshIO(std::string filename, IOFlags flags)
        : filename_(std::move(filename)), flags_(std::move(flags))
    {
    }

    void read(SurfaceMesh& mesh);

    void write(const SurfaceMesh& mesh);

private:
    void read_stl(SurfaceMesh& mesh);

    void write_obj(const SurfaceMesh& mesh);
    void write_stl(const SurfaceMesh& mesh);

    std::string filename_;
    IOFlags flags_;
};

} // namespace pmp
