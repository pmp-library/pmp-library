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


#include <surface_mesh/Surface_mesh.h>


//=============================================================================


using namespace surface_mesh;


//=============================================================================


int main(int /*argc*/, char** argv)
{
    Surface_mesh mesh;

    mesh.read(argv[1]);

    float mean_valence = 0.0f;
    unsigned int vertex_valence;

    // instantiate iterator and circulators
    Surface_mesh::Vertex_iterator vit;
    Surface_mesh::Vertex_around_vertex_circulator vc, vc_end;

    // loop over all vertices
    for (vit = mesh.vertices_begin(); vit != mesh.vertices_end(); ++vit)
    {
        // initialize circulators
        vc = mesh.vertices(*vit);
        vc_end = vc;

        // reset counter
        vertex_valence = 0;

        // loop over all incident vertices
        do
        {
            ++vertex_valence;
        } while (++vc != vc_end);

        // sum up vertex valences
        mean_valence += vertex_valence;
    }

    mean_valence /= mesh.n_vertices();

    std::cout << "mean valence: " << mean_valence << std::endl;
}


//=============================================================================
