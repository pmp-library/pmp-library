//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
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
#include <set>
#include <float.h>

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
//! See \cite kimmel_1998_geodesic and \cite novotni_2002_geodesic for details.
class SurfaceGeodesic
{
public:
    //! Construct with mesh and seed vertices.
    //! Grow around seed up to specified maximum distance.
    //! Set whether to use virtual edges (more computation, more accurate result)
    SurfaceGeodesic(SurfaceMesh& mesh,
                    std::vector<Vertex> seed,
                    Scalar maxdist=FLT_MAX,
                    bool use_virtual_edges=true);

    // destructor
    ~SurfaceGeodesic();

    //! access computed geodesic distance
    Scalar operator()(Vertex v) const { return distance_[v]; }

    //! use (normalized) distances as texture coordinates
    void distance_to_texture_coordinates();

private:

    // functor for comparing two vertices w.r.t. their geodesic distance
    class VertexCmp
    {
        public:
            VertexCmp(const VertexProperty<Scalar>& dist) : dist_(dist) {}

            bool operator()(Vertex v0, Vertex v1) const
            {
                return ((dist_[v0] == dist_[v1]) ? (v0 < v1) : (dist_[v0] < dist_[v1]));
            }

        private:
            const VertexProperty<Scalar>& dist_;
    };

    // priority queue using geodesic distance as sorting criterion
    typedef std::set<Vertex, VertexCmp>  PriorityQueue;

    // virtual edges for walking through obtuse triangles
    struct VirtualEdge
    {
        VirtualEdge(Vertex v, Scalar l) : vertex(v), length(l) {}
        Vertex  vertex;
        Scalar  length;
    };

    // set for storing virtual edges
    typedef std::map<Halfedge, VirtualEdge>  VirtualEdges;


    void  find_virtual_edges();
    void  init_front();
    void  propagate_front();
    void  heap_vertex(Vertex v);
    Scalar distance(Vertex v0, Vertex v1, Vertex v2,
                    Scalar r0=FLT_MAX, Scalar r1=FLT_MAX);


    SurfaceMesh&                             mesh_;
    VertexProperty<Scalar>      distance_;
    VertexProperty<bool>        processed_;
    PriorityQueue*                           front_;
    std::vector<Vertex>                      seed_;
    Scalar                                   maxdist_;
    bool                                     use_virtual_edges_;
    VirtualEdges                             virtual_edges_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
