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

#include <pmp/types.h>
#include <pmp/SurfaceMesh.h>

//=============================================================================

namespace pmp {

//=============================================================================

// clamp cotangent and cosince values as if angles are in [1, 179]

inline double clamp_cot(const double v)
{
    const double bound = 19.1; // 3 degrees
    return (v < -bound ? -bound : (v > bound ? bound : v));
}

inline double clamp_cos(const double v)
{
    const double bound = 0.9986; // 3 degrees
    return (v < -bound ? -bound : (v > bound ? bound : v));
}

// compute angle between two (un-normalized) vectors
inline Scalar angle(const Point& v0, const Point& v1)
{
    return atan2(norm(cross(v0, v1)), dot(v0, v1));
}

// compute sine of angle between two (un-normalized) vectors
inline Scalar sin(const Point& v0, const Point& v1)
{
    return norm(cross(v0, v1)) / (norm(v0) * norm(v1));
}

// compute cosine of angle between two (un-normalized) vectors
inline Scalar cos(const Point& v0, const Point& v1)
{
    return dot(v0, v1) / (norm(v0) * norm(v1));
}

// compute cotangent of angle between two (un-normalized) vectors
inline Scalar cotan(const Point& v0, const Point& v1)
{
    return clamp_cot(dot(v0, v1) / norm(cross(v0, v1)));
}

//! compute area of a triangle given by three points
Scalar triangle_area(const Point& p0, const Point& p1, const Point& p2);

//! compute area of triangle f
Scalar triangle_area(const SurfaceMesh& mesh, SurfaceMesh::Face f);

//! compute the cotangent weight for edge e
double cotan_weight(const SurfaceMesh& mesh, SurfaceMesh::Edge e);

//! compute (mixed) Voronoi area of vertex v
double voronoi_area(const SurfaceMesh& mesh, SurfaceMesh::Vertex v);

//! compute barycentric Voronoi area of vertex v
double voronoi_area_barycentric(const SurfaceMesh& mesh, SurfaceMesh::Vertex v);

//! compute Laplace vector for vertex v (normalized by Voronoi area)
Point laplace(const SurfaceMesh& mesh, SurfaceMesh::Vertex v);

//! compute the sum of angles around vertex v (used for Gaussian curvature)
Scalar angle_sum(const SurfaceMesh& mesh, SurfaceMesh::Vertex v);

//! discrete curvature information for a vertex. used for vertex_curvature()
struct VertexCurvature
{
    VertexCurvature() : mean(0.0), gauss(0.0), max(0.0), min(0.0) {}

    Scalar mean;
    Scalar gauss;
    Scalar max;
    Scalar min;
};

//! compute min, max, mean, and Gaussian curvature for vertex v. this will not
//! give realiable values for boundary vertices.
VertexCurvature vertex_curvature(const SurfaceMesh& mesh, SurfaceMesh::Vertex v);

//=============================================================================
} // namespace pmp
//=============================================================================
