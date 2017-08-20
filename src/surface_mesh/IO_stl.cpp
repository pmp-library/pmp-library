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
#include <cfloat>
#include <map>
#include <fstream>


//== NAMESPACES ===============================================================


namespace surface_mesh {


//== IMPLEMENTATION ===========================================================


// helper function
template <typename T> void read(FILE* in, T& t)
{
    size_t nItems(0);
    nItems = fread((char*)&t, 1, sizeof(t), in);
    SM_ASSERT(nItems > 0);
}


//-----------------------------------------------------------------------------


// helper class for STL reader
class CmpVec
{
public:
    CmpVec(float eps=FLT_MIN) : m_eps(eps) {}

    bool operator()(const Vec3f& v0, const Vec3f& v1) const
    {
        if (fabs(v0[0] - v1[0]) <= m_eps)
        {
            if (fabs(v0[1] - v1[1]) <= m_eps)
            {
                return (v0[2] < v1[2] - m_eps);
            }
            else return (v0[1] < v1[1] - m_eps);
        }
        else return (v0[0] < v1[0] - m_eps);
    }

private:
    float m_eps;
};


//-----------------------------------------------------------------------------


bool readSTL(SurfaceMesh& mesh, const std::string& filename)
{
    char                            line[100], *c;
    unsigned int                    i, nT;
    Vec3f                           p;
    SurfaceMesh::Vertex               v;
    std::vector<SurfaceMesh::Vertex>  vertices(3);
    size_t nItems(0);

    CmpVec comp(FLT_MIN);
    std::map<Vec3f, SurfaceMesh::Vertex, CmpVec>            vMap(comp);
    std::map<Vec3f, SurfaceMesh::Vertex, CmpVec>::iterator  vMapIt;


    // clear mesh
    mesh.clear();


    // open file (in ASCII mode)
    FILE* in = fopen(filename.c_str(), "r");
    if (!in) return false;


    // ASCII or binary STL?
    c = fgets(line, 6, in);
    SM_ASSERT(c != NULL);
    const bool binary = ((strncmp(line, "SOLID", 5) != 0) &&
                         (strncmp(line, "solid", 5) != 0));


    // parse binary STL
    if (binary)
    {
        // re-open file in binary mode
        fclose(in);
        in = fopen(filename.c_str(), "rb");
        if (!in) return false;

        // skip dummy header
        nItems = fread(line, 1, 80, in);
        SM_ASSERT(nItems > 0);

        // read number of triangles
        read(in, nT);

        // read triangles
        while (nT)
        {
            // skip triangle normal
            nItems = fread(line, 1, 12, in);
            SM_ASSERT(nItems > 0);
            // triangle's vertices
            for (i=0; i<3; ++i)
            {
                read(in, p);

                // has vector been referenced before?
                if ((vMapIt=vMap.find(p)) == vMap.end())
                {
                    // No : add vertex and remember idx/vector mapping
                    v = mesh.addVertex((Point)p);
                    vertices[i] = v;
                    vMap[p] = v;
                }
                else
                {
                    // Yes : get index from map
                    vertices[i] = vMapIt->second;
                }
            }

            // Add face only if it is not degenerated
            if ((vertices[0] != vertices[1]) &&
                (vertices[0] != vertices[2]) &&
                (vertices[1] != vertices[2]))
                mesh.addFace(vertices);

            nItems = fread(line, 1, 2, in);
            SM_ASSERT(nItems > 0);
            --nT;
        }
    }


    // parse ASCII STL
    else
    {
        // parse line by line
        while (in && !feof(in) && fgets(line, 100, in))
        {
            // skip white-space
            for (c=line; isspace(*c) && *c!='\0'; ++c) {};

            // face begins
            if ((strncmp(c, "outer", 5) == 0) ||
                (strncmp(c, "OUTER", 5) == 0))
            {
                // read three vertices
                for (i=0; i<3; ++i)
                {
                    // read line
                    c = fgets(line, 100, in);
                    SM_ASSERT(c != NULL);

                    // skip white-space
                    for (c=line; isspace(*c) && *c!='\0'; ++c) {};

                    // read x, y, z
                    sscanf(c+6, "%f %f %f", &p[0], &p[1], &p[2]);

                    // has vector been referenced before?
                    if ((vMapIt=vMap.find(p)) == vMap.end())
                    {
                        // No : add vertex and remember idx/vector mapping
                        v = mesh.addVertex((Point)p);
                        vertices[i] = v;
                        vMap[p] = v;
                    }
                    else
                    {
                        // Yes : get index from map
                        vertices[i] = vMapIt->second;
                    }
                }

                // Add face only if it is not degenerated
                if ((vertices[0] != vertices[1]) &&
                    (vertices[0] != vertices[2]) &&
                    (vertices[1] != vertices[2]))
                    mesh.addFace(vertices);
            }
        }
    }


    fclose(in);
    return true;
}


//-----------------------------------------------------------------------------


bool writeSTL(const SurfaceMesh& mesh, const std::string& filename)
{
    if (!mesh.isTriangleMesh())
    {
        std::cerr << "writeSTL: not a triangle mesh!" << std::endl;
        return false;
    }

    auto fnormals = mesh.getFaceProperty<Normal>("f:normal");
    if (!fnormals)
    {
        std::cerr << "writeSTL: no face normals present!" << std::endl;
        return false;
    }

    std::ofstream ofs(filename.c_str());
    auto points = mesh.getVertexProperty<Point>("v:point");

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
    return true;
}


//=============================================================================
} // namespace surface_mesh
//=============================================================================
