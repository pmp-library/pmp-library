// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/TriangleKdTree.h"

#include <limits>

#include "pmp/algorithms/DistancePointTriangle.h"
#include "pmp/BoundingBox.h"

namespace pmp {

TriangleKdTree::TriangleKdTree(const SurfaceMesh& mesh, unsigned int max_faces,
                               unsigned int max_depth)
{
    // init
    root_ = new Node();
    root_->faces = new Triangles();
    auto points = mesh.get_vertex_property<Point>("v:point");

    // collect triangles
    Triangle tri;
    root_->faces->reserve(mesh.n_faces());
    for (const auto& f : mesh.faces())
    {
        auto v = mesh.vertices(f);
        tri.x[0] = points[*v];
        ++v;
        tri.x[1] = points[*v];
        ++v;
        tri.x[2] = points[*v];
        tri.f = f;
        root_->faces->push_back(tri);
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
        for (const auto& p : f.x)
        {
            bbox += p;
        }
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
    left->faces = new Triangles();
    left->faces->reserve(node->faces->size() / 2);
    auto* right = new Node();
    right->faces = new Triangles;
    right->faces->reserve(node->faces->size() / 2);

    // partition for left and right child
    for (const auto& f : *node->faces)
    {
        bool l = false, r = false;

        if (f.x[0][axis] <= split)
            l = true;
        else
            r = true;
        if (f.x[1][axis] <= split)
            l = true;
        else
            r = true;
        if (f.x[2][axis] <= split)
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

        // recurse to childen
        build_recurse(node->left_child, max_faces, depth - 1);
        build_recurse(node->right_child, max_faces, depth - 1);
    }
}

TriangleKdTree::NearestNeighbor TriangleKdTree::nearest(const Point& p) const
{
    NearestNeighbor data;
    data.dist = std::numeric_limits<Scalar>::max();
    data.tests = 0;
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
            auto d = dist_point_triangle(point, f.x[0], f.x[1], f.x[2], n);
            ++data.tests;
            if (d < data.dist)
            {
                data.dist = d;
                data.face = f.f;
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

} // namespace pmp
