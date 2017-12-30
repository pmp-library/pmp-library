//=============================================================================

#include <pmp/algorithms/SurfaceParametrization.h>
#include <pmp/algorithms/DifferentialGeometry.h>
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/Sparse>

//=============================================================================

namespace pmp {

//=============================================================================


Parameterizer::Parameterizer(SurfaceMesh& m)
    : mesh(m)
{
}


//-----------------------------------------------------------------------------


bool Parameterizer::setup_boundary_constraints()
{
    // get properties
    auto points  = mesh.vertexProperty<Point>("v:point");
    auto tex     = mesh.vertexProperty<TextureCoordinate>("v:tex");


    SurfaceMesh::VertexIterator  vit, vend=mesh.verticesEnd();
    SurfaceMesh::Vertex      vh;
    SurfaceMesh::Halfedge    hh;
    std::vector<SurfaceMesh::Vertex>  loop;


    // Initialize all texture coordinates to the origin.
    for (auto v: mesh.vertices())
        tex[v] = TextureCoordinate(0.5, 0.5);

    // find 1st boundary vertex
    for (vit=mesh.verticesBegin(); vit!=vend; ++vit)
        if (mesh.isBoundary(*vit))
            break;

    // no boundary found ?
    if (vit == vend)
    {
        std::cerr << "Mesh has no boundary." << std::endl;
        return false;
    }

    // collect boundary loop
    vh = *vit;
    hh = mesh.halfedge(vh);
    do
    {
        loop.push_back(mesh.toVertex(hh));
        hh = mesh.nextHalfedge(hh);
    }
    while (hh != mesh.halfedge(vh));

    // map boundary loop to unit circle in texture domain
    unsigned int        i, n = loop.size();
    Scalar              angle, l, length;
    TextureCoordinate  t;

    // compute length of boundary loop
    for (i=0, length=0.0; i<n; ++i)
        length += distance(points[loop[i]], points[loop[(i+1)%n]]);

    // map length intervalls to unit circle intervals
    for (i=0, l=0.0; i<n; )
    {
        angle = 2.0*M_PI*(1.0-l/length); // go from 2pi to 0 to preserve orientation

        t[0]  = 0.5 + 0.5*cosf(angle);
        t[1]  = 0.5 + 0.5*sinf(angle);

        tex[loop[i]] = t;

        ++i;
        if (i<n)
        {
            l += distance(points[loop[i]], points[loop[(i+1)%n]]);
        }
    }

    return true;
}


//-----------------------------------------------------------------------------


void
Parameterizer::
parameterize(bool uniform)
{
    // get properties
    auto tex     = mesh.vertexProperty<TextureCoordinate>("v:tex");
    auto eweight = mesh.edgeProperty<Scalar>("e:cotan");


    // compute Laplace weight per edge: cotan or uniform
    for (auto e : mesh.edges())
    {
        eweight[e] = uniform ? 1.0 : std::max(0.0, cotanWeight(mesh, e));
    }


    // map boundary to circle
    if (!setup_boundary_constraints())
    {
        std::cerr << "Could not perform setup of boundary constraints.\nExiting..." << std::endl;
        return;
    }


    // collect free (non-boundary) vertices in array free_vertices[]
    // assign indices such that idx[ free_vertices[i] ] == i
    unsigned i = 0;
    auto idx = mesh.addVertexProperty<int>("v:idx", -1);
    std::vector<SurfaceMesh::Vertex>   free_vertices;
    free_vertices.reserve(mesh.nVertices());
    for (auto v: mesh.vertices())
    {
        if (!mesh.isBoundary(v))
        {
            idx[v] = i++;
            free_vertices.push_back(v);
        }
    }


    // setup matrix A and rhs B
    const unsigned int           n = free_vertices.size();
    Eigen::SparseMatrix<double>  A(n,n);
    Eigen::MatrixXd              B(n,2);
    std::vector< Eigen::Triplet<double> > triplets;
    double w, ww;
    SurfaceMesh::Vertex v, vv;
    SurfaceMesh::Edge   e;
    for (i = 0; i < n; ++i)
    {
        v = free_vertices[i];

        // rhs row
        B(i,0) = 0.0;
        B(i,1) = 0.0;

        // lhs row
        ww = 0.0;
        for (auto h: mesh.halfedges(v))
        {
            vv = mesh.toVertex(h);
            e  = mesh.edge(h);
            w  = eweight[e];
            ww += w;

            if (mesh.isBoundary(vv))
            {
				B(i,0) -= -w * tex[vv][0];
				B(i,1) -= -w * tex[vv][1];
            }
            else
            {
                triplets.push_back(Eigen::Triplet<double>(i, idx[vv], -w));
            }
		}
        triplets.push_back(Eigen::Triplet<double>(i, i, ww));
    }


    // build sparse matrix from triplets
    A.setFromTriplets(triplets.begin(), triplets.end());


    // solve A*X = B
    Eigen::SimplicialLDLT< Eigen::SparseMatrix<double> > solver(A);
    Eigen::MatrixXd X = solver.solve(B);
    if (solver.info() != Eigen::Success)
    {
        std::cerr << "SurfaceParameterization: Could not solve linear system\n";
    }
    else
    {
        // copy solution
        for (i = 0; i < n; ++i)
        {
            v = free_vertices[i];
            tex[v][0] = X(i,0);
            tex[v][1] = X(i,1);
        }
    }


    // clean-up
    mesh.removeVertexProperty(idx);
}


//=============================================================================
}
//=============================================================================
