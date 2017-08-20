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


#include <surface_mesh/SurfaceMesh.h>


//=============================================================================


using namespace surface_mesh;


//=============================================================================


int main(int argc, char** argv)
{
    // instantiate a SurfaceMesh object
    SurfaceMesh mesh;

    // read a mesh specified as the first command line argument
    if (argc > 1)
        mesh.read(argv[1]);

    // ...
    // do fancy stuff with the mesh
    // ...

    // write the mesh to the file specified as second argument
    if (argc > 2)
        mesh.write(argv[2]);

    return 0;
}


//=============================================================================
