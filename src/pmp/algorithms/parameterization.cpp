// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/parameterization.h"
#include "pmp/algorithms/laplace.h"

#include <cmath>

#include "pmp/surface_mesh.h"

namespace pmp {
namespace {

bool has_boundary(const SurfaceMesh& mesh)
{
    for (auto v : mesh.vertices())
        if (mesh.is_boundary(v))
            return true;
    return false;
}

void setup_boundary_constraints(SurfaceMesh& mesh)
{
    // get properties
    auto points = mesh.vertex_property<Point>("v:point");
    auto tex = mesh.vertex_property<TexCoord>("v:tex");

    SurfaceMesh::VertexIterator vit, vend = mesh.vertices_end();
    Vertex vh;
    Halfedge hh;
    std::vector<Vertex> loop;

    // Initialize all texture coordinates to the origin.
    for (auto v : mesh.vertices())
        tex[v] = TexCoord(0.5, 0.5);

    // find 1st boundary vertex
    for (vit = mesh.vertices_begin(); vit != vend; ++vit)
        if (mesh.is_boundary(*vit))
            break;

    // collect boundary loop
    vh = *vit;
    hh = mesh.halfedge(vh);
    do
    {
        loop.push_back(mesh.to_vertex(hh));
        hh = mesh.next_halfedge(hh);
    } while (hh != mesh.halfedge(vh));

    // map boundary loop to unit circle in texture domain
    unsigned int i, n = loop.size();
    Scalar angle, l, length;
    TexCoord t;

    // compute length of boundary loop
    for (i = 0, length = 0.0; i < n; ++i)
        length += distance(points[loop[i]], points[loop[(i + 1) % n]]);

    // map length intervalls to unit circle intervals
    for (i = 0, l = 0.0; i < n;)
    {
        // go from 2pi to 0 to preserve orientation
        angle = 2.0 * M_PI * (1.0 - l / length);

        t[0] = 0.5 + 0.5 * cosf(angle);
        t[1] = 0.5 + 0.5 * sinf(angle);

        tex[loop[i]] = t;

        ++i;
        if (i < n)
        {
            l += distance(points[loop[i]], points[loop[(i + 1) % n]]);
        }
    }
}

void setup_lscm_boundary(SurfaceMesh& mesh)
{
    // constrain the two boundary vertices farthest from each other to fix
    // the translation and rotation of the resulting parameterization

    // vertex properties
    auto pos = mesh.vertex_property<Point>("v:point");
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    auto locked = mesh.add_vertex_property<bool>("v:locked", false);

    // find boundary vertices and store handles in vector
    std::vector<Vertex> boundary;
    for (auto v : mesh.vertices())
        if (mesh.is_boundary(v))
            boundary.push_back(v);

    // find boundary vertices with largest distance
    Scalar diam(0.0), d;
    Vertex v1, v2;
    for (auto vv1 : boundary)
    {
        for (auto vv2 : boundary)
        {
            d = distance(pos[vv1], pos[vv2]);
            if (d > diam)
            {
                diam = d;
                v1 = vv1;
                v2 = vv2;
            }
        }
    }

    // pin these two boundary vertices
    for (auto v : mesh.vertices())
    {
        tex[v] = TexCoord(0.5, 0.5);
        locked[v] = false;
    }
    tex[v1] = TexCoord(0.0, 0.0);
    tex[v2] = TexCoord(1.0, 1.0);
    locked[v1] = true;
    locked[v2] = true;
}
} // namespace

void harmonic_parameterization(SurfaceMesh& mesh, bool use_uniform_weights)
{
    // check precondition
    if (!has_boundary(mesh))
    {
        auto what = std::string{__func__} + ": Mesh has no boundary.";
        throw InvalidInputException(what);
    }

    // map boundary to circle
    setup_boundary_constraints(mesh);

    // get properties
    auto tex = mesh.vertex_property<TexCoord>("v:tex");

    // build system matrix (clamp negative cotan weights to zero)
    SparseMatrix L;
    if (use_uniform_weights)
        uniform_laplace_matrix(mesh, L);
    else
        laplace_matrix(mesh, L, true);

    // build right-hand side B and inject boundary constraints
    DenseMatrix B(mesh.n_vertices(), 2);
    B.setZero();
    for (auto v : mesh.vertices())
        if (mesh.is_boundary(v))
            B.row(v.idx()) = static_cast<Eigen::Vector2d>(tex[v]);

    // solve system
    auto is_constrained = [&](unsigned int i) {
        return mesh.is_boundary(Vertex(i));
    };
    DenseMatrix X = cholesky_solve(L, B, is_constrained, B);

    // copy solution
    for (auto v : mesh.vertices())
        if (!mesh.is_boundary(v))
            tex[v] = X.row(v.idx());
}

void lscm_parameterization(SurfaceMesh& mesh)
{
    // check precondition
    if (!has_boundary(mesh))
    {
        auto what = std::string{__func__} + ": Mesh has no boundary.";
        throw InvalidInputException(what);
    }

    // boundary constraints
    setup_lscm_boundary(mesh);

    // properties
    auto pos = mesh.vertex_property<Point>("v:point");
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    auto idx = mesh.add_vertex_property<int>("v:idx", -1);
    auto weight = mesh.add_halfedge_property<dvec2>("h:lscm");
    auto locked = mesh.get_vertex_property<bool>("v:locked");
    assert(locked);

    // compute weights/gradients per face/halfedge
    for (auto f : mesh.faces())
    {
        // collect face halfedge
        auto fh_it = mesh.halfedges(f);
        auto ha = *fh_it;
        ++fh_it;
        auto hb = *fh_it;
        ++fh_it;
        auto hc = *fh_it;

        // collect face vertices
        auto a = (dvec3)pos[mesh.to_vertex(ha)];
        auto b = (dvec3)pos[mesh.to_vertex(hb)];
        auto c = (dvec3)pos[mesh.to_vertex(hc)];

        // calculate local coordinate system
        dvec3 z = normalize(cross(normalize(c - b), normalize(a - b)));
        dvec3 x = normalize(b - a);
        dvec3 y = normalize(cross(z, x));

        // calculate local vertex coordinates
        dvec2 a2d(0.0, 0.0);
        dvec2 b2d(norm(b - a), 0.0);
        dvec2 c2d(dot(c - a, x), dot(c - a, y));

        // calculate double triangle area
        z = cross(c - b, a - b);
        double area = norm(z);
        if (area)
            area = 1.0 / area;

        // uncomment the following line if your meshes are very bad...
        //area = 1.0;

        // calculate W_j,Ti (index by corner a,b,c and real/imaginary)
        double w_ar = c2d[0] - b2d[0];
        double w_br = a2d[0] - c2d[0];
        double w_cr = b2d[0] - a2d[0];
        double w_ai = c2d[1] - b2d[1];
        double w_bi = a2d[1] - c2d[1];
        double w_ci = b2d[1] - a2d[1];

        // store matrix information per halfedge
        weight[ha] = dvec2(w_ar * area, w_ai * area);
        weight[hb] = dvec2(w_br * area, w_bi * area);
        weight[hc] = dvec2(w_cr * area, w_ci * area);
    }

    // collect free (non-boundary) vertices in array free_vertices[]
    // assign indices such that idx[ free_vertices[j] ] == j
    unsigned int j = 0;
    std::vector<Vertex> free_vertices;
    free_vertices.reserve(mesh.n_vertices());
    for (auto v : mesh.vertices())
    {
        if (!locked[v])
        {
            idx[v] = j++;
            free_vertices.push_back(v);
        }
    }

    // build matrix and rhs
    const unsigned int nv2 = 2 * mesh.n_vertices();
    const unsigned int nv = mesh.n_vertices();
    const unsigned int n = free_vertices.size();
    Vertex vi, vj;
    Halfedge hh;
    double si, sj0, sj1, sign;
    int row(0), c0, c1;

    Eigen::SparseMatrix<double> A(2 * n, 2 * n);
    Eigen::VectorXd b = Eigen::VectorXd::Zero(2 * n);
    std::vector<Eigen::Triplet<double>> triplets;

    for (unsigned int i = 0; i < nv2; ++i)
    {
        vi = Vertex(i % nv);

        if (i < nv)
        {
            sign = 1.0;
            c0 = 0;
            c1 = 1;
        }
        else
        {
            sign = -1.0;
            c0 = 1;
            c1 = 0;
        }

        if (!locked[vi])
        {
            si = 0;

            for (auto h : mesh.halfedges(vi))
            {
                vj = mesh.to_vertex(h);
                sj0 = sj1 = 0;

                if (!mesh.is_boundary(h))
                {
                    const dvec2& wj = weight[h];
                    const dvec2& wi = weight[mesh.prev_halfedge(h)];

                    sj0 += sign * wi[c0] * wj[0] + wi[c1] * wj[1];
                    sj1 += -sign * wi[c0] * wj[1] + wi[c1] * wj[0];
                    si += wi[0] * wi[0] + wi[1] * wi[1];
                }

                h = mesh.opposite_halfedge(h);
                if (!mesh.is_boundary(h))
                {
                    const dvec2& wi = weight[h];
                    const dvec2& wj = weight[mesh.prev_halfedge(h)];

                    sj0 += sign * wi[c0] * wj[0] + wi[c1] * wj[1];
                    sj1 += -sign * wi[c0] * wj[1] + wi[c1] * wj[0];
                    si += wi[0] * wi[0] + wi[1] * wi[1];
                }

                if (!locked[vj])
                {
                    triplets.emplace_back(row, idx[vj], sj0);
                    triplets.emplace_back(row, idx[vj] + n, sj1);
                }
                else
                {
                    b[row] -= sj0 * tex[vj][0];
                    b[row] -= sj1 * tex[vj][1];
                }
            }

            triplets.emplace_back(row, idx[vi] + (i < nv ? 0 : n), 0.5 * si);

            ++row;
        }
    }

    // build sparse matrix from triplets
    A.setFromTriplets(triplets.begin(), triplets.end());

    // solve A*X = B
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver(A);
    Eigen::VectorXd x = solver.solve(b);
    if (solver.info() != Eigen::Success)
    {
        // clean-up
        mesh.remove_vertex_property(idx);
        mesh.remove_vertex_property(locked);
        mesh.remove_halfedge_property(weight);
        auto what = std::string{__func__} + ": Failed solve linear system.";
        throw SolverException(what);
    }
    else
    {
        // copy solution
        for (unsigned int i = 0; i < n; ++i)
        {
            tex[free_vertices[i]] = TexCoord(x[i], x[i + n]);
        }
    }

    // scale tex coordinates to unit square
    TexCoord bbmin(1, 1), bbmax(0, 0);
    for (auto v : mesh.vertices())
    {
        bbmin = min(bbmin, tex[v]);
        bbmax = max(bbmax, tex[v]);
    }
    bbmax -= bbmin;
    Scalar s = std::max(bbmax[0], bbmax[1]);
    for (auto v : mesh.vertices())
    {
        tex[v] -= bbmin;
        tex[v] /= s;
    }

    // clean-up
    mesh.remove_vertex_property(idx);
    mesh.remove_vertex_property(locked);
    mesh.remove_halfedge_property(weight);
}

} // namespace pmp
