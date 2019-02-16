//=============================================================================
// Copyright (C) 2013-2017 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================

#include <pmp/SurfaceMesh.h>

//=============================================================================

using namespace pmp;

//=============================================================================

int main(int argc, char** argv)
{
    //! [io]

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

    //! [io]
    return 0;
}


//=============================================================================
