// Copyright 2024 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "gtest/gtest.h"

#include "pmp/algorithms/hole_filling.h"
#include "pmp/algorithms/subdivision.h"
#include "pmp/algorithms/triangulation.h"
#include "pmp/algorithms/decimation.h"
#include "pmp/algorithms/remeshing.h"
#include "pmp/algorithms/utilities.h"
#include "pmp/io/io.h"
#include "pmp/surface_mesh.h"

using namespace pmp;

namespace {
void grow_selection(const SurfaceMesh& mesh)
{
    auto selected = mesh.get_vertex_property<bool>("v:selected");
    std::vector<pmp::Vertex> collected;
    for (const auto v : mesh.vertices())
        if (selected[v])
            for (const auto vv : mesh.vertices(v))
                collected.push_back(vv);

    for (auto v : collected)
        selected[v] = true;
};

Halfedge find_boundary(const SurfaceMesh& mesh)
{
    for (const auto h : mesh.halfedges())
        if (mesh.is_boundary(h))
            return h;
    return Halfedge();
};
} // namespace

TEST(RegressionTest, bunny)
{
    SurfaceMesh mesh;
    read(mesh, "data/off/bunny_adaptive.off");

    // up-sample
    loop_subdivision(mesh);
    quad_tri_subdivision(mesh);
    catmull_clark_subdivision(mesh);
    triangulate(mesh);

    // down-sample
    decimate(mesh, (int)mesh.n_vertices() / 100);

    // remesh
    auto bb = bounds(mesh).size();
    adaptive_remeshing(mesh, Scalar{0.001f} * bb, // min length
                       Scalar{1.0f} * bb,         // max length
                       Scalar{0.001f} * bb);      // approx. error

    // poke a hole
    auto selected = mesh.add_vertex_property<bool>("v:selected", false);
    selected[Vertex(0)] = true;
    for (int i{0}; i < 4; i++)
        grow_selection(mesh);

    for (const auto v : mesh.vertices())
        if (selected[v])
            mesh.delete_vertex(v);
    mesh.garbage_collection();

    // fill hole
    auto h = find_boundary(mesh);
    fill_hole(mesh, h);

    EXPECT_FALSE(find_boundary(mesh).is_valid());
}