// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/DeformationTransfer.h"
#include "pmp/algorithms/DifferentialGeometry.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>
typedef Eigen::Triplet<double> Triplet;
typedef Eigen::SparseMatrix<double> SparseMatrix;
typedef Eigen::MatrixXd DenseMatrix;
typedef Eigen::SimplicialLDLT<SparseMatrix> Solver;

namespace pmp {

DeformationTransfer::DeformationTransfer(const SurfaceMesh& source,
                                         const SurfaceMesh& target)
    : source_(source), target_(target)
{
    // verify that input meshes are triangle meshes and that they are compatible
    if (!source_.is_triangle_mesh() || !target_.is_triangle_mesh())
        throw InvalidInputException("Input is not a pure triangle mesh!");
    if (source_.n_vertices() != target_.n_vertices() ||
        source_.n_faces() != target_.n_faces())
        throw InvalidInputException("Input meshes have different tesselation!");
}

void DeformationTransfer::transfer(const SurfaceMesh& deformed_source,
                                   SurfaceMesh& deformed_target)
{
    // verify that input meshes are compatible
    if (source_.n_vertices() != deformed_source.n_vertices())
        throw InvalidInputException("Input meshes have different tesselation!");

    // initialize deformed target to undeformed target
    deformed_target = target_;

    // allocate properties
    auto cotan = deformed_target.add_edge_property<double>("deftrans:cotan");
    auto locked =
        deformed_target.add_vertex_property<bool>("deftrans:locked", false);
    auto idx = deformed_target.add_vertex_property<int>("deftrans:idx", -1);
    auto defgrad =
        deformed_target.add_face_property<dmat3>("deftrans:gradients");
    auto target_laplacians = deformed_target.add_vertex_property<dvec3>(
        "deftrans:laplacians", dvec3(0, 0, 0));

    // compute cotan weights on undeformed target mesh
    compute_cotan_weights(target_, cotan);

    // extract deformation gradients from source mesh and its deformed version
    extract_deformation(source_, deformed_source, defgrad);

    // apply source deformation to target gradients
    compute_target_laplacians(defgrad, target_laplacians);

    // lock vertices that do not move in source mesh
    lock_vertices(source_, deformed_source, locked);

    // assign index for linear system to non-locked vertices
    std::vector<Vertex> free_vertices;
    for (auto v : deformed_target.vertices())
    {
        if (!locked[v])
        {
            idx[v] = free_vertices.size();
            free_vertices.push_back(v);
        }
    }
    const int N = free_vertices.size();

    // setup linear system and right-hand side
    SparseMatrix A(N, N);
    DenseMatrix B(N, 3);
    std::vector<Triplet> triplets;
    for (unsigned int i = 0; i < free_vertices.size(); ++i)
    {
        auto v = free_vertices[i];

        B(i, 0) = target_laplacians[v][0];
        B(i, 1) = target_laplacians[v][1];
        B(i, 2) = target_laplacians[v][2];

        double ww = 0.0;

        for (auto h : deformed_target.halfedges(v))
        {
            auto vv = deformed_target.to_vertex(h);
            auto e = deformed_target.edge(h);
            auto w = cotan[e];
            ww += w;

            if (!locked[vv])
            {
                triplets.push_back(Triplet(i, idx[vv], w));
            }
            else
            {
                const Point& x = deformed_target.position(vv);
                B(i, 0) -= x[0] * w;
                B(i, 1) -= x[1] * w;
                B(i, 2) -= x[2] * w;
            }
        }

        triplets.push_back(Triplet(i, idx[v], -ww));
    }
    A.setFromTriplets(triplets.begin(), triplets.end());

    // solve system
    DenseMatrix X(N, 3);
    Solver llt(A);
    X = llt.solve(B);
    if (llt.info() != Eigen::Success)
    {
        throw SolverException("Input meshes have different tesselation!");
    }

    // copy solution to vertices of deformed target mesh
    for (unsigned int i = 0; i < free_vertices.size(); ++i)
    {
        auto v = free_vertices[i];
        auto p = Point(X(i, 0), X(i, 1), X(i, 2));
        deformed_target.position(v) = p;
    }

    // free properties
    deformed_target.remove_edge_property(cotan);
    deformed_target.remove_vertex_property(locked);
    deformed_target.remove_vertex_property(idx);
    deformed_target.remove_vertex_property(target_laplacians);
    deformed_target.remove_face_property(defgrad);
}

void DeformationTransfer::compute_cotan_weights(const SurfaceMesh& mesh,
                                                EdgeProperty<double>& cotan)
{
    // edge weights are sufficient, since we need vertex weights of 1/2, which we can multiply to edge weights.
    // important: do not clamp cotan weights, since our right-hand side will also not be clamped.
    for (auto e : mesh.edges())
    {
        cotan[e] = 0.5 * cotan_weight(mesh, e, false);
    }
}

void DeformationTransfer::lock_vertices(const SurfaceMesh& source_before,
                                        const SurfaceMesh& source_after,
                                        VertexProperty<bool>& locked)
{
    // compute bounding box of source mesh
    BoundingBox bb;
    for (auto v : source_before.vertices())
    {
        bb += source_before.position(v);
    }

    // threshold for classifying a vertex as not moved
    const Scalar eps = 0.001 * bb.size();

    // lock all vertices that did not move from before to after
    for (auto v : source_before.vertices())
    {
        locked[v] =
            distance(source_before.position(v), source_after.position(v)) < eps;
    }

    // debug output
    int counter(0);
    for (auto v : source_before.vertices())
        if (locked[v])
            ++counter;
    std::clog << "DefTrans: locking " << counter << " of "
              << source_before.n_vertices() << " vertices\n";
}

void DeformationTransfer::extract_deformation(const SurfaceMesh& before,
                                              const SurfaceMesh& after,
                                              FaceProperty<dmat3>& defgrad)
{
    for (auto f : before.faces())
    {
        auto vertex_iter = before.vertices(f);
        Vertex va = *vertex_iter;
        Vertex vb = *(++vertex_iter);
        Vertex vc = *(++vertex_iter);

        const dvec3 a_before = (dvec3)before.position(va);
        const dvec3 b_before = (dvec3)before.position(vb);
        const dvec3 c_before = (dvec3)before.position(vc);
        const dvec3 edge0_before = c_before - a_before;
        const dvec3 edge1_before = b_before - a_before;
        const dvec3 normal_before =
            normalize(cross(edge0_before, edge1_before));
        const dmat3 orientation_before{edge0_before, edge1_before,
                                       normal_before};

        const dvec3 a_after = (dvec3)after.position(va);
        const dvec3 b_after = (dvec3)after.position(vb);
        const dvec3 c_after = (dvec3)after.position(vc);
        const dvec3 edge0_after = c_after - a_after;
        const dvec3 edge1_after = b_after - a_after;
        const dvec3 normal_after = normalize(cross(edge0_after, edge1_after));
        const dmat3 orientation_after{edge0_after, edge1_after, normal_after};

        defgrad[f] = orientation_after * inverse(orientation_before);
    }
}

void DeformationTransfer::compute_target_laplacians(
    const FaceProperty<dmat3>& defgrad, VertexProperty<dvec3>& laplacians)
{
    for (auto v : target_.vertices())
    {
        laplacians[v] = dvec3(0, 0, 0);
    }

    for (auto f : target_.faces())
    {
        auto vertex_iter = target_.vertices(f);
        Vertex va = *vertex_iter;
        Vertex vb = *(++vertex_iter);
        Vertex vc = *(++vertex_iter);

        const dvec3 a = (dvec3)target_.position(va);
        const dvec3 b = (dvec3)target_.position(vb);
        const dvec3 c = (dvec3)target_.position(vc);

        // face area
        const double area = 0.5 * norm(cross((b - a), (c - a)));

        // gradient of basis functions of a
        dvec3 d = normalize(c - b);
        dvec3 ga = b + d * dot(d, (a - b)) - a;
        ga /= sqrnorm(ga);

        // gradient of basis functions of b
        d = normalize(c - a);
        dvec3 gb = a + d * dot(d, (b - a)) - b;
        gb /= sqrnorm(gb);

        // gradient of basis functions of c
        d = normalize(b - a);
        dvec3 gc = a + d * dot(d, (c - a)) - c;
        gc /= sqrnorm(gc);

        // deformation gradient matrix
        const dmat3 G = transpose(defgrad[f]);
        const dvec3 gx = dvec3(G(0, 0), G(1, 0),
                               G(2, 0)); // x components of all three gradients
        const dvec3 gy = dvec3(G(0, 1), G(1, 1),
                               G(2, 1)); // y components of all three gradients
        const dvec3 gz = dvec3(G(0, 2), G(1, 2),
                               G(2, 2)); // z components of all three gradients

        // apply divergence: accumulate face gradient in vertex laplacian
        laplacians[va][0] += dot(ga, gx) * area;
        laplacians[va][1] += dot(ga, gy) * area;
        laplacians[va][2] += dot(ga, gz) * area;
        laplacians[vb][0] += dot(gb, gx) * area;
        laplacians[vb][1] += dot(gb, gy) * area;
        laplacians[vb][2] += dot(gb, gz) * area;
        laplacians[vc][0] += dot(gc, gx) * area;
        laplacians[vc][1] += dot(gc, gy) * area;
        laplacians[vc][2] += dot(gc, gz) * area;
    }
}

} // namespace pmp
