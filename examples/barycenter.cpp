// Copyright 2013-2017 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include <pmp/SurfaceMesh.h>
#include <pmp/io/io.h>

using namespace pmp;

// clang-format off
int main(int argc, char** argv)
{
SurfaceMesh mesh;

if (argc > 1)
    read(mesh, argv[1]);

//! [barycenter]
// get pre-defined property storing vertex positions
auto points = mesh.get_vertex_property<Point>("v:point");

Point p(0, 0, 0);

for (auto v : mesh.vertices())
{
    // access point property like an array
    p += points[v];
}

p /= mesh.n_vertices();

std::cout << "barycenter: " << p << std::endl;
//! [barycenter]
}
// clang-format on
