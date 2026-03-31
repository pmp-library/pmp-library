// Copyright 2025 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#include "pmp/surface_mesh.h"
#include "pmp/bounding_box.h"

#include <vector>
#include <algorithm>
#include <optional>
#include <numeric>

namespace pmp {

class AABBTree
{
public:
    // hit object storing face handle and distance
    struct Hit
    {
        Face face;
        double t;
    };

    AABBTree(const SurfaceMesh& mesh) : mesh_(mesh)
    {
        // collect face bounding boxes and centroids
        std::vector<Face> faces;
        std::vector<BoundingBox> face_boxes;
        std::vector<vec3> centroids;
        for (auto f : mesh_.faces())
        {
            BoundingBox bb;
            for (auto v : mesh_.vertices(f))
                bb += mesh_.position(v);
            face_boxes.push_back(bb);
            centroids.push_back(bb.center());
            faces.push_back(f);
        }
        root_ = build(faces, face_boxes, centroids, 0, (int)faces.size());
    }

    // intersect given origin and dir
    std::optional<Hit> intersect(
        const vec3& orig, const vec3& dir, float tmin = 0.0f,
        float tmax = std::numeric_limits<float>::max()) const
    {
        return intersect_node(root_, orig, dir, double(tmin), double(tmax));
    }

private:
    // node of the tree
    struct Node
    {
        BoundingBox bbox;
        int left = -1, right = -1; // indices in nodes_
        Face face{};               // leaf: face in mesh
        bool is_leaf() const { return face.is_valid(); }
    };

    const SurfaceMesh& mesh_;
    std::vector<Node> nodes_;
    int root_;

    int build(std::vector<Face>& faces, std::vector<BoundingBox>& boxes,
              std::vector<vec3>& centroids, int begin, int end)
    {
        Node node;
        // compute bounding box for this node
        BoundingBox bb;
        for (int i = begin; i < end; ++i)
            bb += boxes[i];
        node.bbox = bb;

        const auto range = end - begin;
        if (range == 1)
        {
            node.face = faces[begin];
            nodes_.push_back(node);
            return (int)nodes_.size() - 1;
        }

        // split along largest axis
        vec3 cmin = centroids[begin], cmax = centroids[begin];
        for (int i = begin + 1; i < end; ++i)
        {
            cmin = min(cmin, centroids[i]);
            cmax = max(cmax, centroids[i]);
        }
        int axis = 0;
        vec3 diff = cmax - cmin;
        if (diff[1] > diff[0])
            axis = 1;
        if (diff[2] > diff[axis])
            axis = 2;

        // sort faces, boxes, and centroids together by centroid[axis] using permutation
        // use std::nth_element for partial sort O(n)
        std::vector<int> indices(range);
        std::iota(indices.begin(), indices.end(), begin);
        int mid = (begin + end) / 2;
        std::nth_element(indices.begin(), indices.begin() + (mid - begin),
                         indices.end(), [&](int a, int b) {
                             return centroids[a][axis] < centroids[b][axis];
                         });

        // reorder faces, boxes, centroids according to indices
        std::vector<Face> faces_tmp(range);
        std::vector<BoundingBox> boxes_tmp(range);
        std::vector<vec3> centroids_tmp(range);
        for (int i = 0; i < range; ++i)
        {
            faces_tmp[i] = faces[indices[i]];
            boxes_tmp[i] = boxes[indices[i]];
            centroids_tmp[i] = centroids[indices[i]];
        }
        for (int i = 0; i < range; ++i)
        {
            faces[begin + i] = faces_tmp[i];
            boxes[begin + i] = boxes_tmp[i];
            centroids[begin + i] = centroids_tmp[i];
        }

        node.left = build(faces, boxes, centroids, begin, mid);
        node.right = build(faces, boxes, centroids, mid, end);

        nodes_.push_back(node);
        return (int)nodes_.size() - 1;
    }

    std::optional<Hit> intersect_node(int idx, const vec3& orig,
                                      const vec3& dir, double tmin,
                                      double tmax) const
    {
        // invalid index
        if (idx < 0)
            return std::nullopt;

        // check if we intersect the node
        const Node& node = nodes_[idx];
        double t0{tmin}, t1{tmax};
        if (!intersect_bbox(node.bbox, orig, dir, t0, t1) || t1 < tmin ||
            t0 > tmax)
            return std::nullopt;

        // leaf node?
        if (node.is_leaf())
        {
            auto hit = intersect_face(node.face, orig, dir, tmin, tmax);
            if (hit)
                return hit;
            return std::nullopt;
        }

        // continue traversal
        auto left_hit = intersect_node(node.left, orig, dir, tmin, tmax);
        auto right_hit = intersect_node(node.right, orig, dir, tmin,
                                        left_hit ? left_hit->t : tmax);
        if (left_hit && right_hit)
            return (left_hit->t < right_hit->t) ? left_hit : right_hit;
        return left_hit ? left_hit : right_hit;
    }

    bool intersect_bbox(const BoundingBox& bb, const vec3& orig,
                        const vec3& dir, double& t0, double& t1) const
    {
        t0 = 0.0;
        t1 = std::numeric_limits<double>::max();
        const double epsilon = 1e-12;
        for (int i = 0; i < 3; ++i)
        {
            double dir_d = static_cast<double>(dir[i]);
            double orig_d = static_cast<double>(orig[i]);
            double min_d = static_cast<double>(bb.min()[i]);
            double max_d = static_cast<double>(bb.max()[i]);
            if (std::abs(dir_d) < epsilon)
            {
                // ray is parallel to slab, no hit if origin not within slab
                if (orig_d < min_d || orig_d > max_d)
                    return false;
            }
            else
            {
                double inv_d = 1.0 / dir_d;
                double t_near = (min_d - orig_d) * inv_d;
                double t_far = (max_d - orig_d) * inv_d;
                if (t_near > t_far)
                    std::swap(t_near, t_far);
                t0 = std::max(t0, t_near);
                t1 = std::min(t1, t_far);
                if (t0 > t1)
                    return false;
            }
        }
        return true;
    }

    std::optional<Hit> intersect_face(const Face& f, const vec3& orig,
                                      const vec3& dir, double tmin,
                                      double tmax) const
    {
        // Möller-Trumbore triangle intersection tets
        auto moller_trumbore = [&](const dvec3& v0, const dvec3& v1,
                                   const dvec3& v2, double tmin,
                                   double tmax) -> std::optional<Hit> {
            const double eps = 1e-12;
            dvec3 e1 = v1 - v0, e2 = v2 - v0;
            dvec3 h = cross(dvec3(dir), e2);
            double a = dot(e1, h);
            if (fabs(a) < eps)
                return std::nullopt;
            double f_inv = 1.0 / a;
            dvec3 s = dvec3(orig) - v0;
            double u = f_inv * dot(s, h);
            if (u < 0.0 || u > 1.0)
                return std::nullopt;
            dvec3 q = cross(s, e1);
            double v = f_inv * dot(dvec3(dir), q);
            if (v < 0.0 || u + v > 1.0)
                return std::nullopt;
            double t = f_inv * dot(e2, q);
            if (t < tmin || t > tmax)
                return std::nullopt;
            return Hit{f, t};
        };

        // face is a triangle, test and return
        if (mesh_.valence(f) == 3)
        {
            auto fv = mesh_.vertices(f);
            const dvec3 v0 = dvec3(mesh_.position(*fv));
            const dvec3 v1 = dvec3(mesh_.position(*++fv));
            const dvec3 v2 = dvec3(mesh_.position(*++fv));
            return moller_trumbore(v0, v1, v2, tmin, tmax);
        }

        // face is a polygon, do fan triangulation
        std::vector<dvec3> vertices;
        for (auto v : mesh_.vertices(f))
            vertices.emplace_back(mesh_.position(v));
        std::optional<Hit> closest_hit;
        double closest_t = tmax;
        for (size_t i = 1; i + 1 < vertices.size(); ++i)
        {
            auto hit = moller_trumbore(vertices[0], vertices[i],
                                       vertices[i + 1], tmin, closest_t);
            if (hit && hit->t < closest_t)
            {
                closest_t = hit->t;
                closest_hit = hit;
            }
        }
        return closest_hit;
    }
};

} // namespace pmp
