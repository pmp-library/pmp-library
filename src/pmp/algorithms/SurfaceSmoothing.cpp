//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
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

#include <pmp/algorithms/SurfaceFairing.h>
#include <pmp/algorithms/DifferentialGeometry.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>

//=============================================================================

namespace pmp {

//=============================================================================

typedef Eigen::SparseMatrix<double> SparseMatrix;
typedef Eigen::Triplet<double>      Triplet;

//=============================================================================


void explicit_smoothing(SurfaceMesh& mesh, unsigned int iters, bool uniform)
{
    auto points  = mesh.vertexProperty<Point>("v:point");
    auto eweight = mesh.addEdgeProperty<Scalar>("e:cotan");
    auto laplace = mesh.addVertexProperty<Point>("v:laplace");


    // compute Laplace weight per edge: cotan or uniform
    if (uniform)
    {
        for (auto e : mesh.edges())
            eweight[e] = 1.0;
    }
    else
    {
        for (auto e : mesh.edges())
            eweight[e] = std::max(0.0, cotanWeight(mesh, e));
    }


    // smoothing iterations
    SurfaceMesh::Vertex vv;
    SurfaceMesh::Edge   e;
    for (unsigned int i=0; i<iters; ++i)
    {
        // step 1: compute Laplace for each vertex
        for (auto v: mesh.vertices())
        {
            Point  l(0,0,0);

            if (!mesh.isBoundary(v))
            {
                Scalar w(0);

                for (auto h: mesh.halfedges(v))
                {
                    vv = mesh.toVertex(h);
                    e  = mesh.edge(h);
                    l += eweight[e] * (points[vv] - points[v]);
                    w += eweight[e];
                }

                l /= w;
            }

            laplace[v] = l;
        }


        // step 2: move each vertex by its (damped) Laplacian
        for (auto v: mesh.vertices())
        {
            points[v] += 0.5f * laplace[v];
        }
    }


    // clean-up custom properties
    mesh.removeVertexProperty(laplace);
    mesh.removeEdgeProperty(eweight);
}


//-----------------------------------------------------------------------------


void implicit_smoothing(SurfaceMesh& mesh, Scalar timestep, bool uniform)
{
    const int n = mesh.nVertices();
    if (!n) return;


    // properties
    auto points  = mesh.vertexProperty<Point>("v:point");
    auto vweight = mesh.addVertexProperty<Scalar>("v:area");
    auto eweight = mesh.addEdgeProperty<Scalar>("e:cotan");


    // compute weights: cotan or uniform
    if (uniform)
    {
        for (auto v : mesh.vertices())
            vweight[v] = 1.0 / mesh.valence(v);
        for (auto e : mesh.edges())
            eweight[e] = 1.0;
    }
    else
    {
        for (auto v : mesh.vertices())
            vweight[v] = 0.5 / voronoiArea(mesh, v);
        for (auto e : mesh.edges())
            eweight[e] = std::max(0.0, cotanWeight(mesh, e));
    }


    // A*X = B
    Eigen::SparseMatrix<double> A(n,n);
    Eigen::MatrixXd B(n,3);


    // nonzero elements of A as triplets: (row, column, value)
    std::vector< Eigen::Triplet<double> > triplets;


    // setup matrix A and rhs B
    double ww;
    SurfaceMesh::Vertex vv;
    SurfaceMesh::Edge   e;
    for (int i = 0; i < n; ++i)
    {
        SurfaceMesh::Vertex v(i);

        // rhs row
        B(i,0) = points[v][0] / vweight[v];
        B(i,1) = points[v][1] / vweight[v];
        B(i,2) = points[v][2] / vweight[v];

        // lhs row
        ww = 0.0;
        for (auto h: mesh.halfedges(v))
        {
            vv = mesh.toVertex(h);
            e  = mesh.edge(h);
            ww += eweight[e];
            triplets.push_back(Eigen::Triplet<double>(i, vv.idx(), -timestep*eweight[e]));
        }
        triplets.push_back(Eigen::Triplet<double>(i, i, 1.0/vweight[v] + timestep*ww));
    }


    // build sparse matrix from triplets
    A.setFromTriplets(triplets.begin(), triplets.end());


    // solve A*X = B
    Eigen::SimplicialLDLT< Eigen::SparseMatrix<double> > solver(A);
    Eigen::MatrixXd X = solver.solve(B);
    if (solver.info() != Eigen::Success) 
    {
        std::cerr << "SurfaceSmoothing: Could not solve linear system\n";
    }
    else
    {
        // copy solution
        for (int i = 0; i < n; ++i)
        {
            SurfaceMesh::Vertex v(i);
            points[v][0] = X(i,0);
            points[v][1] = X(i,1);
            points[v][2] = X(i,2);
        }
    }


    // clean-up
    mesh.removeVertexProperty(vweight);
    mesh.removeEdgeProperty(eweight);
}


//=============================================================================
} // namespace pmp
//=============================================================================
