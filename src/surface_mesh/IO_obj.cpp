//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011-2013 by Graphics & Geometry Group, Bielefeld University
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public License
// as published by the Free Software Foundation, version 2.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================


//== INCLUDES =================================================================

#include <surface_mesh/IO.h>

#include <cstdio>


//== NAMESPACES ===============================================================


namespace surface_mesh {


//== IMPLEMENTATION ===========================================================


bool readOBJ(SurfaceMesh& mesh, const std::string& filename)
{
    char   s[200];
    float  x, y, z;
    std::vector<SurfaceMesh::Vertex>  vertices;
    std::vector<TextureCoordinate> allTexCoords;   //individual texture coordinates
    std::vector<int> halfedgeTexIdx; //texture coordinates sorted for halfedges
    SurfaceMesh::HalfedgeProperty <TextureCoordinate> texCoords = mesh.halfedgeProperty<TextureCoordinate>("h:texcoord");
    bool withTexCoord=false;

    // clear mesh
    mesh.clear();


    // open file (in ASCII mode)
    FILE* in = fopen(filename.c_str(), "r");
    if (!in) return false;


    // clear line once
    memset(&s, 0, 200);


    // parse line by line (currently only supports vertex positions & faces
    while(in && !feof(in) && fgets(s, 200, in))
    {
        // comment
        if (s[0] == '#' || isspace(s[0])) continue;

        // vertex
        else if (strncmp(s, "v ", 2) == 0)
        {
            if (sscanf(s, "v %f %f %f", &x, &y, &z))
            {
                mesh.addVertex(Point(x,y,z));
            }
        }
        // normal
        else if (strncmp(s, "vn ", 3) == 0)
        {
          if (sscanf(s, "vn %f %f %f", &x, &y, &z))
          {
            // problematic as it can be either a vertex property when interpolated
            // or a halfedge property for hard edges
          }
        }

        // texture coordinate
        else if (strncmp(s, "vt ", 3) == 0)
        {
          if (sscanf(s, "vt %f %f", &x, &y))
          {
            z=1;
            allTexCoords.push_back(TextureCoordinate(x,y,z));
          }
        }

        // face
        else if (strncmp(s, "f ", 2) == 0)
        {
          int component(0), nV(0);
          bool endOfVertex(false);
          char *p0, *p1(s+1);

          vertices.clear();
          halfedgeTexIdx.clear();

          // skip white-spaces
          while (*p1==' ') ++p1;

          while (p1)
          {
            p0 = p1;

            // overwrite next separator

            // skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
            while (*p1!='/' && *p1!='\r' && *p1!='\n' && *p1!=' ' && *p1!='\0') ++p1;

            // detect end of vertex
            if (*p1 != '/')
            {
              endOfVertex = true;
            }

            // replace separator by '\0'
            if (*p1 != '\0')
            {
              *p1 = '\0';
              p1++; // point to next token
            }

            // detect end of line and break
            if (*p1 == '\0' || *p1 == '\n')
            {
              p1 = 0;
            }

            // read next vertex component
            if (*p0 != '\0')
            {
              switch (component)
              {
                case 0: // vertex
                {
                  vertices.push_back( SurfaceMesh::Vertex(atoi(p0) - 1) );
                  break;
                }
                case 1: // texture coord
                {
                  int idx = atoi(p0)-1;
                  halfedgeTexIdx.push_back(idx);
                  withTexCoord=true;
                  break;
                }
                case 2: // normal
                  break;
              }
            }

            ++component;

            if (endOfVertex)
            {
              component = 0;
              nV++;
              endOfVertex = false;
            }
          }

          SurfaceMesh::Face f=mesh.addFace(vertices);


          // add texture coordinates
          if(withTexCoord)
          {
              SurfaceMesh::HalfedgeAroundFaceCirculator h_fit = mesh.halfedges(f);
              SurfaceMesh::HalfedgeAroundFaceCirculator h_end = h_fit;
              unsigned v_idx =0;
              do
              {
                  texCoords[*h_fit]=allTexCoords.at(halfedgeTexIdx.at(v_idx));
                  ++v_idx;
                  ++h_fit;
              }
              while(h_fit!=h_end);
          }
        }
        // clear line
        memset(&s, 0, 200);
    }

    fclose(in);
    return true;
}


//-----------------------------------------------------------------------------


bool writeOBJ(const SurfaceMesh& mesh, const std::string& filename)
{
    FILE* out = fopen(filename.c_str(), "w");
    if (!out)
        return false;

    // comment
    fprintf(out, "# OBJ export from SurfaceMesh\n");

    //vertices
    SurfaceMesh::VertexProperty<Point> points = mesh.getVertexProperty<Point>("v:point");
    for (SurfaceMesh::VertexIterator vit=mesh.verticesBegin(); vit!=mesh.verticesEnd(); ++vit)
    {
        const Point& p = points[*vit];
        fprintf(out, "v %.10f %.10f %.10f\n", p[0], p[1], p[2]);
    }

    //normals
    SurfaceMesh::VertexProperty<Point> normals = mesh.getVertexProperty<Point>("v:normal");
    if(normals)
    {
        for (SurfaceMesh::VertexIterator vit=mesh.verticesBegin(); vit!=mesh.verticesEnd(); ++vit)
        {
            const Point& p = normals[*vit];
            fprintf(out, "vn %.10f %.10f %.10f\n", p[0], p[1], p[2]);
        }
    }

    // optional texture coordinates
    // do we have them?
    std::vector<std::string> hprops= mesh.halfedgeProperties();
    bool withTexCoord = false;
    std::vector<std::string>::iterator hpropEnd = hprops.end();
    std::vector<std::string>::iterator hpropStart= hprops.begin();
    while(hpropStart!=hpropEnd)
    {
        if(0==(*hpropStart).compare("h:texcoord"))
        {
            withTexCoord=true;
        }
        ++hpropStart;
    }

    //if so then add
    if(withTexCoord)
    {
        SurfaceMesh::HalfedgeProperty<TextureCoordinate> texCoord = mesh.getHalfedgeProperty<TextureCoordinate>("h:texcoord");
        for (SurfaceMesh::HalfedgeIterator hit=mesh.halfedgesBegin(); hit!=mesh.halfedgesEnd(); ++hit)
        {
            const TextureCoordinate& pt = texCoord[*hit];
            fprintf(out, "vt %.10f %.10f %.10f\n", pt[0], pt[1], pt[2]);
        }
    }

    //faces
    for (SurfaceMesh::FaceIterator fit=mesh.facesBegin(); fit!=mesh.facesEnd(); ++fit)
    {
        fprintf(out, "f");
        SurfaceMesh::VertexAroundFaceCirculator fvit=mesh.vertices(*fit), fvend=fvit;
        SurfaceMesh::HalfedgeAroundFaceCirculator fhit=mesh.halfedges(*fit);
        do
        {
            if(withTexCoord)
            {
                // write vertex index, texCoord index and normal index
                fprintf(out, " %d/%d/%d", (*fvit).idx()+1, (*fhit).idx()+1, (*fvit).idx()+1);
                ++fhit;
            }
            else
            {
                // write vertex index and normal index
                fprintf(out, " %d//%d", (*fvit).idx()+1, (*fvit).idx()+1);
            }
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
