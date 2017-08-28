//=============================================================================
// Copyright (C) 2011 by Graphics & Geometry Group, Bielefeld University
// Copyright (C) 2017 Daniel Sieger
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

#include "PerformanceTest.h"

#include <surface_mesh/SurfaceMesh.h>
#include <surface_mesh/IO.h>

//=============================================================================

using namespace surface_mesh;

//=============================================================================

class SurfaceMeshPerformance : public PerformanceTest
{
public:
    SurfaceMeshPerformance() : PerformanceTest()
    {
        points   = mesh.vertexProperty<Point>("v:point");
        vnormals = mesh.vertexProperty<Point>("v:normal");
        fnormals = mesh.faceProperty<Point>("f:normal");
    }

private:
    SurfaceMesh                        mesh;
    SurfaceMesh::VertexProperty<Point> points;
    SurfaceMesh::VertexProperty<Point> vnormals;
    SurfaceMesh::FaceProperty<Point>   fnormals;

private:
    virtual bool read_mesh(const char* filename) { return readMesh(mesh,filename); }

    virtual bool write_mesh(const char* filename)
    {
        return writeMesh(mesh,filename);
    }

    virtual int circulator_test()
    {
        SurfaceMesh::VertexIterator vit, vend = mesh.verticesEnd();
        SurfaceMesh::FaceIterator   fit, fend = mesh.facesEnd();

        SurfaceMesh::FaceAroundVertexCirculator vfit, vfend;
        SurfaceMesh::VertexAroundFaceCirculator fvit, fvend;

        int counter = 0;

        for (vit = mesh.verticesBegin(); vit != vend; ++vit)
        {
            vfit = vfend = mesh.faces(*vit);
            if (vfit)
                do
                {
                    ++counter;
                } while (++vfit != vfend);
        }

        for (fit = mesh.facesBegin(); fit != fend; ++fit)
        {
            fvit = fvend = mesh.vertices(*fit);
            do
            {
                --counter;
            } while (++fvit != fvend);
        }

        return counter;
    }

    virtual void barycenter_test()
    {
        Point                       p(0, 0, 0);
        SurfaceMesh::VertexIterator vit, vend = mesh.verticesEnd();

        for (vit = mesh.verticesBegin(); vit != vend; ++vit)
            p += points[*vit];

        p /= mesh.nVertices();

        for (vit = mesh.verticesBegin(); vit != vend; ++vit)
            points[*vit] -= p;
    }

    virtual void normal_test()
    {
        SurfaceMesh::VertexIterator             vit, vend = mesh.verticesEnd();
        SurfaceMesh::FaceIterator               fit, fend = mesh.facesEnd();
        SurfaceMesh::FaceAroundVertexCirculator vfit, vfend;

        for (fit = mesh.facesBegin(); fit != fend; ++fit)
        {
            SurfaceMesh::Halfedge h  = mesh.halfedge(*fit);
            Point                 p0 = points[mesh.toVertex(h)];
            h                        = mesh.nextHalfedge(h);
            Point p1                 = points[mesh.toVertex(h)];
            p1 -= p0;
            h        = mesh.nextHalfedge(h);
            Point p2 = points[mesh.toVertex(h)];
            p2 -= p0;
            fnormals[*fit] = cross(p1, p2).normalize();
        }

        for (vit = mesh.verticesBegin(); vit != vend; ++vit)
        {
            Point n(0, 0, 0);
            vfit = vfend = mesh.faces(*vit);
            if (vfit)
                do
                {
                    n += fnormals[*vfit];
                } while (++vfit != vfend);
            vnormals[*vit] = n.normalize();
        }
    }

    virtual void smoothing_test()
    {
        SurfaceMesh::VertexIterator vit, vend = mesh.verticesEnd();
        SurfaceMesh::VertexAroundVertexCirculator vvit, vvend;

        for (vit = mesh.verticesBegin(); vit != vend; ++vit)
        {
            if (!mesh.isBoundary(*vit))
            {
                Point  p(0, 0, 0);
                Scalar c(0);
                vvit = vvend = mesh.vertices(*vit);
                do
                {
                    p += points[*vvit];
                    ++c;
                } while (++vvit != vvend);
                p /= c;
                points[*vit] = p;
            }
        }
    }

    virtual void subdivision_test()
    {
        // reserve memory
        int nv = mesh.nVertices();
        int ne = mesh.nEdges();
        int nf = mesh.nFaces();
        mesh.reserve(nv + nf, ne + 3 * nf, 3 * nf);

        // iterators
        SurfaceMesh::VertexIterator vit, vend = mesh.verticesEnd();
        SurfaceMesh::FaceIterator   fit, fend = mesh.facesEnd();
        SurfaceMesh::EdgeIterator   eit, eend = mesh.edgesEnd();

        // compute new positions of old vertices
        SurfaceMesh::VertexProperty<Point> new_pos =
            mesh.addVertexProperty<Point>("v:np");
        for (vit = mesh.verticesBegin(); vit != vend; ++vit)
        {
            if (!mesh.isBoundary(*vit))
            {
                Scalar n     = mesh.valence(*vit);
                Scalar alpha = (4.0 - 2.0 * cos(2.0 * M_PI / n)) / 9.0;
                Point  p(0, 0, 0);
                SurfaceMesh::VertexAroundVertexCirculator vvit = mesh.vertices(
                                                              *vit),
                                                          vvend = vvit;
                do
                {
                    p += points[*vvit];
                } while (++vvit != vvend);
                p             = (1.0f - alpha) * points[*vit] + alpha / n * p;
                new_pos[*vit] = p;
            }
        }

        // split faces
        for (fit = mesh.facesBegin(); fit != fend; ++fit)
        {
            Point                                   p(0, 0, 0);
            Scalar                                  c(0);
            SurfaceMesh::VertexAroundFaceCirculator fvit  = mesh.vertices(*fit),
                                                    fvend = fvit;
            do
            {
                p += points[*fvit];
                ++c;
            } while (++fvit != fvend);
            p /= c;

            mesh.split(*fit, p);
        }

        // set new positions of old vertices
        for (vit = mesh.verticesBegin(); vit != vend; ++vit)
            if (!mesh.isBoundary(*vit))
                points[*vit] = new_pos[*vit];
        mesh.removeVertexProperty(new_pos);

        // flip old edges
        for (eit = mesh.edgesBegin(); eit != eend; ++eit)
            if (mesh.isFlipOk(*eit))
                mesh.flip(*eit);
    }

    virtual void collapse_test()
    {
        // reserve memory
        int nv = mesh.nVertices();
        int ne = mesh.nEdges();
        int nf = mesh.nFaces();
        mesh.reserve(nv + nf, ne + 3 * nf, 3 * nf);

        // iterators
        SurfaceMesh::VertexIterator vit, vend = mesh.verticesEnd();
        SurfaceMesh::FaceIterator   fit, fend = mesh.facesEnd();

        // split faces
        Point p(0, 0, 0);
        for (fit = mesh.facesBegin(); fit != fend; ++fit)
            mesh.split(*fit, p);

        // collapse new edges
        vit  = vend;
        vend = mesh.verticesEnd();
        for (; vit != vend; ++vit)
            mesh.collapse(mesh.halfedge(*vit));

        // remove deleted items
        mesh.garbageCollection();
    }
};

//=============================================================================
