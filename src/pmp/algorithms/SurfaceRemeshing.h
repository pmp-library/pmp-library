//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
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
    void uniformRemeshing(Scalar edgeLength, unsigned int iterations = 10,
                          bool useProjection = true);

    //! adaptive remeshing with min/max edge length and approximation error
    void adaptiveRemeshing(Scalar minEdgeLength, Scalar maxEdgeLength,
                           Scalar approxError, unsigned int iterations = 10,
                           bool useProjection = true);

private:
    void preprocessing();
    void postprocessing();

    void splitLongEdges();
    void collapseShortEdges();
    void flipEdges();
    void tangentialSmoothing(unsigned int iterations);
    void removeCaps();

    void projectToReference(SurfaceMesh::Vertex v);
    TriangleKdTree::NearestNeighbor closestFace(SurfaceMesh::Vertex v);

    bool isTooLong(SurfaceMesh::Vertex v0, SurfaceMesh::Vertex v1) const
    {
        return distance(m_points[v0], m_points[v1]) >
               4.0 / 3.0 * std::min(m_vsizing[v0], m_vsizing[v1]);
    }
    bool isTooShort(SurfaceMesh::Vertex v0, SurfaceMesh::Vertex v1) const
    {
        return distance(m_points[v0], m_points[v1]) <
               4.0 / 5.0 * std::min(m_vsizing[v0], m_vsizing[v1]);
    }

private:
    SurfaceMesh& m_mesh;
    SurfaceMesh* m_refmesh;

    bool m_useProjection;
    TriangleKdTree* m_kDTree;

    bool m_uniform;
    Scalar m_targetEdgeLength;
    Scalar m_minEdgeLength;
    Scalar m_maxEdgeLength;
    Scalar m_approxError;

    SurfaceMesh::VertexProperty<Point> m_points;
    SurfaceMesh::VertexProperty<Point> m_vnormal;
    SurfaceMesh::VertexProperty<bool> m_vfeature;
    SurfaceMesh::EdgeProperty<bool> m_efeature;
    SurfaceMesh::VertexProperty<bool> m_vlocked;
    SurfaceMesh::EdgeProperty<bool> m_elocked;
    SurfaceMesh::VertexProperty<Scalar> m_vsizing;

    SurfaceMesh::VertexProperty<Point> m_refpoints;
    SurfaceMesh::VertexProperty<Point> m_refnormals;
    SurfaceMesh::VertexProperty<Scalar> m_refsizing;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================
