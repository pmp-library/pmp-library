// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/laplace.h"
#include "pmp/algorithms/DifferentialGeometry.h"

namespace pmp {

void setup_mass_matrix(const SurfaceMesh& mesh, DiagonalMatrix& M, bool uniform)
{
    const unsigned int n = mesh.n_vertices();
    Eigen::VectorXd diag(n);
    for (auto v : mesh.vertices())
        diag[v.idx()] = uniform ? mesh.valence(v) : voronoi_area(mesh, v);

    M = diag.asDiagonal();
}

void setup_stiffness_matrix(const SurfaceMesh& mesh, SparseMatrix& S,
                            bool uniform, bool clamp)
{
    const unsigned int n = mesh.n_vertices();
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(7 * n);

    for (auto vi : mesh.vertices())
    {
        Scalar sum_weights = 0.0;
        for (auto h : mesh.halfedges(vi))
        {
            Vertex vj = mesh.to_vertex(h);
            Scalar w = uniform ? 1.0 : cotan_weight(mesh, mesh.edge(h));
            if (clamp && w < 0.0)
                w = 0.0;
            sum_weights += w;
            triplets.emplace_back(vi.idx(), vj.idx(), w);
        }
        triplets.emplace_back(vi.idx(), vi.idx(), -sum_weights);
    }

    S.resize(n, n);
    S.setFromTriplets(triplets.begin(), triplets.end());
}

} // namespace pmp