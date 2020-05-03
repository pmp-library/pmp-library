// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <vector>

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief A k-d tree for triangles
//! \ingroup algorithms
class TriangleKdTree
{
public:
    //! Construct with mesh.
    TriangleKdTree(const SurfaceMesh& mesh, unsigned int max_faces = 10,
                   unsigned int max_depth = 30);

    //! destructor
    ~TriangleKdTree() { delete root_; }

    //! nearest neighbor information
    struct NearestNeighbor
    {
        Scalar dist;
        Face face;
        Point nearest;
        int tests;
    };

    //! Return handle of the nearest neighbor
    NearestNeighbor nearest(const Point& p) const;

private:
    // triangle stores corners and face handle
    struct Triangle
    {
        Triangle() {}
        Triangle(const Point& x0, const Point& x1, const Point& x2, Face ff)
        {
            x[0] = x0;
            x[1] = x1;
            x[2] = x2;
            f = ff;
        }

        Point x[3];
        Face f;
    };

    // vector of Triangle
    typedef std::vector<Triangle> Triangles;

    // Node of the tree: contains parent, children and splitting plane
    struct Node
    {
        Node() : faces(nullptr), left_child(nullptr), right_child(nullptr){};

        ~Node()
        {
            delete faces;
            delete left_child;
            delete right_child;
        }

        unsigned char axis;
        Scalar split;
        Triangles* faces;
        Node* left_child;
        Node* right_child;
    };

    // Recursive part of build()
    unsigned int build_recurse(Node* node, unsigned int max_handles,
                               unsigned int depth);

    // Recursive part of nearest()
    void nearest_recurse(Node* node, const Point& point,
                         NearestNeighbor& data) const;

    Node* root_;
};

} // namespace pmp
