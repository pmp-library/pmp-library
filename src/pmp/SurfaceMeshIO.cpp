// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/SurfaceMeshIO.h"

#include <clocale>
#include <cstring>
#include <cctype>

#include <fstream>
#include <limits>

#include <rply.h>

// helper function
template <typename T>
void tfread(FILE* in, const T& t)
{
    size_t n_items = fread((char*)&t, 1, sizeof(t), in);
    PMP_ASSERT(n_items > 0);
}

// helper function
template <typename T>
void tfwrite(FILE* out, const T& t)
{
    size_t n_items = fwrite((char*)&t, 1, sizeof(t), out);
    PMP_ASSERT(n_items > 0);
}

namespace pmp {

bool SurfaceMeshIO::read(SurfaceMesh& mesh)
{
    std::setlocale(LC_NUMERIC, "C");

    // clear mesh before reading from file
    mesh.clear();

    // extract file extension
    std::string::size_type dot(filename_.rfind("."));
    if (dot == std::string::npos)
        return false;
    std::string ext = filename_.substr(dot + 1, filename_.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == "off")
    {
        return read_off(mesh);
    }
    else if (ext == "obj")
    {
        return read_obj(mesh);
    }
    else if (ext == "stl")
    {
        return read_stl(mesh);
    }
    else if (ext == "ply")
    {
        return read_ply(mesh);
    }
    else if (ext == "pmp")
    {
        return read_pmp(mesh);
    }
    else if (ext == "xyz")
    {
        return read_xyz(mesh);
    }
    else if (ext == "agi")
    {
        return read_agi(mesh);
    }

    // we didn't find a reader module
    return false;
}

bool SurfaceMeshIO::write(const SurfaceMesh& mesh)
{
    // extract file extension
    std::string::size_type dot(filename_.rfind("."));
    if (dot == std::string::npos)
        return false;
    std::string ext = filename_.substr(dot + 1, filename_.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    // extension determines reader
    if (ext == "off")
    {
        return write_off(mesh);
    }
    else if (ext == "obj")
    {
        return write_obj(mesh);
    }
    else if (ext == "stl")
    {
        return write_stl(mesh);
    }
    else if (ext == "ply")
    {
        return write_ply(mesh);
    }
    else if (ext == "pmp")
    {
        return write_pmp(mesh);
    }
    else if (ext == "xyz")
    {
        return write_xyz(mesh);
    }

    // we didn't find a writer module
    return false;
}

bool SurfaceMeshIO::read_obj(SurfaceMesh& mesh)
{
    char s[200];
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
        return false;

    // clear line once
    memset(&s, 0, 200);

    // parse line by line (currently only supports vertex positions & faces
    while (in && !feof(in) && fgets(s, 200, in))
    {
        // comment
        if (s[0] == '#' || isspace(s[0]))
            continue;

        // vertex
        else if (strncmp(s, "v ", 2) == 0)
        {
            if (sscanf(s, "v %f %f %f", &x, &y, &z))
            {
                mesh.add_vertex(Point(x, y, z));
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
                all_tex_coords.emplace_back(x, y);
            }
        }

        // face
        else if (strncmp(s, "f ", 2) == 0)
        {
            int component(0), nv(0);
            bool end_of_vertex(false);
            char *p0, *p1(s + 1);

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
                            vertices.emplace_back(atoi(p0) - 1);
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
            if (with_tex_coord)
            {
                SurfaceMesh::HalfedgeAroundFaceCirculator h_fit =
                    mesh.halfedges(f);
                SurfaceMesh::HalfedgeAroundFaceCirculator h_end = h_fit;
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
        memset(&s, 0, 200);
    }

    // if there are no textures, delete texture property!
    if (!with_tex_coord)
    {
        mesh.remove_halfedge_property(tex_coords);
    }

    fclose(in);
    return true;
}

bool SurfaceMeshIO::write_obj(const SurfaceMesh& mesh)
{
    FILE* out = fopen(filename_.c_str(), "w");
    if (!out)
        return false;

    // comment
    fprintf(out, "# OBJ export from SurfaceMesh\n");

    //vertices
    VertexProperty<Point> points = mesh.get_vertex_property<Point>("v:point");
    for (SurfaceMesh::VertexIterator vit = mesh.vertices_begin();
         vit != mesh.vertices_end(); ++vit)
    {
        const Point& p = points[*vit];
        fprintf(out, "v %.10f %.10f %.10f\n", p[0], p[1], p[2]);
    }

    //normals
    VertexProperty<Point> normals = mesh.get_vertex_property<Point>("v:normal");
    if (normals)
    {
        for (SurfaceMesh::VertexIterator vit = mesh.vertices_begin();
             vit != mesh.vertices_end(); ++vit)
        {
            const Point& p = normals[*vit];
            fprintf(out, "vn %.10f %.10f %.10f\n", p[0], p[1], p[2]);
        }
    }

    // optional texture coordinates
    // do we have them?
    std::vector<std::string> hprops = mesh.halfedge_properties();
    bool with_tex_coord = false;
    auto hpropEnd = hprops.end();
    auto hpropStart = hprops.begin();
    while (hpropStart != hpropEnd)
    {
        if (0 == (*hpropStart).compare("h:tex"))
        {
            with_tex_coord = true;
        }
        ++hpropStart;
    }

    //if so then add
    if (with_tex_coord)
    {
        HalfedgeProperty<TexCoord> texCoord =
            mesh.get_halfedge_property<TexCoord>("h:tex");
        for (SurfaceMesh::HalfedgeIterator hit = mesh.halfedges_begin();
             hit != mesh.halfedges_end(); ++hit)
        {
            const TexCoord& pt = texCoord[*hit];
            fprintf(out, "vt %.10f %.10f \n", pt[0], pt[1]);
        }
    }

    //faces
    for (SurfaceMesh::FaceIterator fit = mesh.faces_begin();
         fit != mesh.faces_end(); ++fit)
    {
        fprintf(out, "f");
        SurfaceMesh::VertexAroundFaceCirculator fvit = mesh.vertices(*fit),
                                                fvend = fvit;
        SurfaceMesh::HalfedgeAroundFaceCirculator fhit = mesh.halfedges(*fit);
        do
        {
            if (with_tex_coord)
            {
                // write vertex index, texCoord index and normal index
                fprintf(out, " %d/%d/%d", (*fvit).idx() + 1, (*fhit).idx() + 1,
                        (*fvit).idx() + 1);
                ++fhit;
            }
            else
            {
                // write vertex index and normal index
                fprintf(out, " %d//%d", (*fvit).idx() + 1, (*fvit).idx() + 1);
            }
        } while (++fvit != fvend);
        fprintf(out, "\n");
    }

    fclose(out);
    return true;
}

bool read_off_ascii(SurfaceMesh& mesh, FILE* in, const bool has_normals,
                    const bool has_texcoords, const bool has_colors)
{
    char line[1000], *lp;
    int nc;
    unsigned int i, j, items, idx;
    unsigned int nv, nf, ne;
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

    // #Vertice, #Faces, #Edges
    items = fscanf(in, "%d %d %d\n", (int*)&nv, (int*)&nf, (int*)&ne);
    PMP_ASSERT(items);

    mesh.reserve(nv, std::max(3 * nv, ne), nf);

    // read vertices: pos [normal] [color] [texcoord]
    for (i = 0; i < nv && !feof(in); ++i)
    {
        // read line
        lp = fgets(line, 1000, in);
        lp = line;

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
        // read line
        lp = fgets(line, 1000, in);
        lp = line;

        // #vertices
        items = sscanf(lp, "%d%n", (int*)&nv, &nc);
        assert(items == 1);
        vertices.resize(nv);
        lp += nc;

        // indices
        for (j = 0; j < nv; ++j)
        {
            items = sscanf(lp, "%d%n", (int*)&idx, &nc);
            assert(items == 1);
            vertices[j] = Vertex(idx);
            lp += nc;
        }
        if (vertices.size() == nv)
            mesh.add_face(vertices);
        else
            std::cerr << "OFF: fail to read face " << i << std::endl;
    }

    return true;
}

bool read_off_binary(SurfaceMesh& mesh, FILE* in, const bool has_normals,
                     const bool has_texcoords, const bool has_colors)
{
    IndexType i, j, idx(0);
    IndexType nv(0), nf(0), ne(0);
    Point p, n, c;
    vec2 t;
    Vertex v;

    // binary cannot (yet) read colors
    if (has_colors)
        return false;

    // properties
    VertexProperty<Normal> normals;
    VertexProperty<TexCoord> texcoords;
    if (has_normals)
        normals = mesh.vertex_property<Normal>("v:normal");
    if (has_texcoords)
        texcoords = mesh.vertex_property<TexCoord>("v:tex");

    // #Vertice, #Faces, #Edges
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
        mesh.add_face(vertices);
    }

    return true;
}

bool SurfaceMeshIO::write_off_binary(const SurfaceMesh& mesh)
{
    FILE* out = fopen(filename_.c_str(), "w");
    if (!out)
        return false;

    fprintf(out, "OFF BINARY\n");
    fclose(out);
    IndexType nv = (IndexType)mesh.n_vertices();
    IndexType nf = (IndexType)mesh.n_faces();
    IndexType ne = 0;
    ;

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
        IndexType nv = mesh.valence(f);
        tfwrite(out, nv);
        for (auto fv : mesh.vertices(f))
            tfwrite(out, (IndexType)fv.idx());
    }
    fclose(out);
    return true;
}

bool SurfaceMeshIO::read_off(SurfaceMesh& mesh)
{
    char line[200];
    bool has_texcoords = false;
    bool has_normals = false;
    bool has_colors = false;
    bool has_hcoords = false;
    bool has_dim = false;
    bool is_binary = false;

    // open file (in ASCII mode)
    FILE* in = fopen(filename_.c_str(), "r");
    if (!in)
        return false;

    // read header: [ST][C][N][4][n]OFF BINARY
    char* c = fgets(line, 200, in);
    assert(c != nullptr);
    c = line;
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
        return false;
    } // no OFF
    if (strncmp(c + 4, "BINARY", 6) == 0)
        is_binary = true;

    // homogeneous coords, and vertex dimension != 3 are not supported
    if (has_hcoords || has_dim)
    {
        fclose(in);
        return false;
    }

    // if binary: reopen file in binary mode
    if (is_binary)
    {
        fclose(in);
        in = fopen(filename_.c_str(), "rb");
        c = fgets(line, 200, in);
        assert(c != nullptr);
    }

    // read as ASCII or binary
    bool ok = (is_binary ? read_off_binary(mesh, in, has_normals, has_texcoords,
                                           has_colors)
                         : read_off_ascii(mesh, in, has_normals, has_texcoords,
                                          has_colors));

    fclose(in);
    return ok;
}

bool SurfaceMeshIO::write_off(const SurfaceMesh& mesh)
{
    if (flags_.use_binary)
        return write_off_binary(mesh);

    FILE* out = fopen(filename_.c_str(), "w");
    if (!out)
        return false;

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
    for (SurfaceMesh::VertexIterator vit = mesh.vertices_begin();
         vit != mesh.vertices_end(); ++vit)
    {
        const Point& p = points[*vit];
        fprintf(out, "%.10f %.10f %.10f", p[0], p[1], p[2]);

        if (has_normals)
        {
            const Normal& n = normals[*vit];
            fprintf(out, " %.10f %.10f %.10f", n[0], n[1], n[2]);
        }

        if (has_colors)
        {
            const Color& c = colors[*vit];
            fprintf(out, " %.10f %.10f %.10f", c[0], c[1], c[2]);
        }

        if (has_texcoords)
        {
            const TexCoord& t = texcoords[*vit];
            fprintf(out, " %.10f %.10f", t[0], t[1]);
        }

        fprintf(out, "\n");
    }

    // faces
    for (SurfaceMesh::FaceIterator fit = mesh.faces_begin();
         fit != mesh.faces_end(); ++fit)
    {
        int nv = mesh.valence(*fit);
        fprintf(out, "%d", nv);
        SurfaceMesh::VertexAroundFaceCirculator fvit = mesh.vertices(*fit),
                                                fvend = fvit;
        do
        {
            fprintf(out, " %d", (*fvit).idx());
        } while (++fvit != fvend);
        fprintf(out, "\n");
    }

    fclose(out);
    return true;
}

bool SurfaceMeshIO::read_pmp(SurfaceMesh& mesh)
{
    // open file (in binary mode)
    FILE* in = fopen(filename_.c_str(), "rb");
    if (!in)
        return false;

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
    return true;
}

bool SurfaceMeshIO::read_xyz(SurfaceMesh& mesh)
{
    // open file (in ASCII mode)
    FILE* in = fopen(filename_.c_str(), "r");
    if (!in)
        return false;

    // add normal property
    // \todo this adds property even if no normals present. change it.
    auto vnormal = mesh.vertex_property<Normal>("v:normal");

    char line[200];
    float x, y, z;
    float nx, ny, nz;
    int n;
    Vertex v;

    // read data
    while (in && !feof(in) && fgets(line, 200, in))
    {
        n = sscanf(line, "%f %f %f %f %f %f", &x, &y, &z, &nx, &ny, &nz);
        if (n >= 3)
        {
            v = mesh.add_vertex(Point(x, y, z));
            if (n >= 6)
            {
                vnormal[v] = Normal(nx, ny, nz);
            }
        }
    }

    fclose(in);
    return true;
}

// \todo remove duplication with read_xyz
bool SurfaceMeshIO::read_agi(SurfaceMesh& mesh)
{
    // open file (in ASCII mode)
    FILE* in = fopen(filename_.c_str(), "r");
    if (!in)
        return false;

    // add normal property
    auto normal = mesh.vertex_property<Normal>("v:normal");
    auto color = mesh.vertex_property<Color>("v:color");

    char line[200];
    float x, y, z;
    float nx, ny, nz;
    float r, g, b;
    int n;
    Vertex v;

    // read data
    while (in && !feof(in) && fgets(line, 200, in))
    {
        n = sscanf(line, "%f %f %f %f %f %f %f %f %f", &x, &y, &z, &r, &g, &b,
                   &nx, &ny, &nz);
        if (n == 9)
        {
            v = mesh.add_vertex(Point(x, y, z));
            normal[v] = Normal(nx, ny, nz);
            color[v] = Color(r / 255.0, g / 255.0, b / 255.0);
        }
    }

    fclose(in);
    return true;
}

bool SurfaceMeshIO::write_pmp(const SurfaceMesh& mesh)
{
    // open file (in binary mode)
    FILE* out = fopen(filename_.c_str(), "wb");
    if (!out)
        return false;

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
    return true;
}

// helper to assemble vertex data
static int vertexCallback(p_ply_argument argument)
{
    long idx;
    void* pdata;
    ply_get_argument_user_data(argument, &pdata, &idx);

    auto* mesh = (pmp::SurfaceMesh*)pdata;
    auto point = mesh->get_object_property<pmp::Point>("g:point");
    point[0][idx] = ply_get_argument_value(argument);

    if (idx == 2)
        mesh->add_vertex(point[0]);

    return 1;
}

// helper to assemble face data
static int faceCallback(p_ply_argument argument)
{
    long length, value_index;
    void* pdata;
    long idata;
    ply_get_argument_user_data(argument, &pdata, &idata);
    ply_get_argument_property(argument, nullptr, &length, &value_index);

    auto* mesh = (pmp::SurfaceMesh*)pdata;
    auto vertices =
        mesh->get_object_property<std::vector<pmp::Vertex>>("g:vertices");

    if (value_index == 0)
        vertices[0].clear();

    pmp::IndexType idx = (pmp::IndexType)ply_get_argument_value(argument);
    vertices[0].push_back(pmp::Vertex(idx));

    if (value_index == length - 1)
        mesh->add_face(vertices[0]);

    return 1;
}

bool SurfaceMeshIO::read_ply(SurfaceMesh& mesh)
{
    // add object properties to hold temporary data
    auto point = mesh.add_object_property<Point>("g:point");
    auto vertices = mesh.add_object_property<std::vector<Vertex>>("g:vertices");

    // open file, read header
    p_ply ply = ply_open(filename_.c_str(), nullptr, 0, nullptr);

    if (!ply)
        return false;

    if (!ply_read_header(ply))
        return false;

    // setup callbacks for basic properties
    ply_set_read_cb(ply, "vertex", "x", vertexCallback, &mesh, 0);
    ply_set_read_cb(ply, "vertex", "y", vertexCallback, &mesh, 1);
    ply_set_read_cb(ply, "vertex", "z", vertexCallback, &mesh, 2);

    ply_set_read_cb(ply, "face", "vertex_indices", faceCallback, &mesh, 0);

    // read the data
    if (!ply_read(ply))
        return false;

    ply_close(ply);

    // clean-up properties
    mesh.remove_object_property(point);
    mesh.remove_object_property(vertices);

    return true;
}

bool SurfaceMeshIO::write_ply(const SurfaceMesh& mesh)
{
    e_ply_storage_mode mode = flags_.use_binary ? PLY_LITTLE_ENDIAN : PLY_ASCII;
    p_ply ply = ply_create(filename_.c_str(), mode, nullptr, 0, nullptr);

    ply_add_comment(ply, "File written with pmp-library");
    ply_add_element(ply, "vertex", mesh.n_vertices());
    ply_add_scalar_property(ply, "x", PLY_FLOAT);
    ply_add_scalar_property(ply, "y", PLY_FLOAT);
    ply_add_scalar_property(ply, "z", PLY_FLOAT);
    ply_add_element(ply, "face", mesh.n_faces());
    ply_add_property(ply, "vertex_indices", PLY_LIST, PLY_UCHAR, PLY_INT);
    ply_write_header(ply);

    // write vertices
    auto points = mesh.get_vertex_property<Point>("v:point");
    for (auto v : mesh.vertices())
    {
        ply_write(ply, points[v][0]);
        ply_write(ply, points[v][1]);
        ply_write(ply, points[v][2]);
    }

    // write faces
    for (auto f : mesh.faces())
    {
        ply_write(ply, mesh.valence(f));
        for (auto fv : mesh.vertices(f))
            ply_write(ply, fv.idx());
    }

    ply_close(ply);
    return true;
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

bool SurfaceMeshIO::read_stl(SurfaceMesh& mesh)
{
    char line[100], *c;
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
        return false;

    // ASCII or binary STL?
    c = fgets(line, 6, in);
    PMP_ASSERT(c != nullptr);
    const bool binary =
        ((strncmp(line, "SOLID", 5) != 0) && (strncmp(line, "solid", 5) != 0));

    // parse binary STL
    if (binary)
    {
        // re-open file in binary mode
        fclose(in);
        in = fopen(filename_.c_str(), "rb");
        if (!in)
            return false;

        // skip dummy header
        n_items = fread(line, 1, 80, in);
        PMP_ASSERT(n_items > 0);

        // read number of triangles
        tfread(in, nT);

        // read triangles
        while (nT)
        {
            // skip triangle normal
            n_items = fread(line, 1, 12, in);
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

            n_items = fread(line, 1, 2, in);
            PMP_ASSERT(n_items > 0);
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
            for (c = line; isspace(*c) && *c != '\0'; ++c)
            {
            };

            // face begins
            if ((strncmp(c, "outer", 5) == 0) || (strncmp(c, "OUTER", 5) == 0))
            {
                // read three vertices
                for (i = 0; i < 3; ++i)
                {
                    // read line
                    c = fgets(line, 100, in);
                    PMP_ASSERT(c != nullptr);

                    // skip white-space
                    for (c = line; isspace(*c) && *c != '\0'; ++c)
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
    return true;
}

bool SurfaceMeshIO::write_stl(const SurfaceMesh& mesh)
{
    if (!mesh.is_triangle_mesh())
    {
        std::cerr << "write_stl: not a triangle mesh!" << std::endl;
        return false;
    }

    auto fnormals = mesh.get_face_property<Normal>("f:normal");
    if (!fnormals)
    {
        std::cerr << "write_stl: no face normals present!" << std::endl;
        return false;
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
    return true;
}

bool SurfaceMeshIO::write_xyz(const SurfaceMesh& mesh)
{
    std::ofstream ofs(filename_);
    if (!ofs)
        return false;

    auto vnormal = mesh.get_vertex_property<Normal>("v:normal");
    for (auto v : mesh.vertices())
    {
        ofs << mesh.position(v);
        ofs << " ";
        if (vnormal)
        {
            ofs << vnormal[v];
        }
        ofs << std::endl;
    }

    ofs.close();
    return true;
}

} // namespace pmp
