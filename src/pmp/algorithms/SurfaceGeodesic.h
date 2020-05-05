// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <limits>
#include <set>
#include <vector>

#include "pmp/SurfaceMesh.h"

namespace pmp {

//! \brief Compute geodesic distance from a set of seed vertices
//! \details The method works by a Dykstra-like breadth first traversal from
//! the seed vertices, implemented by a heap structure.
//! See \cite kimmel_1998_geodesic for details.
//! \ingroup algorithms
class SurfaceGeodesic
{
public:
    //! \brief Construct from mesh.
    //! \param mesh The mesh on which to compute the geodesic distances.
    //! \param use_virtual_edges A flag to control the use of virtual edges.
    //! Default: true.
    //! \sa compute() to actually compute the geodesic distances.
    SurfaceGeodesic(SurfaceMesh& mesh, bool use_virtual_edges = true);

    // destructor
    ~SurfaceGeodesic();

    //! \brief Compute geodesic distances from specified seed points.
    //! \param[in] seed The vector of seed vertices.
    //! \param[in] maxdist The maximum distance up to which to compute the
    //! geodesic distances.
    //! \param[in] maxnum The maximum number of neighbors up to which to
    //! compute the geodesic distances.
    //! \param[out] neighbors The vector of neighbor vertices.
    //! \return The number of neighbors that have been found.
    unsigned int compute(const std::vector<Vertex>& seed,
                         Scalar maxdist = std::numeric_limits<Scalar>::max(),
                         unsigned int maxnum = INT_MAX,
                         std::vector<Vertex>* neighbors = nullptr);

    //! \brief Access the computed geodesic distance.
    //! \param[in] v The vertex for which to return the geodesic distance.
    //! \return The geodesic distance of vertex \p v.
    //! \pre The function compute() has been called before.
    //! \pre The vertex \p v needs to be a valid vertex handle of the mesh
    //! used during construction.
    Scalar operator()(Vertex v) const { return distance_[v]; }

    //! \brief Use the normalized distances as texture coordinates
    //! \details Stores the normalized distances in a vertex property of type
    //! TexCoord named "v:tex". Re-uses any existing vertex property of the
    //! same type and name.
    void distance_to_texture_coordinates();

private:
    // functor for comparing two vertices w.r.t. their geodesic distance
    class VertexCmp
    {
    public:
        VertexCmp(const VertexProperty<Scalar>& dist) : dist_(dist) {}

        bool operator()(Vertex v0, Vertex v1) const
        {
            return ((dist_[v0] == dist_[v1]) ? (v0 < v1)
                                             : (dist_[v0] < dist_[v1]));
        }

    private:
        const VertexProperty<Scalar>& dist_;
    };

    // priority queue using geodesic distance as sorting criterion
    typedef std::set<Vertex, VertexCmp> PriorityQueue;

    // virtual edges for walking through obtuse triangles
    struct VirtualEdge
    {
        VirtualEdge(Vertex v, Scalar l) : vertex(v), length(l) {}
        Vertex vertex;
        Scalar length;
    };

    // set for storing virtual edges
    typedef std::map<Halfedge, VirtualEdge> VirtualEdges;

    void find_virtual_edges();
    unsigned int init_front(const std::vector<Vertex>& seed,
                            std::vector<Vertex>* neighbors);
    unsigned int propagate_front(Scalar maxdist, unsigned int maxnum,
                                 std::vector<Vertex>* neighbors);
    void heap_vertex(Vertex v);
    Scalar distance(Vertex v0, Vertex v1, Vertex v2,
                    Scalar r0 = std::numeric_limits<Scalar>::max(),
                    Scalar r1 = std::numeric_limits<Scalar>::max());

    SurfaceMesh& mesh_;

    bool use_virtual_edges_;
    VirtualEdges virtual_edges_;

    PriorityQueue* front_;

    VertexProperty<Scalar> distance_;
    VertexProperty<bool> processed_;
};

} // namespace pmp
