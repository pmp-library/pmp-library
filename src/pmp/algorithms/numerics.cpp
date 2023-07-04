// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/numerics.h"

namespace pmp {

DenseMatrix cholesky_solve(const SparseMatrix& A, const DenseMatrix& b)
{
    Eigen::SimplicialLDLT<SparseMatrix> solver;
    solver.compute(A);
    if (solver.info() != Eigen::Success)
    {
        auto what = std::string{__func__} + ": Failed to solve linear system.";
        throw SolverException(what);
    }
    return solver.solve(b);
}

DenseMatrix cholesky_solve(const SparseMatrix& A, const DenseMatrix& B,
                           std::function<bool(unsigned int)> is_constrained,
                           const DenseMatrix& C)
{
    // if nothing is fixed, then use unconstrained solve
    int n_constraints(0);
    for (int i = 0; i < A.cols(); ++i)
        if (is_constrained(i))
            ++n_constraints;
    if (!n_constraints)
        return cholesky_solve(A, B);

    // build index map; n is #dofs
    int n = 0;
    std::vector<int> idx(A.cols(), -1);
    for (int i = 0; i < A.cols(); ++i)
        if (!is_constrained(i))
            idx[i] = n++;

    // copy columns for rhs
    DenseMatrix BB(n, B.cols());
    for (int i = 0; i < A.cols(); ++i)
        if (idx[i] != -1)
            BB.row(idx[i]) = B.row(i);

    // collect entries for reduced matrix
    // update rhs with constraints
    std::vector<Triplet> triplets;
    triplets.reserve(A.nonZeros());
    for (unsigned int k = 0; k < A.outerSize(); k++)
    {
        for (SparseMatrix::InnerIterator iter(A, k); iter; ++iter)
        {
            const int i = iter.row();
            const int j = iter.col();

            if (idx[i] != -1) // row is dof
            {
                if (idx[j] != -1) // col is dof
                {
                    triplets.push_back(Triplet(idx[i], idx[j], iter.value()));
                }
                else // col is constraint
                {
                    BB.row(idx[i]) -= iter.value() * C.row(j);
                }
            }
        }
    }
    SparseMatrix AA(n, n);
    AA.setFromTriplets(triplets.begin(), triplets.end());

    // solve linear system
    Eigen::SimplicialLDLT<SparseMatrix> solver;
    solver.compute(AA);
    if (solver.info() != Eigen::Success)
    {
        auto what = std::string{__func__} + ": Failed to solve linear system.";
        throw SolverException(what);
    }
    const DenseMatrix XX = solver.solve(BB);

    // build full-size result vector from solver result (X) and constraints (C)
    DenseMatrix X(B.rows(), B.cols());
    for (int i = 0; i < A.cols(); ++i)
        X.row(i) = idx[i] == -1 ? C.row(i) : XX.row(idx[i]);

    return X;
}

void setup_selector_matrix(const SurfaceMesh& mesh,
                           std::function<bool(Vertex)> is_selected,
                           SparseMatrix& S)
{
    std::vector<Triplet> triplets;
    triplets.reserve(mesh.n_vertices());

    int row = 0;
    for (auto v : mesh.vertices())
    {
        if (is_selected(v))
        {
            triplets.emplace_back(Triplet(row++, v.idx(), 1.0));
        }
    }

    S.resize(row, mesh.n_vertices());
    S.setFromTriplets(triplets.begin(), triplets.end());
}

} // namespace pmp