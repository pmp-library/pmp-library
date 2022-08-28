// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include <pmp/visualization/MeshViewer.h>
#include <pmp/algorithms/DeformationTransfer.h>
#include <pmp/algorithms/PointRegistration.h>
#include <imgui.h>

using namespace pmp;

class Viewer : public MeshViewer
{
public:
    Viewer(const char* title, int width, int height, bool showgui);
    bool load_meshes(const std::string& source_undeformed,
                     const std::string& target_undeformed,
                     const std::string& source_deformed);

protected:
    virtual void process_imgui() override;
    virtual void draw(const std::string& draw_mode) override;

private:
    SurfaceMeshGL& source_;
    SurfaceMeshGL target_;
    SurfaceMeshGL source_deformed_;
    SurfaceMeshGL target_deformed_;

    enum DrawableMeshes
    {
        SOURCE_UNDEFORMED,
        SOURCE_DEFORMED,
        TARGET_UNDEFORMED,
        TARGET_DEFORMED,
        ALL
    };
    DrawableMeshes to_draw;
};

Viewer::Viewer(const char* title, int width, int height, bool showgui)
    : MeshViewer(title, width, height, showgui), source_(mesh_)
{
    set_draw_mode("Solid Smooth");
}

bool Viewer::load_meshes(const std::string& source,
                         const std::string& source_deformed,
                         const std::string& target)
{
    try
    {
        source_.read(source);
        source_deformed_.read(source_deformed);
        target_.read(target);
    }
    catch (...)
    {
        return false;
    }

    BoundingBox bb = source_.bounds();
    bb += source_deformed_.bounds();
    bb += target_.bounds();

    set_scene((vec3)bb.center(), 0.5 * bb.size());
    to_draw = SOURCE_UNDEFORMED;

    return true;
}

void Viewer::process_imgui()
{
    MeshViewer::process_imgui();

    if (ImGui::CollapsingHeader("What to draw", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::RadioButton("Source undeformed", (int*)(&to_draw),
                           (int)SOURCE_UNDEFORMED);
        ImGui::RadioButton("Source deformed", (int*)(&to_draw),
                           (int)SOURCE_DEFORMED);
        ImGui::RadioButton("Target undeformed", (int*)(&to_draw),
                           (int)TARGET_UNDEFORMED);
        ImGui::RadioButton("Target deformed", (int*)(&to_draw),
                           (int)TARGET_DEFORMED);
        ImGui::RadioButton("All meshes", (int*)(&to_draw), (int)ALL);
    }

    if (ImGui::CollapsingHeader("Deformation Transfer",
                                ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("L1 alignment of source meshes"))
        {
            mat4 M = registration_l1(source_deformed_.positions(),
                                     source_.positions());
            for (auto v : source_deformed_.vertices())
                source_deformed_.position(v) =
                    affine_transform(M, source_deformed_.position(v));
            source_deformed_.update_opengl_buffers();
        }

        if (ImGui::Button("Transfer deformation to target"))
        {
            DeformationTransfer deftrans(source_, target_);
            deftrans.transfer(source_deformed_, target_deformed_);
            target_deformed_.update_opengl_buffers();
        }

        if (ImGui::Button("Save deformed target"))
        {
            std::cout << "Save to \"deformed_target.off\"\n";
            target_deformed_.write("deformed_target.off");
        }
    }
}

void Viewer::draw(const std::string& draw_mode)
{
    switch (to_draw)
    {
        case SOURCE_UNDEFORMED:
        {
            source_.draw(projection_matrix_, modelview_matrix_, draw_mode);
            break;
        }
        case SOURCE_DEFORMED:
        {
            source_deformed_.draw(projection_matrix_, modelview_matrix_,
                                  draw_mode);
            break;
        }
        case TARGET_UNDEFORMED:
        {
            target_.draw(projection_matrix_, modelview_matrix_, draw_mode);
            break;
        }
        case TARGET_DEFORMED:
        {
            target_deformed_.draw(projection_matrix_, modelview_matrix_,
                                  draw_mode);
            break;
        }
        case ALL:
        {
            GLint w = width() / 2;
            GLint h = height() / 2;

            glViewport(0, h, w, h);
            source_.draw(projection_matrix_, modelview_matrix_, draw_mode);

            glViewport(w, h, w, h);
            source_deformed_.draw(projection_matrix_, modelview_matrix_,
                                  draw_mode);

            glViewport(0, 0, w, h);
            target_.draw(projection_matrix_, modelview_matrix_, draw_mode);

            glViewport(w, 0, w, h);
            target_deformed_.draw(projection_matrix_, modelview_matrix_,
                                  draw_mode);

            glViewport(0, 0, width(), height());
            break;
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <source> <source_deformed> <target>\n";
        exit(1);
    }

    Viewer viewer("Deformation Transfer", 800, 600, true);

    if (!viewer.load_meshes(argv[1], argv[2], argv[3]))
    {
        std::cerr << "Cannot load meshes\n";
        exit(1);
    }

    return viewer.run();
}
