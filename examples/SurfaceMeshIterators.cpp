// Copyright 2013-2017 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include <pmp/SurfaceMesh.h>

using namespace pmp;

int main(int argc, char** argv)
{
    //! [iterators]

    SurfaceMesh mesh;

    if (argc > 1)
        mesh.read(argv[1]);

    float mean_valence = 0.0f;

    // loop over all vertices
    for (auto vit : mesh.vertices())
    {
        // sum up vertex valences
        mean_valence += mesh.valence(vit);
    }

    mean_valence /= mesh.n_vertices();

    std::cout << "mean valence: " << mean_valence << std::endl;

    //! [iterators]
}
