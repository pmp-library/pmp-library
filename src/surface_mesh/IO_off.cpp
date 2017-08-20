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

//== NAMESPACE ================================================================


namespace surface_mesh {


//== IMPLEMENTATION ===========================================================


// helper function
template <typename T> int read(FILE* in, T& t)
{
    int err = 0;
    err = fread(&t, 1, sizeof(t), in);
    return err;
}


//-----------------------------------------------------------------------------


bool readOFFAscii(SurfaceMesh& mesh,
                    FILE* in,
                    const bool hasNormals,
                    const bool hasTexcoords,
                    const bool hasColors)
{
    char                 line[200], *lp;
    int                  nc;
    unsigned int         i, j, items, idx;
    unsigned int         nV, nF, nE;
    Vec3f                p, n, c;
    Vec2f                t;
    SurfaceMesh::Vertex v;


    // properties
    SurfaceMesh::VertexProperty<Normal>              normals;
    SurfaceMesh::VertexProperty<TextureCoordinate>  texcoords;
    SurfaceMesh::VertexProperty<Color>               colors;
    if (hasNormals)   normals   = mesh.vertexProperty<Normal>("v:normal");
    if (hasTexcoords) texcoords = mesh.vertexProperty<TextureCoordinate>("v:texcoord");
    if (hasColors)    colors    = mesh.vertexProperty<Color>("v:color");


    // #Vertice, #Faces, #Edges
    items = fscanf(in, "%d %d %d\n", (int*)&nV, (int*)&nF, (int*)&nE);
    SM_ASSERT(items);

    mesh.clear();
    mesh.reserve(nV, std::max(3*nV, nE), nF);


    // read vertices: pos [normal] [color] [texcoord]
    for (i=0; i<nV && !feof(in); ++i)
    {
        // read line
        lp = fgets(line, 200, in);
        lp = line;

        // position
        items = sscanf(lp, "%f %f %f%n", &p[0], &p[1], &p[2], &nc);
        assert(items==3);
        v = mesh.addVertex((Point)p);
        lp += nc;

        // normal
        if (hasNormals)
        {
            if (sscanf(lp, "%f %f %f%n", &n[0], &n[1], &n[2], &nc) == 3)
            {
                normals[v] = n;
            }
            lp += nc;
        }

        // color
        if (hasColors)
        {
            if (sscanf(lp, "%f %f %f%n", &c[0], &c[1], &c[2], &nc) == 3)
            {
                if (c[0]>1.0f || c[1]>1.0f || c[2]>1.0f) c *= (1.0/255.0);
                colors[v] = c;
            }
            lp += nc;
        }

        // tex coord
        if (hasTexcoords)
        {
            items = sscanf(lp, "%f %f%n", &t[0], &t[1], &nc);
            assert(items == 2);
            texcoords[v][0] = t[0];
            texcoords[v][1] = t[1];
            lp += nc;
        }
    }



    // read faces: #N v[1] v[2] ... v[n-1]
    std::vector<SurfaceMesh::Vertex> vertices;
    for (i=0; i<nF; ++i)
    {
        // read line
        lp = fgets(line, 200, in);
        lp = line;

        // #vertices
        items = sscanf(lp, "%d%n", (int*)&nV, &nc);
        assert(items == 1);
        vertices.resize(nV);
        lp += nc;

        // indices
        for (j=0; j<nV; ++j)
        {
            items = sscanf(lp, "%d%n", (int*)&idx, &nc);
            assert(items == 1);
            vertices[j] = SurfaceMesh::Vertex(idx);
            lp += nc;
        }
        mesh.addFace(vertices);
    }


    return true;
}


//-----------------------------------------------------------------------------


bool readOFFBinary(SurfaceMesh& mesh,
                     FILE* in,
                     const bool hasNormals,
                     const bool hasTexcoords,
                     const bool hasColors)
{
    unsigned int       i, j, idx;
    unsigned int       nV, nF, nE;
    Vec3f              p, n, c;
    Vec2f              t;
    SurfaceMesh::Vertex  v;


    // binary cannot (yet) read colors
    if (hasColors) return false;


    // properties
    SurfaceMesh::VertexProperty<Normal>              normals;
    SurfaceMesh::VertexProperty<TextureCoordinate>  texcoords;
    if (hasNormals)   normals   = mesh.vertexProperty<Normal>("v:normal");
    if (hasTexcoords) texcoords = mesh.vertexProperty<TextureCoordinate>("v:texcoord");


    // #Vertice, #Faces, #Edges
    read(in, nV);
    read(in, nF);
    read(in, nE);
    mesh.clear();
    mesh.reserve(nV, std::max(3*nV, nE), nF);


    // read vertices: pos [normal] [color] [texcoord]
    for (i=0; i<nV && !feof(in); ++i)
    {
        // position
        read(in, p);
        v = mesh.addVertex((Point)p);

        // normal
        if (hasNormals)
        {
            read(in, n);
            normals[v] = n;
        }

        // tex coord
        if (hasTexcoords)
        {
            read(in, t);
            texcoords[v][0] = t[0];
            texcoords[v][1] = t[1];
        }
    }


    // read faces: #N v[1] v[2] ... v[n-1]
    std::vector<SurfaceMesh::Vertex> vertices;
    for (i=0; i<nF; ++i)
    {
        read(in, nV);
        vertices.resize(nV);
        for (j=0; j<nV; ++j)
        {
            read(in, idx);
            vertices[j] = SurfaceMesh::Vertex(idx);
        }
        mesh.addFace(vertices);
    }


    return true;
}


//-----------------------------------------------------------------------------


bool readOFF(SurfaceMesh& mesh, const std::string& filename)
{
    char  line[200];
    bool  hasTexcoords = false;
    bool  hasNormals   = false;
    bool  hasColors    = false;
    bool  hasHcoords   = false;
    bool  hasDim       = false;
    bool  isBinary     = false;


    // open file (in ASCII mode)
    FILE* in = fopen(filename.c_str(), "r");
    if (!in) return false;


    // read header: [ST][C][N][4][n]OFF BINARY
    char *c = fgets(line, 200, in);
    assert(c != NULL);
    c = line;
    if (c[0] == 'S' && c[1] == 'T') { hasTexcoords = true; c += 2; }
    if (c[0] == 'C') { hasColors  = true; ++c; }
    if (c[0] == 'N') { hasNormals = true; ++c; }
    if (c[0] == '4') { hasHcoords = true; ++c; }
    if (c[0] == 'n') { hasDim     = true; ++c; }
    if (strncmp(c, "OFF", 3) != 0) { fclose(in); return false; } // no OFF
    if (strncmp(c+4, "BINARY", 6) == 0) isBinary = true;


    // homogeneous coords, and vertex dimension != 3 are not supported
    if (hasHcoords || hasDim)
    {
        fclose(in);
        return false;
    }


    // if binary: reopen file in binary mode
    if (isBinary)
    {
        fclose(in);
        in = fopen(filename.c_str(), "rb");
        c = fgets(line, 200, in);
        assert(c != NULL);
    }


    // read as ASCII or binary
    bool ok = (isBinary ?
               readOFFBinary(mesh, in, hasNormals, hasTexcoords, hasColors) :
               readOFFAscii(mesh, in, hasNormals, hasTexcoords, hasColors));


    fclose(in);
    return ok;
}


//-----------------------------------------------------------------------------


bool writeOFF(const SurfaceMesh& mesh, const std::string& filename)
{
    FILE* out = fopen(filename.c_str(), "w");
    if (!out)
        return false;


    bool  hasNormals   = false;
    bool  hasTexcoords = false;
    bool  hasColors = false;
    SurfaceMesh::VertexProperty<Normal> normals = mesh.getVertexProperty<Normal>("v:normal");
    SurfaceMesh::VertexProperty<TextureCoordinate>  texcoords = mesh.getVertexProperty<TextureCoordinate>("v:texcoord");
    SurfaceMesh::VertexProperty<Color> colors = mesh.getVertexProperty<Color>("v:color");
    if (normals)   hasNormals = true;
    if (texcoords) hasTexcoords = true;
    if (colors) hasColors = true;


    // header
    if(hasTexcoords)
        fprintf(out, "ST");
    if(hasColors)
        fprintf(out, "C");
    if(hasNormals)
        fprintf(out, "N");
    fprintf(out, "OFF\n%d %d 0\n", mesh.nVertices(), mesh.nFaces());


    // vertices, and optionally normals and texture coordinates
    SurfaceMesh::VertexProperty<Point> points = mesh.getVertexProperty<Point>("v:point");
    for (SurfaceMesh::VertexIterator vit=mesh.verticesBegin(); vit!=mesh.verticesEnd(); ++vit)
    {
        const Point& p = points[*vit];
        fprintf(out, "%.10f %.10f %.10f", p[0], p[1], p[2]);

        if (hasNormals)
        {
            const Normal& n = normals[*vit];
            fprintf(out, " %.10f %.10f %.10f", n[0], n[1], n[2]);
        }

        if (hasColors)
        {
            const Color& c = colors[*vit];
            fprintf(out, " %.10f %.10f %.10f", c[0], c[1], c[2]);
        }

        if (hasTexcoords)
        {
            const TextureCoordinate& t = texcoords[*vit];
            fprintf(out, " %.10f %.10f", t[0], t[1]);
        }

        fprintf(out, "\n");
    }


    // faces
    for (SurfaceMesh::FaceIterator fit=mesh.facesBegin(); fit!=mesh.facesEnd(); ++fit)
    {
        int nV = mesh.valence(*fit);
        fprintf(out, "%d", nV);
        SurfaceMesh::VertexAroundFaceCirculator fvit=mesh.vertices(*fit), fvend=fvit;
        do
        {
            fprintf(out, " %d", (*fvit).idx());
        }
        while (++fvit != fvend);
        fprintf(out, "\n");
    }

    fclose(out);
    return true;
}


//=============================================================================
} // namespace surface_mesh
//=============================================================================
