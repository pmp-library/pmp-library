//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011-2016 by Graphics & Geometry Group, Bielefeld University
// Copyright (C) 2017 Daniel Sieger
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================

//== INCLUDES =================================================================

#include <surface_mesh/IO.h>

#include <cstdio>

//== NAMESPACES ===============================================================

namespace surface_mesh {

template<typename T> using VertexProperty = SurfaceMesh::VertexProperty<T>;
template<typename T> using HalfedgeProperty = SurfaceMesh::HalfedgeProperty<T>;
template<typename T> using FaceProperty = SurfaceMesh::FaceProperty<T>;
typedef SurfaceMesh::VertexConnectivity VertexConnectivity;
typedef SurfaceMesh::HalfedgeConnectivity HalfedgeConnectivity;
typedef SurfaceMesh::FaceConnectivity FaceConnectivity;

//== IMPLEMENTATION ===========================================================

// helper function
template <typename T>
void read(FILE* in, T& t)
{
    size_t nItems = fread((char*)&t, 1, sizeof(t), in);
    SM_ASSERT(nItems > 0);
}

// helper function
template <typename T>
void write(FILE* out, T& t)
{
    size_t nItems = fwrite((char*)&t, 1, sizeof(t), out);
    SM_ASSERT(nItems > 0);
}

//-----------------------------------------------------------------------------

bool readPoly(SurfaceMesh& mesh, const std::string& filename)
{
    // open file (in binary mode)
    FILE* in = fopen(filename.c_str(), "rb");
    if (!in)
        return false;

    // clear mesh
    mesh.clear();

    // how many elements?
    unsigned int nv, ne, nh, nf;
    read(in, nv);
    read(in, ne);
    read(in, nf);
    nh = 2 * ne;

    // resize containers
    mesh.m_vprops.resize(nv);
    mesh.m_hprops.resize(nh);
    mesh.m_eprops.resize(ne);
    mesh.m_fprops.resize(nf);

    // get properties
    auto vconn = mesh.vertexProperty<VertexConnectivity>("v:connectivity");
    auto hconn = mesh.halfedgeProperty<HalfedgeConnectivity>("h:connectivity");
    auto fconn = mesh.faceProperty<FaceConnectivity>("f:connectivity");
    auto point = mesh.vertexProperty<Point>("v:point");

    // read properties from file
    size_t nvc = fread((char*)vconn.data(), sizeof(VertexConnectivity), nv, in);
    size_t nhc = fread((char*)hconn.data(), sizeof(HalfedgeConnectivity), nh, in);
    size_t nfc = fread((char*)fconn.data(), sizeof(FaceConnectivity), nf, in);
    size_t np  = fread((char*)point.data(), sizeof(Point), nv, in);

    SM_ASSERT(nvc == nv);
    SM_ASSERT(nhc == nh);
    SM_ASSERT(nfc == nf);
    SM_ASSERT(np  == nv);

    fclose(in);
    return true;
}

//-----------------------------------------------------------------------------

bool writePoly(const SurfaceMesh& mesh, const std::string& filename)
{
    // check for colors
    auto color     = mesh.getVertexProperty<Color>("v:color");
    bool hasColors = color;

    // open file (in binary mode)
    FILE* out = fopen(filename.c_str(), "wb");
    if (!out)
        return false;

    // how many elements?
    unsigned int nv, ne, nh, nf;
    nv = mesh.nVertices();
    ne = mesh.nEdges();
    nh = mesh.nHalfedges();
    nf = mesh.nFaces();

    write(out, nv);
    write(out, ne);
    write(out, nf);
    write(out, hasColors);
    nh = 2 * ne;

    // get properties
    auto vconn = mesh.getVertexProperty<VertexConnectivity>("v:connectivity");
    auto hconn = mesh.getHalfedgeProperty<HalfedgeConnectivity>("h:connectivity");
    auto fconn = mesh.getFaceProperty<FaceConnectivity>("f:connectivity");
    auto point = mesh.getVertexProperty<Point>("v:point");

    // write properties to file
    fwrite((char*)vconn.data(), sizeof(VertexConnectivity), nv, out);
    fwrite((char*)hconn.data(), sizeof(HalfedgeConnectivity), nh, out);
    fwrite((char*)fconn.data(), sizeof(FaceConnectivity), nf, out);
    fwrite((char*)point.data(), sizeof(Point), nv, out);

    if (hasColors)
        fwrite((char*)color.data(), sizeof(Color), nv, out);

    fclose(out);

    return true;
}

//=============================================================================
} // namespace surface_mesh
//=============================================================================
