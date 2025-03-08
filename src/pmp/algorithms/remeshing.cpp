#include "remeshing.h"
// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/remeshing.h"

#include <cmath>

#include <algorithm>
#include <memory>
#include <limits>
#include <numbers>

#include "pmp/algorithms/curvature.h"
#include "pmp/algorithms/normals.h"
#include "pmp/algorithms/barycentric_coordinates.h"
#include "pmp/algorithms/differential_geometry.h"
#include "pmp/algorithms/distance_point_triangle.h"
#include "pmp/bounding_box.h"

namespace pmp {
namespace {

class TriangleKdTree
{
public:
    TriangleKdTree(std::shared_ptr<const SurfaceMesh> mesh,
                   unsigned int max_faces = 10, unsigned int max_depth = 30);

    ~TriangleKdTree() { delete root_; }

    struct NearestNeighbor
    {
        Scalar dist;
        Face face;
        Point nearest;
    };

    NearestNeighbor nearest(const Point& p) const;

private:
    using Faces = std::vector<Face>;

    struct Node
    {
        Node() = default;

        ~Node()
        {
            delete faces;
            delete left_child;
            delete right_child;
        }

        unsigned char axis;
        Scalar split;
        Faces* faces{nullptr};
        Node* left_child{nullptr};
        Node* right_child{nullptr};
    };

    void build_recurse(Node* node, unsigned int max_handles,
                       unsigned int depth);

    void nearest_recurse(Node* node, const Point& point,
                         NearestNeighbor& data) const;

    Node* root_;
    std::vector<std::array<Point, 3>> face_points_;
};

TriangleKdTree::TriangleKdTree(std::shared_ptr<const SurfaceMesh> mesh,
                               unsigned int max_faces, unsigned int max_depth)
{
    // init
    root_ = new Node();
    root_->faces = new Faces();

    // collect faces and points
    root_->faces->reserve(mesh->n_faces());
    face_points_.reserve(mesh->n_faces());
    auto points = mesh->get_vertex_property<Point>("v:point");

    for (const auto& f : mesh->faces())
    {
        root_->faces->push_back(f);

        auto v = mesh->vertices(f);
        const auto& p0 = points[*v];
        ++v;
        const auto& p1 = points[*v];
        ++v;
        const auto& p2 = points[*v];
        face_points_.push_back({p0, p1, p2});
    }

    // call recursive helper
    build_recurse(root_, max_faces, max_depth);
}

void TriangleKdTree::build_recurse(Node* node, unsigned int max_faces,
                                   unsigned int depth)
{
    // should we stop at this level ?
    if ((depth == 0) || (node->faces->size() <= max_faces))
        return;

    // compute bounding box
    BoundingBox bbox;
    for (const auto& f : *node->faces)
    {
        const auto idx = f.idx();
        bbox += face_points_[idx][0];
        bbox += face_points_[idx][1];
        bbox += face_points_[idx][2];
    }

    // split longest side of bounding box
    Point bb = bbox.max() - bbox.min();
    Scalar length = bb[0];
    int axis = 0;
    if (bb[1] > length)
        length = bb[(axis = 1)];
    if (bb[2] > length)
        length = bb[(axis = 2)];

    // split in the middle
    Scalar split = bbox.center()[axis];

    // create children
    auto* left = new Node();
    left->faces = new Faces();
    left->faces->reserve(node->faces->size() / 2);
    auto* right = new Node();
    right->faces = new Faces;
    right->faces->reserve(node->faces->size() / 2);

    // partition for left and right child
    for (const auto& f : *node->faces)
    {
        bool l = false, r = false;

        const auto& pos = face_points_[f.idx()];

        if (pos[0][axis] <= split)
            l = true;
        else
            r = true;
        if (pos[1][axis] <= split)
            l = true;
        else
            r = true;
        if (pos[2][axis] <= split)
            l = true;
        else
            r = true;

        if (l)
        {
            left->faces->push_back(f);
        }

        if (r)
        {
            right->faces->push_back(f);
        }
    }

    // stop here?
    if (left->faces->size() == node->faces->size() ||
        right->faces->size() == node->faces->size())
    {
        // compact my memory
        node->faces->shrink_to_fit();

        // delete new nodes
        delete left;
        delete right;

        // stop recursion
        return;
    }

    // or recurse further?
    else
    {
        // free my memory
        delete node->faces;
        node->faces = nullptr;

        // store internal data
        node->axis = axis;
        node->split = split;
        node->left_child = left;
        node->right_child = right;

        // recurse to children
        build_recurse(node->left_child, max_faces, depth - 1);
        build_recurse(node->right_child, max_faces, depth - 1);
    }
}

TriangleKdTree::NearestNeighbor TriangleKdTree::nearest(const Point& p) const
{
    NearestNeighbor data;
    data.dist = std::numeric_limits<Scalar>::max();
    nearest_recurse(root_, p, data);
    return data;
}

void TriangleKdTree::nearest_recurse(Node* node, const Point& point,
                                     NearestNeighbor& data) const
{
    // terminal node?
    if (!node->left_child)
    {
        for (const auto& f : *node->faces)
        {
            Point n;
            const auto& pos = face_points_[f.idx()];
            auto d = dist_point_triangle(point, pos[0], pos[1], pos[2], n);
            if (d < data.dist)
            {
                data.dist = d;
                data.face = f;
                data.nearest = n;
            }
        }
    }

    // non-terminal node
    else
    {
        Scalar dist = point[node->axis] - node->split;

        if (dist <= 0.0)
        {
            nearest_recurse(node->left_child, point, data);
            if (fabs(dist) < data.dist)
                nearest_recurse(node->right_child, point, data);
        }
        else
        {
            nearest_recurse(node->right_child, point, data);
            if (fabs(dist) < data.dist)
                nearest_recurse(node->left_child, point, data);
        }
    }
}

class Remeshing
{
public:
    Remeshing(SurfaceMesh& mesh);

    void uniform_remeshing(Scalar edge_length, unsigned int iterations = 10,
                           bool use_projection = true);

    void adaptive_remeshing(Scalar min_edge_length, Scalar max_edge_length,
                            Scalar approx_error, unsigned int iterations = 10,
                            bool use_projection = true);

    void custom_remeshing(std::vector<double>& target_edge_lengths,
                          unsigned int iterations = 10, bool use_projection = true);
                            

private:
    void preprocessing();
    void postprocessing();

    void split_long_edges();
    void collapse_short_edges();
    void flip_edges();
    void tangential_smoothing(unsigned int iterations);
    void remove_caps();

    Point minimize_squared_areas(Vertex v);
    Point weighted_centroid(Vertex v);

    void project_to_reference(Vertex v);

    bool is_too_long(Vertex v0, Vertex v1) const
    {
        return distance(points_[v0], points_[v1]) >
               4.0 / 3.0 * std::min(vsizing_[v0], vsizing_[v1]);
    }
    bool is_too_short(Vertex v0, Vertex v1) const
    {
        return distance(points_[v0], points_[v1]) <
               4.0 / 5.0 * std::min(vsizing_[v0], vsizing_[v1]);
    }

    SurfaceMesh& mesh_;
    std::shared_ptr<SurfaceMesh> refmesh_;

    bool use_projection_;
    std::unique_ptr<TriangleKdTree> kd_tree_;

    bool uniform_;
    bool custom_vertex_edgeLengths_;
    std::vector<double> target_edge_lengths_per_vertex_;
    Scalar target_edge_length_;
    Scalar min_edge_length_;
    Scalar max_edge_length_;
    Scalar approx_error_;

    bool has_feature_vertices_{false};
    bool has_feature_edges_{false};
    VertexProperty<Point> points_;
    VertexProperty<Point> vnormal_;
    VertexProperty<bool> vfeature_;
    EdgeProperty<bool> efeature_;
    VertexProperty<bool> vlocked_;
    EdgeProperty<bool> elocked_;
    VertexProperty<Scalar> vsizing_;

    VertexProperty<Point> refpoints_;
    VertexProperty<Point> refnormals_;
    VertexProperty<Scalar> refsizing_;
};

Remeshing::Remeshing(SurfaceMesh& mesh)
    : mesh_(mesh), refmesh_(nullptr), kd_tree_(nullptr)
{
    if (!mesh_.is_triangle_mesh())
        throw InvalidInputException("Input is not a triangle mesh!");

    points_ = mesh_.vertex_property<Point>("v:point");

    vertex_normals(mesh_);
    vnormal_ = mesh_.vertex_property<Point>("v:normal");

    has_feature_vertices_ = mesh_.has_vertex_property("v:feature");
    has_feature_edges_ = mesh_.has_edge_property("e:feature");
}

void Remeshing::uniform_remeshing(Scalar edge_length, unsigned int iterations,
                                  bool use_projection)
{
    uniform_ = true;
    custom_vertex_edgeLengths_ = false;
    use_projection_ = use_projection;
    target_edge_length_ = edge_length;

    preprocessing();

    for (unsigned int i = 0; i < iterations; ++i)
    {
        split_long_edges();

        vertex_normals(mesh_);

        collapse_short_edges();

        flip_edges();

        tangential_smoothing(5);
    }

    remove_caps();

    postprocessing();
}

void Remeshing::adaptive_remeshing(Scalar min_edge_length,
                                   Scalar max_edge_length, Scalar approx_error,
                                   unsigned int iterations, bool use_projection)
{
    uniform_ = false;
    custom_vertex_edgeLengths_ = false;
    min_edge_length_ = min_edge_length;
    max_edge_length_ = max_edge_length;
    approx_error_ = approx_error;
    use_projection_ = use_projection;

    preprocessing();

    for (unsigned int i = 0; i < iterations; ++i)
    {
        split_long_edges();

        vertex_normals(mesh_);

        collapse_short_edges();

        flip_edges();

        tangential_smoothing(5);
    }

    remove_caps();

    postprocessing();
}


void Remeshing::custom_remeshing(std::vector<double>& target_edge_lengths,
                                 unsigned int iterations, bool use_projection)
{
    uniform_ = false;
    custom_vertex_edgeLengths_ = true;
    target_edge_lengths_per_vertex_ = target_edge_lengths;
    use_projection_ = use_projection;
    preprocessing();

    for (unsigned int i = 0; i < iterations; ++i)
    {
        split_long_edges();

        vertex_normals(mesh_);

        collapse_short_edges();

        flip_edges();

        tangential_smoothing(5);
    }
    remove_caps();
    postprocessing();
}

void Remeshing::preprocessing()
{
    // properties
    vfeature_ = mesh_.vertex_property<bool>("v:feature", false);
    efeature_ = mesh_.edge_property<bool>("e:feature", false);
    vlocked_ = mesh_.add_vertex_property<bool>("v:locked", false);
    elocked_ = mesh_.add_edge_property<bool>("e:locked", false);
    vsizing_ = mesh_.add_vertex_property<Scalar>("v:sizing");

    if (!vsizing_)
    {
        std::cerr << "Failed to add v:sizing property to the mesh.\n";
    }

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
    if (uniform_ && !custom_vertex_edgeLengths_)
    {
        for (auto v : mesh_.vertices())
        {
            vsizing_[v] = target_edge_length_;
        }
    }
    else if (!uniform_ && !custom_vertex_edgeLengths_)
    {
        // compute curvature for all mesh vertices, using cotan or Cohen-Steiner
        // don't use two-ring neighborhood, since we otherwise compute
        // curvature over sharp features edges, leading to high curvatures.
        // prefer tensor analysis over cotan-Laplace, since the former is more
        // robust and gives better results on the boundary.
        // don't smooth curvatures here, since it does not take feature edges
        // into account.
        curvature(mesh_, Curvature::max_abs, 0, true, false);
        auto curvatures = mesh_.get_vertex_property<Scalar>("v:curv");

        // smooth curvatures while taking feature edges into account
        for (auto v : mesh_.vertices())
        {
            if (vfeature_ && vfeature_[v])
                continue;
            Scalar curv(0), weight, sum_weights(0);
            for (auto vh : mesh_.halfedges(v))
            {
                auto vv = mesh_.to_vertex(vh);
                if (vfeature_ && vfeature_[vv])
                    continue;
                weight = std::max(0.0, cotan_weight(mesh_, mesh_.edge(vh)));
                sum_weights += weight;
                curv += weight * curvatures[vv];
            }
            if (sum_weights)
                curvatures[v] = curv / sum_weights;
        }

        // use vsizing_ to store/smooth curvatures to avoid another vertex property

        // curvature values for feature vertices and boundary vertices
        // are not meaningful. mark them as negative values.
        for (auto v : mesh_.vertices())
        {
            if (mesh_.is_boundary(v) || (vfeature_ && vfeature_[v]))
                vsizing_[v] = -1.0;
            else
                vsizing_[v] = curvatures[v];
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

    else // if custom_vertex_edgeLengths_ is true
    {    
        size_t id = 0;
        for (auto v : mesh_.vertices())
        {
            vsizing_[v] = target_edge_lengths_per_vertex_[id];
            id++;
        }
    }

    if (use_projection_)
    {
        // build reference mesh
        refmesh_ = std::make_shared<SurfaceMesh>();
        refmesh_->assign(mesh_);
        vertex_normals(*refmesh_);
        refpoints_ = refmesh_->vertex_property<Point>("v:point");
        refnormals_ = refmesh_->vertex_property<Point>("v:normal");

        // copy sizing field from mesh_
        refsizing_ = refmesh_->add_vertex_property<Scalar>("v:sizing");
        for (auto v : refmesh_->vertices())
        {
            refsizing_[v] = vsizing_[v];
        }

        // build kd-tree
        kd_tree_ = std::make_unique<TriangleKdTree>(refmesh_, 0);
    }
}

void Remeshing::postprocessing()
{
    // remove properties
    mesh_.remove_vertex_property(vlocked_);
    mesh_.remove_edge_property(elocked_);
    mesh_.remove_vertex_property(vsizing_);

    if (!has_feature_vertices_)
    {
        mesh_.remove_vertex_property(vfeature_);
    }
    if (!has_feature_edges_)
    {
        mesh_.remove_edge_property(efeature_);
    }
}

void Remeshing::project_to_reference(Vertex v)
{
    if (!use_projection_)
    {
        return;
    }

    // find closest triangle of reference mesh
    auto nn = kd_tree_->nearest(points_[v]);
    const Point p = nn.nearest;
    const Face f = nn.face;

    // get face data
    auto fvIt = refmesh_->vertices(f);
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

void Remeshing::split_long_edges()
{
    Vertex vnew, v0, v1;
    Edge enew;
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
                vnormal_[vnew] = vertex_normal(mesh_, vnew);
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

void Remeshing::collapse_short_edges()
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

void Remeshing::flip_edges()
{
    Vertex v0, v1, v2, v3;
    Halfedge h;
    int val0, val1, val2, val3;
    int val_opt0, val_opt1, val_opt2, val_opt3;
    int ve0, ve1, ve2, ve3, ve_before, ve_after;
    bool ok;
    int i;

    // precompute valences
    auto valence = mesh_.add_vertex_property<int>("valence");
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

void Remeshing::tangential_smoothing(unsigned int iterations)
{
    Vertex vv;
    Scalar w, ww;
    Point u, n, t, b;

    // add property
    auto update = mesh_.add_vertex_property<Point>("v:update");

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
                    Point p(0);
                    try
                    {
                        p = minimize_squared_areas(v);
                    }
                    catch (SolverException&)
                    {
                        p = weighted_centroid(v);
                        throw SolverException("Failed at tangential_smoothing");
                    }
                    u = p - mesh_.position(v);

                    n = vnormal_[v];
                    u -= n * dot(u, n);

                    update[v] = u;
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
        vertex_normals(mesh_);
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

void Remeshing::remove_caps()
{
    Halfedge h;
    Vertex v, vb, vd;
    Scalar a0, a1, amin, aa(::cos(170.0 * std::numbers::pi / 180.0));
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

Point Remeshing::minimize_squared_areas(Vertex v)
{
    dmat3 A(0);
    dvec3 b(0), x;

    for (auto h : mesh_.halfedges(v))
    {
        assert(!mesh_.is_boundary(h));

        // get edge opposite to vertex v
        auto v0 = mesh_.to_vertex(h);
        auto v1 = mesh_.to_vertex(mesh_.next_halfedge(h));
        auto p = (dvec3)points_[v0];
        auto q = (dvec3)points_[v1];
        auto d = q - p;
        auto w = 1.0 / norm(d);

        // build squared cross-product-with-d matrix
        dmat3 D;
        D(0, 0) = d[1] * d[1] + d[2] * d[2];
        D(1, 1) = d[0] * d[0] + d[2] * d[2];
        D(2, 2) = d[0] * d[0] + d[1] * d[1];
        D(1, 0) = D(0, 1) = -d[0] * d[1];
        D(2, 0) = D(0, 2) = -d[0] * d[2];
        D(1, 2) = D(2, 1) = -d[1] * d[2];
        A += w * D;

        // build right-hand side
        b += w * D * p;
    }

    // compute minimizer
    try
    {
        x = inverse(A) * b;
    }
    catch (...)
    {
        auto what = std::string{__func__} + ": Matrix not invertible.";
        throw SolverException(what);
    }

    return Point(x);
}

Point Remeshing::weighted_centroid(Vertex v)
{
    auto p = Point(0);
    double ww = 0;

    for (auto h : mesh_.halfedges(v))
    {
        auto v1 = v;
        auto v2 = mesh_.to_vertex(h);
        auto v3 = mesh_.to_vertex(mesh_.next_halfedge(h));

        auto b = points_[v1];
        b += points_[v2];
        b += points_[v3];
        b *= (1.0 / 3.0);

        double area =
            norm(cross(points_[v2] - points_[v1], points_[v3] - points_[v1]));

        // take care of degenerate faces to avoid all zero weights and division
        // by zero later on
        if (area == 0)
            area = 1.0;

        double w =
            area / pow((vsizing_[v1] + vsizing_[v2] + vsizing_[v3]) / 3.0, 2.0);

        p += w * b;
        ww += w;
    }

    p /= ww;

    return p;
}
} // namespace

void uniform_remeshing(SurfaceMesh& mesh, Scalar edge_length,
                       unsigned int iterations, bool use_projection)
{
    Remeshing(mesh).uniform_remeshing(edge_length, iterations, use_projection);
}

void adaptive_remeshing(SurfaceMesh& mesh, Scalar min_edge_length,
                        Scalar max_edge_length, Scalar approx_error,
                        unsigned int iterations, bool use_projection)
{
    Remeshing(mesh).adaptive_remeshing(min_edge_length, max_edge_length,
                                       approx_error, iterations,
                                       use_projection);
}

void custom_remeshing(SurfaceMesh& mesh, 
                      std::vector<double>& target_edge_lengths,
                      unsigned int iterations, bool use_projection)
{
    Remeshing(mesh).custom_remeshing(target_edge_lengths, iterations,
                                       use_projection);
}

} // namespace pmp
