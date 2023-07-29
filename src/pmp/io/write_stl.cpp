// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/write_stl.h"

#include "pmp/exceptions.h"

#include <fstream>
#include <array>
#include <ios>

namespace pmp {

namespace {
void write_binary_stl(const SurfaceMesh& mesh,
                      const std::filesystem::path& file)
{
    std::ofstream ofs(file.string(), std::ios::binary);

    // write 80 byte header
    std::array<char, 80> header{};
    for (auto c : header)
        ofs.write(&c, sizeof(char));

    //  write number of triangles
    auto n_triangles = static_cast<uint32_t>(mesh.n_faces());
    ofs.write((char*)&n_triangles, sizeof(n_triangles));

    // write normal, points, and attribute byte count
    auto normals = mesh.get_face_property<Normal>("f:normal");
    auto points = mesh.get_vertex_property<Point>("v:point");
    for (auto f : mesh.faces())
    {
        auto n = (vec3)normals[f];
        ofs.write((char*)&n[0], sizeof(float));
        ofs.write((char*)&n[1], sizeof(float));
        ofs.write((char*)&n[2], sizeof(float));

        for (auto v : mesh.vertices(f))
        {
            auto p = (vec3)points[v];
            ofs.write((char*)&p[0], sizeof(float));
            ofs.write((char*)&p[1], sizeof(float));
            ofs.write((char*)&p[2], sizeof(float));
        }
        ofs << "  ";
    }
    ofs.close();
}

} // namespace

void write_stl(const SurfaceMesh& mesh, const std::filesystem::path& file,
               const IOFlags& flags)
{
    if (!mesh.is_triangle_mesh())
    {
        auto what = "write_stl: Not a triangle mesh.";
        throw InvalidInputException(what);
    }

    auto fnormals = mesh.get_face_property<Normal>("f:normal");
    if (!fnormals)
    {
        auto what = "write_stl: No face normals present.";
        throw InvalidInputException(what);
    }

    if (flags.use_binary)
    {
        write_binary_stl(mesh, file);
        return;
    }

    std::ofstream ofs(file.string().c_str());
    auto points = mesh.get_vertex_property<Point>("v:point");

    ofs << "solid stl" << std::endl;
    Normal n;
    Point p;

    for (auto f : mesh.faces())
    {
        n = fnormals[f];
        ofs << "  facet normal ";
        ofs << n[0] << " " << n[1] << " " << n[2] << std::endl;
        ofs << "    outer loop" << std::endl;
        for (auto v : mesh.vertices(f))
        {
            p = points[v];
            ofs << "      vertex ";
            ofs << p[0] << " " << p[1] << " " << p[2] << std::endl;
        }
        ofs << "    endloop" << std::endl;
        ofs << "  endfacet" << std::endl;
    }
    ofs << "endsolid" << std::endl;
    ofs.close();
}

} // namespace pmp
