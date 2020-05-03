// Copyright 2013-2017 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include <pmp/SurfaceMesh.h>

using namespace pmp;

int main(int argc, char** argv)
{
    //! [barycenter]

    SurfaceMesh mesh;

    if (argc > 1)
        mesh.read(argv[1]);

    // get (pre-defined) property storing vertex positions
    auto points = mesh.get_vertex_property<Point>("v:point");

    Point p(0, 0, 0);

    for (auto vit : mesh.vertices())
    {
        // access point property like an array
        p += points[vit];
    }

    p /= mesh.n_vertices();

    std::cout << "barycenter: " << p << std::endl;

    //! [barycenter]
}
