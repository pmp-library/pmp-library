// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include "pmp/SurfaceMesh.h"

namespace pmp {

class TriangleKdTree;

//! \brief A class for uniform and adaptive surface remeshing.
//! \details The algorithm implemented here performs incremental remeshing based
//! on edge collapse, split, flip, and tangential relaxation.
//! See \cite botsch_2004_remeshing and \cite dunyach_2013_adaptive for a more
//! detailed description.
//! \ingroup algorithms
class SurfaceRemeshing
{
public:
    //! \brief Construct with mesh to be remeshed.
    //! \pre Input mesh needs to be a pure triangle mesh.
    //! \throw InvalidInputException if the input precondition is violated.
    SurfaceRemeshing(SurfaceMesh& mesh);

    // destructor
    ~SurfaceRemeshing();

    //! \brief Perform uniform remeshing.
    //! \param edge_length the target edge length.
    //! \param iterations the number of iterations
    //! \param use_projection use back-projection to the input surface
    void uniform_remeshing(Scalar edge_length, unsigned int iterations = 10,
                           bool use_projection = true);

    //! \brief Perform adaptive remeshing.
    //! \param min_edge_length the minimum edge length.
    //! \param max_edge_length the maximum edge length.
    //! \param approx_error the maximum approximation error
    //! \param iterations the number of iterations
    //! \param use_projection use back-projection to the input surface
    void adaptive_remeshing(Scalar min_edge_length, Scalar max_edge_length,
                            Scalar approx_error, unsigned int iterations = 10,
                            bool use_projection = true);

private:
    void preprocessing();
    void postprocessing();

    void split_long_edges();
    void collapse_short_edges();
    void flip_edges();
    void tangential_smoothing(unsigned int iterations);
    void remove_caps();

    Point minimize_squared_areas(Vertex v);
    Point weighted_centroid(Vertex v);

    void project_to_reference(Vertex v);

    bool is_too_long(Vertex v0, Vertex v1) const
    {
        return distance(points_[v0], points_[v1]) >
               4.0 / 3.0 * std::min(vsizing_[v0], vsizing_[v1]);
    }
    bool is_too_short(Vertex v0, Vertex v1) const
    {
        return distance(points_[v0], points_[v1]) <
               4.0 / 5.0 * std::min(vsizing_[v0], vsizing_[v1]);
    }

private:
    SurfaceMesh& mesh_;
    SurfaceMesh* refmesh_;

    bool use_projection_;
    TriangleKdTree* kd_tree_;

    bool uniform_;
    Scalar target_edge_length_;
    Scalar min_edge_length_;
    Scalar max_edge_length_;
    Scalar approx_error_;

    VertexProperty<Point> points_;
    VertexProperty<Point> vnormal_;
    VertexProperty<bool> vfeature_;
    EdgeProperty<bool> efeature_;
    VertexProperty<bool> vlocked_;
    EdgeProperty<bool> elocked_;
    VertexProperty<Scalar> vsizing_;

    VertexProperty<Point> refpoints_;
    VertexProperty<Point> refnormals_;
    VertexProperty<Scalar> refsizing_;
};

} // namespace pmp
