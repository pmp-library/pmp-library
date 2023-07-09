// Copyright 2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

namespace pmp {

//! Flags to control reading and writing.
//! \ingroup io
struct IOFlags
{
    bool use_binary = false;             //!< Read / write binary format.
    bool use_vertex_normals = false;     //!< Read / write vertex normals.
    bool use_vertex_colors = false;      //!< Read / write vertex colors.
    bool use_vertex_texcoords = false;   //!< Read / write vertex texcoords.
    bool use_face_normals = false;       //!< Read / write face normals.
    bool use_face_colors = false;        //!< Read / write face colors.
    bool use_halfedge_texcoords = false; //!< Read / write halfedge texcoords.
};

} // namespace pmp
