// Copyright 2013-2017 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include <pmp/SurfaceMesh.h>

using namespace pmp;

// clang-format off
int main()
{
//! [basics]
// instantiate a SurfaceMesh object
SurfaceMesh mesh;

// add 4 vertices
const auto v0 = mesh.add_vertex(Point(0, 0, 0));
const auto v1 = mesh.add_vertex(Point(1, 0, 0));
const auto v2 = mesh.add_vertex(Point(0, 1, 0));
const auto v3 = mesh.add_vertex(Point(0, 0, 1));

// add 4 triangular faces
mesh.add_triangle(v0, v1, v3);
mesh.add_triangle(v1, v2, v3);
mesh.add_triangle(v2, v0, v3);
mesh.add_triangle(v0, v2, v1);

std::cout << "vertices: " << mesh.n_vertices() << std::endl;
std::cout << "edges: " << mesh.n_edges() << std::endl;
std::cout << "faces: " << mesh.n_faces() << std::endl;
//! [basics]
return 0;
}
// clang-format on
