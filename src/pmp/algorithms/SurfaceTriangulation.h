// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <vector>

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief Triangulate polygons to get a pure triangle mesh.
//! \details Triangulate n-gons into n-2 triangles. Finds the triangulation that
//! minimizes the sum of squared triangle areas, or the one that maximizes the
//! minimum angle.
//! See \cite liepa_2003_filling for details.
//! \ingroup algorithms
class SurfaceTriangulation
{
public:
    //! Triangulation objective
    enum class Objective
    {
        MIN_AREA, //!< minimize the sum of squared areas
        MAX_ANGLE //!< maximize the minimum angle
    } objective_;

    //! Construct with mesh
    SurfaceTriangulation(SurfaceMesh& mesh);

    //! Triangulate all faces
    void triangulate(Objective o = Objective::MIN_AREA);

    //! Triangulate the Face \p f
    //! \pre The input face is manifold
    //! \throw InvalidInputException in case the input precondition is violated
    void triangulate(Face f, Objective o = Objective::MIN_AREA);

private:
    // Compute the weight of the triangle (i,j,k).
    Scalar compute_weight(int i, int j, int k) const;

    // Does edge (a,b) exist?
    bool is_edge(Vertex a, Vertex b) const;

    // Add edge from vertex i to j.
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
