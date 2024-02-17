// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/read_off.h"

#include "pmp/io/helpers.h"
#include "pmp/exceptions.h"

namespace pmp {

void read_off_ascii(SurfaceMesh& mesh, FILE* in, const bool has_normals,
                    const bool has_texcoords, const bool has_colors, char *first_line);
void read_off_binary(SurfaceMesh& mesh, FILE* in, const bool has_normals,
                     const bool has_texcoords, const bool has_colors);

void read_off(SurfaceMesh& mesh, const std::filesystem::path& file)
{
    std::array<char, 200> line;
    bool has_texcoords = false;
    bool has_normals = false;
    bool has_colors = false;
    bool has_hcoords = false;
    bool has_dim = false;
    bool is_binary = false;

    // open file (in ASCII mode)
    FILE* in = fopen(file.string().c_str(), "r");
    if (!in)
        throw IOException("Failed to open file: " + file.string());

    // read header: [ST][C][N][4][n]OFF BINARY
    char *c = fgets(line.data(), 200, in);
    assert(c != nullptr);
    c = line.data();
    if (c[0] == 'S' && c[1] == 'T')
    {
        has_texcoords = true;
        c += 2;
    }
    if (c[0] == 'C')
    {
        has_colors = true;
        ++c;
    }
    if (c[0] == 'N')
    {
        has_normals = true;
        ++c;
    }
    if (c[0] == '4')
    {
        has_hcoords = true;
        ++c;
    }
    if (c[0] == 'n')
    {
        has_dim = true;
        ++c;
    }
    if (strncmp(c, "OFF", 3) != 0)
    {
        fclose(in);
        throw IOException("Failed to parse OFF header");
    }
    c += 3;
    if (c[0] == ' ')
        ++c;
    if (strncmp(c, "BINARY", 6) == 0) {
        is_binary = true;
        c += 6;
    }
    if (c[0] == ' ')
        ++c;

    if (has_hcoords)
    {
        fclose(in);
        throw IOException("Error: Homogeneous coordinates not supported.");
    }
    if (has_dim)
    {
        fclose(in);
        throw IOException("Error: vertex dimension != 3 not supported");
    }

    // if binary: reopen file in binary mode
    if (is_binary)
    {
        fclose(in);
        in = fopen(file.string().c_str(), "rb");
        c = fgets(line.data(), 200, in);
        assert(c != nullptr);
    }

    // read as ASCII or binary
    if (is_binary)
        read_off_binary(mesh, in, has_normals, has_texcoords, has_colors);
    else
        read_off_ascii(mesh, in, has_normals, has_texcoords, has_colors, c);

    fclose(in);
}

void read_off_ascii(SurfaceMesh& mesh, FILE* in, const bool has_normals,
                    const bool has_texcoords, const bool has_colors, char *first_line)
{
    std::array<char, 1000> line;
    char *lp = first_line;
    int nc;
    long int i, j, idx;
    long int nv, nf, ne;
    float x, y, z, r, g, b;
    Vertex v;

    // properties
    VertexProperty<Normal> normals;
    VertexProperty<TexCoord> texcoords;
    VertexProperty<Color> colors;
    if (has_normals)
        normals = mesh.vertex_property<Normal>("v:normal");
    if (has_texcoords)
        texcoords = mesh.vertex_property<TexCoord>("v:tex");
    if (has_colors)
        colors = mesh.vertex_property<Color>("v:color");

    // read line, but skip comment lines
    while(lp && (lp[0] == '#' || lp[0] == '\n')) {
        lp = fgets(line.data(), 1000, in);
    }

    // #Vertices, #Faces, #Edges
    auto items = sscanf(lp, "%ld %ld %ld\n", &nv, &nf, &ne);

    if (items < 3 || nv < 1 || nf < 1 || ne < 0)
        throw IOException("Failed to parse OFF header");

    mesh.reserve(nv, std::max(3 * nv, ne), nf);

    // read vertices: pos [normal] [color] [texcoord]
    for (i = 0; i < nv && !feof(in); ++i)
    {
        // read line, but skip comment lines
        do {
            lp = fgets(line.data(), 1000, in);
        } while(lp && (lp[0] == '#' || lp[0] == '\n'));
        lp = line.data();

        // position
        items = sscanf(lp, "%f %f %f%n", &x, &y, &z, &nc);
        assert(items == 3);
        v = mesh.add_vertex(Point(x, y, z));
        lp += nc;

        // normal
        if (has_normals)
        {
            if (sscanf(lp, "%f %f %f%n", &x, &y, &z, &nc) == 3)
            {
                normals[v] = Normal(x, y, z);
            }
            lp += nc;
        }

        // color
        if (has_colors)
        {
            if (sscanf(lp, "%f %f %f%n", &r, &g, &b, &nc) == 3)
            {
                if (r > 1.0f || g > 1.0f || b > 1.0f)
                {
                    r /= 255.0f;
                    g /= 255.0f;
                    b /= 255.0f;
                }
                colors[v] = Color(r, g, b);
            }
            lp += nc;
        }

        // tex coord
        if (has_texcoords)
        {
            items = sscanf(lp, "%f %f%n", &x, &y, &nc);
            assert(items == 2);
            texcoords[v][0] = x;
            texcoords[v][1] = y;
            lp += nc;
        }
    }

    // read faces: #N v[1] v[2] ... v[n-1]
    std::vector<Vertex> vertices;
    for (i = 0; i < nf; ++i)
    {
        // read line, but skip comment lines
        do {
            lp = fgets(line.data(), 1000, in);
        } while(lp && (lp[0] == '#' || lp[0] == '\n'));
        lp = line.data();

        // #vertices
        items = sscanf(lp, "%ld%n", &nv, &nc);
        assert(items == 1);
        if (nv < 1)
            throw IOException("Invalid index count");
        vertices.resize(nv);
        lp += nc;

        // indices
        for (j = 0; j < nv; ++j)
        {
            items = sscanf(lp, "%ld%n", &idx, &nc);
            assert(items == 1);
            if (idx < 0)
                throw IOException("Invalid index");
            vertices[j] = Vertex(idx);
            lp += nc;
        }
        try
        {
            mesh.add_face(vertices);
        }
        catch (const TopologyException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

void read_off_binary(SurfaceMesh& mesh, FILE* in, const bool has_normals,
                     const bool has_texcoords, const bool has_colors)
{
    uint32_t i, j, idx(0);
    uint32_t nv(0), nf(0), ne(0);
    vec3 p, n;
    vec2 t;
    Vertex v;

    // binary cannot (yet) read colors
    if (has_colors)
        throw IOException("Colors not supported for binary OFF file.");

    // properties
    VertexProperty<Normal> normals;
    VertexProperty<TexCoord> texcoords;
    if (has_normals)
        normals = mesh.vertex_property<Normal>("v:normal");
    if (has_texcoords)
        texcoords = mesh.vertex_property<TexCoord>("v:tex");

    // #Vertices, #Faces, #Edges
    tfread(in, nv);
    tfread(in, nf);
    tfread(in, ne);
    mesh.reserve(nv, std::max(3 * nv, ne), nf);

    // read vertices: pos [normal] [color] [texcoord]
    for (i = 0; i < nv && !feof(in); ++i)
    {
        // position
        tfread(in, p);
        v = mesh.add_vertex((Point)p);

        // normal
        if (has_normals)
        {
            tfread(in, n);
            normals[v] = (Normal)n;
        }

        // tex coord
        if (has_texcoords)
        {
            tfread(in, t);
            texcoords[v][0] = t[0];
            texcoords[v][1] = t[1];
        }
    }

    // read faces: #N v[1] v[2] ... v[n-1]
    std::vector<Vertex> vertices;
    for (i = 0; i < nf; ++i)
    {
        tfread(in, nv);
        vertices.resize(nv);
        for (j = 0; j < nv; ++j)
        {
            tfread(in, idx);
            vertices[j] = Vertex(idx);
        }
        try
        {
            mesh.add_face(vertices);
        }
        catch (const TopologyException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

} // namespace pmp