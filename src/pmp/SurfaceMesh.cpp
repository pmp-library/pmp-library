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

//== INCLUDES =================================================================

#include <pmp/SurfaceMesh.h>
#include <pmp/io/SurfaceMeshIO.h>
#include <pmp/io/EdgeSetIO.h>

#include <cmath>

//== NAMESPACE ================================================================

namespace pmp {

//== IMPLEMENTATION ===========================================================

SurfaceMesh::SurfaceMesh() : EdgeSet()
{
    // allocate standard properties
    // same list is used in operator=() and assign()
    m_hfconn = addHalfedgeProperty<HalfedgeFaceConnectivity>("hf:connectivity");
    m_fconn = addFaceProperty<FaceConnectivity>("f:connectivity");
    m_fdeleted = addFaceProperty<bool>("f:deleted", false);

    m_deletedFaces = 0;
}

//-----------------------------------------------------------------------------

SurfaceMesh::~SurfaceMesh() = default;

//-----------------------------------------------------------------------------

SurfaceMesh& SurfaceMesh::operator=(const SurfaceMesh& rhs)
{
    EdgeSet::operator=(rhs);

    if (this != &rhs)
    {
        // deep copy of property containers
        m_fprops = rhs.m_fprops;

        // property handles contain pointers, have to be reassigned
        m_hfconn =
            halfedgeProperty<HalfedgeFaceConnectivity>("hf:connectivity");
        m_fconn = faceProperty<FaceConnectivity>("f:connectivity");
        m_fdeleted = faceProperty<bool>("f:deleted");

        // how many elements are deleted?
        m_deletedFaces = rhs.m_deletedFaces;
    }

    return *this;
}

//-----------------------------------------------------------------------------

SurfaceMesh& SurfaceMesh::assign(const SurfaceMesh& rhs)
{
    EdgeSet::assign(rhs);

    if (this != &rhs)
    {
        // clear properties
        m_fprops.clear();

        // allocate standard properties
        m_hfconn =
            addHalfedgeProperty<HalfedgeFaceConnectivity>("hf:connectivity");
        m_fconn = addFaceProperty<FaceConnectivity>("f:connectivity");
        m_fdeleted = addFaceProperty<bool>("f:deleted", false);

        // copy properties from other mesh
        m_hfconn.array() = rhs.m_hfconn.array();
        m_fconn.array() = rhs.m_fconn.array();
        m_fdeleted.array() = rhs.m_fdeleted.array();

        // resize (needed by property containers)
        m_fprops.resize(rhs.facesSize());

        // how many elements are deleted?
        m_deletedFaces = rhs.m_deletedFaces;
    }

    return *this;
}

//-----------------------------------------------------------------------------

bool SurfaceMesh::read(const std::string& filename, const IOOptions& options)
{
    SurfaceMeshIO reader(options);
    bool success = reader.read(*this, filename);

    // try parent class if no reader is found
    if (!success)
    {
        success = EdgeSet::read(filename, options);
    }

    return success;
}

//-----------------------------------------------------------------------------

bool SurfaceMesh::write(const std::string& filename,
                        const IOOptions& options) const
{
    SurfaceMeshIO writer(options);
    bool success = writer.write(*this, filename);

    // try parent class if no writer is found
    if (!success)
    {
        success = EdgeSet::write(filename, options);
    }

    return success;
}

//-----------------------------------------------------------------------------

void SurfaceMesh::clear()
{
    m_fprops.resize(0);
    m_deletedFaces = 0;
    EdgeSet::clear();
}

//-----------------------------------------------------------------------------

void SurfaceMesh::freeMemory()
{
    m_fprops.freeMemory();
    EdgeSet::freeMemory();
}

//-----------------------------------------------------------------------------

void SurfaceMesh::reserve(size_t nvertices, size_t nedges, size_t nfaces)
{
    EdgeSet::reserve(nvertices, nedges);
    m_fprops.reserve(nfaces);
}

//-----------------------------------------------------------------------------

void SurfaceMesh::propertyStats() const
{
    std::vector<std::string> props;

    EdgeSet::propertyStats();

    std::cout << "face properties:\n";
    props = faceProperties();
    for (const auto& prop : props)
        std::cout << "\t" << prop << std::endl;
}

//-----------------------------------------------------------------------------

void SurfaceMesh::adjustOutgoingHalfedge(Vertex v)
{
    Halfedge h = halfedge(v);
    const Halfedge hh = h;

    if (h.isValid())
    {
        do
        {
            if (isSurfaceBoundary(h))
            {
                setHalfedge(v, h);
                return;
            }
            h = cwRotatedHalfedge(h);
        } while (h != hh);
    }
}

//-----------------------------------------------------------------------------

SurfaceMesh::Face SurfaceMesh::addTriangle(Vertex v0, Vertex v1, Vertex v2)
{
    m_addFaceVertices.resize(3);
    m_addFaceVertices[0] = v0;
    m_addFaceVertices[1] = v1;
    m_addFaceVertices[2] = v2;
    return addFace(m_addFaceVertices);
}

//-----------------------------------------------------------------------------

SurfaceMesh::Face SurfaceMesh::addQuad(Vertex v0, Vertex v1, Vertex v2,
                                       Vertex v3)
{
    m_addFaceVertices.resize(4);
    m_addFaceVertices[0] = v0;
    m_addFaceVertices[1] = v1;
    m_addFaceVertices[2] = v2;
    m_addFaceVertices[3] = v3;
    return addFace(m_addFaceVertices);
}

//-----------------------------------------------------------------------------

SurfaceMesh::Face SurfaceMesh::addFace(const std::vector<Vertex>& vertices)
{
    const size_t n(vertices.size());
    assert(n > 2);

    Vertex v;
    size_t i, ii, id;
    Halfedge innerNext, innerPrev, outerNext, outerPrev, boundaryNext,
        boundaryPrev, patchStart, patchEnd;

    // use global arrays to avoid new/delete of local arrays!!!
    std::vector<Halfedge>& halfedges = m_addFaceHalfedges;
    std::vector<bool>& isNew = m_addFaceIsNew;
    std::vector<bool>& needsAdjust = m_addFaceNeedsAdjust;
    NextCache& nextCache = m_addFaceNextCache;
    halfedges.clear();
    halfedges.resize(n);
    isNew.clear();
    isNew.resize(n);
    needsAdjust.clear();
    needsAdjust.resize(n, false);
    nextCache.clear();
    nextCache.reserve(3 * n);

    // test for topological errors
    for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
    {
        if (!isSurfaceBoundary(vertices[i]))
        {
            std::cerr << "SurfaceMesh::addFace: complex vertex\n";
            return Face();
        }

        halfedges[i] = findHalfedge(vertices[i], vertices[ii]);
        isNew[i] = !halfedges[i].isValid();

        if (!isNew[i] && !isSurfaceBoundary(halfedges[i]))
        {
            std::cerr << "SurfaceMesh::addFace: complex edge\n";
            return Face();
        }
    }

    // re-link patches if necessary
    for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
    {
        if (!isNew[i] && !isNew[ii])
        {
            innerPrev = halfedges[i];
            innerNext = halfedges[ii];

            if (nextHalfedge(innerPrev) != innerNext)
            {
                // here comes the ugly part... we have to relink a whole patch

                // search a free gap
                // free gap will be between boundaryPrev and boundaryNext
                outerPrev = oppositeHalfedge(innerNext);
                outerNext = oppositeHalfedge(innerPrev);
                boundaryPrev = outerPrev;
                do
                {
                    boundaryPrev = oppositeHalfedge(nextHalfedge(boundaryPrev));
                } while (!isSurfaceBoundary(boundaryPrev) ||
                         boundaryPrev == innerPrev);
                boundaryNext = nextHalfedge(boundaryPrev);
                assert(isSurfaceBoundary(boundaryPrev));
                assert(isSurfaceBoundary(boundaryNext));

                // ok ?
                if (boundaryNext == innerNext)
                {
                    std::cerr
                        << "SurfaceMeshT::addFace: patch re-linking failed\n";
                    return Face();
                }

                // other halfedges' handles
                patchStart = nextHalfedge(innerPrev);
                patchEnd = prevHalfedge(innerNext);

                // relink
                nextCache.emplace_back(boundaryPrev, patchStart);
                nextCache.emplace_back(patchEnd, boundaryNext);
                nextCache.emplace_back(innerPrev, innerNext);
            }
        }
    }

    // create missing edges
    for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
    {
        if (isNew[i])
        {
            halfedges[i] = newEdge(vertices[i], vertices[ii]);
        }
    }

    // create the face
    Face f(newFace());
    setHalfedge(f, halfedges[n - 1]);

    // setup halfedges
    for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
    {
        v = vertices[ii];
        innerPrev = halfedges[i];
        innerNext = halfedges[ii];

        id = 0;
        if (isNew[i])
            id |= 1;
        if (isNew[ii])
            id |= 2;

        if (id)
        {
            outerPrev = oppositeHalfedge(innerNext);
            outerNext = oppositeHalfedge(innerPrev);

            // set outer links
            switch (id)
            {
                case 1: // prev is new, next is old
                    boundaryPrev = prevHalfedge(innerNext);
                    nextCache.emplace_back(boundaryPrev, outerNext);
                    setHalfedge(v, outerNext);
                    break;

                case 2: // next is new, prev is old
                    boundaryNext = nextHalfedge(innerPrev);
                    nextCache.emplace_back(outerPrev, boundaryNext);
                    setHalfedge(v, boundaryNext);
                    break;

                case 3: // both are new
                    if (!halfedge(v).isValid())
                    {
                        setHalfedge(v, outerNext);
                        nextCache.emplace_back(outerPrev, outerNext);
                    }
                    else
                    {
                        boundaryNext = halfedge(v);
                        boundaryPrev = prevHalfedge(boundaryNext);
                        nextCache.emplace_back(boundaryPrev, outerNext);
                        nextCache.emplace_back(outerPrev, boundaryNext);
                    }
                    break;
            }

            // set inner link
            nextCache.emplace_back(innerPrev, innerNext);
        }
        else
            needsAdjust[ii] = (halfedge(v) == innerNext);

        // set face handle
        setFace(halfedges[i], f);
    }

    // process next halfedge cache
    NextCache::const_iterator ncIt(nextCache.begin()), ncEnd(nextCache.end());
    for (; ncIt != ncEnd; ++ncIt)
    {
        setNextHalfedge(ncIt->first, ncIt->second);
    }

    // adjust vertices' halfedge handle
    for (i = 0; i < n; ++i)
    {
        if (needsAdjust[i])
        {
            adjustOutgoingHalfedge(vertices[i]);
        }
    }

    return f;
}

//-----------------------------------------------------------------------------

size_t SurfaceMesh::valence(Face f) const
{
    size_t count(0);

    for (auto v : vertices(f))
    {
        PMP_ASSERT(v.isValid());
        ++count;
    }

    return count;
}

//-----------------------------------------------------------------------------

bool SurfaceMesh::isTriangleMesh() const
{
    for (auto f : faces())
        if (valence(f) != 3)
            return false;

    return true;
}

//-----------------------------------------------------------------------------

bool SurfaceMesh::isQuadMesh() const
{
    for (auto f : faces())
        if (valence(f) != 4)
            return false;

    return true;
}

//-----------------------------------------------------------------------------

void SurfaceMesh::triangulate()
{
    // The iterators will stay valid, even though new faces are added, because
    // they are now implemented index-based instead of pointer-based.
    auto fend = facesEnd();
    for (auto fit = facesBegin(); fit != fend; ++fit)
        triangulate(*fit);
}

//-----------------------------------------------------------------------------

void SurfaceMesh::triangulate(Face f)
{
    // Split an arbitrary face into triangles by connecting each vertex of \c f
    // after its second to \c v .\c f will remain valid (it will become one of
    // the triangles). The halfedge handles of the new triangles will point to
    // the old halfedges.

    Halfedge baseH = halfedge(f);
    Vertex startV = fromVertex(baseH);
    Halfedge nextH = nextHalfedge(baseH);

    while (toVertex(nextHalfedge(nextH)) != startV)
    {
        Halfedge nextNextH(nextHalfedge(nextH));

        Face newF = newFace();
        setHalfedge(newF, baseH);

        Halfedge newH = newEdge(toVertex(nextH), startV);

        setNextHalfedge(baseH, nextH);
        setNextHalfedge(nextH, newH);
        setNextHalfedge(newH, baseH);

        setFace(baseH, newF);
        setFace(nextH, newF);
        setFace(newH, newF);

        baseH = oppositeHalfedge(newH);
        nextH = nextNextH;
    }
    setHalfedge(f, baseH); //the last face takes the handle baseH

    setNextHalfedge(baseH, nextH);
    setNextHalfedge(nextHalfedge(nextH), baseH);

    setFace(baseH, f);
}

//-----------------------------------------------------------------------------

void SurfaceMesh::split(Face f, Vertex v)
{
    // Split an arbitrary face into triangles by connecting each vertex of \c f
    // to \c v . \c f will remain valid (it will become one of the
    // triangles). The halfedge handles of the new triangles will point to the
    // old halfedges.

    Halfedge hend = halfedge(f);
    Halfedge h = nextHalfedge(hend);

    Halfedge hold = newEdge(toVertex(hend), v);

    setNextHalfedge(hend, hold);
    setFace(hold, f);

    hold = oppositeHalfedge(hold);

    while (h != hend)
    {
        Halfedge hnext = nextHalfedge(h);

        Face fnew = newFace();
        setHalfedge(fnew, h);

        Halfedge hnew = newEdge(toVertex(h), v);

        setNextHalfedge(hnew, hold);
        setNextHalfedge(hold, h);
        setNextHalfedge(h, hnew);

        setFace(hnew, fnew);
        setFace(hold, fnew);
        setFace(h, fnew);

        hold = oppositeHalfedge(hnew);

        h = hnext;
    }

    setNextHalfedge(hold, hend);
    setNextHalfedge(nextHalfedge(hend), hold);

    setFace(hold, f);

    setHalfedge(v, hold);
}

//-----------------------------------------------------------------------------

SurfaceMesh::Halfedge SurfaceMesh::split(Edge e, Vertex v)
{
    Halfedge h0 = halfedge(e, 0);
    Halfedge o0 = halfedge(e, 1);

    Vertex v2 = toVertex(o0);

    Halfedge e1 = newEdge(v, v2);
    Halfedge t1 = oppositeHalfedge(e1);

    Face f0 = face(h0);
    Face f3 = face(o0);

    setHalfedge(v, h0);
    setVertex(o0, v);

    if (!isSurfaceBoundary(h0))
    {
        Halfedge h1 = nextHalfedge(h0);
        Halfedge h2 = nextHalfedge(h1);

        Vertex v1 = toVertex(h1);

        Halfedge e0 = newEdge(v, v1);
        Halfedge t0 = oppositeHalfedge(e0);

        Face f1 = newFace();
        setHalfedge(f0, h0);
        setHalfedge(f1, h2);

        setFace(h1, f0);
        setFace(t0, f0);
        setFace(h0, f0);

        setFace(h2, f1);
        setFace(t1, f1);
        setFace(e0, f1);

        setNextHalfedge(h0, h1);
        setNextHalfedge(h1, t0);
        setNextHalfedge(t0, h0);

        setNextHalfedge(e0, h2);
        setNextHalfedge(h2, t1);
        setNextHalfedge(t1, e0);
    }
    else
    {
        setNextHalfedge(prevHalfedge(h0), t1);
        setNextHalfedge(t1, h0);
        // halfedge handle of vh already is h0
    }

    if (!isSurfaceBoundary(o0))
    {
        Halfedge o1 = nextHalfedge(o0);
        Halfedge o2 = nextHalfedge(o1);

        Vertex v3 = toVertex(o1);

        Halfedge e2 = newEdge(v, v3);
        Halfedge t2 = oppositeHalfedge(e2);

        Face f2 = newFace();
        setHalfedge(f2, o1);
        setHalfedge(f3, o0);

        setFace(o1, f2);
        setFace(t2, f2);
        setFace(e1, f2);

        setFace(o2, f3);
        setFace(o0, f3);
        setFace(e2, f3);

        setNextHalfedge(e1, o1);
        setNextHalfedge(o1, t2);
        setNextHalfedge(t2, e1);

        setNextHalfedge(o0, e2);
        setNextHalfedge(e2, o2);
        setNextHalfedge(o2, o0);
    }
    else
    {
        setNextHalfedge(e1, nextHalfedge(o0));
        setNextHalfedge(o0, e1);
        setHalfedge(v, e1);
    }

    if (halfedge(v2) == h0)
        setHalfedge(v2, t1);

    return t1;
}

//-----------------------------------------------------------------------------

SurfaceMesh::Halfedge SurfaceMesh::insertVertex(Halfedge h0, Vertex v)
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

    Vertex v2 = toVertex(h0);
    Halfedge o0 = oppositeHalfedge(h0);

    Halfedge o1 = EdgeSet::insertVertex(h0, v);
    Halfedge h1 = oppositeHalfedge(o1);

    Face fh = face(h0);
    Face fo = face(o0);

    setFace(h1, fh);
    setFace(o1, fo);

    adjustOutgoingHalfedge(v2);
    adjustOutgoingHalfedge(v);

    // adjust face connectivity
    if (fh.isValid())
        setHalfedge(fh, h0);
    if (fo.isValid())
        setHalfedge(fo, o1);

    return o1;
}

//-----------------------------------------------------------------------------

SurfaceMesh::Halfedge SurfaceMesh::insertEdge(Halfedge h0, Halfedge h1)
{
    assert(face(h0) == face(h1));
    assert(face(h0).isValid());

    Vertex v0 = toVertex(h0);
    Vertex v1 = toVertex(h1);

    Halfedge h2 = nextHalfedge(h0);
    Halfedge h3 = nextHalfedge(h1);

    Halfedge h4 = newEdge(v0, v1);
    Halfedge h5 = oppositeHalfedge(h4);

    Face f0 = face(h0);
    Face f1 = newFace();

    setHalfedge(f0, h0);
    setHalfedge(f1, h1);

    setNextHalfedge(h0, h4);
    setNextHalfedge(h4, h3);
    setFace(h4, f0);

    setNextHalfedge(h1, h5);
    setNextHalfedge(h5, h2);
    Halfedge h = h2;
    do
    {
        setFace(h, f1);
        h = nextHalfedge(h);
    } while (h != h2);

    return h4;
}

//-----------------------------------------------------------------------------

bool SurfaceMesh::isFlipOk(Edge e) const
{
    // boundary edges cannot be flipped
    if (isSurfaceBoundary(e))
        return false;

    // check if the flipped edge is already present in the mesh
    Halfedge h0 = halfedge(e, 0);
    Halfedge h1 = halfedge(e, 1);

    Vertex v0 = toVertex(nextHalfedge(h0));
    Vertex v1 = toVertex(nextHalfedge(h1));

    if (v0 == v1) // this is generally a bad sign !!!
        return false;

    if (findHalfedge(v0, v1).isValid())
        return false;

    return true;
}

//-----------------------------------------------------------------------------

void SurfaceMesh::flip(Edge e)
{
    //let's make it sure it is actually checked
    assert(isFlipOk(e));

    Halfedge a0 = halfedge(e, 0);
    Halfedge b0 = halfedge(e, 1);

    Halfedge a1 = nextHalfedge(a0);
    Halfedge a2 = nextHalfedge(a1);

    Halfedge b1 = nextHalfedge(b0);
    Halfedge b2 = nextHalfedge(b1);

    Vertex va0 = toVertex(a0);
    Vertex va1 = toVertex(a1);

    Vertex vb0 = toVertex(b0);
    Vertex vb1 = toVertex(b1);

    Face fa = face(a0);
    Face fb = face(b0);

    setVertex(a0, va1);
    setVertex(b0, vb1);

    setNextHalfedge(a0, a2);
    setNextHalfedge(a2, b1);
    setNextHalfedge(b1, a0);

    setNextHalfedge(b0, b2);
    setNextHalfedge(b2, a1);
    setNextHalfedge(a1, b0);

    setFace(a1, fb);
    setFace(b1, fa);

    setHalfedge(fa, a0);
    setHalfedge(fb, b0);

    if (halfedge(va0) == b0)
        setHalfedge(va0, a1);
    if (halfedge(vb0) == a0)
        setHalfedge(vb0, b1);
}

//-----------------------------------------------------------------------------

bool SurfaceMesh::isCollapseOk(Halfedge v0v1)
{
    Halfedge v1v0(oppositeHalfedge(v0v1));
    Vertex v0(toVertex(v1v0));
    Vertex v1(toVertex(v0v1));
    Vertex vv, vl, vr;
    Halfedge h1, h2;

    // the edges v1-vl and vl-v0 must not be both boundary edges
    if (!isSurfaceBoundary(v0v1))
    {
        vl = toVertex(nextHalfedge(v0v1));
        h1 = nextHalfedge(v0v1);
        h2 = nextHalfedge(h1);
        if (isSurfaceBoundary(oppositeHalfedge(h1)) &&
            isSurfaceBoundary(oppositeHalfedge(h2)))
            return false;
    }

    // the edges v0-vr and vr-v1 must not be both boundary edges
    if (!isSurfaceBoundary(v1v0))
    {
        vr = toVertex(nextHalfedge(v1v0));
        h1 = nextHalfedge(v1v0);
        h2 = nextHalfedge(h1);
        if (isSurfaceBoundary(oppositeHalfedge(h1)) &&
            isSurfaceBoundary(oppositeHalfedge(h2)))
            return false;
    }

    // if vl and vr are equal or both invalid -> fail
    if (vl == vr)
        return false;

    // edge between two boundary vertices should be a boundary edge
    if (isSurfaceBoundary(v0) && isSurfaceBoundary(v1) &&
        !isSurfaceBoundary(v0v1) && !isSurfaceBoundary(v1v0))
        return false;

    // test intersection of the one-rings of v0 and v1
    VertexAroundVertexCirculator vvit, vvend;
    vvit = vvend = vertices(v0);
    do
    {
        vv = *vvit;
        if (vv != v1 && vv != vl && vv != vr)
            if (findHalfedge(vv, v1).isValid())
                return false;
    } while (++vvit != vvend);

    // passed all tests
    return true;
}

//-----------------------------------------------------------------------------

void SurfaceMesh::collapse(Halfedge h)
{
    Halfedge h0 = h;
    Halfedge h1 = prevHalfedge(h0);
    Halfedge o0 = oppositeHalfedge(h0);
    Halfedge o1 = nextHalfedge(o0);

    // remove edge
    removeEdge(h0);

    // remove loops
    if (nextHalfedge(nextHalfedge(h1)) == h1)
        removeLoop(h1);
    if (nextHalfedge(nextHalfedge(o1)) == o1)
        removeLoop(o1);
}

//-----------------------------------------------------------------------------

void SurfaceMesh::removeEdge(Halfedge h)
{
    Halfedge hn = nextHalfedge(h);
    Halfedge hp = prevHalfedge(h);

    Halfedge o = oppositeHalfedge(h);
    Halfedge on = nextHalfedge(o);
    Halfedge op = prevHalfedge(o);

    Face fh = face(h);
    Face fo = face(o);

    Vertex vh = toVertex(h);
    Vertex vo = toVertex(o);

    // halfedge -> vertex
    HalfedgeAroundVertexCirculator vhit, vhend;
    vhit = vhend = halfedges(vo);
    do
    {
        setVertex(oppositeHalfedge(*vhit), vh);
    } while (++vhit != vhend);

    // halfedge -> halfedge
    setNextHalfedge(hp, hn);
    setNextHalfedge(op, on);

    // face -> halfedge
    if (fh.isValid())
        setHalfedge(fh, hn);
    if (fo.isValid())
        setHalfedge(fo, on);

    // vertex -> halfedge
    if (halfedge(vh) == o)
        setHalfedge(vh, hn);
    adjustOutgoingHalfedge(vh);
    setHalfedge(vo, Halfedge());

    // delete stuff
    m_vdeleted[vo] = true;
    ++m_deletedVertices;
    m_edeleted[edge(h)] = true;
    ++m_deletedEdges;
    m_garbage = true;
}

//-----------------------------------------------------------------------------

void SurfaceMesh::removeLoop(Halfedge h)
{
    Halfedge h0 = h;
    Halfedge h1 = nextHalfedge(h0);

    Halfedge o0 = oppositeHalfedge(h0);
    Halfedge o1 = oppositeHalfedge(h1);

    Vertex v0 = toVertex(h0);
    Vertex v1 = toVertex(h1);

    Face fh = face(h0);
    Face fo = face(o0);

    // is it a loop ?
    assert((nextHalfedge(h1) == h0) && (h1 != o0));

    // halfedge -> halfedge
    setNextHalfedge(h1, nextHalfedge(o0));
    setNextHalfedge(prevHalfedge(o0), h1);

    // halfedge -> face
    setFace(h1, fo);

    // vertex -> halfedge
    setHalfedge(v0, h1);
    adjustOutgoingHalfedge(v0);
    setHalfedge(v1, o1);
    adjustOutgoingHalfedge(v1);

    // face -> halfedge
    if (fo.isValid() && halfedge(fo) == o0)
        setHalfedge(fo, h1);

    // delete stuff
    if (fh.isValid())
    {
        m_fdeleted[fh] = true;
        ++m_deletedFaces;
    }
    m_edeleted[edge(h)] = true;
    ++m_deletedEdges;
    m_garbage = true;
}

//-----------------------------------------------------------------------------

void SurfaceMesh::deleteVertex(Vertex v)
{
    if (isDeleted(v))
        return;

    // collect incident faces
    std::vector<Face> incident_faces;
    incident_faces.reserve(6);

    for (auto f : faces(v))
        incident_faces.push_back(f);

    // delete incident faces
    for (auto f : incident_faces)
        deleteFace(f);

    EdgeSet::deleteVertex(v);
}

//-----------------------------------------------------------------------------

void SurfaceMesh::deleteEdge(Edge e)
{
    if (isDeleted(e))
        return;

    Face f0 = face(halfedge(e, 0));
    Face f1 = face(halfedge(e, 1));

    if (f0.isValid())
        deleteFace(f0);
    if (f1.isValid())
        deleteFace(f1);

    // edge w/o faces: call the parent's delete function
    if (!f0.isValid() && !f1.isValid())
        EdgeSet::deleteEdge(e);
}

//-----------------------------------------------------------------------------

void SurfaceMesh::deleteFace(Face f)
{
    if (m_fdeleted[f])
        return;

    // mark face deleted
    if (!m_fdeleted[f])
    {
        m_fdeleted[f] = true;
        m_deletedFaces++;
    }

    // boundary edges of face f to be deleted
    std::vector<Edge> deletedEdges;
    deletedEdges.reserve(3);

    // vertices of face f for updating their outgoing halfedge
    std::vector<Vertex> vertices;
    vertices.reserve(3);

    // for all halfedges of face f do:
    //   1) invalidate face handle.
    //   2) collect all boundary halfedges, set them deleted
    //   3) store vertex handles
    HalfedgeAroundFaceCirculator hc, hcend;
    hc = hcend = halfedges(f);

    do
    {
        setFace(*hc, Face());

        if (isSurfaceBoundary(oppositeHalfedge(*hc)))
            deletedEdges.push_back(edge(*hc));

        vertices.push_back(toVertex(*hc));

    } while (++hc != hcend);

    // delete all collected (half)edges
    // delete isolated vertices
    if (!deletedEdges.empty())
    {
        auto delit(deletedEdges.begin()), delend(deletedEdges.end());

        Halfedge h0, h1, next0, next1, prev0, prev1;
        Vertex v0, v1;

        for (; delit != delend; ++delit)
        {
            EdgeSet::deleteEdge(*delit);
        }
    }

    // update outgoing halfedge handles of remaining vertices
    auto vit(vertices.begin()), vend(vertices.end());
    for (; vit != vend; ++vit)
        adjustOutgoingHalfedge(*vit);

    setGarbage();
}

//-----------------------------------------------------------------------------

void SurfaceMesh::beginGarbage()
{
    // delete marked vertices
    EdgeSet::beginGarbage();

    const int nH = m_garbageprops["nH"];

    int i, i0, i1, nF(facesSize());

    Face f;
    Halfedge h;

    // setup handle mapping
    FaceProperty<Face> fmap = addFaceProperty<Face>("f:garbage-collection");
    const HalfedgeProperty<Halfedge> hmap =
        getHalfedgeProperty<Halfedge>("h:garbage-collection");

    for (i = 0; i < nF; ++i)
        fmap[Face(i)] = Face(i);

    // remove deleted faces
    if (nF > 0)
    {
        i0 = 0;
        i1 = nF - 1;

        while (true)
        {
            // find 1st deleted and last un-deleted
            while (!m_fdeleted[Face(i0)] && i0 < i1)
                ++i0;
            while (m_fdeleted[Face(i1)] && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;

            // swap
            m_fprops.swap(i0, i1);
        };

        // remember new size
        nF = m_fdeleted[Face(i0)] ? i0 : i0 + 1;
    }

    // update halfedge connectivity
    for (i = 0; i < nH; ++i)
    {
        h = Halfedge(i);
        if (!isSurfaceBoundary(h))
            setFace(h, fmap[face(h)]);
    }

    // update handles of faces
    for (i = 0; i < nF; ++i)
    {
        f = Face(i);
        setHalfedge(f, hmap[halfedge(f)]);
    }

    m_garbageprops["nF"] = nF;
}

//-----------------------------------------------------------------------------

void SurfaceMesh::finalizeGarbage()
{
    FaceProperty<Face> fmap = getFaceProperty<Face>("f:garbage-collection");

    // remove handle maps
    removeFaceProperty(fmap);

    // finally resize arrays
    m_fprops.resize(m_garbageprops["nF"]);
    m_fprops.freeMemory();

    m_deletedFaces = 0;

    EdgeSet::finalizeGarbage();
}

//=============================================================================
} // namespace pmp
//=============================================================================
