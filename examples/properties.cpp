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

//! [edge-properties]
// get pre-defined point property
auto points = mesh.get_vertex_property<Point>("v:point");

// add new property storing a point per edge
auto edge_points = mesh.add_edge_property<Point>("e:point");

// loop over all edges
for (auto e : mesh.edges())
{
    // compute midpoint
    const auto p0 = points[mesh.vertex(e, 0)];
    const auto p1 = points[mesh.vertex(e, 1)];
    const auto midpoint = (p0 + p1 ) * Scalar(0.5);

    // access edge property like an array
    edge_points[e] = midpoint;
}

// remove property to free memory
mesh.remove_edge_property(edge_points);
//! [edge-properties]

}
// clang-format on
