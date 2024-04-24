// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/io/read_obj.h"
#include "pmp/exceptions.h"

namespace pmp {

void read_obj(SurfaceMesh& mesh, const std::filesystem::path& file)
{
    std::array<char, 600> s;
    float x, y, z, r, g, b;
    uint vert_count = 0;
    Vertex v;
    std::vector<Vertex> vertices;
    std::vector<TexCoord> all_tex_coords; //individual texture coordinates
    std::vector<int>
        halfedge_tex_idx; //texture coordinates sorted for halfedges
    HalfedgeProperty<TexCoord> tex_coords =
        mesh.halfedge_property<TexCoord>("h:tex");
    bool with_tex_coord = false;
    VertexProperty<Color> colors;
    bool with_vert_colors = false;

    // open file (in ASCII mode)
    FILE* in = fopen(file.string().c_str(), "r");
    if (!in)
        throw IOException("Failed to open file: " + file.string());

    // clear line once
    memset(s.data(), 0, 600);

    // parse line by line (currently only supports vertex positions & faces
    while (in && !feof(in) && fgets(s.data(), 600, in))
    {
        // comment
        if ((s[0] == '#' && s[1] != 'M') || isspace(s[0]))
            continue;

        // vertex
        else if (strncmp(s.data(), "v ", 2) == 0)
        {
            if (sscanf(s.data(), "v %f %f %f %f %f %f", &x, &y, &z, &r, &g, &b))
            {
                if (!with_vert_colors)
                {
                    colors = mesh.vertex_property<Color>("v:color");
                    with_vert_colors = true;
                }
                v = mesh.add_vertex(Point(x, y, z));
                colors[v] = Color(r, g, b);
            }
            else if (sscanf(s.data(), "v %f %f %f", &x, &y, &z))
            {
                mesh.add_vertex(Point(x, y, z));
            }
        }

        // zbrush vertex color
        else if (strncmp(s.data(), "#MRGB ", 6) == 0)
        {
            if (!with_vert_colors)
            {
                colors = mesh.vertex_property<Color>("v:color");
                with_vert_colors = true;
            }
            int colors_in_line = (std::strlen(s.data()) - 7) / 8;
            for (int i = 0; i < colors_in_line; i++)
            {
                std::cout << vert_count << std::endl;
                // For these purposes, mask is ignored
                std::stringstream red, green, blue;
                int red_of_255, green_of_255, blue_of_255;

                // Read hex codes into string stream
                red << s[i * 8 + 8] << s[i * 8 + 9];
                green << s[i * 8 + 10] << s[i * 8 + 11];
                blue << s[i * 8 + 12] << s[i * 8 + 13];

                // convert string stream into int
                red >> std::hex >> red_of_255;
                green >> std::hex >> green_of_255;
                blue >> std::hex >> blue_of_255;

                // convert 1 - 255 int scale to 0-1 float scale
                r = red_of_255 / 255;
                g = green_of_255 / 255;
                b = blue_of_255 / 255;

                // So horribly slow, if possible replace with direct index access
                for (Vertex vi : mesh.vertices())
                {
                    if (vi.idx() == vert_count)
                    {
                        v = vi;
                        break;
                    }
                }
                colors[v] = Color(r, g, b);
                vert_count++;

                red.clear();
                green.clear();
                blue.clear();
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
            int component(0);
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
                    end_of_vertex = false;
                }
            }

            Face f;
            try
            {
                f = mesh.add_face(vertices);
            }
            catch (const TopologyException& e)
            {
                std::cerr << e.what();
            }

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

    if (!with_vert_colors)
    {
        mesh.remove_vertex_property(colors);
    }

    fclose(in);
}

} // namespace pmp
