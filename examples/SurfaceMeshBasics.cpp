//=============================================================================
// Copyright (C) 2013 by Graphics & Geometry Group, Bielefeld University
// Copyright (C) 2017 Daniel Sieger
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================

#include <pmp/SurfaceMesh.h>

//=============================================================================

using namespace pmp;

//=============================================================================

int main(void)
{
    //! [basics]

    // instantiate a SurfaceMesh object
    SurfaceMesh mesh;

    // instantiate 4 vertex handles
    Vertex v0,v1,v2,v3;

    // add 4 vertices
    v0 = mesh.add_vertex(Point(0,0,0));
    v1 = mesh.add_vertex(Point(1,0,0));
    v2 = mesh.add_vertex(Point(0,1,0));
    v3 = mesh.add_vertex(Point(0,0,1));

    // add 4 triangular faces
    mesh.add_triangle(v0,v1,v3);
    mesh.add_triangle(v1,v2,v3);
    mesh.add_triangle(v2,v0,v3);
    mesh.add_triangle(v0,v2,v1);

    std::cout << "vertices: " << mesh.n_vertices() << std::endl;
    std::cout << "edges: "    << mesh.n_edges()    << std::endl;
    std::cout << "faces: "    << mesh.n_faces()    << std::endl;

    //! [basics]
    return 0;
}

//=============================================================================
