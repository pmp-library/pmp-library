// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/differential_geometry.h"

#include <cmath>
#include <limits>

namespace pmp {

Scalar triangle_area(const Point& p0, const Point& p1, const Point& p2)
{
    return Scalar(0.5) * norm(cross(p1 - p0, p2 - p0));
}

Scalar face_area(const SurfaceMesh& mesh, Face f)
{
    Point a(0, 0, 0), q, r;
    for (auto h : mesh.halfedges(f))
    {
        q = mesh.position(mesh.from_vertex(h));
        r = mesh.position(mesh.to_vertex(h));
        a += cross(q, r);
    }

    return 0.5 * norm(a);
}

Scalar surface_area(const SurfaceMesh& mesh)
{
    Scalar A(0);
    for (auto f : mesh.faces())
    {
        A += face_area(mesh, f);
    }
    return A;
}

Scalar edge_area(const SurfaceMesh& mesh, Edge e)
{
    Scalar A(0.0);
    Face f0 = mesh.face(e, 0);
    Face f1 = mesh.face(e, 1);
    if (f0.is_valid())
        A += face_area(mesh, f0) / mesh.valence(f0);
    if (f1.is_valid())
        A += face_area(mesh, f1) / mesh.valence(f1);
    return A;
}

Scalar voronoi_area(const SurfaceMesh& mesh, Vertex v)
{
    Scalar A(0.0);
    for (auto f : mesh.faces(v))
        A += face_area(mesh, f) / mesh.valence(f);
    return A;
}

Scalar voronoi_area_mixed(const SurfaceMesh& mesh, Vertex v)
{
    Scalar area(0.0);

    if (!mesh.is_isolated(v))
    {
        Halfedge h0, h1, h2;
        dvec3 p, q, r, pq, qr, pr;
        double dotp, dotq, dotr;
        double cotq, cotr;

        for (auto h : mesh.halfedges(v))
        {
            h0 = h;
            h1 = mesh.next_halfedge(h0);
            h2 = mesh.next_halfedge(h1);

            if (mesh.is_boundary(h0))
                continue;

            // three vertex positions
            p = (dvec3)mesh.position(mesh.to_vertex(h2));
            q = (dvec3)mesh.position(mesh.to_vertex(h0));
            r = (dvec3)mesh.position(mesh.to_vertex(h1));

            // edge vectors
            (pq = q) -= p;
            (qr = r) -= q;
            (pr = r) -= p;

            // compute and check triangle area
            const auto triangle_area = norm(cross(pq, pr));
            if (triangle_area <= std::numeric_limits<double>::min())
                continue;

            // dot products for each corner (of its two emanating edge vectors)
            dotp = dot(pq, pr);
            dotq = -dot(qr, pq);
            dotr = dot(qr, pr);

            // angle at p is obtuse
            if (dotp < 0.0)
            {
                area += 0.25 * triangle_area;
            }
            // angle at q or r obtuse
            else if (dotq < 0.0 || dotr < 0.0)
            {
                area += 0.125 * triangle_area;
            }
            // no obtuse angles
            else
            {
                // cot(angle) = cos(angle)/sin(angle) = dot(A,B)/norm(cross(A,B))
                cotq = dotq / triangle_area;
                cotr = dotr / triangle_area;

                // clamp cot(angle) by clamping angle to [3, 177]
                area += 0.125 * (sqrnorm(pr) * clamp_cot(cotq) +
                                 sqrnorm(pq) * clamp_cot(cotr));
            }
        }
    }

    assert(!std::isnan(area));
    assert(!std::isinf(area));

    return area;
}

Scalar volume(const SurfaceMesh& mesh)
{
    if (!mesh.is_triangle_mesh())
    {
        throw InvalidInputException("Input is not a triangle mesh!");
    }

    Scalar volume(0);
    for (const auto f : mesh.faces())
    {
        auto fv = mesh.vertices(f);
        const auto& p0 = mesh.position(*fv);
        const auto& p1 = mesh.position(*(++fv));
        const auto& p2 = mesh.position(*(++fv));

        volume += Scalar(1.0) / Scalar(6.0) * dot(cross(p0, p1), p2);
    }

    return std::abs(volume);
}

Point centroid(const SurfaceMesh& mesh, Face f)
{
    Point c(0, 0, 0);
    Scalar n(0);
    for (auto v : mesh.vertices(f))
    {
        c += mesh.position(v);
        ++n;
    }
    c /= n;
    return c;
}

Point centroid(const SurfaceMesh& mesh)
{
    Point center(0, 0, 0), c;
    Scalar aa(0), a;
    for (auto f : mesh.faces())
    {
        a = face_area(mesh, f);
        c = centroid(mesh, f);
        aa += a;
        center += a * c;
    }
    center /= aa;
    return center;
}

void dual(SurfaceMesh& mesh)
{
    // the new dualized mesh
    SurfaceMesh tmp;

    // remember new vertices per face
    auto fvertex = mesh.add_face_property<Vertex>("f:vertex");

    // add centroid for each face
    for (auto f : mesh.faces())
        fvertex[f] = tmp.add_vertex(centroid(mesh, f));

    // add new face for each vertex
    for (auto v : mesh.vertices())
    {
        std::vector<Vertex> vertices;
        for (auto f : mesh.faces(v))
            vertices.push_back(fvertex[f]);

        tmp.add_face(vertices);
    }

    // swap old and new meshes, don't copy properties
    mesh.assign(tmp);
}

double cotan_weight(const SurfaceMesh& mesh, Edge e)
{
    double weight = 0.0;

    const Halfedge h0 = mesh.halfedge(e, 0);
    const Halfedge h1 = mesh.halfedge(e, 1);

    const dvec3 p0 = (dvec3)mesh.position(mesh.to_vertex(h0));
    const dvec3 p1 = (dvec3)mesh.position(mesh.to_vertex(h1));

    if (!mesh.is_boundary(h0))
    {
        const dvec3 p2 =
            (dvec3)mesh.position(mesh.to_vertex(mesh.next_halfedge(h0)));
        const dvec3 d0 = p0 - p2;
        const dvec3 d1 = p1 - p2;
        const double area = norm(cross(d0, d1));
        if (area > std::numeric_limits<double>::min())
        {
            const double cot = dot(d0, d1) / area;
            // weight += clamp_cot(cot);
            weight += cot;
        }
    }

    if (!mesh.is_boundary(h1))
    {
        const dvec3 p2 =
            (dvec3)mesh.position(mesh.to_vertex(mesh.next_halfedge(h1)));
        const dvec3 d0 = p0 - p2;
        const dvec3 d1 = p1 - p2;
        const double area = norm(cross(d0, d1));
        if (area > std::numeric_limits<double>::min())
        {
            const double cot = dot(d0, d1) / area;
            // weight += clamp_cot(cot);
            weight += cot;
        }
    }

    assert(!std::isnan(weight));
    assert(!std::isinf(weight));

    return weight;
}

Point laplace(const SurfaceMesh& mesh, Vertex v)
{
    Point laplace(0.0, 0.0, 0.0);

    if (!mesh.is_isolated(v))
    {
        Scalar sum_weights(0.0);

        for (auto h : mesh.halfedges(v))
        {
            const auto weight = cotan_weight(mesh, mesh.edge(h));
            sum_weights += weight;
            laplace += weight * mesh.position(mesh.to_vertex(h));
        }

        laplace -= sum_weights * mesh.position(v);
        laplace /= Scalar(2.0) * voronoi_area(mesh, v);
    }

    return laplace;
}

} // namespace pmp
