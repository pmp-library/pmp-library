// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <limits>
#include <vector>

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief Close simple holes
//! \details Close simple holes (boundary loops of manifold vertices) by first
//! filling the hole with an angle/area-minimizing triangulation, followed
//! by isometric remeshing, and finished by curvature-minimizing fairing of the
//! filled-in patch.
//! See \cite liepa_2003_filling for details.
//! \ingroup algorithms
class SurfaceHoleFilling
{
public:
    //! construct with mesh
    SurfaceHoleFilling(SurfaceMesh& mesh);

    //! fill the hole specified by halfedge h
    bool fill_hole(Halfedge h);

private:
    struct Weight
    {
        Weight(Scalar _angle = std::numeric_limits<Scalar>::max(),
               Scalar _area = std::numeric_limits<Scalar>::max())
            : angle(_angle), area(_area)
        {
        }

        Weight operator+(const Weight& _rhs) const
        {
            return Weight(std::max(angle, _rhs.angle), area + _rhs.area);
        }

        bool operator<(const Weight& _rhs) const
        {
            return (angle < _rhs.angle ||
                    (angle == _rhs.angle && area < _rhs.area));
        }

        Scalar angle;
        Scalar area;
    };

    // compute optimal triangulation of hole
    bool triangulate_hole(Halfedge h);

    // compute the weight of the triangle (i,j,k).
    Weight compute_weight(int i, int j, int k) const;

    // refine triangulation (isotropic remeshing)
    void refine();
    void split_long_edges(const Scalar lmax);
    void collapse_short_edges(const Scalar lmin);
    void flip_edges();
    void relaxation();
    void fairing();

    // return i'th vertex of hole
    Vertex hole_vertex(unsigned int i) const
    {
        assert(i < hole_.size());
        return mesh_.to_vertex(hole_[i]);
    }

    // return vertex opposite edge (i-1,i)
    Vertex opposite_vertex(unsigned int i) const
    {
        assert(i < hole_.size());
        return mesh_.to_vertex(
            mesh_.next_halfedge(mesh_.opposite_halfedge(hole_[i])));
    }

    // does interior edge (_a,_b) exist already?
    bool is_interior_edge(Vertex _a, Vertex _b) const;

    // triangle area
    Scalar compute_area(Vertex _a, Vertex _b, Vertex _c) const;

    // triangle normal
    Point compute_normal(Vertex _a, Vertex _b, Vertex _c) const;

    // dihedral angle
    Scalar compute_angle(const Point& _n1, const Point& _n2) const;

    // mesh and properties
    SurfaceMesh& mesh_;
    VertexProperty<Point> points_;
    VertexProperty<bool> vlocked_;
    EdgeProperty<bool> elocked_;

    std::vector<Halfedge> hole_;

    // data for computing optimal triangulation
    std::vector<std::vector<Weight>> weight_;
    std::vector<std::vector<int>> index_;
};

} // namespace pmp
