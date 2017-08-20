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
    SurfaceMesh mesh;

    if (argc > 1)
        mesh.read(argv[1]);

    float meanValence = 0.0f;

    // loop over all vertices
    for (auto vit : mesh.vertices())
    {
        // sum up vertex valences
        meanValence += mesh.valence(vit);
    }

    meanValence /= mesh.nVertices();

    std::cout << "mean valence: " << meanValence << std::endl;
}


//=============================================================================
