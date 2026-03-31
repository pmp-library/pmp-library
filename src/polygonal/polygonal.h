// Copyright 2026 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include <pmp/viewers/trackball_viewer.h>
#include <pmp/viewers/renderer.h>
#include "aabb_tree.h"
#include "lasso_drawable.h"

#include <memory>
#include <filesystem>

namespace pmp {

class Polygonal : public TrackballViewer
{
public:
    Polygonal(const char* title, int width, int height);

    void load_mesh(const std::filesystem::path& filename);
    void update_mesh();
    void draw(const std::string& draw_mode) override;

protected:
    void keyboard(int key, int code, int action, int mod) override;
    void mouse(int button, int action, int mods) override;
    void motion(double xpos, double ypos) override;
    void draw_imgui() override;

private:
    // ui
    void draw_menu_bar();
    void draw_sidebar();
    void draw_status_bar();

    // file
    void open();
    void save();
    void export_stl();
    void export_obj();
    void export_off();
    void revert();
    void close();

    // edit
    void cut();
    void flip_edge();
    void split_edge();

    // select
    void select_all();
    void select_none();
    void select_boundary();
    void select_non_manifold();
    void invert_selection();
    void grow_selection();
    void shrink_selection();

    // view
    void reset_scene();
    std::string get_draw_mode() { return draw_mode_names_.at(draw_mode_); }

    // help
    void show_about_window();
    bool show_about_{false};

    // error
    void show_error_window();
    bool show_error_{false};
    std::string error_message_{};

    // hole filling
    using Hole = std::pair<Halfedge, int>;
    std::vector<Hole> holes_;
    bool selected_hole_changed_{false};

    // react on changes
    void topology_changed();
    void geometry_changed();

    // properties
    void show_properties_window();
    bool show_properties_{false};

    // picking
    Vertex pick_vertex(int x, int y);
    std::unique_ptr<AABBTree> tree_;

    // lasso selection
    void select_lasso(bool surface = true);
    std::vector<ivec2> lasso_;
    std::chrono::time_point<std::chrono::steady_clock> last_click_time_;
    static constexpr double double_click_threshold_ = 0.3; // seconds

    // status bar text
    std::string status_;

    // moving selected vertices
    bool move_mode_{false};
    std::vector<Vertex> handle_;
    vec3 delta_;
    vec3 translation(double xpos, double ypos);
    void start_move();
    void accept_move();

    // selection mode
    enum class SelectionMode
    {
        Add,
        Remove,
        Set
    };
    SelectionMode selection_mode_;

    SurfaceMesh mesh_;
    Renderer renderer_;
    std::shared_ptr<LassoDrawable> lasso_drawable_;
    std::filesystem::path filename_;
};

} // namespace pmp
