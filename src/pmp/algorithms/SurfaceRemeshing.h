//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
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
#include <pmp/algorithms/TriangleKdTree.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup algorithms algorithms
//! @{

//=============================================================================

//! \brief A class for uniform and adaptive surface remeshing.
//! \details The algorithm implemented here performs incremental remeshing based
//! on edge collapse, split, flip, and tangential relaxation.
//! See \cite botsch_2004_remeshing and \cite dunyach_2013_adaptive for a more
//! detailed description.
class SurfaceRemeshing
{
public:
    // constructor
    SurfaceRemeshing(SurfaceMesh& mesh);

    // destructor
    ~SurfaceRemeshing();

    //! uniform remeshing with target edge length
    void uniform_remeshing(Scalar edge_length, unsigned int iterations = 10,
                          bool use_projection = true);

    //! adaptive remeshing with min/max edge length and approximation error
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

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
