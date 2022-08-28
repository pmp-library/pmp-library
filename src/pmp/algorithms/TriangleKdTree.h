// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <vector>
#include <memory>

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A k-d tree for triangles
//! \ingroup algorithms
class TriangleKdTree
{
public:
    //! Construct with mesh.
    TriangleKdTree(std::shared_ptr<const SurfaceMesh> mesh,
                   unsigned int max_faces = 10, unsigned int max_depth = 30);

    //! destructor
    ~TriangleKdTree() { delete root_; }

    //! nearest neighbor information
    struct NearestNeighbor
    {
        Scalar dist;
        Face face;
        Point nearest;
    };

    //! Return handle of the nearest neighbor
    NearestNeighbor nearest(const Point& p) const;

private:
    // vector of Faces
    using Faces = std::vector<Face>;

    // Node of the tree: contains parent, children and splitting plane
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

    // Recursive part of build()
    void build_recurse(Node* node, unsigned int max_handles,
                       unsigned int depth);

    // Recursive part of nearest()
    void nearest_recurse(Node* node, const Point& point,
                         NearestNeighbor& data) const;

    Node* root_;

    std::vector<std::array<Point, 3>> face_points_;
};

} // namespace pmp
