//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under a MIT-style license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT-with-employer-disclaimer
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/SurfaceMesh.h>
#include <vector>
#include <float.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup algorithms algorithms
//! @{

//=============================================================================

//! \brief Triangulate polygons to get a pure triangle mesh

//! \details Tringulate n-gons into n-2 triangles. Find the triangulation that
//! minimizes the sum of squared triangle areas.
//! See \cite liepa_2003_filling for details.
class SurfaceTriangulation
{
public:

    /// triangulation objective: find the triangulation that minimizes the
    /// sum of squared triangle areas, or the one that maximizes the minimum
    /// angle.
    enum Objective { MIN_AREA, MAX_ANGLE } objective_;

    /// construct with mesh
    SurfaceTriangulation(SurfaceMesh& mesh);

    /// triangulate all faces
    void triangulate(Objective _o = MIN_AREA);

    /// triangulate a particular face f
    void triangulate(Face f, Objective _o = MIN_AREA);


private: //-------------------------------------------------- private functions

    // compute the weight of the triangle (i,j,k).
    Scalar compute_weight(int i, int j, int k) const;

    // does edge (_a,_b) exist?
    bool is_edge(Vertex _a, Vertex _b) const;

    // does edge (_a,_b) exist and is non-boundary?
    bool is_interior_edge(Vertex _a, Vertex _b) const;

    // add edges from vertex i to j
    bool insert_edge(int i, int j);


private: //------------------------------------------------------- private data

    // mesh and properties
    SurfaceMesh& mesh_;
    VertexProperty<Point> points_;
    std::vector<Halfedge> halfedges_;
    std::vector<Vertex>   vertices_;

    // data for computing optimal triangulation
    std::vector<std::vector<Scalar>> weight_;
    std::vector<std::vector<int>> index_;
};

//=============================================================================
/// @}
//=============================================================================
}
//=============================================================================
