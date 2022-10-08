// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/write_off.h"
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
            fprintf(out, " %d", (*fv).idx());
        } while (++fv != fvend);
        fprintf(out, "\n");
    }

    fclose(out);
}

void write_off_binary(const SurfaceMesh& mesh,
                      const std::filesystem::path& file)
{
    FILE* out = fopen(file.string().c_str(), "w");
    if (!out)
        throw IOException("Failed to open file: " + file.string());

    fprintf(out, "OFF BINARY\n");
    fclose(out);
    auto nv = (IndexType)mesh.n_vertices();
    auto nf = (IndexType)mesh.n_faces();
    auto ne = IndexType{};

    out = fopen(file.string().c_str(), "ab");
    tfwrite(out, nv);
    tfwrite(out, nf);
    tfwrite(out, ne);
    auto points = mesh.get_vertex_property<Point>("v:point");
    for (auto v : mesh.vertices())
    {
        const Point& p = points[v];
        tfwrite(out, p);
    }

    for (auto f : mesh.faces())
    {
        IndexType valence = mesh.valence(f);
        tfwrite(out, valence);
        for (auto fv : mesh.vertices(f))
            tfwrite(out, (IndexType)fv.idx());
    }
    fclose(out);
}

} // namespace pmp