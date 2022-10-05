// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "SurfaceMeshTest.h"
#include "Helpers.h"

#include <vector>

using namespace pmp;

TEST_F(SurfaceMeshTest, emptyMesh)
{
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
    EXPECT_EQ(mesh.n_edges(), size_t(0));
    EXPECT_EQ(mesh.n_faces(), size_t(0));
}

TEST_F(SurfaceMeshTest, insert_remove_single_vertex)
{
    auto v = mesh.add_vertex(Point(0, 0, 0));
    EXPECT_EQ(mesh.n_vertices(), size_t(1));
    mesh.delete_vertex(v);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
}

TEST_F(SurfaceMeshTest, insert_remove_single_triangle)
{
    add_triangle();
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_edges(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    mesh.delete_face(f0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
    EXPECT_EQ(mesh.n_edges(), size_t(0));
    EXPECT_EQ(mesh.n_faces(), size_t(0));
}

TEST_F(SurfaceMeshTest, insert_remove_single_quad)
{
    add_quad();
    EXPECT_EQ(mesh.n_vertices(), size_t(4));
    EXPECT_EQ(mesh.n_edges(), size_t(4));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    mesh.delete_face(f0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
    EXPECT_EQ(mesh.n_edges(), size_t(0));
    EXPECT_EQ(mesh.n_faces(), size_t(0));
}

TEST_F(SurfaceMeshTest, insert_remove_single_polygonal_face)
{
    std::vector<Vertex> vertices(4);
    vertices[0] = mesh.add_vertex(Point(0, 0, 0));
    vertices[1] = mesh.add_vertex(Point(1, 0, 0));
    vertices[2] = mesh.add_vertex(Point(1, 1, 0));
    vertices[3] = mesh.add_vertex(Point(0, 1, 0));

    auto f = mesh.add_face(vertices);
    EXPECT_EQ(mesh.n_vertices(), size_t(4));
    EXPECT_EQ(mesh.n_edges(), size_t(4));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    mesh.delete_face(f);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
    EXPECT_EQ(mesh.n_edges(), size_t(0));
    EXPECT_EQ(mesh.n_faces(), size_t(0));
}

TEST_F(SurfaceMeshTest, delete_center_vertex)
{
    mesh = vertex_onering();
    EXPECT_EQ(mesh.n_vertices(), size_t(7));
    EXPECT_EQ(mesh.n_faces(), size_t(6));
    Vertex v(3); // the central vertex
    mesh.delete_vertex(v);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(0));
    EXPECT_EQ(mesh.n_faces(), size_t(0));
}

TEST_F(SurfaceMeshTest, delete_center_edge)
{
    mesh = edge_onering();
    EXPECT_EQ(mesh.n_vertices(), size_t(10));
    EXPECT_EQ(mesh.n_faces(), size_t(10));
    // the two vertices of the center edge
    v0 = Vertex(4);
    v1 = Vertex(5);

    auto e = mesh.find_edge(v0, v1);
    mesh.delete_edge(e);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), size_t(10));
    EXPECT_EQ(mesh.n_faces(), size_t(8));
}

TEST_F(SurfaceMeshTest, copy)
{
    add_triangle();
    SurfaceMesh m2 = mesh;
    EXPECT_EQ(m2.n_vertices(), size_t(3));
    EXPECT_EQ(m2.n_edges(), size_t(3));
    EXPECT_EQ(m2.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshTest, assignment)
{
    add_triangle();
    SurfaceMesh m2;
    m2.assign(mesh);
    EXPECT_EQ(m2.n_vertices(), size_t(3));
    EXPECT_EQ(m2.n_edges(), size_t(3));
    EXPECT_EQ(m2.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshTest, object_properties)
{
    // explicit add
    auto midx = mesh.add_object_property<int>("m:idx");
    midx[0] = 0;
    EXPECT_EQ(mesh.object_properties().size(), size_t(1));
    mesh.remove_object_property(midx);
    EXPECT_EQ(mesh.object_properties().size(), size_t(0));

    // implicit add
    midx = mesh.object_property<int>("m:idx2");
    EXPECT_EQ(mesh.object_properties().size(), size_t(1));
    mesh.remove_object_property(midx);
    EXPECT_EQ(mesh.object_properties().size(), size_t(0));
}

TEST_F(SurfaceMeshTest, vertex_properties)
{
    add_triangle();

    auto osize = mesh.vertex_properties().size();

    // explicit add
    auto vidx = mesh.add_vertex_property<int>("v:idx");
    vidx[v0] = 0;
    EXPECT_EQ(mesh.vertex_properties().size(), osize + 1);
    mesh.remove_vertex_property(vidx);
    EXPECT_EQ(mesh.vertex_properties().size(), osize);

    // implicit add
    vidx = mesh.vertex_property<int>("v:idx2");
    EXPECT_EQ(mesh.vertex_properties().size(), osize + 1);
    mesh.remove_vertex_property(vidx);
    EXPECT_EQ(mesh.vertex_properties().size(), osize);
}

TEST_F(SurfaceMeshTest, halfedge_properties)
{
    add_triangle();
    // explicit add
    auto hidx = mesh.add_halfedge_property<int>("h:idx");
    auto h = mesh.halfedge(v0);
    hidx[h] = 0;
    EXPECT_EQ(mesh.halfedge_properties().size(), size_t(2));
    mesh.remove_halfedge_property(hidx);
    EXPECT_EQ(mesh.halfedge_properties().size(), size_t(1));

    // implicit add
    hidx = mesh.halfedge_property<int>("h:idx2");
    EXPECT_EQ(mesh.halfedge_properties().size(), size_t(2));
    mesh.remove_halfedge_property(hidx);
    EXPECT_EQ(mesh.halfedge_properties().size(), size_t(1));
}

TEST_F(SurfaceMeshTest, edge_properties)
{
    add_triangle();
    // explicit add
    auto eidx = mesh.add_edge_property<int>("e:idx");
    auto e = mesh.edge(mesh.halfedge(v0));
    eidx[e] = 0;
    EXPECT_EQ(mesh.edge_properties().size(), size_t(2));
    mesh.remove_edge_property(eidx);
    EXPECT_EQ(mesh.edge_properties().size(), size_t(1));

    // implicit add
    eidx = mesh.edge_property<int>("e:idx2");
    EXPECT_EQ(mesh.edge_properties().size(), size_t(2));
    mesh.remove_edge_property(eidx);
    EXPECT_EQ(mesh.edge_properties().size(), size_t(1));
}

TEST_F(SurfaceMeshTest, face_properties)
{
    add_triangle();
    // explicit add
    auto fidx = mesh.add_face_property<int>("f:idx");
    fidx[f0] = 0;
    EXPECT_EQ(mesh.face_properties().size(), size_t(3));
    mesh.remove_face_property(fidx);
    EXPECT_EQ(mesh.face_properties().size(), size_t(2));

    // implicit add
    fidx = mesh.face_property<int>("f:idx2");
    EXPECT_EQ(mesh.face_properties().size(), size_t(3));
    mesh.remove_face_property(fidx);
    EXPECT_EQ(mesh.face_properties().size(), size_t(2));
}

TEST_F(SurfaceMeshTest, vertex_iterators)
{
    add_triangle();
    size_t sumIdx(0);
    for (auto v : mesh.vertices())
    {
        sumIdx += v.idx();
    }
    EXPECT_EQ(sumIdx, size_t(3));
}

TEST_F(SurfaceMeshTest, edge_iterators)
{
    add_triangle();
    size_t sumIdx(0);
    for (auto e : mesh.edges())
    {
        sumIdx += e.idx();
    }
    EXPECT_EQ(sumIdx, size_t(3));
}

TEST_F(SurfaceMeshTest, halfedge_iterators)
{
    add_triangle();
    size_t sumIdx(0);
    for (auto h : mesh.halfedges())
    {
        sumIdx += h.idx();
    }
    EXPECT_EQ(sumIdx, size_t(15));
}

TEST_F(SurfaceMeshTest, face_iterators)
{
    add_triangle();
    size_t sumIdx(0);
    for (auto f : mesh.faces())
    {
        sumIdx += f.idx();
    }
    EXPECT_EQ(sumIdx, size_t(0));
}

TEST_F(SurfaceMeshTest, is_triangle_mesh)
{
    add_triangle();
    EXPECT_TRUE(mesh.is_triangle_mesh());
}

TEST_F(SurfaceMeshTest, is_quad_mesh)
{
    add_quad();
    EXPECT_TRUE(mesh.is_quad_mesh());
}

TEST_F(SurfaceMeshTest, poly_mesh)
{
    std::vector<Vertex> vertices(5);
    vertices[0] = mesh.add_vertex(Point(0, 0, 0));
    vertices[1] = mesh.add_vertex(Point(1, 0, 0));
    vertices[2] = mesh.add_vertex(Point(1, 1, 0));
    vertices[3] = mesh.add_vertex(Point(0.5, 1, 0));
    vertices[4] = mesh.add_vertex(Point(0, 1, 0));
    mesh.add_face(vertices);
    EXPECT_FALSE(mesh.is_triangle_mesh() || mesh.is_quad_mesh());
}

TEST_F(SurfaceMeshTest, vertex_valence)
{
    add_triangle();
    auto val = mesh.valence(*mesh.vertices_begin());
    EXPECT_EQ(val, 2u);
}

TEST_F(SurfaceMeshTest, face_valence)
{
    add_triangle();
    auto val = mesh.valence(*mesh.faces_begin());
    EXPECT_EQ(val, 3u);
}

TEST_F(SurfaceMeshTest, collapse)
{
    add_triangles();
    EXPECT_EQ(mesh.n_faces(), size_t(2));
    auto h0 = mesh.find_halfedge(v3, v2);
    if (mesh.is_collapse_ok(h0))
        mesh.collapse(h0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshTest, edge_removal_ok)
{
    add_triangles();
    Edge e(1); // diagonal of triangulated quad
    EXPECT_TRUE(mesh.is_removal_ok(e));
}

TEST_F(SurfaceMeshTest, edge_removal_not_ok)
{
    add_triangle();
    Edge e(0); // boundary edge
    EXPECT_FALSE(mesh.is_removal_ok(e));
}

TEST_F(SurfaceMeshTest, remove_edge)
{
    add_triangles();
    Edge e(1); // diagonal of triangulated quad
    mesh.remove_edge(e);
    EXPECT_TRUE(mesh.is_quad_mesh());
}

TEST_F(SurfaceMeshTest, face_split)
{
    add_quad();
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    Point p(0.5, 0.5, 0);
    mesh.split(f0, p);
    EXPECT_EQ(mesh.n_faces(), size_t(4));
}

TEST_F(SurfaceMeshTest, edge_split)
{
    add_triangle();
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    auto e = mesh.find_edge(v0, v1);
    Point p0 = mesh.position(v0);
    Point p1 = mesh.position(v1);
    Point p = (p0 + p1) * 0.5;
    mesh.split(e, p);
    EXPECT_EQ(mesh.n_faces(), size_t(2));
}

TEST_F(SurfaceMeshTest, vertex_split)
{
    Vertex v0 = mesh.add_vertex(Point(0, 0, 0));
    Vertex v1 = mesh.add_vertex(Point(1, 0, 0));
    Vertex v2 = mesh.add_vertex(Point(0, 1, 0));
    Vertex v3 = mesh.add_vertex(Point(-1, 0, 0));
    Vertex v4 = mesh.add_vertex(Point(0, -1, 0));

    Face f1 = mesh.add_triangle(v0, v1, v2);
    Face f2 = mesh.add_triangle(v0, v2, v3);
    Face f3 = mesh.add_triangle(v0, v3, v4);
    Face f4 = mesh.add_triangle(v0, v4, v1);

    Halfedge h1 = mesh.find_halfedge(v1, v0);
    Halfedge h2 = mesh.find_halfedge(v2, v0);
    Halfedge h3 = mesh.find_halfedge(v3, v0);
    Halfedge h4 = mesh.find_halfedge(v4, v0);

    Vertex v5 = mesh.add_vertex(Point(0, 0, 1));
    Halfedge h = mesh.split_vertex(h1, h3, v5);
    EXPECT_EQ(mesh.from_vertex(h), v5);
    EXPECT_EQ(mesh.to_vertex(h), v0);

    EXPECT_EQ(mesh.valence(f1), 3);
    EXPECT_EQ(mesh.valence(f2), 4);
    EXPECT_EQ(mesh.valence(f3), 3);
    EXPECT_EQ(mesh.valence(f4), 4);

    EXPECT_EQ(mesh.to_vertex(h1), v0);
    EXPECT_EQ(mesh.to_vertex(h2), v0);
    EXPECT_EQ(mesh.to_vertex(h3), v5);
    EXPECT_EQ(mesh.to_vertex(h4), v5);
}

TEST_F(SurfaceMeshTest, edge_flip)
{
    mesh = edge_onering();
    EXPECT_EQ(mesh.n_vertices(), size_t(10));
    EXPECT_EQ(mesh.n_faces(), size_t(10));

    // the two vertices of the center edge
    v0 = Vertex(4);
    v1 = Vertex(5);
    auto e = mesh.find_edge(v0, v1);
    if (mesh.is_flip_ok(e))
        mesh.flip(e);
    EXPECT_EQ(mesh.n_vertices(), size_t(10));
    EXPECT_EQ(mesh.n_faces(), size_t(10));
}

TEST_F(SurfaceMeshTest, is_manifold)
{
    mesh = vertex_onering();
    for (auto v : mesh.vertices())
        EXPECT_TRUE(mesh.is_manifold(v));
}

TEST_F(SurfaceMeshTest, edge_length)
{
    add_quad();
    Scalar sum(0);
    for (auto e : mesh.edges())
    {
        sum += mesh.edge_length(e);
    }
    sum /= (Scalar)mesh.n_edges();
    EXPECT_FLOAT_EQ(sum, 1.0);
}

TEST_F(SurfaceMeshTest, property_stats)
{
    mesh.property_stats();
}
