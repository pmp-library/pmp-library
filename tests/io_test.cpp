// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "pmp/io/io_flags.h"
#include "surface_mesh_test.h"

#include <pmp/algorithms/normals.h>
#include <pmp/io/io.h>

using namespace pmp;

class IOTest : public SurfaceMeshTest
{
};

TEST_F(IOTest, obj_io)
{
    add_triangle();
    vertex_normals(mesh);
    mesh.add_halfedge_property<TexCoord>("h:tex", TexCoord(0, 0));
    auto filename = "test.obj";
    write(mesh, filename);
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    read(mesh, filename);
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    std::remove(filename);
}

TEST_F(IOTest, off_io)
{
    add_triangle();
    vertex_normals(mesh);
    mesh.add_vertex_property<TexCoord>("v:texcoord", TexCoord(0, 0));
    mesh.add_vertex_property<Color>("v:color", Color(0, 0, 0));

    IOFlags flags;
    flags.use_binary = false;
    flags.use_vertex_normals = true;
    flags.use_vertex_colors = true;
    flags.use_vertex_texcoords = true;

    auto filename = "test.off";
    write(mesh, filename, flags);
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    read(mesh, filename);
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    std::remove(filename);
}

TEST_F(IOTest, off_io_binary)
{
    add_triangle();

    IOFlags flags;
    flags.use_binary = true;

    auto filename = "binary.off";
    write(mesh, filename, flags);
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    read(mesh, filename);
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    std::remove(filename);
}

TEST_F(IOTest, pmp_io)
{
    add_triangle();
    auto filename = "test.pmp";
    write(mesh, filename);
    mesh.clear();
    EXPECT_TRUE(mesh.is_empty());
    read(mesh, filename);
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    std::remove(filename);

    // check malformed file names
    EXPECT_THROW(write(mesh, "testpolyly"), IOException);
}

TEST_F(IOTest, read_stl_ascii)
{
    read(mesh, "data/stl/icosahedron_ascii.stl");
    EXPECT_EQ(mesh.n_vertices(), size_t(12));
    EXPECT_EQ(mesh.n_faces(), size_t(20));
    EXPECT_EQ(mesh.n_edges(), size_t(30));
}
TEST_F(IOTest, read_stl_binary)
{
    read(mesh, "data/stl/icosahedron_binary.stl");
    EXPECT_EQ(mesh.n_vertices(), size_t(12));
    EXPECT_EQ(mesh.n_faces(), size_t(20));
    EXPECT_EQ(mesh.n_edges(), size_t(30));
}

TEST_F(IOTest, write_stl_binary)
{
    add_triangle();
    face_normals(mesh);
    IOFlags flags;
    flags.use_binary = true;
    auto filename = "binary.stl";
    write(mesh, filename, flags);

    // inject solid keyword to test for robustness
    auto fp = fopen(filename, "r+b");
    std::string key{"solid"};
    fwrite(key.c_str(), 1, key.size(), fp);
    fclose(fp);

    read(mesh, filename);
    EXPECT_EQ(mesh.n_vertices(), size_t(3));
    EXPECT_EQ(mesh.n_faces(), size_t(1));
    std::remove(filename);
}

TEST_F(IOTest, write_stl_no_normals)
{
    // try to write without normals being present
    add_triangle();
    ASSERT_THROW(write(mesh, "test.stl"), InvalidInputException);
}

TEST_F(IOTest, write_stl_with_normals)
{
    // the same with normals computed
    add_triangle();
    face_normals(mesh);
    EXPECT_NO_THROW(write(mesh, "test.stl"));
    std::remove("test.stl");
}

TEST_F(IOTest, write_stl_no_triangles)
{
    // try to write non-triangle mesh
    add_quad();
    ASSERT_THROW(write(mesh, "test.stl"), InvalidInputException);
}
