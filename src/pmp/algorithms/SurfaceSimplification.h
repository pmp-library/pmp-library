//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/SurfaceMesh.h>

#include <pmp/algorithms/Heap.h>
#include <pmp/algorithms/NormalCone.h>
#include <pmp/algorithms/Quadric.h>

#include <set>
#include <vector>

//=============================================================================

namespace pmp {

//=============================================================================
//! \addtogroup algorithms algorithms
//! @{
//=============================================================================

//! \brief Surface mesh simplification based on approximation error and fairness criteria.
//! \details Performs incremental greedy mesh simplification based on halfedge
//! collapses. See \cite kobbelt_1998_general and \cite garland_1997_surface for
//! details.
class SurfaceSimplification
{
public:
    // constructor
    SurfaceSimplification(SurfaceMesh& mesh);

    // destructor
    ~SurfaceSimplification();

    //! initialize decimater
    void initialize(Scalar aspect_ratio = 0.0, Scalar edge_length = 0.0,
                    unsigned int max_valence = 0, Scalar normal_deviation = 0.0,
                    Scalar hausdorff_error = 0.0);

    //! decimate down to n vertices
    void simplify(unsigned int n_vertices);

private: //------------------------------------------------------ private types
    //! Store data for an halfedge collapse
    /*
                vl
                *
               / \
              /   \
             / fl  \
         v0 *------>* v1
             \ fr  /
              \   /
               \ /
                *
                vr
    */
    struct CollapseData
    {
        CollapseData(SurfaceMesh& sm, Halfedge h);

        SurfaceMesh& mesh;

        Halfedge v0v1; // Halfedge to be collapsed
        Halfedge v1v0; // Reverse halfedge
        Vertex v0;     // Vertex to be removed
        Vertex v1;     // Remaining vertex
        Face fl;       // Left face
        Face fr;       // Right face
        Vertex vl;     // Left vertex
        Vertex vr;     // Right vertex
        Halfedge v1vl, vlv0, v0vr, vrv1;
    };

    //! Heap interface
    class HeapInterface
    {
    public:
        HeapInterface(VertexProperty<float> prio, VertexProperty<int> pos)
            : prio_(prio), pos_(pos)
        {
        }

        bool less(Vertex v0, Vertex v1) { return prio_[v0] < prio_[v1]; }
        bool greater(Vertex v0, Vertex v1) { return prio_[v0] > prio_[v1]; }
        int get_heap_position(Vertex v) { return pos_[v]; }
        void set_heap_position(Vertex v, int pos) { pos_[v] = pos; }

    private:
        VertexProperty<float> prio_;
        VertexProperty<int> pos_;
    };

    typedef Heap<Vertex, HeapInterface> PriorityQueue;

    typedef std::vector<Point> Points;

private: //-------------------------------------------------- private functions
    // put the vertex v in the priority queue
    void enqueue_vertex(Vertex v);

    // is collapsing the halfedge h allowed?
    bool is_collapse_legal(const CollapseData& cd);

    // what is the priority of collapsing the halfedge h
    float priority(const CollapseData& cd);

    // postprocess halfedge collapse
    void postprocess_collapse(const CollapseData& cd);

    // compute aspect ratio for face f
    Scalar aspect_ratio(Face f) const;

    // compute distance from p to triagle f
    Scalar distance(Face f, const Point& p) const;

private: //------------------------------------------------------- private data
    SurfaceMesh& mesh_;

    bool initialized_;

    VertexProperty<float> vpriority_;
    VertexProperty<Halfedge> vtarget_;
    VertexProperty<int> heap_pos_;
    VertexProperty<Quadric> vquadric_;
    FaceProperty<NormalCone> normal_cone_;
    FaceProperty<Points> face_points_;

    VertexProperty<Point> vpoint_;
    FaceProperty<Point> fnormal_;
    VertexProperty<bool> vselected_;
    VertexProperty<bool> vfeature_;
    EdgeProperty<bool> efeature_;

    PriorityQueue* queue_;

    bool has_selection_;
    bool has_features_;
    Scalar normal_deviation_;
    Scalar hausdorff_error_;
    Scalar aspect_ratio_;
    Scalar edge_length_;
    unsigned int max_valence_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
