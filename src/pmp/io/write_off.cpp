// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/write_off.h"
#include <bit>
#include <cstdint>
#include <fstream>
#include <ios>
#include "pmp/exceptions.h"
#include "pmp/types.h"
#include "pmp/io/helpers.h"

namespace pmp {

void write_off_binary(const SurfaceMesh& mesh,
                      const std::filesystem::path& file);

void write_off(const SurfaceMesh& mesh, const std::filesystem::path& file,
               const IOFlags& flags)
{
    if (flags.use_binary)
    {
        write_off_binary(mesh, file);
        return;
    }

    // check if we can write the mesh using 32-bit indices
    if (const auto max_idx = std::numeric_limits<uint32_t>::max();
        mesh.n_vertices() > max_idx)
        throw InvalidInputException(
            "Mesh too large to be written with 32-bit indices.");

    FILE* out = fopen(file.string().c_str(), "w");
    if (!out)
        throw IOException("Failed to open file: " + file.string());

    bool has_normals = false;
    bool has_texcoords = false;
    bool has_colors = false;

    auto normals = mesh.get_vertex_property<Normal>("v:normal");
    auto texcoords = mesh.get_vertex_property<TexCoord>("v:tex");
    auto colors = mesh.get_vertex_property<Color>("v:color");

    if (normals && flags.use_vertex_normals)
        has_normals = true;
    if (texcoords && flags.use_vertex_texcoords)
        has_texcoords = true;
    if (colors && flags.use_vertex_colors)
        has_colors = true;

    // header
    if (has_texcoords)
        fprintf(out, "ST");
    if (has_colors)
        fprintf(out, "C");
    if (has_normals)
        fprintf(out, "N");
    fprintf(out, "OFF\n%zu %zu 0\n", mesh.n_vertices(), mesh.n_faces());

    // vertices, and optionally normals and texture coordinates
    VertexProperty<Point> points = mesh.get_vertex_property<Point>("v:point");
    for (auto v : mesh.vertices())
    {
        const Point& p = points[v];
        fprintf(out, "%.10f %.10f %.10f", p[0], p[1], p[2]);

        if (has_normals)
        {
            const Normal& n = normals[v];
            fprintf(out, " %.10f %.10f %.10f", n[0], n[1], n[2]);
        }

        if (has_colors)
        {
            const Color& c = colors[v];
            fprintf(out, " %.10f %.10f %.10f", c[0], c[1], c[2]);
        }

        if (has_texcoords)
        {
            const TexCoord& t = texcoords[v];
            fprintf(out, " %.10f %.10f", t[0], t[1]);
        }

        fprintf(out, "\n");
    }

    // faces
    for (auto f : mesh.faces())
    {
        auto nv = mesh.valence(f);
        fprintf(out, "%zu", nv);
        auto fv = mesh.vertices(f);
        auto fvend = fv;
        do
        {
            fprintf(out, " %d", (uint32_t)(*fv).idx());
        } while (++fv != fvend);
        fprintf(out, "\n");
    }

    fclose(out);
}

template <class T>
    requires(sizeof(T) == 4)
void write_binary(std::ofstream& ofs, const T& val)
{
    if constexpr (std::endian::native == std::endian::little)
    {
        const auto u32v = std::bit_cast<uint32_t>(val);
        const auto vv = byteswap32(u32v);
        ofs.write(reinterpret_cast<const char*>(&vv), sizeof(vv));
    }
    else
    {
        ofs.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
}

void write_off_binary(const SurfaceMesh& mesh,
                      const std::filesystem::path& file)
{
    if constexpr (sizeof(IndexType) == 8 || sizeof(Scalar) == 8)
        throw IOException("Binary OFF files only support 32-bit types.");

    std::ofstream ofs(file.string());
    if (ofs.fail())
        throw IOException("Failed to open file: " + file.string());

    ofs << "OFF BINARY\n";
    ofs.close();
    ofs.open(file.string(), std::ios::binary | std::ios::app);

    const auto nv = static_cast<uint32_t>(mesh.n_vertices());
    const auto nf = static_cast<uint32_t>(mesh.n_faces());
    const uint32_t ne = 0;

    write_binary(ofs, nv);
    write_binary(ofs, nf);
    write_binary(ofs, ne);

    auto points = mesh.get_vertex_property<Point>("v:point");
    for (auto v : mesh.vertices())
    {
        const vec3 p = (vec3)points[v];
        write_binary(ofs, p[0]);
        write_binary(ofs, p[1]);
        write_binary(ofs, p[2]);
    }

    for (auto f : mesh.faces())
    {
        const auto valence = static_cast<uint32_t>(mesh.valence(f));
        write_binary(ofs, valence);
        for (auto fv : mesh.vertices(f))
        {
            const uint32_t idx = fv.idx();
            write_binary(ofs, idx);
        }
    }
    ofs.close();
}

} // namespace pmp
