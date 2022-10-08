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

//! [iterators]
float mean_valence = 0.0f;

// loop over all vertices
for (auto v : mesh.vertices())
{
    // sum up vertex valences
    mean_valence += mesh.valence(v);
}

mean_valence /= mesh.n_vertices();

std::cout << "mean valence: " << mean_valence << std::endl;
//! [iterators]
}
// clang-format on
