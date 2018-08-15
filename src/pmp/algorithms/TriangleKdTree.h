//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/SurfaceMesh.h>
#include <vector>

//=============================================================================

namespace pmp {

//=============================================================================

//! A k-d tree for triangles
class TriangleKdTree
{
public:
    //! construct with mesh
    TriangleKdTree(const SurfaceMesh& mesh, unsigned int maxFaces = 10,
                   unsigned int maxDepth = 30);

    //! destructur
    ~TriangleKdTree() { delete m_root; }

    //! nearest neighbor information
    struct NearestNeighbor
    {
        Scalar dist;
        SurfaceMesh::Face face;
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
        Triangle(const Point& x0, const Point& x1, const Point& x2,
                 SurfaceMesh::Face ff)
        {
            x[0] = x0;
            x[1] = x1;
            x[2] = x2;
            f = ff;
        }

        Point x[3];
        SurfaceMesh::Face f;
    };

    // vector of Triangle
    typedef std::vector<Triangle> Triangles;

    // Node of the tree: contains parent, children and splitting plane
    struct Node
    {
        Node() : faces(nullptr), leftChild(nullptr), rightChild(nullptr){};

        ~Node()
        {
            delete faces;
            delete leftChild;
            delete rightChild;
        }

        unsigned char axis;
        Scalar split;
        Triangles* faces;
        Node* leftChild;
        Node* rightChild;
    };

    // Recursive part of build()
    unsigned int buildRecurse(Node* node, unsigned int maxHandles,
                              unsigned int depth);

    // Recursive part of nearest()
    void nearestRecurse(Node* node, const Point& point,
                        NearestNeighbor& data) const;

private:
    Node* m_root;
};

//=============================================================================
//! @}
//=============================================================================w
} // namespace pmp
//=============================================================================
