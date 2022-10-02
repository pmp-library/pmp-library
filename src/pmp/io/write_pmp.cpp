// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/write_pmp.h"
#include "pmp/Types.h"
#include "pmp/io/helpers.h"

namespace pmp {

void write_pmp(const SurfaceMesh& mesh, const std::string& filename,
               const IOFlags&)
{
    // open file (in binary mode)
    FILE* out = fopen(filename.c_str(), "wb");
    if (!out)
        throw IOException("Failed to open file: " + filename);

    // get properties
    auto vconn = mesh.get_vertex_property<SurfaceMesh::VertexConnectivity>(
        "v:connectivity");
    auto hconn = mesh.get_halfedge_property<SurfaceMesh::HalfedgeConnectivity>(
        "h:connectivity");
    auto fconn =
        mesh.get_face_property<SurfaceMesh::FaceConnectivity>("f:connectivity");
    auto point = mesh.get_vertex_property<Point>("v:point");
    auto htex = mesh.get_halfedge_property<TexCoord>("h:tex");

    // how many elements?
    unsigned int nv, ne, nh, nf;
    nv = mesh.n_vertices();
    ne = mesh.n_edges();
    nh = mesh.n_halfedges();
    nf = mesh.n_faces();

    // write header
    tfwrite(out, nv);
    tfwrite(out, ne);
    tfwrite(out, nf);
    tfwrite(out, (bool)htex);

    // write properties to file
    fwrite((char*)vconn.data(), sizeof(SurfaceMesh::VertexConnectivity), nv,
           out);
    fwrite((char*)hconn.data(), sizeof(SurfaceMesh::HalfedgeConnectivity), nh,
           out);
    fwrite((char*)fconn.data(), sizeof(SurfaceMesh::FaceConnectivity), nf, out);
    fwrite((char*)point.data(), sizeof(Point), nv, out);

    // texture coordinates
    if (htex)
        fwrite((char*)htex.data(), sizeof(TexCoord), nh, out);

    fclose(out);
}

} // namespace pmp