//=============================================================================
// Copyright (C) 2011-2016 by Graphics & Geometry Group, Bielefeld University
// All rights reserved.
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

#include <surface_mesh/SurfaceMesh.h>

#include <surface_mesh/algorithms/Heap.h>
#include <surface_mesh/algorithms/NormalCone.h>
#include <surface_mesh/algorithms/Quadric.h>

#include <set>
#include <vector>

//=============================================================================

namespace surface_mesh {

//=============================================================================

/* Store data for an halfedge collapse

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
public:
    CollapseData(SurfaceMesh& mesh, SurfaceMesh::Halfedge h);

    SurfaceMesh& mesh;

    SurfaceMesh::Halfedge v0v1; // Halfedge to be collapsed
    SurfaceMesh::Halfedge v1v0; // Reverse halfedge
    SurfaceMesh::Vertex   v0;   // Vertex to be removed
    SurfaceMesh::Vertex   v1;   // Remaining vertex
    SurfaceMesh::Face     fl;   // Left face
    SurfaceMesh::Face     fr;   // Right face
    SurfaceMesh::Vertex   vl;   // Left vertex
    SurfaceMesh::Vertex   vr;   // Right vertex
    SurfaceMesh::Halfedge v1vl, vlv0, v0vr, vrv1;
};

//=============================================================================
//! \addtogroup algorithms algorithms
//! @{
//=============================================================================

//! Surface simplification based on error quadrics
class SurfaceSimplification
{
public:
    // constructor
    SurfaceSimplification(SurfaceMesh& mesh);

    // destructor
    ~SurfaceSimplification();

    //! initialize decimater
    void initialize(Scalar aspectRatio = 0.0, Scalar edgeLength = 0.0,
                    unsigned int maxValence = 0, Scalar normalDeviation = 0.0,
                    Scalar hausdorffError = 0.0);

    //! decimate down to n vertices
    void simplify(unsigned int n);

private: //-------------------------------------------------- private functions
    // put the vertex v in the priority queue
    void enqueueVertex(SurfaceMesh::Vertex v);

    // is collapsing the halfedge h allowed?
    bool isCollapseLegal(const CollapseData& cd);

    // what is the priority of collapsing the halfedge h
    float priority(const CollapseData& cd);

    // postprocess halfedge collapse
    void postprocessCollapse(const CollapseData& cd);

    // compute aspect ratio for face f
    Scalar aspectRatio(SurfaceMesh::Face f) const;

    // compute distance from p to triagle f
    Scalar distance(SurfaceMesh::Face f, const Point& p) const;

private: //------------------------------------------------------ private types
    //! Heap interface
    class HeapInterface
    {
    public:
        HeapInterface(SurfaceMesh::VertexProperty<float> Prio,
                      SurfaceMesh::VertexProperty<int>   Pos)
            : prio_(Prio), pos_(Pos)
        {
        }

        bool less(SurfaceMesh::Vertex v0, SurfaceMesh::Vertex v1)
        {
            return prio_[v0] < prio_[v1];
        }
        bool greater(SurfaceMesh::Vertex v0, SurfaceMesh::Vertex v1)
        {
            return prio_[v0] > prio_[v1];
        }
        int getHeapPosition(SurfaceMesh::Vertex v) { return pos_[v]; }
        void setHeapPosition(SurfaceMesh::Vertex v, int pos) { pos_[v] = pos; }

    private:
        SurfaceMesh::VertexProperty<float> prio_;
        SurfaceMesh::VertexProperty<int>   pos_;
    };

    typedef HeapT<SurfaceMesh::Vertex, HeapInterface> PriorityQueue;

    typedef std::vector<Point> Points;

private: //------------------------------------------------------- private data
    SurfaceMesh& m_mesh;

    bool m_initialized;

    SurfaceMesh::VertexProperty<float>                 m_vpriority;
    SurfaceMesh::VertexProperty<SurfaceMesh::Halfedge> m_vtarget;
    SurfaceMesh::VertexProperty<int>                   m_heapPos;
    SurfaceMesh::VertexProperty<Quadric>               m_vquadric;
    SurfaceMesh::FaceProperty<NormalCone>              m_normalCone;
    SurfaceMesh::FaceProperty<Points>                  m_facePoints;

    SurfaceMesh::VertexProperty<Point> m_vpoint;
    SurfaceMesh::FaceProperty<Point>   m_fnormal;
    SurfaceMesh::VertexProperty<bool>  m_vselected;
    SurfaceMesh::VertexProperty<bool>  m_vfeature;
    SurfaceMesh::EdgeProperty<bool>    m_efeature;

    PriorityQueue* m_queue;

    bool         m_hasSelection;
    bool         m_hasFeatures;
    Scalar       m_normalDeviation;
    Scalar       m_hausdorffError;
    Scalar       m_aspectRatio;
    Scalar       m_edgeLength;
    unsigned int m_maxValence;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace surface_mesh
//=============================================================================
