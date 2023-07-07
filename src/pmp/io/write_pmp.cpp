// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/write_pmp.h"
#include "pmp/types.h"
#include "pmp/io/helpers.h"

namespace pmp {

void write_pmp(const SurfaceMesh& mesh, const std::filesystem::path& file,
               const IOFlags&)
{
    // open file (in binary mode)
    FILE* out = fopen(file.string().c_str(), "wb");
    if (!out)
        throw IOException("Failed to open file: " + file.string());

    // get properties
    auto htex = mesh.get_halfedge_property<TexCoord>("h:tex");

    // how many elements?
    auto nv = mesh.n_vertices();
    auto ne = mesh.n_edges();
    auto nh = mesh.n_halfedges();
    auto nf = mesh.n_faces();

    // write header
    tfwrite(out, nv);
    tfwrite(out, ne);
    tfwrite(out, nf);
    tfwrite(out, (bool)htex);

    // write properties to file
    // clang-format off
    fwrite((char*)mesh.vconn_.data(), sizeof(SurfaceMesh::VertexConnectivity), nv, out);
    fwrite((char*)mesh.hconn_.data(), sizeof(SurfaceMesh::HalfedgeConnectivity), nh, out);
    fwrite((char*)mesh.fconn_.data(), sizeof(SurfaceMesh::FaceConnectivity), nf, out);
    fwrite((char*)mesh.vpoint_.data(), sizeof(Point), nv, out);
    // clang-format on

    // texture coordinates
    if (htex)
        fwrite((char*)htex.data(), sizeof(TexCoord), nh, out);

    fclose(out);
}

} // namespace pmp