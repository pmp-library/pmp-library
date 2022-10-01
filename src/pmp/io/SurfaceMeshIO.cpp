// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/SurfaceMeshIO.h"

#include "pmp/io/read_off.h"
#include "pmp/io/helpers.h"

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
        read_obj(mesh);
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
        write_off(mesh);
    else if (ext == "obj")
        write_obj(mesh);
    else if (ext == "stl")
        write_stl(mesh);
    else
        throw IOException("Could not find writer for " + filename_);
}

void SurfaceMeshIO::read_obj(SurfaceMesh& mesh)
{
    std::array<char, 200> s;
    float x, y, z;
    std::vector<Vertex> vertices;
    std::vector<TexCoord> all_tex_coords; //individual texture coordinates
    std::vector<int>
        halfedge_tex_idx; //texture coordinates sorted for halfedges
    HalfedgeProperty<TexCoord> tex_coords =
        mesh.halfedge_property<TexCoord>("h:tex");
    bool with_tex_coord = false;

    // open file (in ASCII mode)
    FILE* in = fopen(filename_.c_str(), "r");
    if (!in)
        throw IOException("Failed to open file: " + filename_);

    // clear line once
    memset(s.data(), 0, 200);

    // parse line by line (currently only supports vertex positions & faces
    while (in && !feof(in) && fgets(s.data(), 200, in))
    {
        // comment
        if (s[0] == '#' || isspace(s[0]))
            continue;

        // vertex
        else if (strncmp(s.data(), "v ", 2) == 0)
        {
            if (sscanf(s.data(), "v %f %f %f", &x, &y, &z))
            {
                mesh.add_vertex(Point(x, y, z));
            }
        }

        // normal
        else if (strncmp(s.data(), "vn ", 3) == 0)
        {
            if (sscanf(s.data(), "vn %f %f %f", &x, &y, &z))
            {
                // problematic as it can be either a vertex property when interpolated
                // or a halfedge property for hard edges
            }
        }

        // texture coordinate
        else if (strncmp(s.data(), "vt ", 3) == 0)
        {
            if (sscanf(s.data(), "vt %f %f", &x, &y))
            {
                all_tex_coords.emplace_back(x, y);
            }
        }

        // face
        else if (strncmp(s.data(), "f ", 2) == 0)
        {
            int component(0), nv(0);
            bool end_of_vertex(false);
            char *p0, *p1(s.data() + 1);

            vertices.clear();
            halfedge_tex_idx.clear();

            // skip white-spaces
            while (*p1 == ' ')
                ++p1;

            while (p1)
            {
                p0 = p1;

                // overwrite next separator

                // skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
                while (*p1 != '/' && *p1 != '\r' && *p1 != '\n' && *p1 != ' ' &&
                       *p1 != '\0')
                    ++p1;

                // detect end of vertex
                if (*p1 != '/')
                {
                    end_of_vertex = true;
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
                    p1 = nullptr;
                }

                // read next vertex component
                if (*p0 != '\0')
                {
                    switch (component)
                    {
                        case 0: // vertex
                        {
                            int idx = atoi(p0);
                            if (idx < 0)
                                idx = mesh.n_vertices() + idx + 1;
                            vertices.emplace_back(idx - 1);
                            break;
                        }
                        case 1: // texture coord
                        {
                            int idx = atoi(p0) - 1;
                            halfedge_tex_idx.push_back(idx);
                            with_tex_coord = true;
                            break;
                        }
                        case 2: // normal
                            break;
                    }
                }

                ++component;

                if (end_of_vertex)
                {
                    component = 0;
                    nv++;
                    end_of_vertex = false;
                }
            }

            Face f = mesh.add_face(vertices);

            // add texture coordinates
            if (with_tex_coord && f.is_valid())
            {
                auto h_fit = mesh.halfedges(f);
                auto h_end = h_fit;
                unsigned v_idx = 0;
                do
                {
                    tex_coords[*h_fit] =
                        all_tex_coords.at(halfedge_tex_idx.at(v_idx));
                    ++v_idx;
                    ++h_fit;
                } while (h_fit != h_end);
            }
        }
        // clear line
        memset(s.data(), 0, 200);
    }

    // if there are no textures, delete texture property!
    if (!with_tex_coord)
    {
        mesh.remove_halfedge_property(tex_coords);
    }

    fclose(in);
}

void SurfaceMeshIO::write_obj(const SurfaceMesh& mesh)
{
    FILE* out = fopen(filename_.c_str(), "w");
    if (!out)
        throw IOException("Failed to open file: " + filename_);

    // comment
    fprintf(out, "# OBJ export from PMP\n");

    // write vertices
    auto points = mesh.get_vertex_property<Point>("v:point");
    for (auto v : mesh.vertices())
    {
        const Point& p = points[v];
        fprintf(out, "v %.10f %.10f %.10f\n", p[0], p[1], p[2]);
    }

    // write normals
    auto normals = mesh.get_vertex_property<Normal>("v:normal");
    if (normals)
    {
        for (auto v : mesh.vertices())
        {
            const Normal& n = normals[v];
            fprintf(out, "vn %.10f %.10f %.10f\n", n[0], n[1], n[2]);
        }
    }

    // write texture coordinates
    auto tex_coords = mesh.get_halfedge_property<TexCoord>("h:tex");
    if (tex_coords)
    {
        for (auto h : mesh.halfedges())
        {
            const TexCoord& pt = tex_coords[h];
            fprintf(out, "vt %.10f %.10f\n", pt[0], pt[1]);
        }
    }

    // write faces
    for (auto f : mesh.faces())
    {
        fprintf(out, "f");

        auto h = mesh.halfedges(f);
        for (auto v : mesh.vertices(f))
        {
            auto idx = v.idx() + 1;
            if (tex_coords)
            {
                // write vertex index, texCoord index and normal index
                fprintf(out, " %d/%d/%d", idx, (*h).idx() + 1, idx);
                ++h;
            }
            else
            {
                // write vertex index and normal index
                fprintf(out, " %d//%d", idx, idx);
            }
        }
        fprintf(out, "\n");
    }

    fclose(out);
}

void SurfaceMeshIO::write_off_binary(const SurfaceMesh& mesh)
{
    FILE* out = fopen(filename_.c_str(), "w");
    if (!out)
        throw IOException("Failed to open file: " + filename_);

    fprintf(out, "OFF BINARY\n");
    fclose(out);
    auto nv = (IndexType)mesh.n_vertices();
    auto nf = (IndexType)mesh.n_faces();
    auto ne = IndexType{};

    out = fopen(filename_.c_str(), "ab");
    tfwrite(out, nv);
    tfwrite(out, nf);
    tfwrite(out, ne);
    auto points = mesh.get_vertex_property<Point>("v:point");
    for (auto v : mesh.vertices())
    {
        const Point& p = points[v];
        tfwrite(out, p);
    }

    for (auto f : mesh.faces())
    {
        IndexType valence = mesh.valence(f);
        tfwrite(out, valence);
        for (auto fv : mesh.vertices(f))
            tfwrite(out, (IndexType)fv.idx());
    }
    fclose(out);
}

void SurfaceMeshIO::write_off(const SurfaceMesh& mesh)
{
    if (flags_.use_binary)
    {
        write_off_binary(mesh);
        return;
    }

    FILE* out = fopen(filename_.c_str(), "w");
    if (!out)
        throw IOException("Failed to open file: " + filename_);

    bool has_normals = false;
    bool has_texcoords = false;
    bool has_colors = false;

    auto normals = mesh.get_vertex_property<Normal>("v:normal");
    auto texcoords = mesh.get_vertex_property<TexCoord>("v:tex");
    auto colors = mesh.get_vertex_property<Color>("v:color");

    if (normals && flags_.use_vertex_normals)
        has_normals = true;
    if (texcoords && flags_.use_vertex_texcoords)
        has_texcoords = true;
    if (colors && flags_.use_vertex_colors)
        has_colors = true;

    // header
    if (has_texcoords)
        fprintf(out, "ST");
    if (has_colors)
        fprintf(out, "C");
    if (has_normals)
        fprintf(out, "N");
    fprintf(out, "OFF\n%zu %zu 0\n", mesh.n_vertices(), mesh.n_faces());

    // vertices, and optionally normals and texture coordinates
    VertexProperty<Point> points = mesh.get_vertex_property<Point>("v:point");
    for (auto v : mesh.vertices())
    {
        const Point& p = points[v];
        fprintf(out, "%.10f %.10f %.10f", p[0], p[1], p[2]);

        if (has_normals)
        {
            const Normal& n = normals[v];
            fprintf(out, " %.10f %.10f %.10f", n[0], n[1], n[2]);
        }

        if (has_colors)
        {
            const Color& c = colors[v];
            fprintf(out, " %.10f %.10f %.10f", c[0], c[1], c[2]);
        }

        if (has_texcoords)
        {
            const TexCoord& t = texcoords[v];
            fprintf(out, " %.10f %.10f", t[0], t[1]);
        }

        fprintf(out, "\n");
    }

    // faces
    for (auto f : mesh.faces())
    {
        auto nv = mesh.valence(f);
        fprintf(out, "%zu", nv);
        auto fv = mesh.vertices(f);
        auto fvend = fv;
        do
        {
            fprintf(out, " %d", (*fv).idx());
        } while (++fv != fvend);
        fprintf(out, "\n");
    }

    fclose(out);
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
