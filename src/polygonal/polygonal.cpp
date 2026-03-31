// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "polygonal.h"

#include <pmp/algorithms/curvature.h>
#include <pmp/algorithms/decimation.h>
#include <pmp/algorithms/differential_geometry.h>
#include <pmp/algorithms/features.h>
#include <pmp/algorithms/hole_filling.h>
#include <pmp/algorithms/normals.h>
#include <pmp/algorithms/remeshing.h>
#include <pmp/algorithms/shapes.h>
#include <pmp/algorithms/smoothing.h>
#include <pmp/algorithms/subdivision.h>
#include <pmp/algorithms/triangulation.h>
#include <pmp/algorithms/utilities.h>
#include <pmp/io/io.h>
#include <pmp/viewers/inter_font.h>

#include <imgui.h>

#include <filesystem>

#include "GLFW/glfw3.h"

namespace pmp {

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
Polygonal* instance = nullptr;
extern "C" {
void load_mesh(const char* file)
{
    if (instance)
        instance->load_mesh(file);
}
}

// create buffer from filename and download
EM_JS(void, download, (const char* filename), {
    var fileName = UTF8ToString(filename);
    var data = FS.readFile(fileName);
    var a = document.createElement('a');
    a.download = fileName;
    a.href = URL.createObjectURL(new Blob([data.buffer], {
        type:
            'octet-stream'
    }));
    a.click();
});

EM_JS(void, open_url, (const char* url), {
    var a = document.createElement('a');
    a.href = UTF8ToString(url);
    a.click();
});
#endif

Polygonal::Polygonal(const char* title, int width, int height)
    : TrackballViewer(title, width, height), renderer_(mesh_)
{
#ifdef __EMSCRIPTEN__
    instance = this; // required to be able to call load_mesh
#endif

    // create and add lasso drawable as an overlay
    auto lasso = std::make_shared<LassoDrawable>();
    lasso_drawable_ = lasso;
    renderer_.add_overlay(lasso);

    // setup draw modes
    clear_draw_modes();
    add_draw_mode("Points");
    add_draw_mode("Hidden Line");
    add_draw_mode("Smooth Shading");
    add_draw_mode("Texture");
    set_draw_mode("Hidden Line");

    color_mode(ColorMode::DarkMode);
    clear_color_ = vec3(0.17, 0.17, 0.17);

    const auto scale = imgui_scaling();
    ImFontConfig config;
    config.OversampleH = 3;
    config.OversampleV = 3;
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryCompressedTTF(InterRoman_compressed_data,
                                             InterRoman_compressed_size,
                                             15 * scale, &config);

    // window style
    ImGui::GetStyle() = ImGuiStyle(); // reset to default
    ImGuiStyle& style = ImGui::GetStyle();
    style.FramePadding = ImVec2(4 * scale, 4 * scale);
    style.ItemSpacing = ImVec2(7 * scale, 7 * scale);
    style.WindowBorderSize = 0;

    const auto bgcolor = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = bgcolor;
    style.Colors[ImGuiCol_PopupBg] = bgcolor;
    style.Colors[ImGuiCol_ModalWindowDimBg] =
        ImVec4(0.10f, 0.10f, 0.10f, 0.80f);

    renderer_.set_crease_angle(0);
    selection_mode_ = SelectionMode::Add;
    last_click_time_ = std::chrono::high_resolution_clock::now();
}

void Polygonal::load_mesh(const std::filesystem::path& filename)
{
    // load mesh
    try
    {
        read(mesh_, filename);
    }
    catch (const std::exception& e)
    {
        error_message_ = e.what();
        show_error_ = true;
        mesh_.clear();
        return;
    }

    // update scene center and bounds
    const BoundingBox bb = bounds(mesh_);
    modelview_matrix_ = mat4::identity();
    set_scene((vec3)bb.center(), 0.5 * bb.size());

    // compute face & vertex normals, update face indices
    update_mesh();

    // set draw mode
    if (mesh_.n_faces() == 0)
    {
        set_draw_mode("Points");
    }

    // print mesh statistic
    std::stringstream ss;
    ss << "Loaded " << filename << ": " << mesh_.n_vertices() << " vertices, "
       << mesh_.n_faces() << " faces\n";
    status_ = ss.str();

    filename_ = filename;
    renderer_.set_crease_angle(0);
}

void Polygonal::update_mesh()
{
    // update scene center and radius, but don't update camera view
    const BoundingBox bb = bounds(mesh_);
    center_ = (vec3)bb.center();
    radius_ = 0.5f * bb.size();

    // re-compute face and vertex normals
    renderer_.update_opengl_buffers();
}

void Polygonal::draw(const std::string& drawMode)
{
    glViewport(300, 0, width(), height());

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw mesh
    renderer_.draw(projection_matrix_, modelview_matrix_, drawMode);
}

void Polygonal::keyboard(int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    switch (key)
    {
        case GLFW_KEY_O:
        {
            if (ctrl_pressed())
                open();
            break;
        }
        case GLFW_KEY_S:
        {
            if (ctrl_pressed())
                save();
            break;
        }
        case GLFW_KEY_BACKSPACE:
        {
            revert();
            break;
        }
        case GLFW_KEY_W:
        {
            if (ctrl_pressed())
                close();
            break;
        }
        case GLFW_KEY_A:
        {
            if (ctrl_pressed())
                select_all();
            break;
        }
        case GLFW_KEY_B:
        {
            if (ctrl_pressed())
                select_boundary();
            break;
        }
        case GLFW_KEY_D:
        {
            if (ctrl_pressed())
                select_none();
            break;
        }
        case GLFW_KEY_G:
        {
            start_move();
            break;
        }
        case GLFW_KEY_I:
        {
            if (ctrl_pressed())
                invert_selection();
            break;
        }
        case GLFW_KEY_PERIOD:
        {
            if (ctrl_pressed())
                grow_selection();
            break;
        }
        case GLFW_KEY_COMMA:
        {
            if (ctrl_pressed())
                shrink_selection();
            break;
        }
        case GLFW_KEY_X:
        {
            if (ctrl_pressed())
                cut();
            break;
        }
        case GLFW_KEY_ESCAPE:
        {
            accept_move();
            break;
        }
        case GLFW_KEY_PAGE_UP:
        case GLFW_KEY_PAGE_DOWN:
        {
            break;
        }
        default:
        {
            TrackballViewer::keyboard(key, scancode, action, mods);
            break;
        }
    }
}

void Polygonal::mouse(int button, int action, int mods)
{
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
    {
        double x, y;
        cursor_pos(x, y);
        if (ctrl_pressed() && !shift_pressed())
        {
            Vertex v = pick_vertex(x, y);
            if (mesh_.is_valid(v))
            {
                auto selected = mesh_.vertex_property<bool>("v:selected");

                // reset selection if required
                if (selection_mode_ == SelectionMode::Set)
                {
                    for (auto vv : mesh_.vertices())
                        selected[vv] = false;
                }

                selected[v] = selection_mode_ != SelectionMode::Remove;
                status_ = "";
                update_mesh();
            }
        }
        else if (shift_pressed())
        {
            lasso_.emplace_back(x, y);
            lasso_drawable_->update_lasso(lasso_);

            auto now = std::chrono::high_resolution_clock::now();
            double elapsed =
                std::chrono::duration<double>(now - last_click_time_).count();
            bool double_click = (elapsed < double_click_threshold_);
            last_click_time_ = now;

            if (double_click)
            {
                select_lasso(alt_pressed());
                lasso_.clear();
                lasso_drawable_->update_lasso(lasso_);
            }
        }
        else if (move_mode_)
        {
            accept_move();
        }
    }
    else
    {
        TrackballViewer::mouse(button, action, mods);
    }
}

void Polygonal::motion(double xpos, double ypos)
{
    if (move_mode_ && !handle_.empty())
    {
        const auto delta = translation(xpos, ypos);
        delta_ += delta * 0.8;
        status_ = "Dx: " + std::to_string(delta_[0]) +
                  " Dy: " + std::to_string(delta_[1]) +
                  " Dz: " + std::to_string(delta_[2]);
        for (auto v : handle_)
            mesh_.position(v) += delta * 0.8;
        // remember points
        prev_point_2d_ = ivec2(xpos, ypos);
        prev_point_ok_ = map_to_sphere(prev_point_2d_, prev_point_3d_);
        renderer_.update_opengl_buffers();
    }
    else if (shift_pressed() && !ctrl_pressed())
    {
        lasso_.emplace_back(xpos, ypos);
        lasso_drawable_->update_lasso(lasso_);
    }
    else
        TrackballViewer::motion(xpos, ypos);
}

void Polygonal::draw_imgui()
{
    draw_menu_bar();
    draw_sidebar();
    draw_status_bar();

    if (show_about_)
        show_about_window();
    if (show_error_)
        show_error_window();
    if (show_properties_)
        show_properties_window();
}

void Polygonal::draw_menu_bar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::BeginMenu("New"))
            {
                if (ImGui::MenuItem("Cube"))
                {
                    mesh_ = hexahedron();
                    reset_scene();
                    save();
                    topology_changed();
                }
                if (ImGui::MenuItem("Sphere"))
                {
                    mesh_ = icosphere();
                    reset_scene();
                    save();
                    topology_changed();
                }
                if (ImGui::MenuItem("Plane"))
                {
                    mesh_ = plane(10);
                    reset_scene();
                    save();
                    topology_changed();
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                open();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                save();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Export"))
            {
                if (ImGui::MenuItem("STL (.stl)"))
                {
                    export_stl();
                }
                if (ImGui::MenuItem("Wavefront (.obj)"))
                {
                    export_obj();
                }
                if (ImGui::MenuItem("Geomview (.off)"))
                {
                    export_off();
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Revert", "Backspace"))
            {
                revert();
            }
            if (ImGui::MenuItem("Close", "Ctrl+W"))
            {
                close();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Cut", "Ctrl+X"))
            {
                cut();
                topology_changed();
            }
            if (ImGui::MenuItem("Flip Edge"))
            {
                flip_edge();
            }
            if (ImGui::MenuItem("Split Edge"))
            {
                split_edge();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Select"))
        {
            if (ImGui::MenuItem("All", "Ctrl+A"))
            {
                select_all();
            }
            if (ImGui::MenuItem("None", "Ctrl+D"))
            {
                select_none();
            }
            if (ImGui::MenuItem("Invert", "Ctrl+I"))
            {
                invert_selection();
            }
            if (ImGui::MenuItem("Boundary", "Ctrl+B"))
            {
                select_boundary();
            }
            if (ImGui::MenuItem("Non-Manifold"))
            {
                select_non_manifold();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Grow", "Ctrl+."))
            {
                grow_selection();
            }
            if (ImGui::MenuItem("Shrink", "Ctrl+,"))
            {
                shrink_selection();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Selection Mode"))
            {
                if (ImGui::MenuItem("Add", nullptr,
                                    selection_mode_ == SelectionMode::Add))
                {
                    selection_mode_ = SelectionMode::Add;
                }
                if (ImGui::MenuItem("Remove", nullptr,
                                    selection_mode_ == SelectionMode::Remove))
                {
                    selection_mode_ = SelectionMode::Remove;
                }
                if (ImGui::MenuItem("Set", nullptr,
                                    selection_mode_ == SelectionMode::Set))
                {
                    selection_mode_ = SelectionMode::Set;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Reset View"))
            {
                reset_scene();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Rendering Mode"))
            {
                if (ImGui::MenuItem("Points", nullptr,
                                    get_draw_mode() == "Points"))
                {
                    set_draw_mode("Points");
                }
                if (ImGui::MenuItem("Hidden Line", nullptr,
                                    get_draw_mode() == "Hidden Line"))
                {
                    renderer_.set_crease_angle(0);
                    set_draw_mode("Hidden Line");
                }
                if (ImGui::MenuItem("Smooth Shading", nullptr,
                                    get_draw_mode() == "Smooth Shading"))
                {
                    renderer_.set_crease_angle(180);
                    set_draw_mode("Smooth Shading");
                }
                if (ImGui::MenuItem("Texture", nullptr,
                                    get_draw_mode() == "Texture"))
                {
                    set_draw_mode("Texture");
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Properties", nullptr, show_properties_))
            {
                show_properties_ = !show_properties_;
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Filters"))
        {
            if (ImGui::MenuItem("Triangulate"))
            {
                try
                {
                    triangulate(mesh_);
                }
                catch (const InvalidInputException& e)
                {
                    error_message_ = e.what();
                    show_error_ = true;
                    return;
                }
                update_mesh();
                topology_changed();
            }
            if (ImGui::MenuItem("Flip Faces"))
            {
                flip_faces(mesh_);
                update_mesh();
            }
            if (ImGui::MenuItem("Dual"))
            {
                dual(mesh_);
                update_mesh();
                topology_changed();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("About", nullptr, &show_about_);
#ifdef __EMSCRIPTEN__
            if (ImGui::MenuItem("Documentation"))
            {
                open_url("https://polygonal.app/docs");
            }
            if (ImGui::MenuItem("Report an Issue"))
            {
                open_url("https://github.com/polygonal-app/polygonal/issues");
            }
#endif
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Polygonal::draw_sidebar()
{
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(
        ImVec2(300, ImGui::GetIO().DisplaySize.y - ImGui::GetFrameHeight() + 1),
        ImGuiCond_Always);
    ImGui::Begin("Sidebar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (ImGui::CollapsingHeader("Curvature"))
    {
        static int curvature_type = 0;
        ImGui::RadioButton("Mean", &curvature_type, 0);
        ImGui::RadioButton("Gauss", &curvature_type, 1);
        ImGui::RadioButton("Maximum Absolute", &curvature_type, 2);

        if (ImGui::Button("Apply###CurvatureApply"))
        {
            switch (curvature_type)
            {
                case 0:
                    curvature(mesh_, Curvature::Mean, 1, true, true);
                    break;
                case 1:
                    curvature(mesh_, Curvature::Gauss, 1, true, true);
                    break;
                case 2:
                    curvature(mesh_, Curvature::MaxAbs, 1, true, true);
                    break;
            }
            curvature_to_texture_coordinates(mesh_);
            renderer_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
    }
    if (ImGui::CollapsingHeader("Smoothing"))
    {
        ImGui::SeparatorText("Laplace");
        static int weight = 0;
        ImGui::RadioButton("Cotangent", &weight, 0);
        ImGui::RadioButton("Uniform##smoothing", &weight, 1);
        bool uniform_laplace = (weight == 1);

        ImGui::SeparatorText("Integration");
        static int integration = 0;
        ImGui::RadioButton("Explicit", &integration, 0);
        ImGui::RadioButton("Implicit", &integration, 1);
        bool implicit = (integration == 1);

        static int explicit_iterations = 10;
        static int implicit_iterations = 1;
        ImGui::PushItemWidth(100);
        if (implicit)
            ImGui::SliderInt("Iterations", &implicit_iterations, 1, 100);
        else
            ImGui::SliderInt("Iterations", &explicit_iterations, 1, 100);
        ImGui::PopItemWidth();

        static float timestep = 0.001;
        if (implicit)
        {
            ImGui::PushItemWidth(100);
            ImGui::SliderFloat("Time Step", &timestep, 0.001, 1.0);
            ImGui::PopItemWidth();
        }

        if (ImGui::Button("Apply###SmoothingApply"))
        {
            if (integration == 0)
            {
                explicit_smoothing(mesh_, explicit_iterations, uniform_laplace);
            }
            else
            {
                // only re-scale if we don't have a (fixed) boundary
                bool has_boundary = false;
                for (auto v : mesh_.vertices())
                    if (mesh_.is_boundary(v))
                        has_boundary = true;
                bool rescale = !has_boundary;

                Scalar dt =
                    uniform_laplace ? timestep : timestep * radius_ * radius_;
                try
                {
                    implicit_smoothing(mesh_, dt, implicit_iterations,
                                       uniform_laplace, rescale);
                }
                catch (const SolverException& e)
                {
                    error_message_ = e.what();
                    show_error_ = true;
                    ImGui::End();
                    return;
                }
            }
            geometry_changed();
            update_mesh();
        }
    }
    if (ImGui::CollapsingHeader("Subdivision"))
    {
        static int subdivision_type = 0;
        ImGui::RadioButton("Loop", &subdivision_type, 0);
        ImGui::RadioButton("Quad-Triangle", &subdivision_type, 1);
        ImGui::RadioButton("Catmull-Clark", &subdivision_type, 2);

        if (ImGui::Button("Apply###SubdivisionApply"))
        {
            try
            {
                switch (subdivision_type)
                {
                    case 0:
                        loop_subdivision(mesh_);
                        break;
                    case 1:
                        quad_tri_subdivision(mesh_);
                        break;
                    case 2:
                        catmull_clark_subdivision(mesh_);
                        break;
                }
            }
            catch (const InvalidInputException& e)
            {
                error_message_ = e.what();
                show_error_ = true;
                ImGui::End();
                return;
            }
            update_mesh();
            topology_changed();
        }
    }
    if (ImGui::CollapsingHeader("Decimation"))
    {
        static int target_percentage = 10;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Percentage", &target_percentage, 1, 99);
        ImGui::PopItemWidth();

        static int normal_deviation = 135;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Normal Deviation", &normal_deviation, 1, 135);
        ImGui::PopItemWidth();

        static int aspect_ratio = 10;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Aspect Ratio", &aspect_ratio, 1, 10);
        ImGui::PopItemWidth();

        if (ImGui::Button("Apply###DecimationApply"))
        {
            try
            {
                auto nv = mesh_.n_vertices() * 0.01 * target_percentage;
                decimate(mesh_, nv, aspect_ratio, 0.0, 0.0, normal_deviation,
                         0.0, 0.01);
            }
            catch (const InvalidInputException& e)
            {
                error_message_ = e.what();
                show_error_ = true;
                ImGui::End();
                return;
            }
            update_mesh();
            topology_changed();
        }
    }
    if (ImGui::CollapsingHeader("Remeshing"))
    {
        static int remeshing_type = 0;
        ImGui::RadioButton("Uniform##remeshing", &remeshing_type, 0);
        ImGui::RadioButton("Adaptive", &remeshing_type, 1);

        ImGui::PushItemWidth(100);

        static double edge_length{0.01};
        if (remeshing_type == 0)
        {
            ImGui::InputDouble("Edge Length", &edge_length, 0, 0, "%g");
        }
        static double min_length{0.001};
        static double max_length{0.05};
        static double max_error{0.0005};
        if (remeshing_type == 1)
        {
            ImGui::InputDouble("Minimum Edge Length", &min_length, 0, 0, "%g");
            ImGui::InputDouble("Maximum Edge Length", &max_length, 0, 0, "%g");
            ImGui::InputDouble("Maximum Error", &max_error, 0, 0, "%g");
        }

        static int n_iterations{10};
        ImGui::SliderInt("Iterations##uniform", &n_iterations, 1, 20);

        static bool use_projection{true};
        ImGui::Checkbox("Use Projection##uniform", &use_projection);

        static bool scale_lengths{true};
        ImGui::Checkbox("Scale Lengths##uniform", &scale_lengths);

        if (ImGui::Button("Apply###RemeshingApply"))
        {
            const auto scaling = scale_lengths ? bounds(mesh_).size() : 1.0;
            try
            {
                if (remeshing_type == 0)
                {
                    uniform_remeshing(mesh_, edge_length * scaling,
                                      n_iterations, use_projection);
                }
                else if (remeshing_type == 1)
                {
                    adaptive_remeshing(
                        mesh_, min_length * scaling, max_length * scaling,
                        max_error * scaling, n_iterations, use_projection);
                }
            }
            catch (const InvalidInputException& e)
            {
                error_message_ = e.what();
                show_error_ = true;
                ImGui::End();
                return;
            }
            update_mesh();
            topology_changed();
        }
        ImGui::PopItemWidth();
    }
    if (ImGui::CollapsingHeader("Feature Edges"))
    {
        static int feature_option = 0;
        ImGui::RadioButton("Angle", &feature_option, 0);
        ImGui::RadioButton("Boundary", &feature_option, 1);
        ImGui::RadioButton("Clear", &feature_option, 2);

        static int feature_angle = 70;
        if (feature_option == 0)
        {
            ImGui::PushItemWidth(80);
            ImGui::SliderInt("Feature Angle", &feature_angle, 1, 180);
            ImGui::PopItemWidth();
        }

        if (ImGui::Button("Apply###FeaturesApply"))
        {
            if (feature_option == 0)
            {
                detect_features(mesh_, feature_angle);
            }
            else if (feature_option == 1)
            {
                detect_boundary(mesh_);
            }
            else if (feature_option == 2)
            {
                clear_features(mesh_);
            }
            update_mesh();
        }
    }
    if (ImGui::CollapsingHeader("Hole Filling"))
    {
        std::vector<std::string> items;
        for (auto h : holes_)
            items.push_back(std::to_string(h.second));

        ImGui::PushItemWidth(120);
        static int item_current_idx = 0;
        auto label_fn = [&](int index) -> std::string {
            return "#" + std::to_string(index + 1) + ": " + items[index] +
                   " edges";
        };
        std::string initial = (int)items.size() > item_current_idx
                                  ? label_fn(item_current_idx)
                                  : "None";
        if (ImGui::BeginCombo("Holes", initial.c_str()))
        {
            for (int n = 0; n < (int)items.size(); n++)
            {
                const bool is_selected = (item_current_idx == n);
                std::string label = label_fn(n);
                if (ImGui::Selectable(label.c_str(), is_selected))
                {
                    item_current_idx = n;
                    selected_hole_changed_ = true;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        if (ImGui::Button("Detect Holes##HoleFilling"))
        {
            holes_.clear();
            selected_hole_changed_ = true;
            auto visited = mesh_.add_edge_property<bool>("e:visited", false);
            for (auto h : mesh_.halfedges())
            {
                if (mesh_.is_boundary(h) && !visited[mesh_.edge(h)])
                {
                    visited[mesh_.edge(h)] = true;
                    Halfedge hh = h;
                    int n_edges = 0;
                    bool is_manifold = true;
                    do
                    {
                        if (!mesh_.is_manifold(mesh_.to_vertex(hh)))
                        {
                            is_manifold = false;
                            break;
                        }
                        n_edges++;
                        visited[mesh_.edge(hh)] = true;
                        hh = mesh_.next_halfedge(hh);
                    } while (hh != h);

                    if (is_manifold)
                        holes_.push_back({h, n_edges});
                }
            }
            mesh_.remove_edge_property(visited);

            // sort by number of edges, descending
            std::sort(holes_.begin(), holes_.end(),
                      [](const auto& a, const auto& b) {
                          return a.second > b.second;
                      });
        }
        if (selected_hole_changed_)
        {
            // clear features
            auto feature = mesh_.edge_property<bool>("e:feature");
            for (auto e : mesh_.edges())
                feature[e] = false;

            if (!holes_.empty())
            {
                const auto h = holes_[item_current_idx].first;
                Halfedge hh = h;
                do
                {
                    feature[mesh_.edge(hh)] = true;
                    hh = mesh_.next_halfedge(hh);
                } while (hh != h);

                update_mesh();
            }
            selected_hole_changed_ = false;
        }

        if (ImGui::Button("Apply###HoleFillingApply"))
        {
            if (holes_.empty())
            {
                bool has_boundary = false;
                for (auto v : mesh_.vertices())
                    if (mesh_.is_boundary(v))
                    {
                        has_boundary = true;
                        break;
                    }

                if (has_boundary)
                {
                    error_message_ =
                        "No manifold boundary loop found.\n"
                        "\nRemove non-manifold vertices from boundary loops "
                        "to fill remaining holes.\n";
                }
                else
                    error_message_ = "Mesh has no holes.\n";
                show_error_ = true;
                ImGui::End();
                return;
            }
            const auto h = holes_[item_current_idx].first;
            if (h.is_valid())
            {
                // clear features
                auto feature = mesh_.edge_property<bool>("e:feature");
                Halfedge hh = h;
                do
                {
                    feature[mesh_.edge(hh)] = false;
                    hh = mesh_.next_halfedge(hh);
                } while (hh != h);
                try
                {
                    fill_hole(mesh_, h);
                }
                catch (const InvalidInputException& e)
                {
                    error_message_ = e.what();
                    show_error_ = true;
                    ImGui::End();
                    return;
                }
                holes_.erase(holes_.begin() + item_current_idx);
                item_current_idx = 0;
                selected_hole_changed_ = true;
                tree_ = nullptr;
                update_mesh();
            }
            else
            {
                error_message_ = "No manifold boundary loop found\n";
                show_error_ = true;
                ImGui::End();
                return;
            }
        }
    }
    ImGui::End();
}

void Polygonal::draw_status_bar()
{
    // draw a border above the status bar, in front of all windows
    ImVec2 pos =
        ImVec2(0, ImGui::GetIO().DisplaySize.y - ImGui::GetFrameHeight());
    ImVec2 size = ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetFrameHeight());
    ImDrawList* draw_list =
        ImGui::GetForegroundDrawList(); // use foreground draw list
    ImU32 border_col = IM_COL32(30, 30, 30, 255);

    // draw the line at the very bottom, just above the status bar, spanning the full window width
    draw_list->AddLine(ImVec2(0, pos.y),
                       ImVec2(ImGui::GetIO().DisplaySize.x, pos.y), border_col,
                       1.0f);

    // status bar
    ImGui::SetNextWindowPos(
        ImVec2(0, ImGui::GetIO().DisplaySize.y - ImGui::GetFrameHeight()),
        ImGuiCond_Always);
    ImGui::SetNextWindowSize(
        ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetFrameHeight()),
        ImGuiCond_Always);
    ImGui::Begin("Status Bar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove);
    ImGui::SetCursorPosY(ImGui::GetTextLineHeight() * 0.1f);
    ImGui::PushFont(nullptr, ImGui::GetStyle().FontSizeBase * 0.85f);
    ImGui::AlignTextToFramePadding();
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
    ImGui::Text("%s", status_.c_str());
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::End();
}

void Polygonal::open()
{
#ifdef __EMSCRIPTEN__
    EM_ASM("document.getElementById('myfile').showPicker()");
#endif
    topology_changed();
}

void Polygonal::save()
{
    auto tmp = std::filesystem::temp_directory_path();
    auto file = tmp.string() + "output.pmp";
    write(mesh_, file.c_str());
    filename_ = file;
}

void Polygonal::export_stl()
{
    // make a copy, don't change original mesh
    auto mesh = mesh_;

    // triangulate polygon mesh for STL export
    if (!mesh.is_triangle_mesh())
    {
        try
        {
            triangulate(mesh);
        }
        catch (const InvalidInputException& e)
        {
            error_message_ = e.what();
            show_error_ = true;
            return;
        }
    }

    // ensure we have face normals
    face_normals(mesh);

    write(mesh, "mesh.stl");
#ifdef __EMSCRIPTEN__
    download("mesh.stl");
#endif
}

void Polygonal::export_obj()
{
    write(mesh_, "mesh.obj");
#ifdef __EMSCRIPTEN__
    download("mesh.obj");
#endif
}

void Polygonal::export_off()
{
    write(mesh_, "mesh.off");
#ifdef __EMSCRIPTEN__
    download("mesh.off");
#endif
}

void Polygonal::revert()
{
    if (!filename_.empty())
        load_mesh(filename_.c_str());
    topology_changed();
}

void Polygonal::close()
{
    mesh_.clear();
    topology_changed();
    renderer_.update_opengl_buffers();
}

void Polygonal::cut()
{
    auto selected = mesh_.vertex_property<bool>("v:selected");

    // delete all selected vertices
    for (auto v : mesh_.vertices())
        if (selected[v])
            mesh_.delete_vertex(v);

    mesh_.garbage_collection();
    update_mesh();
    topology_changed();
}

void Polygonal::flip_edge()
{
    auto selected = mesh_.vertex_property<bool>("v:selected");
    Edge e;
    for (auto v : mesh_.vertices())
        if (selected[v])
            for (auto vv : mesh_.vertices(v))
                if (selected[vv])
                {
                    e = mesh_.find_edge(v, vv);
                    if (e.is_valid() && mesh_.is_flip_ok(e))
                    {
                        mesh_.flip(e);
                        selected[v] = false;
                        selected[vv] = false;
                        break;
                    }
                }
    if (e.is_valid())
    {
        status_ = "Flipped edge #" + std::to_string(e.idx());
        update_mesh();
    }
}

void Polygonal::split_edge()
{
    auto selected = mesh_.vertex_property<bool>("v:selected");
    Edge e;
    for (auto v : mesh_.vertices())
        if (selected[v])
            for (auto vv : mesh_.vertices(v))
                if (selected[vv])
                {
                    e = mesh_.find_edge(v, vv);
                    Point p = (mesh_.position(v) + mesh_.position(vv)) * 0.5;
                    if (e.is_valid())
                    {
                        mesh_.split(e, p);
                        selected[v] = false;
                        selected[vv] = false;
                        break;
                    }
                }
    if (e.is_valid())
    {
        status_ = "Split edge #" + std::to_string(e.idx());
        update_mesh();
    }
}

void Polygonal::select_all()
{
    auto selected = mesh_.vertex_property<bool>("v:selected");
    for (auto v : mesh_.vertices())
        selected[v] = true;
    update_mesh();
    status_ = "Selected " + std::to_string(mesh_.n_vertices()) + " vertices";
}

void Polygonal::select_none()
{
    auto selected = mesh_.get_vertex_property<bool>("v:selected");
    if (selected)
    {
        mesh_.remove_vertex_property(selected);
        update_mesh();
        status_ = "Cleared selection";
    }
}

void Polygonal::select_boundary()
{
    auto selected = mesh_.vertex_property<bool>("v:selected");
    int n_selected = 0;
    for (auto v : mesh_.vertices())
        if (mesh_.is_boundary(v))
        {
            selected[v] = true;
            n_selected++;
        }
    update_mesh();
    status_ = "Selected " + std::to_string(n_selected) + " vertices";
}

void Polygonal::select_non_manifold()
{
    auto selected = mesh_.vertex_property<bool>("v:selected");
    int n_selected = 0;
    for (auto v : mesh_.vertices())
        if (!mesh_.is_manifold(v))
        {
            selected[v] = true;
            n_selected++;
        }
    status_ = "Selected " + std::to_string(n_selected) + " vertices";
    update_mesh();
}

void Polygonal::invert_selection()
{
    auto selected = mesh_.vertex_property<bool>("v:selected");
    for (auto v : mesh_.vertices())
        selected[v] = !selected[v];
    update_mesh();
}

void Polygonal::grow_selection()
{
    auto selected = mesh_.vertex_property<bool>("v:selected");

    // collect vertices to be selected
    std::vector<Vertex> collected;
    for (auto v : mesh_.vertices())
        if (selected[v])
            for (auto vv : mesh_.vertices(v))
                collected.push_back(vv);

    for (auto v : collected)
        selected[v] = true;

    update_mesh();
}

void Polygonal::shrink_selection()
{
    auto selected = mesh_.vertex_property<bool>("v:selected");

    // collect vertices to be de-selected
    std::vector<Vertex> deselect;
    for (auto v : mesh_.vertices())
        if (selected[v])
            for (auto vv : mesh_.vertices(v))
                if (!selected[vv])
                {
                    deselect.push_back(v);
                    break;
                }

    for (auto v : deselect)
        selected[v] = false;

    update_mesh();
}

void Polygonal::reset_scene()
{
    BoundingBox bb = bounds(mesh_);
    modelview_matrix_ = mat4::identity();
    set_scene((vec3)bb.center(), 0.5f * bb.size());
    renderer_.update_opengl_buffers();
}

void Polygonal::show_about_window()
{
    ImGui::OpenPopup("About");
    if (ImGui::BeginPopupModal("About", &show_about_,
                               ImGuiWindowFlags_AlwaysAutoResize |
                                   ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoResize))
    {
        ImGui::Spacing();
        ImGui::Text("Copyright (c) 2026 https://polygonal.app");
        ImGui::Text(
            "This software is provided \"as is\", without any warranty of any "
            "kind.");
        ImGui::Spacing();
        ImGui::Text("Contact: info@polygonal.app");
        ImGui::Spacing();
        if (ImGui::Button("Ok") || ImGui::IsKeyPressed(ImGuiKey_Enter))
        {
            show_about_ = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Polygonal::show_error_window()
{
    ImGui::OpenPopup("Error");
    if (ImGui::BeginPopupModal("Error", &show_error_,
                               ImGuiWindowFlags_AlwaysAutoResize |
                                   ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoResize))
    {
        ImGui::Spacing();
        ImGui::Text("%s\n", error_message_.c_str());
        ImGui::Spacing();
        if (ImGui::Button("Ok") || ImGui::IsKeyPressed(ImGuiKey_Enter))
        {
            error_message_ = "";
            show_error_ = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Polygonal::show_properties_window()
{
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 175, 35),
                            ImGuiCond_Once);
    if (ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SeparatorText("Mesh Properties");
        ImGui::Text("Vertices: %d", (int)mesh_.n_vertices());
        ImGui::Text("Edges: %d", (int)mesh_.n_edges());
        ImGui::Text("Faces: %d", (int)mesh_.n_faces());
        ImGui::SeparatorText("Vertex Properties");
        for (auto p : mesh_.vertex_properties())
            ImGui::Text("%s", p.c_str());
        ImGui::SeparatorText("Edge Properties");
        for (auto p : mesh_.edge_properties())
            ImGui::Text("%s", p.c_str());
        ImGui::SeparatorText("Face Properties");
        for (auto p : mesh_.face_properties())
            ImGui::Text("%s", p.c_str());
    }
    ImGui::End();
}

void Polygonal::geometry_changed()
{
    tree_ = nullptr;
}

void Polygonal::topology_changed()
{
    // clear holes
    holes_.clear();
    tree_ = nullptr;
}

Vertex Polygonal::pick_vertex(int x, int y)
{
    // get viewport data
    std::array<GLint, 4> viewport;
    glGetIntegerv(GL_VIEWPORT, viewport.data());

    // screen (x,y) to ndc
    float ndc_x = (2.0f * (x - viewport[0])) / viewport[2] - 1.0f;
    float ndc_y =
        (2.0f * (viewport[3] - (y - viewport[1]))) / viewport[3] - 1.0f;

    // ray in clip space
    vec4 ray_clip(ndc_x, ndc_y, -1.0f, 1.0f);

    // ray in world space
    mat4 ipm = inverse(projection_matrix_ * modelview_matrix_);
    vec4 ray_world = ipm * ray_clip;
    ray_world /= ray_world[3];

    // camera position in world space (origin transformed by inverse modelview)
    mat4 imv = inverse(modelview_matrix_);
    vec4 cam_pos4 = imv * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec3 cam_pos(cam_pos4[0], cam_pos4[1], cam_pos4[2]);

    // ray direction
    vec3 ray_dir =
        normalize(vec3(ray_world[0], ray_world[1], ray_world[2]) - cam_pos);

    // re-build tree if necessary
    if (tree_ == nullptr)
    {
        tree_ = std::make_unique<AABBTree>(mesh_);
    }

    Vertex result;
    auto hit = tree_->intersect(cam_pos, ray_dir);
    if (hit)
    {
        // Find closest vertex of the hit face to the intersection point
        Face f(hit->face);
        vec3 intersection = cam_pos + hit->t * ray_dir;
        float min_dist = std::numeric_limits<float>::max();
        for (auto v : mesh_.vertices(f))
        {
            vec3 pos = mesh_.position(v);
            float dist = norm(pos - intersection);
            if (dist < min_dist)
            {
                min_dist = dist;
                result = v;
            }
        }
    }

    return result;
}

void Polygonal::select_lasso(bool surface)
{
    if (lasso_.size() < 3)
        return;

    // compute lasso bounding box
    int x_min = lasso_[0][0], x_max = lasso_[0][0];
    int y_min = lasso_[0][1], y_max = lasso_[0][1];
    for (const auto& p : lasso_)
    {
        x_min = std::min(x_min, p[0]);
        x_max = std::max(x_max, p[0]);
        y_min = std::min(y_min, p[1]);
        y_max = std::max(y_max, p[1]);
    }

    // point-in-polygon test using ray casting
    auto point_in_poly = [](double x, double y,
                            const std::vector<ivec2>& poly) {
        const auto n = poly.size();
        bool inside = false;
        for (int i = 0, j = n - 1; i < (int)n; j = i++)
        {
            double xi = poly[i][0], yi = poly[i][1];
            double xj = poly[j][0], yj = poly[j][1];
            if (((yi > y) != (yj > y)) &&
                (x < (xj - xi) * (y - yi) / (yj - yi + 1e-20) + xi))
                inside = !inside;
        }
        return inside;
    };

    // get viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    const auto pmv = projection_matrix_ * modelview_matrix_;

    // camera position in world space
    mat4 imv = inverse(modelview_matrix_);
    vec4 cam_pos4 = imv * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec3 cam_pos(cam_pos4[0], cam_pos4[1], cam_pos4[2]);

    // build AABB tree if needed
    if (surface && !tree_)
        tree_ = std::make_unique<AABBTree>(mesh_);

    // project vertices to screen and test if inside lasso
    auto selected = mesh_.vertex_property<bool>("v:selected");
    int n_selected = 0;

    // clear previous selection if required
    if (selection_mode_ == SelectionMode::Set)
        for (const auto v : mesh_.vertices())
            selected[v] = false;

    auto select_vertex = [&selected, &n_selected, this](Vertex v) {
        if (selection_mode_ == SelectionMode::Remove && selected[v])
        {
            selected[v] = false;
            n_selected++;
        }
        else if (selection_mode_ == SelectionMode::Set ||
                 selection_mode_ == SelectionMode::Add)
        {
            selected[v] = true;
            n_selected++;
        }
    };

    for (const auto v : mesh_.vertices())
    {
        const vec3 pos = mesh_.position(v);

        // project to screen coords
        const vec4 clip = pmv * vec4(pos, 1.0f);
        if (std::abs(clip[3]) < 1e-6f)
            continue;
        const vec3 ndc = vec3(clip[0], clip[1], clip[2]) / clip[3];

        // convert to window coords
        const double x_win = viewport[0] + (ndc[0] + 1.0) * 0.5 * viewport[2];
        const double y_win =
            viewport[1] + (1.0 - (ndc[1] + 1.0) * 0.5) * viewport[3];

        // skip points outside lasso bounding box
        if (x_win < x_min || x_win > x_max || y_win < y_min || y_win > y_max)
            continue;

        if (!point_in_poly(x_win, y_win, lasso_))
            continue;

        if (surface)
        {
            // ray from camera to vertex
            vec3 ray_dir = normalize(pos - cam_pos);
            float dist_to_vertex = norm(pos - cam_pos);

            // intersect with mesh
            auto hit = tree_->intersect(cam_pos, ray_dir);
            if (hit)
            {
                // only select if the intersection is at the vertex (within epsilon)
                if (std::abs(hit->t - dist_to_vertex) < 1e-4f)
                {
                    select_vertex(v);
                }
            }
        }
        else
        {
            select_vertex(v);
        }
    }

    if (n_selected > 0)
    {
        if (selection_mode_ == SelectionMode::Remove)
            status_ = "Deselected " + std::to_string(n_selected) + " vertices";
        else
            status_ = "Selected " + std::to_string(n_selected) + " vertices";
    }
    else
        status_ = "";
    update_mesh();
    lasso_.clear();
}

vec3 Polygonal::translation(double xpos, double ypos)
{
    const ivec2 current_pos(xpos, ypos);
    float w = width();
    float h = height();
    vec3 translation(0);

    if ((current_pos[0] < 0) || (current_pos[0] > w) || (current_pos[1] < 0) ||
        (current_pos[1] > h))
        return vec3(0);

    float radius = 0.5 * bounds(mesh_).size();

    float dx = (current_pos[0] - prev_point_2d_[0]) / w * radius;
    float dy = (current_pos[1] - prev_point_2d_[1]) / h * radius;

    mat4 M = projection_matrix_ * modelview_matrix_;

    vec3 up(M(1, 0), M(1, 1), M(1, 2));
    vec3 right(M(0, 0), M(0, 1), M(0, 2));

    return (-up * dy + right * dx);
}

void Polygonal::start_move()
{
    // invalidate tree
    tree_ = nullptr;

    // get handle vertices
    handle_.clear();
    auto selected = mesh_.vertex_property<bool>("v:selected");
    for (auto v : mesh_.vertices())
        if (selected[v])
            handle_.push_back(v);

    // remember start position
    double x, y;
    cursor_pos(x, y);
    prev_point_2d_ = ivec2(x, y);
    prev_point_ok_ = map_to_sphere(prev_point_2d_, prev_point_3d_);
    move_mode_ = !move_mode_;
}

void Polygonal::accept_move()
{
    auto selected = mesh_.vertex_property<bool>("v:selected");
    for (auto v : handle_)
        selected[v] = false;
    handle_.clear();
    move_mode_ = false;
    status_.clear();
    update_mesh();
}

} // namespace pmp
