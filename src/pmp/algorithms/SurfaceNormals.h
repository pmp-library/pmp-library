//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
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

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup algorithms algorithms
//! @{

//=============================================================================

//! \brief A class for computing surface normals.
//! \details This class provides a set of static functions for computing surface
//! normal information, either
//!
//! \li per vertex: compute_vertex_normal()
//! \li per face: compute_face_normal()
//! \li per corner: compute_corner_normal()
//!
//! The convenience functions compute_vertex_normals() and compute_face_normals()
//! compute the normals for the whole mesh and add a corresponding vertex or
//! face property.
class SurfaceNormals
{
public:
    // delete default and copy constructor
    SurfaceNormals() = delete;
    SurfaceNormals(const SurfaceNormals&) = delete;

    //! \brief Compute vertex normals for the whole \c mesh.
    //! \details Calls compute_vertex_normal() for each vertex and adds a new
    //! vertex property of type Normal named "v:normal".
    static void compute_vertex_normals(SurfaceMesh& mesh);

    //! \brief Compute face normals for the whole \c mesh.
    //! \details Calls compute_face_normal() for each face and adds a new face
    //! property of type Normal named "f:normal".
    static void compute_face_normals(SurfaceMesh& mesh);

    //! \brief Compute the normal vector of vertex \c v.
    static Normal compute_vertex_normal(const SurfaceMesh& mesh,
                                        Vertex v);

    //! \brief Compute the normal vector of face \c f.
    static Normal compute_face_normal(const SurfaceMesh& mesh,
                                      Face f);

    //! \brief Compute the normal vector of the polygon corner specified by the
    //! target vertex of halfedge \c h.
    //! \details Averages incident corner normals if they are within crease_angle
    //! of the face normal. \c crease_angle is in radians, not degrees.
    static Normal compute_corner_normal(const SurfaceMesh& mesh,
                                        Halfedge h,
                                        Scalar crease_angle);
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
