// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "MeshProcessingViewer.h"

#include <pmp/algorithms/Subdivision.h>
#include <pmp/algorithms/Features.h>
#include <pmp/algorithms/Decimation.h>
#include <pmp/algorithms/Fairing.h>
#include <pmp/algorithms/Remeshing.h>
#include <pmp/algorithms/Curvature.h>
#include <pmp/algorithms/Geodesics.h>
#include <pmp/algorithms/HoleFilling.h>
#include <pmp/algorithms/Shapes.h>
#include <pmp/algorithms/Triangulation.h>
#include <pmp/algorithms/DifferentialGeometry.h>

#include <imgui.h>

using namespace pmp;

MeshProcessingViewer::MeshProcessingViewer(const char* title, int width,
                                           int height)
    : MeshViewer(title, width, height), smoother_(mesh_)
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
            Features(mesh_).detect_angle(25);
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
            SurfaceMeshGL new_mesh;
            for (auto v : mesh_.vertices())
            {
                new_mesh.add_vertex(mesh_.position(v));
            }
            for (auto f : mesh_.faces())
            {
                std::vector<Vertex> vertices;
                for (auto v : mesh_.vertices(f))
                {
                    vertices.push_back(v);
                }
                std::reverse(vertices.begin(), vertices.end());
                new_mesh.add_face(vertices);
            }
            mesh_ = new_mesh;
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
            Triangulation tr(mesh_);
            tr.triangulate();
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
                    mesh_.assign(Shapes::tetrahedron());
                    break;
                case GLFW_KEY_2:
                    mesh_.assign(Shapes::octahedron());
                    break;
                case GLFW_KEY_3:
                    mesh_.assign(Shapes::hexahedron());
                    break;
                case GLFW_KEY_4:
                    mesh_.assign(Shapes::icosahedron());
                    break;
                case GLFW_KEY_5:
                    mesh_.assign(Shapes::dodecahedron());
                    break;
                case GLFW_KEY_6:
                    mesh_.assign(Shapes::icosphere(3));
                    break;
                case GLFW_KEY_7:
                    mesh_.assign(Shapes::quad_sphere(3));
                    break;
                case GLFW_KEY_8:
                    mesh_.assign(Shapes::uv_sphere());
                    break;
                case GLFW_KEY_9:
                    mesh_.assign(Shapes::torus());
                    break;
            }

            BoundingBox bb = mesh_.bounds();
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
            Curvature analyzer(mesh_);
            analyzer.analyze_tensor(1, true);
            analyzer.mean_curvature_to_texture_coordinates();
            mesh_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
        if (ImGui::Button("Gauss Curvature"))
        {
            Curvature analyzer(mesh_);
            analyzer.analyze_tensor(1, true);
            analyzer.gauss_curvature_to_texture_coordinates();
            mesh_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
        if (ImGui::Button("Abs. Max. Curvature"))
        {
            Curvature analyzer(mesh_);
            analyzer.analyze_tensor(1, true);
            analyzer.max_curvature_to_texture_coordinates();
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
            smoother_.explicit_smoothing(iterations);
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
                smoother_.implicit_smoothing(dt);
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
                Decimation decimater(mesh_);
                decimater.initialize(aspect_ratio, 0.0, 0.0, normal_deviation,
                                     0.0, 0.01, seam_angle_deviation);
                decimater.decimate(mesh_.n_vertices() * 0.01 *
                                   target_percentage);
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
                Subdivision(mesh_).loop();
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
            Subdivision(mesh_).quad_tri();
            update_mesh();
        }

        if (ImGui::Button("Catmull-Clark Subdivision"))
        {
            Subdivision(mesh_).catmull_clark();
            update_mesh();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Remeshing"))
    {
        if (ImGui::Button("Adaptive Remeshing"))
        {
            auto bb = mesh_.bounds().size();

            try
            {
                Remeshing(mesh_).adaptive_remeshing(
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
                Remeshing(mesh_).uniform_remeshing(l);
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
                    HoleFilling hf(mesh_);
                    hf.fill_hole(hmin);
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
            Geodesics geodist(mesh_);
            geodist.compute(seed);

            // setup texture coordinates for visualization
            geodist.distance_to_texture_coordinates();
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
