//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011-2013 by Graphics & Geometry Group, Bielefeld University
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public License
// as published by the Free Software Foundation, version 2.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================


//== INCLUDES =================================================================


#include <surface_mesh/IO.h>

#include <cstdio>


//== NAMESPACES ===============================================================


namespace surface_mesh {


//== IMPLEMENTATION ===========================================================


// helper function
template <typename T> size_t read(FILE* in, T& t)
{
    return fread((char*)&t, 1, sizeof(t), in);
}


//-----------------------------------------------------------------------------


bool read_poly(Surface_mesh& mesh, const std::string& filename)
{
    unsigned int n_items;

    // open file (in binary mode)
    FILE* in = fopen(filename.c_str(), "rb");
    if (!in) return false;


    // clear mesh
    mesh.clear();


    // how many elements?
    unsigned int nv, ne, nh, nf;
    read(in, nv);
    read(in, ne);
    read(in, nf);
    nh = 2*ne;


    // resize containers
    mesh.vprops_.resize(nv);
    mesh.hprops_.resize(nh);
    mesh.eprops_.resize(ne);
    mesh.fprops_.resize(nf);


    // get properties
    Surface_mesh::Vertex_property<Surface_mesh::Vertex_connectivity>      vconn = mesh.vertex_property<Surface_mesh::Vertex_connectivity>("v:connectivity");
    Surface_mesh::Halfedge_property<Surface_mesh::Halfedge_connectivity>  hconn = mesh.halfedge_property<Surface_mesh::Halfedge_connectivity>("h:connectivity");
    Surface_mesh::Face_property<Surface_mesh::Face_connectivity>          fconn = mesh.face_property<Surface_mesh::Face_connectivity>("f:connectivity");
    Surface_mesh::Vertex_property<Point>                                  point = mesh.vertex_property<Point>("v:point");

    // read properties from file
    n_items = fread((char*)vconn.data(), sizeof(Surface_mesh::Vertex_connectivity),   nv, in);
    n_items = fread((char*)hconn.data(), sizeof(Surface_mesh::Halfedge_connectivity), nh, in);
    n_items = fread((char*)fconn.data(), sizeof(Surface_mesh::Face_connectivity),     nf, in);
    n_items = fread((char*)point.data(), sizeof(Point),                               nv, in);

    fclose(in);
    return true;
}


//=============================================================================
} // namespace surface_mesh
//=============================================================================
