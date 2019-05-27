//=============================================================================
// Copyright (C) 2013-2017 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
//=============================================================================

#include <pmp/SurfaceMesh.h>

//=============================================================================

using namespace pmp;

//=============================================================================

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

//=============================================================================
