//=============================================================================
// Copyright (C) 2013 by Graphics & Geometry Group, Bielefeld University
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


#include <surface_mesh/Surface_mesh.h>


//=============================================================================


using namespace surface_mesh;


//=============================================================================


int main(void)
{
    // instantiate a Surface_mesh object
    Surface_mesh mesh;

    // instantiate 4 vertex handles
    Surface_mesh::Vertex v0,v1,v2,v3;

    // add 4 vertices
    v0 = mesh.add_vertex(Point(0,0,0));
    v1 = mesh.add_vertex(Point(1,0,0));
    v2 = mesh.add_vertex(Point(0,1,0));
    v3 = mesh.add_vertex(Point(0,0,1));

    // add 4 triangular faces
    mesh.add_triangle(v0,v1,v3);
    mesh.add_triangle(v1,v2,v3);
    mesh.add_triangle(v2,v0,v3);
    mesh.add_triangle(v0,v2,v1);

    std::cout << "vertices: " << mesh.n_vertices() << std::endl;
    std::cout << "edges: "    << mesh.n_edges()    << std::endl;
    std::cout << "faces: "    << mesh.n_faces()    << std::endl;

    return 0;
}


//=============================================================================
