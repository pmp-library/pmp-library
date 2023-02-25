// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "MeshProcessingViewer.h"

#include <pmp/algorithms/subdivision.h>
#include <pmp/algorithms/features.h>
#include <pmp/algorithms/decimation.h>
#include <pmp/algorithms/remeshing.h>
#include <pmp/algorithms/curvature.h>
#include <pmp/algorithms/geodesics.h>
#include <pmp/algorithms/hole_filling.h>
#include <pmp/algorithms/shapes.h>
#include <pmp/algorithms/smoothing.h>
#include <pmp/algorithms/triangulation.h>
#include <pmp/algorithms/DifferentialGeometry.h>
#include <pmp/utilities.h>

#include <imgui.h>

using namespace pmp;

MeshProcessingViewer::MeshProcessingViewer(const char* title, int width,
                                           int height)
    : MeshViewer(title, width, height)
{
    // add help items
    add_help_item("O", "Flip mesh orientation", 5);
}

void MeshProcessingViewer::keyboard(int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    switch (key)
    {
        case GLFW_KEY_A:
        {
            detect_features(mesh_, 25);
            update_mesh();
            break;
        }
        case GLFW_KEY_D: // dualize mesh
        {
            dual(mesh_);
            update_mesh();
            break;
        }
        case GLFW_KEY_H:
        {
            set_draw_mode("Hidden Line");
            break;
        }
        case GLFW_KEY_O: // change face orientation
        {
            flip_faces(mesh_);
            update_mesh();
            break;
        }
        case GLFW_KEY_M: // merge two faces incident to longest edge
        {
            Scalar l, ll(0);
            Edge ee;
            for (auto e : mesh_.edges())
            {
                Vertex v0 = mesh_.vertex(e, 0);
                Vertex v1 = mesh_.vertex(e, 1);
                Point p0 = mesh_.position(v0);
                Point p1 = mesh_.position(v1);
                l = distance(p0, p1);
                if (l > ll && mesh_.is_removal_ok(e))
                {
                    ll = l;
                    ee = e;
                }
            }

            if (ee.is_valid())
            {
                std::cout << "Merge faces incident to edge " << ee << std::endl;
                mesh_.remove_edge(ee);
                update_mesh();
            }
            break;
        }
        case GLFW_KEY_T:
        {
            triangulate(mesh_);
            update_mesh();
            break;
        }
        case GLFW_KEY_1:
        case GLFW_KEY_2:
        case GLFW_KEY_3:
        case GLFW_KEY_4:
        case GLFW_KEY_5:
        case GLFW_KEY_6:
        case GLFW_KEY_7:
        case GLFW_KEY_8:
        case GLFW_KEY_9:
        {
            switch (key)
            {
                case GLFW_KEY_1:
                    mesh_.assign(tetrahedron());
                    break;
                case GLFW_KEY_2:
                    mesh_.assign(octahedron());
                    break;
                case GLFW_KEY_3:
                    mesh_.assign(hexahedron());
                    break;
                case GLFW_KEY_4:
                    mesh_.assign(icosahedron());
                    break;
                case GLFW_KEY_5:
                    mesh_.assign(dodecahedron());
                    break;
                case GLFW_KEY_6:
                    mesh_.assign(icosphere(3));
                    break;
                case GLFW_KEY_7:
                    mesh_.assign(quad_sphere(3));
                    break;
                case GLFW_KEY_8:
                    mesh_.assign(uv_sphere());
                    break;
                case GLFW_KEY_9:
                    mesh_.assign(torus());
                    break;
            }

            BoundingBox bb = bounds(mesh_);
            set_scene((vec3)bb.center(), 0.5 * bb.size());
            set_draw_mode("Hidden Line");
            update_mesh();
            break;
        }
        default:
        {
            MeshViewer::keyboard(key, scancode, action, mods);
            break;
        }
    }
}

void MeshProcessingViewer::process_imgui()
{
    MeshViewer::process_imgui();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Curvature"))
    {
        if (ImGui::Button("Mean Curvature"))
        {
            curvature(mesh_, Curvature::mean, 1, true, true);
            curvature_to_texture_coordinates(mesh_);
            mesh_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
        if (ImGui::Button("Gauss Curvature"))
        {
            curvature(mesh_, Curvature::gauss, 1, true, true);
            curvature_to_texture_coordinates(mesh_);
            mesh_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
        if (ImGui::Button("Abs. Max. Curvature"))
        {
            curvature(mesh_, Curvature::max_abs, 1, true, true);
            curvature_to_texture_coordinates(mesh_);
            mesh_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Smoothing"))
    {
        static int iterations = 10;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Iterations", &iterations, 1, 100);
        ImGui::PopItemWidth();

        if (ImGui::Button("Explicit Smoothing"))
        {
            explicit_smoothing(mesh_, iterations);
            update_mesh();
        }

        ImGui::Spacing();

        static float timestep = 0.001;
        float lb = 0.001;
        float ub = 0.1;
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("TimeStep", &timestep, lb, ub);
        ImGui::PopItemWidth();

        if (ImGui::Button("Implicit Smoothing"))
        {
            Scalar dt = timestep * radius_ * radius_;
            try
            {
                implicit_smoothing(mesh_, dt);
            }
            catch (const SolverException& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

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

        static int seam_angle_deviation = 1;
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("Seam Angle Deviation", &seam_angle_deviation, 0, 15);
        ImGui::PopItemWidth();

        if (ImGui::Button("Decimate"))
        {
            try
            {
                auto nv = mesh_.n_vertices() * 0.01 * target_percentage;
                decimate(mesh_, nv, aspect_ratio, 0.0, 0.0, normal_deviation,
                         0.0, 0.01, seam_angle_deviation);
            }
            catch (const InvalidInputException& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Subdivision"))
    {
        if (ImGui::Button("Loop Subdivision"))
        {
            try
            {
                loop_subdivision(mesh_);
            }
            catch (const InvalidInputException& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }

        if (ImGui::Button("Quad-Tri Subdivision"))
        {
            quad_tri_subdivision(mesh_);
            update_mesh();
        }

        if (ImGui::Button("Catmull-Clark Subdivision"))
        {
            catmull_clark_subdivision(mesh_);
            update_mesh();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Remeshing"))
    {
        if (ImGui::Button("Adaptive Remeshing"))
        {
            auto bb = bounds(mesh_).size();

            try
            {
                adaptive_remeshing(mesh_,
                                   0.001 * bb,  // min length
                                   1.0 * bb,    // max length
                                   0.001 * bb); // approx. error
            }
            catch (const InvalidInputException& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }

        if (ImGui::Button("Uniform Remeshing"))
        {
            Scalar l(0);
            for (auto eit : mesh_.edges())
                l += distance(mesh_.position(mesh_.vertex(eit, 0)),
                              mesh_.position(mesh_.vertex(eit, 1)));
            l /= (Scalar)mesh_.n_edges();

            try
            {
                uniform_remeshing(mesh_, l);
            }
            catch (const InvalidInputException& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Hole Filling"))
    {
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
                            l += 123456;
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
}

void MeshProcessingViewer::mouse(int button, int action, int mods)
{
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_MIDDLE &&
        shift_pressed())
    {
        double x, y;
        cursor_pos(x, y);
        Vertex v = pick_vertex(x, y);
        if (mesh_.is_valid(v))
        {
            // setup seed
            std::vector<Vertex> seed;
            seed.push_back(v);

            // compute geodesic distance
            geodesics(mesh_, seed);

            // setup texture coordinates for visualization
            distance_to_texture_coordinates(mesh_);
            mesh_.use_checkerboard_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
    }
    else
    {
        MeshViewer::mouse(button, action, mods);
    }
}
