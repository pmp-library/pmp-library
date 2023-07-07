// Copyright 2011-2022 the Polygon Mesh Processing Library developers.
// Copyright 2020 Astrid Bunge, Philipp Herholz, Misha Kazhdan, Mario Botsch.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/laplace.h"
#include "pmp/algorithms/differential_geometry.h"

namespace pmp {

//=== hidden internal functions =====================================================

namespace {

// compute virtual vertex per polygon, represented by affine weights,
// such that the resulting triangle fan minimizes the sum of squared triangle areas
void compute_virtual_vertex(const DenseMatrix& poly, Eigen::VectorXd& weights)
{
    const int n = poly.rows();

    // setup array of positions and edges
    std::vector<dvec3> x(n), d(n);
    for (int i = 0; i < n; ++i)
        x[i] = poly.row(i);
    for (int i = 0; i < n; ++i)
        d[i] = x[(i + 1) % n] - x[i];

    // setup matrix A and rhs b
    // see Equation (38) of "Polygon Laplacian made simple", Eurographics 2020
    DenseMatrix A(n + 1, n);
    Eigen::VectorXd b(n + 1);
    for (int j = 0; j < n; ++j)
    {
        for (int i = j; i < n; ++i)
        {
            double Aij(0.0), bi(0.0);
            for (int k = 0; k < n; ++k)
            {
                Aij += dot(cross(x[j], d[k]), cross(x[i], d[k]));
                bi += dot(cross(x[i], d[k]), cross(x[k], d[k]));
            }
            A(i, j) = A(j, i) = Aij;
            b(i) = bi;
        }
    }
    for (int j = 0; j < n; ++j)
    {
        A(n, j) = 1.0;
    }
    b(n) = 1.0;

    weights = A.completeOrthogonalDecomposition().solve(b).topRows(n);
}

// compute 3x3 laplace matrix for a triangle
void setup_triangle_laplace_matrix(const Eigen::Vector3d& p0,
                                   const Eigen::Vector3d& p1,
                                   const Eigen::Vector3d& p2, DenseMatrix& Ltri)
{
    double l[3], l2[3], cot[3];

    // squared edge lengths
    l2[0] = (p1 - p2).squaredNorm();
    l2[1] = (p0 - p2).squaredNorm();
    l2[2] = (p0 - p1).squaredNorm();

    // edge lengths
    l[0] = sqrt(l2[0]);
    l[1] = sqrt(l2[1]);
    l[2] = sqrt(l2[2]);

    // triangle area
    const double arg = (l[0] + (l[1] + l[2])) * (l[2] - (l[0] - l[1])) *
                       (l[2] + (l[0] - l[1])) * (l[0] + (l[1] - l[2]));
    const double area = 0.5 * sqrt(arg);

    if (area > std::numeric_limits<double>::min())
    {
        // cotangents of angles
        cot[0] = 0.25 * (l2[1] + l2[2] - l2[0]) / area;
        cot[1] = 0.25 * (l2[2] + l2[0] - l2[1]) / area;
        cot[2] = 0.25 * (l2[0] + l2[1] - l2[2]) / area;
        // cot[0] = clamp_cot(cot[0]);
        // cot[1] = clamp_cot(cot[1]);
        // cot[2] = clamp_cot(cot[2]);

        Ltri(0, 0) = cot[1] + cot[2];
        Ltri(1, 1) = cot[0] + cot[2];
        Ltri(2, 2) = cot[0] + cot[1];

        Ltri(1, 0) = Ltri(0, 1) = -cot[2];
        Ltri(2, 0) = Ltri(0, 2) = -cot[1];
        Ltri(2, 1) = Ltri(1, 2) = -cot[0];
    }
}

// compute NxN laplace matrix for an N-gon
void setup_polygon_laplace_matrix(const DenseMatrix& polygon,
                                  DenseMatrix& Lpoly)
{
    const int n = (int)polygon.rows();
    Lpoly = DenseMatrix::Zero(n, n);

    // shortcut for triangles
    if (n == 3)
    {
        setup_triangle_laplace_matrix(polygon.row(0), polygon.row(1),
                                      polygon.row(2), Lpoly);
        return;
    }

    // compute position of virtual vertex
    Eigen::VectorXd vweights;
    compute_virtual_vertex(polygon, vweights);
    Eigen::Vector3d vvertex = polygon.transpose() * vweights;

    // laplace matrix of refined triangle fan
    DenseMatrix Lfan = DenseMatrix::Zero(n + 1, n + 1);
    DenseMatrix Ltri(3, 3);
    for (int i = 0; i < n; ++i)
    {
        const int j = (i + 1) % n;

        // build laplace matrix of one triangle
        setup_triangle_laplace_matrix(polygon.row(i), polygon.row(j), vvertex,
                                      Ltri);

        // assemble to laplace matrix for refined triangle fan
        Lfan(i, i) += Ltri(0, 0);
        Lfan(i, j) += Ltri(0, 1);
        Lfan(i, n) += Ltri(0, 2);
        Lfan(j, i) += Ltri(1, 0);
        Lfan(j, j) += Ltri(1, 1);
        Lfan(j, n) += Ltri(1, 2);
        Lfan(n, i) += Ltri(2, 0);
        Lfan(n, j) += Ltri(2, 1);
        Lfan(n, n) += Ltri(2, 2);
    }

    // build prolongation matrix
    DenseMatrix P(n + 1, n);
    P.setIdentity();
    P.row(n) = vweights;

    // build polygon laplace matrix by sandwiching
    Lpoly = P.transpose() * Lfan * P;
}

void setup_laplace_matrix_old(const SurfaceMesh& mesh, SparseMatrix& L,
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
            Scalar w = uniform ? 1.0 : 0.5 * cotan_weight(mesh, mesh.edge(h));
            if (clamp && w < 0.0)
                w = 0.0;
            sum_weights += w;
            triplets.emplace_back(vi.idx(), vj.idx(), w);
        }
        triplets.emplace_back(vi.idx(), vi.idx(), -sum_weights);
    }

    L.resize(n, n);
    L.setFromTriplets(triplets.begin(), triplets.end());
}

void setup_uniform_laplace_matrix(const SurfaceMesh& mesh, SparseMatrix& L)
{
    const unsigned int n = mesh.n_vertices();
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(7 * n);

    for (auto vi : mesh.vertices())
    {
        Scalar sum_weights = 0.0;
        for (auto vj : mesh.vertices(vi))
        {
            sum_weights += 1.0;
            triplets.emplace_back(vi.idx(), vj.idx(), 1.0);
        }
        triplets.emplace_back(vi.idx(), vi.idx(), -sum_weights);
    }

    L.resize(n, n);
    L.setFromTriplets(triplets.begin(), triplets.end());
}

void setup_mass_matrix_old(const SurfaceMesh& mesh, DiagonalMatrix& M,
                           bool uniform)
{
    const unsigned int n = mesh.n_vertices();
    Eigen::VectorXd diag(n);
    for (auto v : mesh.vertices())
        diag[v.idx()] = uniform ? mesh.valence(v) : voronoi_area_mixed(mesh, v);
    M = diag.asDiagonal();
}

void setup_uniform_mass_matrix(const SurfaceMesh& mesh, DiagonalMatrix& M)
{
    const unsigned int n = mesh.n_vertices();
    Eigen::VectorXd diag(n);
    for (auto v : mesh.vertices())
        diag[v.idx()] = mesh.valence(v);
    M = diag.asDiagonal();
}

void setup_triangle_mass_matrix(const Eigen::Vector3d& p0,
                                const Eigen::Vector3d& p1,
                                const Eigen::Vector3d& p2, DiagonalMatrix& Mtri)
{
    // three vertex positions
    const dvec3 p[3] = {p0, p1, p2};

    // edge vectors
    dvec3 e[3];
    for (int i = 0; i < 3; ++i)
        e[i] = p[(i + 1) % 3] - p[i];

    // compute and check triangle area
    const auto tri_area = norm(cross(e[0], e[1]));
    if (tri_area <= std::numeric_limits<double>::min())
    {
        Mtri.setZero(3);
        return;
    }

    // dot products for each corner (of its two emanating edge vectors)
    double d[3];
    for (int i = 0; i < 3; ++i)
        d[i] = -dot(e[i], e[(i + 2) % 3]);

    // cotangents for each corner: cot = cos/sin = dot(A,B)/norm(cross(A,B))
    double cot[3];
    for (int i = 0; i < 3; ++i)
        cot[i] = d[i] / tri_area;

    // compute area for each corner
    Eigen::Vector3d area;
    for (int i = 0; i < 3; ++i)
    {
        // angle at corner is obtuse
        if (d[i] < 0.0)
        {
            area[i] = 0.25 * tri_area;
        }
        // angle at some other corner is obtuse
        else if (d[(i + 1) % 3] < 0.0 || d[(i + 2) % 3] < 0.0)
        {
            area[i] = 0.125 * tri_area;
        }
        // no obtuse angles
        else
        {
            area[i] = 0.125 * (sqrnorm(e[i]) * cot[(i + 2) % 3] +
                               sqrnorm(e[(i + 2) % 3]) * cot[(i + 1) % 3]);
        }
    }

    Mtri = area.asDiagonal();
}

void setup_polygon_mass_matrix(const DenseMatrix& polygon,
                               DiagonalMatrix& Mpoly)
{
    const int n = (int)polygon.rows();

    // shortcut for triangles
    if (n == 3)
    {
        setup_triangle_mass_matrix(polygon.row(0), polygon.row(1),
                                   polygon.row(2), Mpoly);
        return;
    }

    // compute position of virtual vertex
    Eigen::VectorXd vweights;
    compute_virtual_vertex(polygon, vweights);
    Eigen::Vector3d vvertex = polygon.transpose() * vweights;

    // laplace matrix of refined triangle fan
    DenseMatrix Mfan = DenseMatrix::Zero(n + 1, n + 1);
    DiagonalMatrix Mtri;
    for (int i = 0; i < n; ++i)
    {
        const int j = (i + 1) % n;

        // build laplace matrix of one triangle
        setup_triangle_mass_matrix(polygon.row(i), polygon.row(j), vvertex,
                                   Mtri);

        // assemble to laplace matrix for refined triangle fan
        // (we are dealing with diagonal matrices)
        Mfan.diagonal()[i] += Mtri.diagonal()[0];
        Mfan.diagonal()[j] += Mtri.diagonal()[1];
        Mfan.diagonal()[n] += Mtri.diagonal()[2];
    }

    // build prolongation matrix
    DenseMatrix P(n + 1, n);
    P.setIdentity();
    P.row(n) = vweights;

    // build polygon laplace matrix by sandwiching
    DenseMatrix PMP = P.transpose() * Mfan * P;
    Mpoly = PMP.rowwise().sum().asDiagonal();
}

} // anonymous namespace

//=== public functions  ==============================================================

void setup_laplace_matrix(const SurfaceMesh& mesh, SparseMatrix& L,
                          bool uniform, bool clamp)
{
    // uniform Laplace
    if (uniform)
    {
        setup_uniform_laplace_matrix(mesh, L);
    }

    // cotan Laplace
    else
    {
        const int nv = mesh.n_vertices();
        std::vector<Vertex> vertices; // polygon vertices
        DenseMatrix polygon;          // positions of polygon vertices
        DenseMatrix Lpoly;            // local laplace matrix

        std::vector<Eigen::Triplet<double>> triplets;
        triplets.reserve(7 * nv);

        for (Face f : mesh.faces())
        {
            // collect polygon vertices
            vertices.clear();
            for (Vertex v : mesh.vertices(f))
            {
                vertices.push_back(v);
            }
            const int n = vertices.size();

            // collect their positions
            polygon.resize(n, 3);
            for (int i = 0; i < n; ++i)
            {
                polygon.row(i) = (Eigen::Vector3d)mesh.position(vertices[i]);
            }

            // setup local laplace matrix
            setup_polygon_laplace_matrix(polygon, Lpoly);

            // assemble to global laplace matrix
            for (int j = 0; j < n; ++j)
            {
                for (int k = 0; k < n; ++k)
                {
                    triplets.emplace_back(vertices[k].idx(), vertices[j].idx(),
                                          -Lpoly(k, j));
                }
            }
        }

        // build sparse matrix from triplets
        L.resize(nv, nv);
        L.setFromTriplets(triplets.begin(), triplets.end());

        // clamp negative off-diagonal entries to zero
        if (clamp)
        {
            for (unsigned int k = 0; k < L.outerSize(); k++)
            {
                double diag_offset(0.0);

                for (SparseMatrix::InnerIterator iter(L, k); iter; ++iter)
                {
                    if (iter.row() != iter.col() && iter.value() < 0.0)
                    {
                        diag_offset += -iter.value();
                        iter.valueRef() = 0.0;
                    }
                }
                for (SparseMatrix::InnerIterator iter(L, k); iter; ++iter)
                {
                    if (iter.row() == iter.col() && iter.value() < 0.0)
                        iter.valueRef() -= diag_offset;
                }
            }
        }
    }

    if (false)
    {
        SparseMatrix LL;
        setup_laplace_matrix_old(mesh, LL, uniform, clamp);
        std::cout << "diff of new vs old laplace matrix: " << (L - LL).norm()
                  << std::endl;
    }
}

void setup_mass_matrix(const SurfaceMesh& mesh, DiagonalMatrix& M, bool uniform)
{
    // uniform case
    if (uniform)
    {
        setup_uniform_mass_matrix(mesh, M);
    }

    // non-uniform case: Voronoi areas
    else
    {
        const int nv = mesh.n_vertices();
        std::vector<Vertex> vertices; // polygon vertices
        DenseMatrix polygon;          // positions of polygon vertices
        DiagonalMatrix Mpoly;         // local mass matrix

        M.setZero(nv);

        for (Face f : mesh.faces())
        {
            // collect polygon vertices
            vertices.clear();
            for (Vertex v : mesh.vertices(f))
            {
                vertices.push_back(v);
            }
            const int n = vertices.size();

            // collect their positions
            polygon.resize(n, 3);
            for (int i = 0; i < n; ++i)
            {
                polygon.row(i) = (Eigen::Vector3d)mesh.position(vertices[i]);
            }

            // setup local mass matrix
            setup_polygon_mass_matrix(polygon, Mpoly);

            // assemble to global mass matrix
            for (int k = 0; k < n; ++k)
            {
                M.diagonal()[vertices[k].idx()] += Mpoly.diagonal()[k];
            }
        }
    }

    if (false)
    {
        DiagonalMatrix MM;
        setup_mass_matrix_old(mesh, MM, uniform);
        std::cout << "diff of new vs old mass matrix: "
                  << (M.diagonal() - MM.diagonal()).norm() << std::endl;
    }
}

void coordinates_to_matrix(const SurfaceMesh& mesh, DenseMatrix& X)
{
    X.resize(mesh.n_vertices(), 3);
    for (auto v : mesh.vertices())
        X.row(v.idx()) = static_cast<Eigen::Vector3d>(mesh.position(v));
}

void matrix_to_coordinates(const DenseMatrix& X, SurfaceMesh& mesh)
{
    assert((size_t)X.rows() == mesh.n_vertices() && X.cols() == 3);
    for (auto v : mesh.vertices())
        mesh.position(v) = X.row(v.idx());
}

} // namespace pmp