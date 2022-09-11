// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <set>
#include <vector>

#include "pmp/SurfaceMesh.h"
#include "pmp/algorithms/Heap.h"
#include "pmp/algorithms/NormalCone.h"
#include "pmp/algorithms/Quadric.h"

namespace pmp {

//! \brief Surface mesh decimation based on approximation error and fairness
//! criteria.
//! \details Performs incremental greedy mesh decimation based on halfedge
//! collapses.
//! See \cite kobbelt_1998_general and \cite garland_1997_surface for details.
//! \ingroup algorithms
class Decimation
{
public:
    //! \brief Construct with mesh to be decimated.
    //! \pre Input mesh needs to be a pure triangle mesh.
    //! \throw InvalidInputException if the input precondition is violated.
    Decimation(SurfaceMesh& mesh);

    //! Destructor.
    ~Decimation();

    //! Initialize with given parameters.
    void initialize(Scalar aspect_ratio = 0.0, Scalar edge_length = 0.0,
                    unsigned int max_valence = 0, Scalar normal_deviation = 0.0,
                    Scalar hausdorff_error = 0.0, Scalar seam_threshold = 1e-2,
                    Scalar seam_angle_deviation = 1);

    //! Decimate mesh to \p n_vertices.
    void decimate(unsigned int n_vertices);

private:
    // Store data for an halfedge collapse
    struct CollapseData
    {
        CollapseData(SurfaceMesh& sm, Halfedge h);

        SurfaceMesh& mesh;

        /*        vl
         *        *
         *       / \
         *      /   \
         *     / fl  \
         * v0 *------>* v1
         *     \ fr  /
         *      \   /
         *       \ /
         *        *
         *        vr
         */
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

    // Heap interface
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

    using PriorityQueue = Heap<Vertex, HeapInterface>;

    using Points = std::vector<Point>;

    // put the vertex v in the priority queue
    void enqueue_vertex(PriorityQueue& queue, Vertex v);

    // is collapsing the halfedge h allowed?
    bool is_collapse_legal(const CollapseData& cd);

    // are texture seams preserved if h is collapsed?
    bool texcoord_check(Halfedge h);

    // what is the priority of collapsing the halfedge h
    float priority(const CollapseData& cd);

    // preprocess halfedge collapse
    void preprocess_collapse(const CollapseData& cd);

    // postprocess halfedge collapse
    void postprocess_collapse(const CollapseData& cd);

    // compute aspect ratio for face f
    Scalar aspect_ratio(Face f) const;

    // compute distance from point p to triangle f
    Scalar distance(Face f, const Point& p) const;

    SurfaceMesh& mesh_;

    bool initialized_{false};

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
    EdgeProperty<bool> texture_seams_;

    bool has_selection_;
    bool has_features_;
    Scalar normal_deviation_;
    Scalar hausdorff_error_;
    Scalar aspect_ratio_;
    Scalar edge_length_;
    Scalar seam_threshold_;
    Scalar seam_angle_deviation_;
    unsigned int max_valence_;
};

} // namespace pmp
