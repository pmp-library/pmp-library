// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/write_obj.h"

namespace pmp {

void write_obj(const SurfaceMesh& mesh, const std::filesystem::path& file,
               const IOFlags& flags)
{
    FILE* out = fopen(file.string().c_str(), "w");
    if (!out)
        throw IOException("Failed to open file: " + file.string());

    // comment
    fprintf(out, "# OBJ export from PMP\n");

    // write vertices
    auto points = mesh.get_vertex_property<Point>("v:point");
    for (auto v : mesh.vertices())
    {
        const Point& p = points[v];
        fprintf(out, "v %.10f %.10f %.10f\n", p[0], p[1], p[2]);
    }

    // write normals
    auto normals = mesh.get_vertex_property<Normal>("v:normal");
    if (normals && flags.use_vertex_normals)
    {
        for (auto v : mesh.vertices())
        {
            const Normal& n = normals[v];
            fprintf(out, "vn %.10f %.10f %.10f\n", n[0], n[1], n[2]);
        }
    }

    // write texture coordinates
    auto tex_coords = mesh.get_halfedge_property<TexCoord>("h:tex");
    if (tex_coords && flags.use_halfedge_texcoords)
    {
        for (auto h : mesh.halfedges())
        {
            const TexCoord& pt = tex_coords[h];
            fprintf(out, "vt %.10f %.10f\n", pt[0], pt[1]);
        }
    }

    // write faces
    for (auto f : mesh.faces())
    {
        fprintf(out, "f");

        auto h = mesh.halfedges(f);
        for (auto v : mesh.vertices(f))
        {
            auto idx = v.idx() + 1;
            if (tex_coords)
            {
                // write vertex index, texCoord index and normal index
                fprintf(out, " %d/%d/%d", idx, (*h).idx() + 1, idx);
                ++h;
            }
            else
            {
                // write vertex index and normal index
                fprintf(out, " %d//%d", idx, idx);
            }
        }
        fprintf(out, "\n");
    }

    fclose(out);
}

} // namespace pmp