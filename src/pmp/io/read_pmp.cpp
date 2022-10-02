// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/read_pmp.h"

#include "pmp/io/helpers.h"

namespace pmp {

void read_pmp(SurfaceMesh& mesh, const std::string& filename)
{
    // open file (in binary mode)
    FILE* in = fopen(filename.c_str(), "rb");
    if (!in)
        throw IOException("Failed to open file: " + filename);

    // how many elements?
    unsigned int nv(0), ne(0), nh(0), nf(0);
    tfread(in, nv);
    tfread(in, ne);
    tfread(in, nf);
    nh = 2 * ne;

    // texture coordinates?
    bool has_htex(false);
    tfread(in, has_htex);

    // resize containers
    mesh.vprops_.resize(nv);
    mesh.hprops_.resize(nh);
    mesh.eprops_.resize(ne);
    mesh.fprops_.resize(nf);

    // get properties
    auto vconn =
        mesh.vertex_property<SurfaceMesh::VertexConnectivity>("v:connectivity");
    auto hconn = mesh.halfedge_property<SurfaceMesh::HalfedgeConnectivity>(
        "h:connectivity");
    auto fconn =
        mesh.face_property<SurfaceMesh::FaceConnectivity>("f:connectivity");
    auto point = mesh.vertex_property<Point>("v:point");

    // read properties from file
    size_t nvc = fread((char*)vconn.data(),
                       sizeof(SurfaceMesh::VertexConnectivity), nv, in);
    size_t nhc = fread((char*)hconn.data(),
                       sizeof(SurfaceMesh::HalfedgeConnectivity), nh, in);
    size_t nfc = fread((char*)fconn.data(),
                       sizeof(SurfaceMesh::FaceConnectivity), nf, in);
    size_t np = fread((char*)point.data(), sizeof(Point), nv, in);
    PMP_ASSERT(nvc == nv);
    PMP_ASSERT(nhc == nh);
    PMP_ASSERT(nfc == nf);
    PMP_ASSERT(np == nv);

    // read texture coordiantes
    if (has_htex)
    {
        auto htex = mesh.halfedge_property<TexCoord>("h:tex");
        size_t nhtc = fread((char*)htex.data(), sizeof(TexCoord), nh, in);
        PMP_ASSERT(nhtc == nh);
    }

    fclose(in);
}

} // namespace pmp
