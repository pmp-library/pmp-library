// Copyright 2013-2017 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include <pmp/SurfaceMesh.h>
#include <pmp/io/io.h>

using namespace pmp;

// clang-format off
int main(int argc, char** argv)
{
//! [io]
// instantiate a SurfaceMesh object
SurfaceMesh mesh;

// read a mesh specified as the first command line argument
if (argc > 1)
    read(mesh, argv[1]);

// ...
// do something with the mesh
// ...

// write the mesh to the file specified as second argument
if (argc > 2)
    write(mesh, argv[2]);
//! [io]

return 0;
}
// clang-format on
