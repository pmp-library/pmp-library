// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/SurfaceSubdivision.h"

namespace pmp {

SurfaceSubdivision::SurfaceSubdivision(SurfaceMesh& mesh) : mesh_(mesh)
{
    points_ = mesh_.vertex_property<Point>("v:point");
    vfeature_ = mesh_.get_vertex_property<bool>("v:feature");
    efeature_ = mesh_.get_edge_property<bool>("e:feature");
}

void SurfaceSubdivision::catmull_clark()
{
    // reserve memory
    size_t nv = mesh_.n_vertices();
    size_t ne = mesh_.n_edges();
    size_t nf = mesh_.n_faces();
    mesh_.reserve(nv + ne + nf, 2 * ne + 4 * nf, 4 * nf);

    // get properties
    auto vpoint = mesh_.add_vertex_property<Point>("catmull:vpoint");
    auto epoint = mesh_.add_edge_property<Point>("catmull:epoint");
    auto fpoint = mesh_.add_face_property<Point>("catmull:fpoint");

    // compute face vertices
    for (auto f : mesh_.faces())
    {
        Point p(0, 0, 0);
        Scalar c(0);
        for (auto v : mesh_.vertices(f))
        {
            p += points_[v];
            ++c;
        }
        p /= c;
        fpoint[f] = p;
    }

    // compute edge vertices
    for (auto e : mesh_.edges())
    {
        // boundary or feature edge?
        if (mesh_.is_boundary(e) || (efeature_ && efeature_[e]))
        {
            epoint[e] = 0.5f * (points_[mesh_.vertex(e, 0)] +
                                points_[mesh_.vertex(e, 1)]);
        }

        // interior edge
        else
        {
            Point p(0, 0, 0);
            p += points_[mesh_.vertex(e, 0)];
            p += points_[mesh_.vertex(e, 1)];
            p += fpoint[mesh_.face(e, 0)];
            p += fpoint[mesh_.face(e, 1)];
            p *= 0.25f;
            epoint[e] = p;
        }
    }

    // compute new positions for old vertices
    for (auto v : mesh_.vertices())
    {
        // isolated vertex?
        if (mesh_.is_isolated(v))
        {
            vpoint[v] = points_[v];
        }

        // boundary vertex?
        else if (mesh_.is_boundary(v))
        {
            auto h1 = mesh_.halfedge(v);
            auto h0 = mesh_.prev_halfedge(h1);

            Point p = points_[v];
            p *= 6.0;
            p += points_[mesh_.to_vertex(h1)];
            p += points_[mesh_.from_vertex(h0)];
            p *= 0.125;

            vpoint[v] = p;
        }

        // interior feature vertex?
        else if (vfeature_ && vfeature_[v])
        {
            Point p = points_[v];
            p *= 6.0;
            int count(0);

            for (auto h : mesh_.halfedges(v))
            {
                if (efeature_[mesh_.edge(h)])
                {
                    p += points_[mesh_.to_vertex(h)];
                    ++count;
                }
            }

            if (count == 2) // vertex is on feature edge
            {
                p *= 0.125;
                vpoint[v] = p;
            }
            else // keep fixed
            {
                vpoint[v] = points_[v];
            }
        }

        // interior vertex
        else
        {
            // weights from SIGGRAPH paper "Subdivision Surfaces in Character Animation"

            const Scalar k = mesh_.valence(v);
            Point p(0, 0, 0);

            for (auto vv : mesh_.vertices(v))
                p += points_[vv];

            for (auto f : mesh_.faces(v))
                p += fpoint[f];

            p /= (k * k);

            p += ((k - 2.0f) / k) * points_[v];

            vpoint[v] = p;
        }
    }

    // assign new positions to old vertices
    for (auto v : mesh_.vertices())
    {
        points_[v] = vpoint[v];
    }

    // split edges
    for (auto e : mesh_.edges())
    {
        // feature edge?
        if (efeature_ && efeature_[e])
        {
            auto h = mesh_.insert_vertex(e, epoint[e]);
            auto v = mesh_.to_vertex(h);
            auto e0 = mesh_.edge(h);
            auto e1 = mesh_.edge(mesh_.next_halfedge(h));

            vfeature_[v] = true;
            efeature_[e0] = true;
            efeature_[e1] = true;
        }

        // normal edge
        else
        {
            mesh_.insert_vertex(e, epoint[e]);
        }
    }

    // split faces
    for (auto f : mesh_.faces())
    {
        auto h0 = mesh_.halfedge(f);
        mesh_.insert_edge(h0, mesh_.next_halfedge(mesh_.next_halfedge(h0)));

        auto h1 = mesh_.next_halfedge(h0);
        mesh_.insert_vertex(mesh_.edge(h1), fpoint[f]);

        auto h =
            mesh_.next_halfedge(mesh_.next_halfedge(mesh_.next_halfedge(h1)));
        while (h != h0)
        {
            mesh_.insert_edge(h1, h);
            h = mesh_.next_halfedge(
                mesh_.next_halfedge(mesh_.next_halfedge(h1)));
        }
    }

    // clean-up properties
    mesh_.remove_vertex_property(vpoint);
    mesh_.remove_edge_property(epoint);
    mesh_.remove_face_property(fpoint);
}

void SurfaceSubdivision::loop()
{
    if (!mesh_.is_triangle_mesh())
        return;

    // reserve memory
    size_t nv = mesh_.n_vertices();
    size_t ne = mesh_.n_edges();
    size_t nf = mesh_.n_faces();
    mesh_.reserve(nv + ne, 2 * ne + 3 * nf, 4 * nf);

    // add properties
    auto vpoint = mesh_.add_vertex_property<Point>("loop:vpoint");
    auto epoint = mesh_.add_edge_property<Point>("loop:epoint");

    // compute vertex positions
    for (auto v : mesh_.vertices())
    {
        // isolated vertex?
        if (mesh_.is_isolated(v))
        {
            vpoint[v] = points_[v];
        }

        // boundary vertex?
        else if (mesh_.is_boundary(v))
        {
            auto h1 = mesh_.halfedge(v);
            auto h0 = mesh_.prev_halfedge(h1);

            Point p = points_[v];
            p *= 6.0;
            p += points_[mesh_.to_vertex(h1)];
            p += points_[mesh_.from_vertex(h0)];
            p *= 0.125;
            vpoint[v] = p;
        }

        // interior feature vertex?
        else if (vfeature_ && vfeature_[v])
        {
            Point p = points_[v];
            p *= 6.0;
            int count(0);

            for (auto h : mesh_.halfedges(v))
            {
                if (efeature_[mesh_.edge(h)])
                {
                    p += points_[mesh_.to_vertex(h)];
                    ++count;
                }
            }

            if (count == 2) // vertex is on feature edge
            {
                p *= 0.125;
                vpoint[v] = p;
            }
            else // keep fixed
            {
                vpoint[v] = points_[v];
            }
        }

        // interior vertex
        else
        {
            Point p(0, 0, 0);
            Scalar k(0);

            for (auto vv : mesh_.vertices(v))
            {
                p += points_[vv];
                ++k;
            }
            p /= k;

            Scalar beta =
                (0.625 - pow(0.375 + 0.25 * cos(2.0 * M_PI / k), 2.0));

            vpoint[v] = points_[v] * (Scalar)(1.0 - beta) + beta * p;
        }
    }

    // compute edge positions
    for (auto e : mesh_.edges())
    {
        // boundary or feature edge?
        if (mesh_.is_boundary(e) || (efeature_ && efeature_[e]))
        {
            epoint[e] =
                (points_[mesh_.vertex(e, 0)] + points_[mesh_.vertex(e, 1)]) *
                Scalar(0.5);
        }

        // interior edge
        else
        {
            auto h0 = mesh_.halfedge(e, 0);
            auto h1 = mesh_.halfedge(e, 1);
            Point p = points_[mesh_.to_vertex(h0)];
            p += points_[mesh_.to_vertex(h1)];
            p *= 3.0;
            p += points_[mesh_.to_vertex(mesh_.next_halfedge(h0))];
            p += points_[mesh_.to_vertex(mesh_.next_halfedge(h1))];
            p *= 0.125;
            epoint[e] = p;
        }
    }

    // set new vertex positions
    for (auto v : mesh_.vertices())
    {
        points_[v] = vpoint[v];
    }

    // insert new vertices on edges
    for (auto e : mesh_.edges())
    {
        // feature edge?
        if (efeature_ && efeature_[e])
        {
            auto h = mesh_.insert_vertex(e, epoint[e]);
            auto v = mesh_.to_vertex(h);
            auto e0 = mesh_.edge(h);
            auto e1 = mesh_.edge(mesh_.next_halfedge(h));

            vfeature_[v] = true;
            efeature_[e0] = true;
            efeature_[e1] = true;
        }

        // normal edge
        else
        {
            mesh_.insert_vertex(e, epoint[e]);
        }
    }

    // split faces
    Halfedge h;
    for (auto f : mesh_.faces())
    {
        h = mesh_.halfedge(f);
        mesh_.insert_edge(h, mesh_.next_halfedge(mesh_.next_halfedge(h)));
        h = mesh_.next_halfedge(h);
        mesh_.insert_edge(h, mesh_.next_halfedge(mesh_.next_halfedge(h)));
        h = mesh_.next_halfedge(h);
        mesh_.insert_edge(h, mesh_.next_halfedge(mesh_.next_halfedge(h)));
    }

    // clean-up properties
    mesh_.remove_vertex_property(vpoint);
    mesh_.remove_edge_property(epoint);
}

void SurfaceSubdivision::sqrt3()
{
    // reserve memory
    int nv = mesh_.n_vertices();
    int ne = mesh_.n_edges();
    int nf = mesh_.n_faces();
    mesh_.reserve(nv + nf, ne + 3 * nf, 3 * nf);

    auto points = mesh_.vertex_property<Point>("v:point");

    // remember end of old vertices and edges
    auto vend = mesh_.vertices_end();
    auto eend = mesh_.edges_end();

    // compute new positions of old vertices
    auto new_pos = mesh_.add_vertex_property<Point>("v:np");
    for (auto v : mesh_.vertices())
    {
        if (!mesh_.is_boundary(v))
        {
            Scalar n = mesh_.valence(v);
            Scalar alpha = (4.0 - 2.0 * cos(2.0 * M_PI / n)) / 9.0;
            Point p(0, 0, 0);

            for (auto vv : mesh_.vertices(v))
                p += points_[vv];

            p = (1.0f - alpha) * points_[v] + alpha / n * p;
            new_pos[v] = p;
        }
    }

    // split faces
    for (auto f : mesh_.faces())
    {
        Point p(0, 0, 0);
        Scalar c(0);

        for (auto fv : mesh_.vertices(f))
        {
            p += points_[fv];
            ++c;
        }

        p /= c;

        mesh_.split(f, p);
    }

    // set new positions of old vertices
    for (auto vit = mesh_.vertices_begin(); vit != vend; ++vit)
    {
        if (!mesh_.is_boundary(*vit))
        {
            points[*vit] = new_pos[*vit];
        }
    }

    mesh_.remove_vertex_property(new_pos);

    // flip old edges
    for (auto eit = mesh_.edges_begin(); eit != eend; ++eit)
    {
        if (mesh_.is_flip_ok(*eit))
        {
            mesh_.flip(*eit);
        }
    }
}

} // namespace pmp
