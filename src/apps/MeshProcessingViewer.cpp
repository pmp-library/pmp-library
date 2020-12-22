// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "MeshProcessingViewer.h"

#include <pmp/algorithms/SurfaceSubdivision.h>
#include <pmp/algorithms/SurfaceFeatures.h>
#include <pmp/algorithms/SurfaceSimplification.h>
#include <pmp/algorithms/SurfaceFairing.h>
#include <pmp/algorithms/SurfaceRemeshing.h>
#include <pmp/algorithms/SurfaceCurvature.h>
#include <pmp/algorithms/SurfaceGeodesic.h>
#include <pmp/algorithms/SurfaceHoleFilling.h>
#include <pmp/algorithms/SurfacePrimitives.h>

#include <imgui.h>

MeshProcessingViewer::MeshProcessingViewer(const char* title, int width,
                                           int height)
    : MeshViewer(title, width, height), smoother_(mesh_)
{
    //crease_angle_ = 90.0;
    //set_draw_mode("Hidden Line");

    // add help items
    add_help_item("O", "Flip mesh orientation", 5);
}

void MeshProcessingViewer::keyboard(int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    switch (key)
    {
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
        case GLFW_KEY_1:
        case GLFW_KEY_2:
        case GLFW_KEY_3:
        case GLFW_KEY_4:
        case GLFW_KEY_5:
        case GLFW_KEY_6:
        case GLFW_KEY_7:
        case GLFW_KEY_8:
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
            SurfaceCurvature analyzer(mesh_);
            analyzer.analyze_tensor(1, true);
            analyzer.mean_curvature_to_texture_coordinates();
            mesh_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
        if (ImGui::Button("Gauss Curvature"))
        {
            SurfaceCurvature analyzer(mesh_);
            analyzer.analyze_tensor(1, true);
            analyzer.gauss_curvature_to_texture_coordinates();
            mesh_.use_cold_warm_texture();
            update_mesh();
            set_draw_mode("Texture");
        }
        if (ImGui::Button("Abs. Max. Curvature"))
        {
            SurfaceCurvature analyzer(mesh_);
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
            smoother_.implicit_smoothing(dt);
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

        if (ImGui::Button("Decimate it!"))
        {
            SurfaceSimplification ss(mesh_);
            ss.initialize(aspect_ratio, 0.0, 0.0, normal_deviation, 0.0);
            ss.simplify(mesh_.n_vertices() * 0.01 * target_percentage);
            update_mesh();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Subdivision"))
    {
        if (ImGui::Button("Loop Subdivision"))
        {
            SurfaceSubdivision(mesh_).loop();
            update_mesh();
        }

        if (ImGui::Button("Sqrt(3) Subdivision"))
        {
            SurfaceSubdivision(mesh_).sqrt3();
            update_mesh();
        }

        if (ImGui::Button("Catmull-Clark Subdivision"))
        {
            SurfaceSubdivision(mesh_).catmull_clark();
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
            SurfaceRemeshing(mesh_).adaptive_remeshing(
                0.001 * bb,  // min length
                1.0 * bb,    // max length
                0.001 * bb); // approx. error
            update_mesh();
        }

        if (ImGui::Button("Uniform Remeshing"))
        {
            Scalar l(0);
            for (auto eit : mesh_.edges())
                l += distance(mesh_.position(mesh_.vertex(eit, 0)),
                              mesh_.position(mesh_.vertex(eit, 1)));
            l /= (Scalar)mesh_.n_edges();
            SurfaceRemeshing(mesh_).uniform_remeshing(l);
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
                SurfaceHoleFilling hf(mesh_);
                hf.fill_hole(hmin);
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
            SurfaceGeodesic geodist(mesh_);
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
