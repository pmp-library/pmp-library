//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
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

#include <pmp/EdgeSet.h>
#include <pmp/io/EdgeSetIO.h>

#include <cmath>
#include <exception>

//=============================================================================

namespace pmp {

//=============================================================================

EdgeSet::EdgeSet() : PointSet()
{
    // allocate standard properties
    // same list is used in operator=() and assign()
    m_vconn = addVertexProperty<VertexConnectivity>("v:connectivity");
    m_hconn = addHalfedgeProperty<HalfedgeConnectivity>("h:connectivity");
    m_edeleted = addEdgeProperty<bool>("e:deleted", false);
    m_deletedEdges = 0;
}

//-----------------------------------------------------------------------------

EdgeSet::~EdgeSet() = default;

//-----------------------------------------------------------------------------

EdgeSet& EdgeSet::operator=(const EdgeSet& rhs)
{
    PointSet::operator=(rhs);

    if (this != &rhs)
    {
        // deep copy of property containers
        m_hprops = rhs.m_hprops;
        m_eprops = rhs.m_eprops;

        // property handles contain pointers, have to be reassigned
        m_vconn = vertexProperty<VertexConnectivity>("v:connectivity");
        m_hconn = halfedgeProperty<HalfedgeConnectivity>("h:connectivity");
        m_edeleted = edgeProperty<bool>("e:deleted");

        // how many elements are deleted?
        m_deletedEdges = rhs.m_deletedEdges;
    }

    return *this;
}

//-----------------------------------------------------------------------------

EdgeSet& EdgeSet::assign(const EdgeSet& rhs)
{
    PointSet::assign(rhs);

    if (this != &rhs)
    {
        // clear properties
        m_hprops.clear();
        m_eprops.clear();

        // allocate standard properties
        m_vconn = addVertexProperty<VertexConnectivity>("v:connectivity");
        m_hconn = addHalfedgeProperty<HalfedgeConnectivity>("h:connectivity");
        m_edeleted = addEdgeProperty<bool>("e:deleted", false);

        // copy properties from other mesh
        m_vconn.array() = rhs.m_vconn.array();
        m_hconn.array() = rhs.m_hconn.array();
        m_edeleted.array() = rhs.m_edeleted.array();

        // resize (needed by property containers)
        m_hprops.resize(rhs.halfedgesSize());
        m_eprops.resize(rhs.edgesSize());

        // how many elements are deleted?
        m_deletedEdges = rhs.m_deletedEdges;
    }

    return *this;
}

//-----------------------------------------------------------------------------

bool EdgeSet::read(const std::string& filename, const IOOptions& options)
{
    EdgeSetIO reader(options);
    bool success = reader.read(*this, filename);

    // try parent class if no reader is found
    if (!success)
    {
        success = PointSet::read(filename, options);
    }

    return success;
}

//-----------------------------------------------------------------------------

bool EdgeSet::write(const std::string& filename, const IOOptions& options) const
{
    EdgeSetIO writer(options);
    bool success = writer.write(*this, filename);

    // try parent class if no writer is found
    if (!success)
    {
        success = PointSet::write(filename, options);
    }

    return success;
}

//-----------------------------------------------------------------------------

void EdgeSet::clear()
{
    m_hprops.resize(0);
    m_eprops.resize(0);

    freeMemory();

    m_deletedEdges = 0;

    PointSet::clear();
}

//-----------------------------------------------------------------------------

void EdgeSet::freeMemory()
{
    m_hprops.freeMemory();
    m_eprops.freeMemory();

    PointSet::freeMemory();
}

//-----------------------------------------------------------------------------

void EdgeSet::reserve(size_t nvertices, size_t nedges)
{
    PointSet::reserve(nvertices);

    m_hprops.reserve(2 * nedges);
    m_eprops.reserve(nedges);
}

//-----------------------------------------------------------------------------

void EdgeSet::propertyStats() const
{
    std::vector<std::string> props;

    PointSet::propertyStats();

    std::cout << "halfedge properties:\n";
    props = halfedgeProperties();
    for (const auto& prop : props)
        std::cout << "\t" << prop << std::endl;

    std::cout << "edge properties:\n";
    props = edgeProperties();
    for (const auto& prop : props)
        std::cout << "\t" << prop << std::endl;
}

//-----------------------------------------------------------------------------

EdgeSet::Halfedge EdgeSet::findHalfedge(Vertex start, Vertex end) const
{
    assert(isValid(start) && isValid(end));

    Halfedge h = halfedge(start);
    const Halfedge hh = h;

    if (h.isValid())
    {
        do
        {
            if (toVertex(h) == end)
                return h;
            h = cwRotatedHalfedge(h);
        } while (h != hh);
    }

    return Halfedge();
}

//-----------------------------------------------------------------------------

EdgeSet::Edge EdgeSet::findEdge(Vertex a, Vertex b) const
{
    Halfedge h = findHalfedge(a, b);
    return h.isValid() ? edge(h) : Edge();
}

//-----------------------------------------------------------------------------

size_t EdgeSet::valence(Vertex v) const
{
    size_t count(0);

    for (auto vv : vertices(v))
    {
        PMP_ASSERT(vv.isValid());
        ++count;
    }

    return count;
}

//-----------------------------------------------------------------------------

Scalar EdgeSet::edgeLength(Edge e) const
{
    return norm(m_vpoint[vertex(e, 0)] - m_vpoint[vertex(e, 1)]);
}

//-----------------------------------------------------------------------------

EdgeSet::Halfedge EdgeSet::insertVertex(Halfedge h0, Vertex v)
{
    // before:
    //
    // v0      h0       v2
    //  o--------------->o
    //   <---------------
    //         o0
    //
    // after:
    //
    // v0  h0   v   h1   v2
    //  o------>o------->o
    //   <------ <-------
    //     o0       o1

    Halfedge h2 = nextHalfedge(h0);
    Halfedge o0 = oppositeHalfedge(h0);
    Halfedge o2 = prevHalfedge(o0);
    Vertex v2 = toVertex(h0);

    Halfedge h1 = newEdge(v, v2);
    Halfedge o1 = oppositeHalfedge(h1);

    // adjust halfedge connectivity
    setNextHalfedge(h1, h2);
    setNextHalfedge(h0, h1);
    setVertex(h0, v);
    setVertex(h1, v2);

    setNextHalfedge(o1, o0);
    setNextHalfedge(o2, o1);
    setVertex(o1, v);

    // adjust vertex connectivity
    setHalfedge(v2, o1);
    setHalfedge(v, h1);

    return o1;
}

//-----------------------------------------------------------------------------

EdgeSet::Halfedge EdgeSet::insertEdge(Vertex v0, Vertex v1)
{
    //  ^                       ^
    //  |                       |
    // he(v0)                 he(v1)
    //  |                       |
    // v0 -------> h0 -------> v1
    // v0 <------- h1 -------- v1
    //  ^                       ^
    //  |                       |
    // prev(he(v0))        prev(he(v1))
    //  |                       |

    // edge already exists
    assert(!findEdge(v0, v1).isValid());

    const Vertex v[2] = {v0, v1};
    const Halfedge h[2] = {newEdge(v0, v1), oppositeHalfedge(h[0])};

    Halfedge he0 = halfedge(v[0]);
    Halfedge he1 = halfedge(v[1]);

    if (he1.isValid())
    {
        Halfedge p1 = prevHalfedge(he1);

        setNextHalfedge(h[0], he1);
        setPrevHalfedge(h[1], p1);
    }
    else
    {
        setNextHalfedge(h[0], h[1]);
    }

    if (he0.isValid())
    {
        Halfedge p0 = prevHalfedge(he0);
        Halfedge op0 = oppositeHalfedge(p0);

        setNextHalfedge(h[1], op0);
        setPrevHalfedge(h[0], oppositeHalfedge(he0));
    }
    else
    {
        setNextHalfedge(h[1], h[0]);
    }

    setHalfedge(v[0], h[0]);
    setHalfedge(v[1], h[1]);

    return h[0];
}

//-----------------------------------------------------------------------------

void EdgeSet::deleteVertex(Vertex v)
{
    if (isDeleted(v))
        return;

    // collect edges to be deleted
    std::vector<Edge> edges;
    for (auto h : halfedges(v))
        if (h.isValid())
            edges.push_back(edge(h));

    for (auto e : edges)
        deleteEdge(e);

    PointSet::deleteVertex(v);
}

//-----------------------------------------------------------------------------

void EdgeSet::deleteEdge(Edge e)
{
    if (isDeleted(e))
        return;

    Halfedge h0, h1, next0, next1, prev0, prev1;
    Vertex v0, v1;

    h0 = halfedge(e, 0);
    v0 = toVertex(h0);
    next0 = nextHalfedge(h0);
    prev0 = prevHalfedge(h0);

    h1 = halfedge(e, 1);
    v1 = toVertex(h1);
    next1 = nextHalfedge(h1);
    prev1 = prevHalfedge(h1);

    // adjust next and prev handles
    setNextHalfedge(prev0, next1);
    setNextHalfedge(prev1, next0);

    // update v0
    if (halfedge(v0) == h1)
    {
        if (next0 == h1)
        {
            PointSet::deleteVertex(v0);
        }
        else
            setHalfedge(v0, next0);
    }

    // update v1
    if (halfedge(v1) == h0)
    {
        if (next1 == h0)
        {
            PointSet::deleteVertex(v1);
        }
        else
            setHalfedge(v1, next1);
    }

    // mark edge deleted
    m_edeleted[e] = true;
    ++m_deletedEdges;
    m_garbage = true;
}

//-----------------------------------------------------------------------------

void EdgeSet::beginGarbage()
{
    // delete marked vertices
    PointSet::beginGarbage();

    const int nV = m_garbageprops["nV"];

    int i0, i1, nE(edgesSize()), nH(halfedgesSize());

    Vertex v;
    Halfedge h;

    // setup handle mapping
    const VertexProperty<Vertex> vmap =
        getVertexProperty<Vertex>("v:garbage-collection");
    HalfedgeProperty<Halfedge> hmap =
        addHalfedgeProperty<Halfedge>("h:garbage-collection");

    for (int i(0); i < nH; ++i)
        hmap[Halfedge(i)] = Halfedge(i);

    // remove deleted edges
    if (nE > 0)
    {
        i0 = 0;
        i1 = nE - 1;

        while (true)
        {
            // find first deleted and last un-deleted
            while (!m_edeleted[Edge(i0)] && i0 < i1)
                ++i0;
            while (m_edeleted[Edge(i1)] && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;

            // swap
            m_eprops.swap(i0, i1);
            m_hprops.swap(2 * i0, 2 * i1);
            m_hprops.swap(2 * i0 + 1, 2 * i1 + 1);
        };

        // remember new size
        nE = m_edeleted[Edge(i0)] ? i0 : i0 + 1;
        nH = 2 * nE;
    }

    // update vertex connectivity
    for (int i = 0; i < nV; ++i)
    {
        v = Vertex(i);
        if (!isIsolated(v))
            setHalfedge(v, hmap[halfedge(v)]);
    }

    // update halfedge connectivity
    for (int i = 0; i < nH; ++i)
    {
        h = Halfedge(i);
        setVertex(h, vmap[toVertex(h)]);
        setNextHalfedge(h, hmap[nextHalfedge(h)]);
    }

    m_garbageprops["nH"] = nH;
    m_garbageprops["nE"] = nE;
}

//-----------------------------------------------------------------------------

void EdgeSet::finalizeGarbage()
{
    HalfedgeProperty<Halfedge> hmap =
        getHalfedgeProperty<Halfedge>("h:garbage-collection");

    // remove handle maps
    removeHalfedgeProperty(hmap);

    // finally resize arrays
    m_hprops.resize(m_garbageprops["nH"]);
    m_hprops.freeMemory();
    m_eprops.resize(m_garbageprops["nE"]);
    m_eprops.freeMemory();

    m_deletedEdges = 0;

    PointSet::finalizeGarbage();
}

//=============================================================================
} // namespace pmp
//=============================================================================
