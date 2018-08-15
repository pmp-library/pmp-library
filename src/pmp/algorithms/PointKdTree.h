//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
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

#include <pmp/PointSet.h>

#include <vector>
#include <set>

//=============================================================================

namespace pmp {

//=============================================================================

//! \brief A class for building a kd-tree from a PointSet.
//! \details Three different query types are supported:
//! \li nearest neighbor, see nearest()
//! \li k nearest neighbors, see kNearest()
//! \li points within ball, see ball()
class PointKdTree
{
public: //------------------------------------------------------- public methods
    //! Constructor
    PointKdTree(const PointSet& pointSet)
        : m_pointSet(pointSet), m_root(nullptr)
    {
    }

    //! Destructor
    ~PointKdTree() { delete m_root; }

    //! \brief Build the kd-tree
    //! \param[in] maxHandles the maximum number of handles per node
    //! \param[in] maxDepth the maximum depth of the tree
    //! \return The number of nodes.
    unsigned int build(unsigned int maxHandles, unsigned int maxDepth);

    //! \brief Compute nearest neighbor of point \c p
    //! \param[in] p The query point
    //! \param[out] result The nearest point
    //! \param[out] idx The index of the nearest point
    //! \return The number of leaf tests performed
    //! \pre build() has to be called first
    int nearest(const Point& p, Point& result, int& idx) const;

    //! \brief Query for k nearest neighbors
    //! \param[in] p The query point
    //! \param[in] k The number of neighbors
    //! \param[out] knn The handles (indices) of the nearest neighbors
    //! \return The number of leaf tests performed
    //! \pre build() has to be called first
    int kNearest(const Point& p, unsigned int k, std::vector<int>& knn) const;

    //! \brief Query for neighbors within a given ball
    //! \param[in] p The center of the ball
    //! \param[in] radius The radius of the ball
    //! \param[out] ball The point handles (indices) inside the ball
    //! \return The number of leaf tests performed
    //! \pre build() has to be called first
    int ball(const Point& p, Scalar radius, std::vector<int>& ball) const;

private: //------------------------------------------------------- private types
    //! the elemnent stored in the tree
    struct Element
    {
        Element(const Point& p, int idx) : m_point(p), m_idx(idx) {}
        Point m_point;
        int m_idx;
    };

    typedef std::vector<Element> Elements;
    typedef Elements::iterator ElementIter;
    typedef Elements::const_iterator ConstElementIter;

    //! Functor for partitioning with regard to a splitting plane
    struct PartitioningPlane
    {
        PartitioningPlane(unsigned char cutDimension, Scalar cutValue)
            : m_cutDimension(cutDimension), m_cutValue(cutValue)
        {
        }

        bool operator()(const Element& e) const
        {
            return e.m_point[m_cutDimension] > m_cutValue;
        }

        unsigned char m_cutDimension;
        Scalar m_cutValue;
    };

    //! Store nearest neighbor information
    struct NearestNeighborData
    {
        Point m_ref;              //!< The query point
        Scalar m_dist;            //!< The distance to the query point
        int m_nearest;            //!< The index of the nearest point
        unsigned int m_leafTests; //!< The number of leaf tests performed
    };

    //! Element of a priority queue storing an index and a distance
    typedef std::pair<int, float> QueueElement;

    //! Comparison operator for QueueElements
    struct MyComparison
    {
        bool operator()(const QueueElement& lhs, const QueueElement& rhs) const
        {
            return (lhs.second < rhs.second);
        }
    };

    //! Priority queue
    typedef std::set<QueueElement, MyComparison> PriorityQueue;

    //! Store kNN information
    struct KNearestNeighborData
    {
        Point m_ref;              //!< The query point
        Scalar m_dist;            //!< The distance to the query point
        PriorityQueue m_kNearest; //!< The priority queue
        size_t m_k;               //!< The number of neighbors
        unsigned int m_leafTests; //!< The number of leaf tests performed
    };

    //! Store ball information
    struct BallData
    {
        Point m_ref;              //!< The query point
        Scalar m_dist;            //!< The distance to the query point
        unsigned int m_leafTests; //!< The number of leaf tests performed
    };

    //! Node of the tree: contains parent, children and splitting plane
    struct Node
    {
        Node(ElementIter begin, ElementIter end)
            : m_leftChild(nullptr),
              m_rightChild(nullptr),
              m_begin(begin),
              m_end(end)
        {
        }

        ~Node()
        {
            delete m_leftChild;
            delete m_rightChild;
        }

        Node* m_leftChild;
        Node* m_rightChild;
        ElementIter m_begin;
        ElementIter m_end;
        unsigned char m_cutDimension;
        Scalar m_cutValue;
    };

private: //----------------------------------------------------- private methods
    //! Recursive part of build()
    void buildRecurse(Node* node, unsigned int maxHandles, unsigned int depth);

    //! Recursive part of nearest()
    void nearestRecurse(Node* node, NearestNeighborData& data) const;

    //! Recursive part of kNearest()
    void kNearestRecurse(Node* node, KNearestNeighborData& data) const;

    //! Recursive part of ball()
    void ballRecurse(Node* node, BallData& data, Scalar squaredRadius,
                     std::vector<int>& ball) const;

private: //-------------------------------------------------------- private data
    const PointSet& m_pointSet; //!< The underlying points
    Elements m_elements;        //!< A vector of elements
    Node* m_root;               //!< The root of the tree
    unsigned int m_nNodes;      //!< The number of nodes
};

//=============================================================================
} // namespace pmp
//=============================================================================
