// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/SurfaceMeshIO.h"

#include "pmp/io/helpers.h"
#include "pmp/io/read_obj.h"
#include "pmp/io/read_off.h"
#include "pmp/io/write_obj.h"
#include "pmp/io/write_off.h"

#include <clocale>
#include <cstring>
#include <cctype>

#include <algorithm>
#include <map>
#include <fstream>
#include <limits>

namespace pmp {

void read(SurfaceMesh& mesh, const std::string& filename, const IOFlags& flags)
{
    SurfaceMeshIO reader(filename, flags);
    reader.read(mesh);
}

void write(const SurfaceMesh& mesh, const std::string& filename,
           const IOFlags& flags)
{
    SurfaceMeshIO writer(filename, flags);
    writer.write(mesh);
}

void SurfaceMeshIO::read(SurfaceMesh& mesh)
{
    std::setlocale(LC_NUMERIC, "C");

    // clear mesh before reading from file
    mesh.clear();

    // extract file extension
    std::string::size_type dot(filename_.rfind("."));
    if (dot == std::string::npos)
        throw IOException("Could not determine file extension!");
    std::string ext = filename_.substr(dot + 1, filename_.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == "off")
        read_off(mesh, filename_);
    else if (ext == "obj")
        read_obj(mesh, filename_);
    else if (ext == "stl")
        read_stl(mesh);
    else
        throw IOException("Could not find reader for " + filename_);
}

void SurfaceMeshIO::write(const SurfaceMesh& mesh)
{
    // extract file extension
    std::string::size_type dot(filename_.rfind("."));
    if (dot == std::string::npos)
        throw IOException("Could not determine file extension!");
    std::string ext = filename_.substr(dot + 1, filename_.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == "off")
        write_off(mesh, filename_, flags_);
    else if (ext == "obj")
        write_obj(mesh, filename_, flags_);
    else if (ext == "stl")
        write_stl(mesh);
    else
        throw IOException("Could not find writer for " + filename_);
}

// helper class for STL reader
class CmpVec
{
public:
    CmpVec(Scalar eps = std::numeric_limits<Scalar>::min()) : eps_(eps) {}

    bool operator()(const vec3& v0, const vec3& v1) const
    {
        if (fabs(v0[0] - v1[0]) <= eps_)
        {
            if (fabs(v0[1] - v1[1]) <= eps_)
            {
                return (v0[2] < v1[2] - eps_);
            }
            else
                return (v0[1] < v1[1] - eps_);
        }
        else
            return (v0[0] < v1[0] - eps_);
    }

private:
    Scalar eps_;
};

void SurfaceMeshIO::read_stl(SurfaceMesh& mesh)
{
    std::array<char, 100> line;
    unsigned int i, nT(0);
    vec3 p;
    Vertex v;
    std::vector<Vertex> vertices(3);
    size_t n_items(0);

    CmpVec comp(std::numeric_limits<Scalar>::min());
    std::map<vec3, Vertex, CmpVec> vMap(comp);
    std::map<vec3, Vertex, CmpVec>::iterator vMapIt;

    // open file (in ASCII mode)
    FILE* in = fopen(filename_.c_str(), "r");
    if (!in)
        throw IOException("Failed to open file: " + filename_);

    // ASCII or binary STL?
    auto c = fgets(line.data(), 6, in);
    PMP_ASSERT(c != nullptr);
    const bool binary = ((strncmp(line.data(), "SOLID", 5) != 0) &&
                         (strncmp(line.data(), "solid", 5) != 0));

    // parse binary STL
    if (binary)
    {
        // re-open file in binary mode
        fclose(in);
        in = fopen(filename_.c_str(), "rb");
        if (!in)
            throw IOException("Failed to open file: " + filename_);

        // skip dummy header
        n_items = fread(line.data(), 1, 80, in);
        PMP_ASSERT(n_items > 0);

        // read number of triangles
        tfread(in, nT);

        // read triangles
        while (nT)
        {
            // skip triangle normal
            n_items = fread(line.data(), 1, 12, in);
            PMP_ASSERT(n_items > 0);
            // triangle's vertices
            for (i = 0; i < 3; ++i)
            {
                tfread(in, p);

                // has vector been referenced before?
                if ((vMapIt = vMap.find(p)) == vMap.end())
                {
                    // No : add vertex and remember idx/vector mapping
                    v = mesh.add_vertex((Point)p);
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
            if ((vertices[0] != vertices[1]) && (vertices[0] != vertices[2]) &&
                (vertices[1] != vertices[2]))
                mesh.add_face(vertices);

            n_items = fread(line.data(), 1, 2, in);
            PMP_ASSERT(n_items > 0);
            --nT;
        }
    }

    // parse ASCII STL
    else
    {
        // parse line by line
        while (in && !feof(in) && fgets(line.data(), 100, in))
        {
            // skip white-space
            for (c = line.data(); isspace(*c) && *c != '\0'; ++c)
            {
            };

            // face begins
            if ((strncmp(c, "outer", 5) == 0) || (strncmp(c, "OUTER", 5) == 0))
            {
                // read three vertices
                for (i = 0; i < 3; ++i)
                {
                    // read line
                    c = fgets(line.data(), 100, in);
                    PMP_ASSERT(c != nullptr);

                    // skip white-space
                    for (c = line.data(); isspace(*c) && *c != '\0'; ++c)
                    {
                    };

                    // read x, y, z
                    sscanf(c + 6, "%f %f %f", &p[0], &p[1], &p[2]);

                    // has vector been referenced before?
                    if ((vMapIt = vMap.find(p)) == vMap.end())
                    {
                        // No : add vertex and remember idx/vector mapping
                        v = mesh.add_vertex((Point)p);
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
                    mesh.add_face(vertices);
            }
        }
    }

    fclose(in);
}

void SurfaceMeshIO::write_stl(const SurfaceMesh& mesh)
{
    if (!mesh.is_triangle_mesh())
    {
        auto what = "SurfaceMeshIO::write_stl: Not a triangle mesh.";
        throw InvalidInputException(what);
    }

    auto fnormals = mesh.get_face_property<Normal>("f:normal");
    if (!fnormals)
    {
        auto what = "SurfaceMeshIO::write_stl: No face normals present.";
        throw InvalidInputException(what);
    }

    std::ofstream ofs(filename_.c_str());
    auto points = mesh.get_vertex_property<Point>("v:point");

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
}

} // namespace pmp
