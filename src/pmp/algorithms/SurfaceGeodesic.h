//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/SurfaceMesh.h>
#include <vector>
#include <set>
#include <float.h>
#include <limits.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup algorithms algorithms
//! @{

//=============================================================================

//! \brief Compute geodesic distance from a set of seed vertices
//!
//! The methods works by a Dykstra-like breadth first traversal from
//! the seed vertices, implemented by a head structure.
//! See \cite kimmel_1998_geodesic for details.
class SurfaceGeodesic
{
public:
    //! Construct with mesh. Computes virtual edges only (if set to true).
    //! Call compute() to compute geodesic distances.
    SurfaceGeodesic(SurfaceMesh& mesh, bool use_virtual_edges = true);

    // destructor
    ~SurfaceGeodesic();

    //! Compute geodesic distances from specified seed points
    //! up to the specified maximum distance and
    //! up to a maximum number of neighbors.
    //! returns how many neighbors have been found
    unsigned int compute(const std::vector<Vertex>& seed,
                         Scalar maxdist = FLT_MAX,
                         unsigned int maxnum = INT_MAX,
                         std::vector<Vertex>* neighbors = nullptr);

    //! access computed geodesic distance
    Scalar operator()(Vertex v) const { return distance_[v]; }

    //! use (normalized) distances as texture coordinates
    void distance_to_texture_coordinates();


private: // private types

    // functor for comparing two vertices w.r.t. their geodesic distance
    class VertexCmp
    {
    public:
        VertexCmp(const VertexProperty<Scalar>& dist) : dist_(dist) {}

        bool operator()(Vertex v0, Vertex v1) const
        {
            return ((dist_[v0] == dist_[v1]) ? (v0 < v1)
                                             : (dist_[v0] < dist_[v1]));
        }

    private:
        const VertexProperty<Scalar>& dist_;
    };

    // priority queue using geodesic distance as sorting criterion
    typedef std::set<Vertex, VertexCmp> PriorityQueue;

    // virtual edges for walking through obtuse triangles
    struct VirtualEdge
    {
        VirtualEdge(Vertex v, Scalar l) : vertex(v), length(l) {}
        Vertex vertex;
        Scalar length;
    };

    // set for storing virtual edges
    typedef std::map<Halfedge, VirtualEdge> VirtualEdges;


private: // private methods

    void find_virtual_edges();
    unsigned int init_front(const std::vector<Vertex>& seed,
                            std::vector<Vertex>* neighbors);
    unsigned int propagate_front(Scalar maxdist, unsigned int maxnum,
                                 std::vector<Vertex>* neighbors);
    void heap_vertex(Vertex v);
    Scalar distance(Vertex v0, Vertex v1, Vertex v2, Scalar r0 = FLT_MAX,
                    Scalar r1 = FLT_MAX);


private: // private data

    SurfaceMesh& mesh_;

    bool use_virtual_edges_;
    VirtualEdges virtual_edges_;
    
    PriorityQueue* front_;

    VertexProperty<Scalar> distance_;
    VertexProperty<bool> processed_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
