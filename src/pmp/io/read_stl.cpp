// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/read_stl.h"

#include "pmp/io/helpers.h"

#include <limits>
#include <map>

namespace pmp {

// comparison operator for vec3
struct CompareVec3
{
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

    Scalar eps_{std::numeric_limits<Scalar>::min()};
};

void read_stl(SurfaceMesh& mesh, const std::filesystem::path& file)
{
    std::array<char, 100> line;
    unsigned int i, nT(0);
    vec3 p;
    Vertex v;
    std::vector<Vertex> vertices(3);
    size_t n_items(0);

    CompareVec3 comp;
    std::map<vec3, Vertex, CompareVec3> vertex_map(comp);

    // open file (in ASCII mode)
    FILE* in = fopen(file.string().c_str(), "r");
    if (!in)
        throw IOException("Failed to open file: " + file.string());

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
        in = fopen(file.string().c_str(), "rb");
        if (!in)
            throw IOException("Failed to open file: " + file.string());

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
                auto it = vertex_map.find(p);
                if (it == vertex_map.end())
                {
                    // No : add vertex and remember idx/vector mapping
                    v = mesh.add_vertex((Point)p);
                    vertices[i] = v;
                    vertex_map[p] = v;
                }
                else
                {
                    // Yes : get index from map
                    vertices[i] = it->second;
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
                    auto it = vertex_map.find(p);
                    if (it == vertex_map.end())
                    {
                        // No : add vertex and remember idx/vector mapping
                        v = mesh.add_vertex((Point)p);
                        vertices[i] = v;
                        vertex_map[p] = v;
                    }
                    else
                    {
                        // Yes : get index from map
                        vertices[i] = it->second;
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

} // namespace pmp