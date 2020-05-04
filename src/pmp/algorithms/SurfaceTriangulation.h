// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <vector>

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief Triangulate polygons to get a pure triangle mesh.
//! \details Tringulate n-gons into n-2 triangles. Find the triangulation that
//! minimizes the sum of squared triangle areas.
//! See \cite liepa_2003_filling for details.
//! \ingroup algorithms
class SurfaceTriangulation
{
public:
    //! triangulation objective: find the triangulation that minimizes the
    //! sum of squared triangle areas, or the one that maximizes the minimum
    //! angle.
    enum Objective
    {
        MIN_AREA,
        MAX_ANGLE
    } objective_;

    //! construct with mesh
    SurfaceTriangulation(SurfaceMesh& mesh);

    //! triangulate all faces
    void triangulate(Objective o = MIN_AREA);

    //! triangulate a particular face f
    void triangulate(Face f, Objective o = MIN_AREA);

private:
    // compute the weight of the triangle (i,j,k).
    Scalar compute_weight(int i, int j, int k) const;

    // does edge (a,b) exist?
    bool is_edge(Vertex a, Vertex b) const;

    // does edge (a,b) exist and is non-boundary?
    bool is_interior_edge(Vertex a, Vertex b) const;

    // add edges from vertex i to j
    bool insert_edge(int i, int j);

    // mesh and properties
    SurfaceMesh& mesh_;
    VertexProperty<Point> points_;
    std::vector<Halfedge> halfedges_;
    std::vector<Vertex> vertices_;

    // data for computing optimal triangulation
    std::vector<std::vector<Scalar>> weight_;
    std::vector<std::vector<int>> index_;
};

} // namespace pmp
