// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/visualization/mesh_viewer.h"
#include "pmp/algorithms/utilities.h"

#include <iostream>
#include <limits>

#include <imgui.h>

#include "pmp/io/io.h"

namespace pmp {

MeshViewer::MeshViewer(const char* title, int width, int height, bool showgui)
    : TrackballViewer(title, width, height, showgui), renderer_(mesh_)
{
    // setup draw modes
    clear_draw_modes();
    add_draw_mode("Points");
    add_draw_mode("Hidden Line");
    add_draw_mode("Smooth Shading");
    add_draw_mode("Texture");
    set_draw_mode("Smooth Shading");

    crease_angle_ = 180.0;
    point_size_ = 5;

    // add help items
    add_help_item("Backspace", "Reload mesh", 3);
#ifndef __EMSCRIPTEN__
    add_help_item("W", "Write mesh to 'output.off'", 4);
#endif
}

void MeshViewer::load_mesh(const char* filename)
{
    // load mesh
    try
    {
        read(mesh_, filename);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }

    // update scene center and bounds
    const BoundingBox bb = bounds(mesh_);
    set_scene((vec3)bb.center(), 0.5 * bb.size());

    // compute face & vertex normals, update face indices
    update_mesh();

    // set draw mode
    if (mesh_.n_faces() == 0)
    {
        set_draw_mode("Points");
    }

    // print mesh statistic
    std::cout << "Loaded " << filename << ": " << mesh_.n_vertices()
              << " vertices, " << mesh_.n_faces() << " faces\n";

    filename_ = filename;
    renderer_.set_crease_angle(crease_angle_);
}

void MeshViewer::load_matcap(const char* filename)
{
    try
    {
        renderer_.load_matcap(filename);
    }
    catch (const IOException& e)
    {
        std::cerr << e.what() << std::endl;
        throw;
    }
    set_draw_mode("Texture");
}

void MeshViewer::load_texture(const char* filename, GLint format,
                              GLint min_filter, GLint mag_filter, GLint wrap)
{
    // load texture from file
    try
    {
        renderer_.load_texture(filename, format, min_filter, mag_filter, wrap);
    }
    catch (const IOException& e)
    {
        std::cerr << e.what() << std::endl;
        throw;
    }

    set_draw_mode("Texture");

    // set material
    renderer_.set_ambient(1.0);
    renderer_.set_diffuse(0.9);
    renderer_.set_specular(0.0);
    renderer_.set_shininess(1.0);
}

void MeshViewer::drop(int count, const char** paths)
{
    for (int i = 0; i < count; ++i)
    {
        const std::filesystem::path filename(paths[i]);
        const std::filesystem::path extension(filename.extension());

        if (extension == ".jpg" || extension == ".png")
        {
            load_texture(paths[i]);
        }
        else
        {
            load_mesh(paths[i]);
        }
    }
}

void MeshViewer::update_mesh()
{
    // update scene center and radius, but don't update camera view
    const BoundingBox bb = bounds(mesh_);
    center_ = (vec3)bb.center();
    radius_ = 0.5f * bb.size();

    // re-compute face and vertex normals
    renderer_.update_opengl_buffers();
}

void MeshViewer::process_imgui()
{
    if (ImGui::CollapsingHeader("Mesh Info", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // mesh statistics
        ImGui::BulletText("%d vertices", (int)mesh_.n_vertices());
        ImGui::BulletText("%d edges", (int)mesh_.n_edges());
        ImGui::BulletText("%d faces", (int)mesh_.n_faces());
        
        // draw mode
        ImGui::PushItemWidth(120);
        const char* current_item = draw_mode_names_[draw_mode_].c_str();
        if (ImGui::BeginCombo("Draw Mode", current_item))
        {
            for (unsigned int i=0; i<n_draw_modes_; ++i)
            {
                const char* item = draw_mode_names_[i].c_str();
                bool is_selected = (current_item == item);
                if (ImGui::Selectable(item, is_selected))
                    draw_mode_ = i;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        if (draw_mode_names_[draw_mode_] == "Points")
        {
            // point size
            ImGui::PushItemWidth(120);
            ImGui::SliderInt("Point Size", &point_size_, 1, 20);
            ImGui::PopItemWidth();
            if (point_size_ != renderer_.point_size())
            {
                renderer_.set_point_size(point_size_);
            }
        }
        else 
        {
            // crease angle
            ImGui::PushItemWidth(120);
            ImGui::SliderFloat("Crease Angle", &crease_angle_, 0.0f, 180.0f,
                            "%.0f");
            ImGui::PopItemWidth();
            if (crease_angle_ != renderer_.crease_angle())
            {
                renderer_.set_crease_angle(crease_angle_);
            }
        }
    }
}

void MeshViewer::draw(const std::string& drawMode)
{
    // draw mesh
    renderer_.draw(projection_matrix_, modelview_matrix_, drawMode);
}

void MeshViewer::keyboard(int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    switch (key)
    {
        case GLFW_KEY_BACKSPACE: // reload model
        {
            if (!filename_.empty())
                load_mesh(filename_.c_str());
            break;
        }

        case GLFW_KEY_W: // write mesh
        {
            write(mesh_, "output.off");
            break;
        }

        default:
        {
            TrackballViewer::keyboard(key, scancode, action, mods);
            break;
        }
    }
}

Vertex MeshViewer::pick_vertex(int x, int y)
{
    Vertex vmin;

    vec3 p;
    Scalar d, dmin(std::numeric_limits<Scalar>::max());

    if (TrackballViewer::pick(x, y, p))
    {
        const Point picked_position(p);
        for (auto v : mesh_.vertices())
        {
            d = distance(mesh_.position(v), picked_position);
            if (d < dmin)
            {
                dmin = d;
                vmin = v;
            }
        }
    }
    return vmin;
}

} // namespace pmp
