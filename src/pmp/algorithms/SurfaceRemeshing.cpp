// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/SurfaceRemeshing.h"

#include <cmath>

#include <algorithm>

#include "pmp/algorithms/TriangleKdTree.h"
#include "pmp/algorithms/SurfaceCurvature.h"
#include "pmp/algorithms/SurfaceNormals.h"
#include "pmp/algorithms/BarycentricCoordinates.h"
#include "pmp/algorithms/DifferentialGeometry.h"

namespace pmp {

SurfaceRemeshing::SurfaceRemeshing(SurfaceMesh& mesh)
    : mesh_(mesh), refmesh_(nullptr), kd_tree_(nullptr)
{
    points_ = mesh_.vertex_property<Point>("v:point");

    SurfaceNormals::compute_vertex_normals(mesh_);
    vnormal_ = mesh_.vertex_property<Point>("v:normal");
}

SurfaceRemeshing::~SurfaceRemeshing() = default;

void SurfaceRemeshing::uniform_remeshing(Scalar edge_length,
                                         unsigned int iterations,
                                         bool use_projection)
{
    if (!mesh_.is_triangle_mesh())
    {
        std::cerr << "Not a triangle mesh!" << std::endl;
        return;
    }

    uniform_ = true;
    use_projection_ = use_projection;
    target_edge_length_ = edge_length;

    preprocessing();

    for (unsigned int i = 0; i < iterations; ++i)
    {
        split_long_edges();

        SurfaceNormals::compute_vertex_normals(mesh_);

        collapse_short_edges();

        flip_edges();

        tangential_smoothing(5);
    }

    remove_caps();

    postprocessing();
}

void SurfaceRemeshing::adaptive_remeshing(Scalar min_edge_length,
                                          Scalar max_edge_length,
                                          Scalar approx_error,
                                          unsigned int iterations,
                                          bool use_projection)
{
    if (!mesh_.is_triangle_mesh())
    {
        std::cerr << "Not a triangle mesh!" << std::endl;
        return;
    }

    uniform_ = false;
    min_edge_length_ = min_edge_length;
    max_edge_length_ = max_edge_length;
    approx_error_ = approx_error;
    use_projection_ = use_projection;

    preprocessing();

    for (unsigned int i = 0; i < iterations; ++i)
    {
        split_long_edges();

        SurfaceNormals::compute_vertex_normals(mesh_);

        collapse_short_edges();

        flip_edges();

        tangential_smoothing(5);
    }

    remove_caps();

    postprocessing();
}

void SurfaceRemeshing::preprocessing()
{
    // properties
    vfeature_ = mesh_.vertex_property<bool>("v:feature", false);
    efeature_ = mesh_.edge_property<bool>("e:feature", false);
    vlocked_ = mesh_.add_vertex_property<bool>("v:locked", false);
    elocked_ = mesh_.add_edge_property<bool>("e:locked", false);
    vsizing_ = mesh_.add_vertex_property<Scalar>("v:sizing");

    // lock unselected vertices if some vertices are selected
    auto vselected = mesh_.get_vertex_property<bool>("v:selected");
    if (vselected)
    {
        bool has_selection = false;
        for (auto v : mesh_.vertices())
        {
            if (vselected[v])
            {
                has_selection = true;
                break;
            }
        }

        if (has_selection)
        {
            for (auto v : mesh_.vertices())
            {
                vlocked_[v] = !vselected[v];
            }

            // lock an edge if one of its vertices is locked
            for (auto e : mesh_.edges())
            {
                elocked_[e] = (vlocked_[mesh_.vertex(e, 0)] ||
                               vlocked_[mesh_.vertex(e, 1)]);
            }
        }
    }

    // lock feature corners
    for (auto v : mesh_.vertices())
    {
        if (vfeature_[v])
        {
            int c = 0;
            for (auto h : mesh_.halfedges(v))
                if (efeature_[mesh_.edge(h)])
                    ++c;

            if (c != 2)
                vlocked_[v] = true;
        }
    }

    // compute sizing field
    if (uniform_)
    {
        for (auto v : mesh_.vertices())
        {
            vsizing_[v] = target_edge_length_;
        }
    }
    else
    {
        // compute curvature for all mesh vertices, using cotan or Cohen-Steiner
        // don't use two-ring neighborhood, since we otherwise compute
        // curvature over sharp features edges, leading to high curvatures.
        // prefer tensor analysis over cotan-Laplace, since the former is more
        // robust and gives better results on the boundary.
        SurfaceCurvature curv(mesh_);
        curv.analyze_tensor(1);

        // use vsizing_ to store/smooth curvatures to avoid another vertex property

        // curvature values for feature vertices and boundary vertices
        // are not meaningful. mark them as negative values.
        for (auto v : mesh_.vertices())
        {
            if (mesh_.is_boundary(v) || (vfeature_ && vfeature_[v]))
                vsizing_[v] = -1.0;
            else
                vsizing_[v] = curv.max_abs_curvature(v);
        }

        // curvature values might be noisy. smooth them.
        // don't consider feature vertices' curvatures.
        // don't consider boundary vertices' curvatures.
        // do this for two iterations, to propagate curvatures
        // from non-feature regions to feature vertices.
        for (int iters = 0; iters < 2; ++iters)
        {
            for (auto v : mesh_.vertices())
            {
                Scalar w, ww = 0.0;
                Scalar c, cc = 0.0;

                for (auto h : mesh_.halfedges(v))
                {
                    c = vsizing_[mesh_.to_vertex(h)];
                    if (c > 0.0)
                    {
                        w = std::max(0.0, cotan_weight(mesh_, mesh_.edge(h)));
                        ww += w;
                        cc += w * c;
                    }
                }

                if (ww)
                    cc /= ww;
                vsizing_[v] = cc;
            }
        }

        // now convert per-vertex curvature into target edge length
        for (auto v : mesh_.vertices())
        {
            Scalar c = vsizing_[v];

            // get edge length from curvature
            const Scalar r = 1.0 / c;
            const Scalar e = approx_error_;
            Scalar h;
            if (e < r)
            {
                // see mathworld: "circle segment" and "equilateral triangle"
                //h = sqrt(2.0*r*e-e*e) * 3.0 / sqrt(3.0);
                h = sqrt(6.0 * e * r - 3.0 * e * e); // simplified...
            }
            else
            {
                // this does not really make sense
                h = e * 3.0 / sqrt(3.0);
            }

            // clamp to min. and max. edge length
            if (h < min_edge_length_)
                h = min_edge_length_;
            else if (h > max_edge_length_)
                h = max_edge_length_;

            // store target edge length
            vsizing_[v] = h;
        }
    }

    if (use_projection_)
    {
        // build reference mesh
        refmesh_ = new SurfaceMesh();
        refmesh_->assign(mesh_);
        SurfaceNormals::compute_vertex_normals(*refmesh_);
        refpoints_ = refmesh_->vertex_property<Point>("v:point");
        refnormals_ = refmesh_->vertex_property<Point>("v:normal");

        // copy sizing field from mesh_
        refsizing_ = refmesh_->add_vertex_property<Scalar>("v:sizing");
        for (auto v : refmesh_->vertices())
        {
            refsizing_[v] = vsizing_[v];
        }

        // build kd-tree
        kd_tree_ = new TriangleKdTree(*refmesh_, 0);
    }
}

void SurfaceRemeshing::postprocessing()
{
    // delete kd-tree and reference mesh
    if (use_projection_)
    {
        delete kd_tree_;
        delete refmesh_;
    }

    // remove properties
    mesh_.remove_vertex_property(vlocked_);
    mesh_.remove_edge_property(elocked_);
    mesh_.remove_vertex_property(vsizing_);
}

void SurfaceRemeshing::project_to_reference(Vertex v)
{
    if (!use_projection_)
    {
        return;
    }

    // find closest triangle of reference mesh
    TriangleKdTree::NearestNeighbor nn = kd_tree_->nearest(points_[v]);
    const Point p = nn.nearest;
    const Face f = nn.face;

    // get face data
    SurfaceMesh::VertexAroundFaceCirculator fvIt = refmesh_->vertices(f);
    const Point p0 = refpoints_[*fvIt];
    const Point n0 = refnormals_[*fvIt];
    const Scalar s0 = refsizing_[*fvIt];
    ++fvIt;
    const Point p1 = refpoints_[*fvIt];
    const Point n1 = refnormals_[*fvIt];
    const Scalar s1 = refsizing_[*fvIt];
    ++fvIt;
    const Point p2 = refpoints_[*fvIt];
    const Point n2 = refnormals_[*fvIt];
    const Scalar s2 = refsizing_[*fvIt];

    // get barycentric coordinates
    Point b = barycentric_coordinates(p, p0, p1, p2);

    // interpolate normal
    Point n;
    n = (n0 * b[0]);
    n += (n1 * b[1]);
    n += (n2 * b[2]);
    n.normalize();
    assert(!std::isnan(n[0]));

    // interpolate sizing field
    Scalar s;
    s = (s0 * b[0]);
    s += (s1 * b[1]);
    s += (s2 * b[2]);

    // set result
    points_[v] = p;
    vnormal_[v] = n;
    vsizing_[v] = s;
}

void SurfaceRemeshing::split_long_edges()
{
    Vertex vnew, v0, v1;
    Edge enew, e0, e1;
    Face f0, f1, f2, f3;
    bool ok, is_feature, is_boundary;
    int i;

    for (ok = false, i = 0; !ok && i < 10; ++i)
    {
        ok = true;

        for (auto e : mesh_.edges())
        {
            v0 = mesh_.vertex(e, 0);
            v1 = mesh_.vertex(e, 1);

            if (!elocked_[e] && is_too_long(v0, v1))
            {
                const Point& p0 = points_[v0];
                const Point& p1 = points_[v1];

                is_feature = efeature_[e];
                is_boundary = mesh_.is_boundary(e);

                vnew = mesh_.add_vertex((p0 + p1) * 0.5f);
                mesh_.split(e, vnew);

                // need normal or sizing for adaptive refinement
                vnormal_[vnew] =
                    SurfaceNormals::compute_vertex_normal(mesh_, vnew);
                vsizing_[vnew] = 0.5f * (vsizing_[v0] + vsizing_[v1]);

                if (is_feature)
                {
                    enew = is_boundary ? Edge(mesh_.n_edges() - 2)
                                       : Edge(mesh_.n_edges() - 3);
                    efeature_[enew] = true;
                    vfeature_[vnew] = true;
                }
                else
                {
                    project_to_reference(vnew);
                }

                ok = false;
            }
        }
    }
}

void SurfaceRemeshing::collapse_short_edges()
{
    Vertex v0, v1;
    Halfedge h0, h1, h01, h10;
    bool ok, b0, b1, l0, l1, f0, f1;
    int i;
    bool hcol01, hcol10;

    for (ok = false, i = 0; !ok && i < 10; ++i)
    {
        ok = true;

        for (auto e : mesh_.edges())
        {
            if (!mesh_.is_deleted(e) && !elocked_[e])
            {
                h10 = mesh_.halfedge(e, 0);
                h01 = mesh_.halfedge(e, 1);
                v0 = mesh_.to_vertex(h10);
                v1 = mesh_.to_vertex(h01);

                if (is_too_short(v0, v1))
                {
                    // get status
                    b0 = mesh_.is_boundary(v0);
                    b1 = mesh_.is_boundary(v1);
                    l0 = vlocked_[v0];
                    l1 = vlocked_[v1];
                    f0 = vfeature_[v0];
                    f1 = vfeature_[v1];
                    hcol01 = hcol10 = true;

                    // boundary rules
                    if (b0 && b1)
                    {
                        if (!mesh_.is_boundary(e))
                            continue;
                    }
                    else if (b0)
                        hcol01 = false;
                    else if (b1)
                        hcol10 = false;

                    // locked rules
                    if (l0 && l1)
                        continue;
                    else if (l0)
                        hcol01 = false;
                    else if (l1)
                        hcol10 = false;

                    // feature rules
                    if (f0 && f1)
                    {
                        // edge must be feature
                        if (!efeature_[e])
                            continue;

                        // the other two edges removed by collapse must not be features
                        h0 = mesh_.prev_halfedge(h01);
                        h1 = mesh_.next_halfedge(h10);
                        if (efeature_[mesh_.edge(h0)] ||
                            efeature_[mesh_.edge(h1)])
                            hcol01 = false;
                        // the other two edges removed by collapse must not be features
                        h0 = mesh_.prev_halfedge(h10);
                        h1 = mesh_.next_halfedge(h01);
                        if (efeature_[mesh_.edge(h0)] ||
                            efeature_[mesh_.edge(h1)])
                            hcol10 = false;
                    }
                    else if (f0)
                        hcol01 = false;
                    else if (f1)
                        hcol10 = false;

                    // topological rules
                    bool collapse_ok = mesh_.is_collapse_ok(h01);

                    if (hcol01)
                        hcol01 = collapse_ok;
                    if (hcol10)
                        hcol10 = collapse_ok;

                    // both collapses possible: collapse into vertex w/ higher valence
                    if (hcol01 && hcol10)
                    {
                        if (mesh_.valence(v0) < mesh_.valence(v1))
                            hcol10 = false;
                        else
                            hcol01 = false;
                    }

                    // try v1 -> v0
                    if (hcol10)
                    {
                        // don't create too long edges
                        for (auto vv : mesh_.vertices(v1))
                        {
                            if (is_too_long(v0, vv))
                            {
                                hcol10 = false;
                                break;
                            }
                        }

                        if (hcol10)
                        {
                            mesh_.collapse(h10);
                            ok = false;
                        }
                    }

                    // try v0 -> v1
                    else if (hcol01)
                    {
                        // don't create too long edges
                        for (auto vv : mesh_.vertices(v0))
                        {
                            if (is_too_long(v1, vv))
                            {
                                hcol01 = false;
                                break;
                            }
                        }

                        if (hcol01)
                        {
                            mesh_.collapse(h01);
                            ok = false;
                        }
                    }
                }
            }
        }
    }

    mesh_.garbage_collection();
}

void SurfaceRemeshing::flip_edges()
{
    Vertex v0, v1, v2, v3;
    Halfedge h;
    int val0, val1, val2, val3;
    int val_opt0, val_opt1, val_opt2, val_opt3;
    int ve0, ve1, ve2, ve3, ve_before, ve_after;
    bool ok;
    int i;

    // precompute valences
    VertexProperty<int> valence = mesh_.add_vertex_property<int>("valence");
    for (auto v : mesh_.vertices())
    {
        valence[v] = mesh_.valence(v);
    }

    for (ok = false, i = 0; !ok && i < 10; ++i)
    {
        ok = true;

        for (auto e : mesh_.edges())
        {
            if (!elocked_[e] && !efeature_[e])
            {
                h = mesh_.halfedge(e, 0);
                v0 = mesh_.to_vertex(h);
                v2 = mesh_.to_vertex(mesh_.next_halfedge(h));
                h = mesh_.halfedge(e, 1);
                v1 = mesh_.to_vertex(h);
                v3 = mesh_.to_vertex(mesh_.next_halfedge(h));

                if (!vlocked_[v0] && !vlocked_[v1] && !vlocked_[v2] &&
                    !vlocked_[v3])
                {
                    val0 = valence[v0];
                    val1 = valence[v1];
                    val2 = valence[v2];
                    val3 = valence[v3];

                    val_opt0 = (mesh_.is_boundary(v0) ? 4 : 6);
                    val_opt1 = (mesh_.is_boundary(v1) ? 4 : 6);
                    val_opt2 = (mesh_.is_boundary(v2) ? 4 : 6);
                    val_opt3 = (mesh_.is_boundary(v3) ? 4 : 6);

                    ve0 = (val0 - val_opt0);
                    ve1 = (val1 - val_opt1);
                    ve2 = (val2 - val_opt2);
                    ve3 = (val3 - val_opt3);

                    ve0 *= ve0;
                    ve1 *= ve1;
                    ve2 *= ve2;
                    ve3 *= ve3;

                    ve_before = ve0 + ve1 + ve2 + ve3;

                    --val0;
                    --val1;
                    ++val2;
                    ++val3;

                    ve0 = (val0 - val_opt0);
                    ve1 = (val1 - val_opt1);
                    ve2 = (val2 - val_opt2);
                    ve3 = (val3 - val_opt3);

                    ve0 *= ve0;
                    ve1 *= ve1;
                    ve2 *= ve2;
                    ve3 *= ve3;

                    ve_after = ve0 + ve1 + ve2 + ve3;

                    if (ve_before > ve_after && mesh_.is_flip_ok(e))
                    {
                        mesh_.flip(e);
                        --valence[v0];
                        --valence[v1];
                        ++valence[v2];
                        ++valence[v3];
                        ok = false;
                    }
                }
            }
        }
    }

    mesh_.remove_vertex_property(valence);
}

void SurfaceRemeshing::tangential_smoothing(unsigned int iterations)
{
    Vertex v1, v2, v3, vv;
    Edge e;
    Scalar w, ww;
    Point u, n, t, b;

    // add property
    VertexProperty<Point> update = mesh_.add_vertex_property<Point>("v:update");

    // project at the beginning to get valid sizing values and normal vectors
    // for vertices introduced by splitting
    if (use_projection_)
    {
        for (auto v : mesh_.vertices())
        {
            if (!mesh_.is_boundary(v) && !vlocked_[v])
            {
                project_to_reference(v);
            }
        }
    }

    for (unsigned int iters = 0; iters < iterations; ++iters)
    {
        for (auto v : mesh_.vertices())
        {
            if (!mesh_.is_boundary(v) && !vlocked_[v])
            {
                if (vfeature_[v])
                {
                    u = Point(0.0);
                    t = Point(0.0);
                    ww = 0;
                    int c = 0;

                    for (auto h : mesh_.halfedges(v))
                    {
                        if (efeature_[mesh_.edge(h)])
                        {
                            vv = mesh_.to_vertex(h);

                            b = points_[v];
                            b += points_[vv];
                            b *= 0.5;

                            w = distance(points_[v], points_[vv]) /
                                (0.5 * (vsizing_[v] + vsizing_[vv]));
                            ww += w;
                            u += w * b;

                            if (c == 0)
                            {
                                t += normalize(points_[vv] - points_[v]);
                                ++c;
                            }
                            else
                            {
                                ++c;
                                t -= normalize(points_[vv] - points_[v]);
                            }
                        }
                    }

                    assert(c == 2);

                    u *= (1.0 / ww);
                    u -= points_[v];
                    t = normalize(t);
                    u = t * dot(u, t);

                    update[v] = u;
                }
                else
                {
#if 0
                    Scalar area;
                    u = Point(0.0);
                    t = Point(0.0);
                    ww = 0;

                    for (auto h : mesh_.halfedges(v))
                    {
                        v1 = v;
                        v2 = mesh_.to_vertex(h);
                        v3 = mesh_.to_vertex(mesh_.next_halfedge(h));

                        b = points_[v1];
                        b += points_[v2];
                        b += points_[v3];
                        b *= (1.0 / 3.0);

                        area = norm(cross(points_[v2] - points_[v1],
                                          points_[v3] - points_[v1]));
                        w = area /
                            pow((vsizing_[v1] + vsizing_[v2] + vsizing_[v3]) /
                                    3.0,
                                2.0);

                        u += w * b;
                        ww += w;
                    }

                    u /= ww;
                    u -= points_[v];
                    n = vnormal_[v];
                    u -= n * dot(u, n);

                    update[v] = u;
#else
                    Point p = minimize_squared_areas(v);
                    u = p - mesh_.position(v);

                    n = vnormal_[v];
                    u -= n * dot(u, n);

                    update[v] = u;
#endif
                }
            }
        }

        // update vertex positions
        for (auto v : mesh_.vertices())
        {
            if (!mesh_.is_boundary(v) && !vlocked_[v])
            {
                points_[v] += update[v];
            }
        }

        // update normal vectors (if not done so through projection)
        SurfaceNormals::compute_vertex_normals(mesh_);
    }

    // project at the end
    if (use_projection_)
    {
        for (auto v : mesh_.vertices())
        {
            if (!mesh_.is_boundary(v) && !vlocked_[v])
            {
                project_to_reference(v);
            }
        }
    }

    // remove property
    mesh_.remove_vertex_property(update);
}

void SurfaceRemeshing::remove_caps()
{
    Halfedge h;
    Vertex v, vb, vd;
    Face fb, fd;
    Scalar a0, a1, amin, aa(::cos(170.0 * M_PI / 180.0));
    Point a, b, c, d;

    for (auto e : mesh_.edges())
    {
        if (!elocked_[e] && mesh_.is_flip_ok(e))
        {
            h = mesh_.halfedge(e, 0);
            a = points_[mesh_.to_vertex(h)];

            h = mesh_.next_halfedge(h);
            b = points_[vb = mesh_.to_vertex(h)];

            h = mesh_.halfedge(e, 1);
            c = points_[mesh_.to_vertex(h)];

            h = mesh_.next_halfedge(h);
            d = points_[vd = mesh_.to_vertex(h)];

            a0 = dot(normalize(a - b), normalize(c - b));
            a1 = dot(normalize(a - d), normalize(c - d));

            if (a0 < a1)
            {
                amin = a0;
                v = vb;
            }
            else
            {
                amin = a1;
                v = vd;
            }

            // is it a cap?
            if (amin < aa)
            {
                // feature edge and feature vertex -> seems to be intended
                if (efeature_[e] && vfeature_[v])
                    continue;

                // project v onto feature edge
                if (efeature_[e])
                    points_[v] = (a + c) * 0.5f;

                // flip
                mesh_.flip(e);
            }
        }
    }
}

Point SurfaceRemeshing::minimize_squared_areas(Vertex v)
{
    // setup matrix of one-ring neighbors' positions
    const unsigned int n = mesh_.valence(v);
    Eigen::MatrixXd poly(n, 3);
    int i = 0;
    for (auto vv : mesh_.vertices(v))
    {
        poly.row(i++) = (Eigen::Vector3d)points_[vv];
    }

    // build Hessian and Jacobian
    Eigen::Matrix3d H;
    H.setZero();
    Eigen::Vector3d J;
    J.setZero();
    for (unsigned int i = 0; i < n; ++i)
    {
        Eigen::Vector3d p = poly.row(i);
        Eigen::Vector3d q = poly.row((i + 1) % n);
        Eigen::Vector3d d = p - q;

        double w = 1.0 / d.norm();

        H(0, 0) += w * (d(1) * d(1) + d(2) * d(2));
        H(1, 0) += w * (-d(0) * d(1));
        H(2, 0) += w * (-d(0) * d(2));

        H(0, 1) += w * (-d(0) * d(1));
        H(1, 1) += w * (d(0) * d(0) + d(2) * d(2));
        H(2, 1) += w * (-d(1) * d(2));

        H(0, 2) += w * (-d(0) * d(2));
        H(1, 2) += w * (-d(1) * d(2));
        H(2, 2) += w * (d(0) * d(0) + d(1) * d(1));

        J(0) += w * (-d(1) * p(1) * q(0) - d(2) * p(2) * q(0) +
                     d(1) * p(0) * q(1) + d(2) * p(0) * q(2));
        J(1) += w * (d(0) * p(1) * q(0) - d(0) * p(0) * q(1) -
                     d(2) * p(2) * q(1) + d(2) * p(1) * q(2));
        J(2) += w * (d(0) * p(2) * q(0) + d(1) * p(2) * q(1) -
                     d(0) * p(0) * q(2) - d(1) * p(1) * q(2));
    }

    // compute minimizer
    Eigen::Vector3d x = H.lu().solve(-J);

    return Point(x);
}

} // namespace pmp
