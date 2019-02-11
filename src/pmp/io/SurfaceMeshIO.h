//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/SurfaceMesh.h>
#include <pmp/io/IOOptions.h>

#include <string>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup io io
//! @{

//=============================================================================

//! Class for reading and writing surface meshes from and to files
//!
//! This class currently supports the following standard mesh formats: OFF, OBJ,
//! STL, PLY, XYZ, and AGI. Note that not all entity properties such as vertex
//! normals or colors are equally supported for all formats and IO modes
//! (reading vs. writing, ascii vs. binary). See below for details on each
//! format.
//!
//! _OFF files_ can be read and written as either ACSII or binary files. The
//! ASCII format supports the color, normal, and texcoord vertex attributes. The
//! binary format currently only supports normals and texcoords for reading and
//! no attributes for writing. Homogeneous coordinates and vertex dimensions
//! \f$\neq 3\f$ are not supported.
//!
//! _OBJ files_ can only be read and written as ASCII files. Normals and texture
//! coordinates are the supported properties.
//!
//! _STL files_ can be read and written as either ASCII or binary files. The STL
//! format only supports pure triangle meshes. Face normals are the only
//! supported property.
//!
//! _PLY files_ can be read and written as either ACSII or binary
//! files. Currently no additional properties are supported.
//!
//! _XYZ_ files can be read and writen. Only ASCII mode is supported. Only
//! supports point coordinates and normals.
//!
//! _AGI_ files can be read. This is a file format used by Agisoft. Format:
//! point coordinates, RGB colors, and normals per line. ASCII only,
//!
//! In addition, we include a custom binary file format (.poly), to efficiently
//! dump a mesh to disk. This currently only includes the connectivity and
//! geometry data, no custom properties.
class SurfaceMeshIO
{
public:
    //! Construct with default IO options
    SurfaceMeshIO(const IOOptions& options = IOOptions())
        : options_(options){};

    //! Read surface mesh from file \c filename
    bool read(SurfaceMesh& mesh, const std::string& filename);

    //! Write surface mesh \c mesh to file \c filename
    bool write(const SurfaceMesh& mesh, const std::string& filename);

private:
    bool read_off(SurfaceMesh& mesh, const std::string& filename);
    bool read_obj(SurfaceMesh& mesh, const std::string& filename);
    bool read_stl(SurfaceMesh& mesh, const std::string& filename);
    bool read_ply(SurfaceMesh& mesh, const std::string& filename);
    bool read_pmp(SurfaceMesh& mesh, const std::string& filename);
    bool read_xyz(SurfaceMesh& mesh, const std::string& filename);
    bool read_agi(SurfaceMesh& mesh, const std::string& filename);

    bool write_off(const SurfaceMesh& mesh, const std::string& filename);
    bool write_off_binary(const SurfaceMesh& mesh, const std::string& filename);
    bool write_obj(const SurfaceMesh& mesh, const std::string& filename);
    bool write_stl(const SurfaceMesh& mesh, const std::string& filename);
    bool write_ply(const SurfaceMesh& mesh, const std::string& filename);
    bool write_pmp(const SurfaceMesh& mesh, const std::string& filename);
    bool write_xyz(const SurfaceMesh& mesh, const std::string& filename);

private:
    IOOptions options_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
