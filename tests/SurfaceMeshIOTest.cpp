// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "SurfaceMeshTest.h"

#include <pmp/algorithms/Normals.h>
#include <pmp/io/SurfaceMeshIO.h>

#include <vector>
#include <fstream>

using namespace pmp;

class SurfaceMeshIOTest : public SurfaceMeshTest
{
};

TEST_F(SurfaceMeshIOTest, obj_io)
{
    add_triangle();
    Normals::compute_vertex_normals(mesh);
    mesh.add_halfedge_property<TexCoord>("h:texcoord", TexCoord(0, 0));
    write(mesh, "test.obj");
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    read(mesh, "test.obj");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, off_io)
{
    add_triangle();
    Normals::compute_vertex_normals(mesh);
    mesh.add_vertex_property<TexCoord>("v:texcoord", TexCoord(0, 0));
    mesh.add_vertex_property<Color>("v:color", Color(0, 0, 0));

    IOFlags flags;
    flags.use_binary = false;
    flags.use_vertex_normals = true;
    flags.use_vertex_colors = true;
    flags.use_vertex_texcoords = true;

    write(mesh, "test.off", flags);
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    read(mesh, "test.off");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, off_io_binary)
{
    add_triangle();

    IOFlags flags;
    flags.use_binary = true;

    write(mesh, "binary.off", flags);
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    read(mesh, "binary.off");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, stl_io)
{
    read(mesh, "pmp-data/stl/icosahedron_ascii.stl");
    EXPECT_EQ(mesh.n_vertices(), size_t(12));
    EXPECT_EQ(mesh.n_faces(), size_t(20));
    EXPECT_EQ(mesh.n_edges(), size_t(30));
    mesh.clear();
    read(mesh, "pmp-data/stl/icosahedron_binary.stl");
    EXPECT_EQ(mesh.n_vertices(), size_t(12));
    EXPECT_EQ(mesh.n_faces(), size_t(20));
    EXPECT_EQ(mesh.n_edges(), size_t(30));

    // try to write without normals being present
    ASSERT_THROW(write(mesh, "test.stl"), InvalidInputException);

    // the same with normals computed
    Normals::compute_face_normals(mesh);
    EXPECT_NO_THROW(write(mesh, "test.stl"));

    // try to write non-triangle mesh
    mesh.clear();
    add_quad();
    ASSERT_THROW(write(mesh, "test.stl"), InvalidInputException);
}

TEST_F(SurfaceMeshIOTest, ply_io)
{
    add_triangle();
    write(mesh, "test.ply");
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    read(mesh, "test.ply");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, ply_io_binary)
{
    add_triangle();

    IOFlags flags;
    flags.use_binary = true;

    write(mesh, "binary.ply", flags);
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    read(mesh, "binary.ply");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
}

TEST_F(SurfaceMeshIOTest, xyz_io)
{
    add_triangle();
    write(mesh, "test.xyz");
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    read(mesh, "test.xyz");
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
}

TEST_F(SurfaceMeshIOTest, agi_io)
{
    // generate example data
    std::ofstream ofs("test.agi");
    ofs << "0 0 0 0 0 0 0 0 0" << std::endl;
    ofs << "1 0 0 1 0 0 1 0 0" << std::endl;
    ofs << "1 1 0 1 1 0 1 1 0" << std::endl;
    ofs << "1 1 1 1 1 1 1 1 1" << std::endl;
    ofs.close();
    read(mesh, "test.agi");
    EXPECT_EQ(mesh.n_vertices(), 4u);
    EXPECT_TRUE(mesh.has_vertex_property("v:color"));
    EXPECT_TRUE(mesh.has_vertex_property("v:normal"));
}