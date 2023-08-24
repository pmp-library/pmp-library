// Copyright 2011-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include <pmp/visualization/mesh_viewer.h>
#include <pmp/algorithms/hole_filling.h>
#include <imgui.h>

using namespace pmp;

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height);

protected:
    void process_imgui() override;
};

Viewer::Viewer(const char* title, int width, int height)
    : MeshViewer(title, width, height)
{
    set_draw_mode("Hidden Line");
    crease_angle_ = 0.0;
}

void Viewer::process_imgui()
{
    MeshViewer::process_imgui();

    if (ImGui::Button("Close smallest hole"))
    {
        // find smallest hole
        Halfedge hmin;
        unsigned int lmin(mesh_.n_halfedges());
        for (auto h : mesh_.halfedges())
        {
            if (mesh_.is_boundary(h))
            {
                Scalar l(0);
                Halfedge hh = h;
                do
                {
                    ++l;
                    if (!mesh_.is_manifold(mesh_.to_vertex(hh)))
                    {
                        l += lmin + 42; // make sure this hole is not chosen
                        break;
                    }
                    hh = mesh_.next_halfedge(hh);
                } while (hh != h);

                if (l < lmin)
                {
                    lmin = l;
                    hmin = h;
                }
            }
        }

        // close smallest hole
        if (hmin.is_valid())
        {
            try
            {
                fill_hole(mesh_, hmin);
            }
            catch (const InvalidInputException& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }
        else
        {
            std::cerr << "No manifold boundary loop found\n";
        }
    }
}

int main(int argc, char** argv)
{
#ifndef __EMSCRIPTEN__
    Viewer window("Hole Filling", 800, 600);
    if (argc == 2)
        window.load_mesh(argv[1]);
    return window.run();
#else
    Viewer window("Hole Filling", 800, 600);
    window.load_mesh(argc == 2 ? argv[1] : "input.off");
    return window.run();
#endif
}
