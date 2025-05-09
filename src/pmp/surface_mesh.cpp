// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/surface_mesh.h"

namespace pmp {

SurfaceMesh::SurfaceMesh()
{
    // allocate standard properties
    // same list is used in operator=() and assign()
    vpoint_ = add_vertex_property<Point>("v:point");
    vconn_ = add_vertex_property<VertexConnectivity>("v:connectivity");
    hconn_ = add_halfedge_property<HalfedgeConnectivity>("h:connectivity");
    fconn_ = add_face_property<FaceConnectivity>("f:connectivity");

    vdeleted_ = add_vertex_property<bool>("v:deleted", false);
    edeleted_ = add_edge_property<bool>("e:deleted", false);
    fdeleted_ = add_face_property<bool>("f:deleted", false);
}

SurfaceMesh::~SurfaceMesh() = default;

SurfaceMesh& SurfaceMesh::operator=(const SurfaceMesh& rhs)
{
    if (this != &rhs)
    {
        // deep copy of property containers
        vprops_ = rhs.vprops_;
        hprops_ = rhs.hprops_;
        eprops_ = rhs.eprops_;
        fprops_ = rhs.fprops_;

        // property handles contain pointers, have to be reassigned
        vpoint_ = vertex_property<Point>("v:point");
        vconn_ = vertex_property<VertexConnectivity>("v:connectivity");
        hconn_ = halfedge_property<HalfedgeConnectivity>("h:connectivity");
        fconn_ = face_property<FaceConnectivity>("f:connectivity");

        vdeleted_ = vertex_property<bool>("v:deleted");
        edeleted_ = edge_property<bool>("e:deleted");
        fdeleted_ = face_property<bool>("f:deleted");

        // how many elements are deleted?
        deleted_vertices_ = rhs.deleted_vertices_;
        deleted_edges_ = rhs.deleted_edges_;
        deleted_faces_ = rhs.deleted_faces_;

        unique_vertices_set_ = rhs.unique_vertices_set_;

        has_garbage_ = rhs.has_garbage_;
    }

    return *this;
}

SurfaceMesh& SurfaceMesh::assign(const SurfaceMesh& rhs)
{
    if (this != &rhs)
    {
        // clear properties
        vprops_.clear();
        hprops_.clear();
        eprops_.clear();
        fprops_.clear();

        // allocate standard properties
        vpoint_ = add_vertex_property<Point>("v:point");
        vconn_ = add_vertex_property<VertexConnectivity>("v:connectivity");
        hconn_ = add_halfedge_property<HalfedgeConnectivity>("h:connectivity");
        fconn_ = add_face_property<FaceConnectivity>("f:connectivity");

        vdeleted_ = add_vertex_property<bool>("v:deleted", false);
        edeleted_ = add_edge_property<bool>("e:deleted", false);
        fdeleted_ = add_face_property<bool>("f:deleted", false);

        // copy properties from other mesh
        vpoint_.array() = rhs.vpoint_.array();
        vconn_.array() = rhs.vconn_.array();
        hconn_.array() = rhs.hconn_.array();
        fconn_.array() = rhs.fconn_.array();

        vdeleted_.array() = rhs.vdeleted_.array();
        edeleted_.array() = rhs.edeleted_.array();
        fdeleted_.array() = rhs.fdeleted_.array();

        unique_vertices_set_ = rhs.unique_vertices_set_;

        // resize (needed by property containers)
        vprops_.resize(rhs.vertices_size());
        hprops_.resize(rhs.halfedges_size());
        eprops_.resize(rhs.edges_size());
        fprops_.resize(rhs.faces_size());

        // how many elements are deleted?
        deleted_vertices_ = rhs.deleted_vertices_;
        deleted_edges_ = rhs.deleted_edges_;
        deleted_faces_ = rhs.deleted_faces_;
        has_garbage_ = rhs.has_garbage_;
    }

    return *this;
}

void SurfaceMesh::clear()
{
    // remove all properties
    vprops_.clear();
    hprops_.clear();
    eprops_.clear();
    fprops_.clear();

    // really free their memory
    free_memory();

    // add the standard properties back
    vpoint_ = add_vertex_property<Point>("v:point");
    vconn_ = add_vertex_property<VertexConnectivity>("v:connectivity");
    hconn_ = add_halfedge_property<HalfedgeConnectivity>("h:connectivity");
    fconn_ = add_face_property<FaceConnectivity>("f:connectivity");
    vdeleted_ = add_vertex_property<bool>("v:deleted", false);
    edeleted_ = add_edge_property<bool>("e:deleted", false);
    fdeleted_ = add_face_property<bool>("f:deleted", false);

    unique_vertices_set_.clear();

    // set initial status (as in constructor)
    deleted_vertices_ = 0;
    deleted_edges_ = 0;
    deleted_faces_ = 0;
    has_garbage_ = false;
}

void SurfaceMesh::free_memory()
{
    vprops_.free_memory();
    hprops_.free_memory();
    eprops_.free_memory();
    fprops_.free_memory();
}

void SurfaceMesh::reserve(size_t nvertices, size_t nedges, size_t nfaces)
{
    vprops_.reserve(nvertices);
    hprops_.reserve(2 * nedges);
    eprops_.reserve(nedges);
    fprops_.reserve(nfaces);
}

Halfedge SurfaceMesh::find_halfedge(Vertex start, Vertex end) const
{
    assert(is_valid(start) && is_valid(end));

    Halfedge h = halfedge(start);
    const Halfedge hh = h;

    if (h.is_valid())
    {
        do
        {
            if (to_vertex(h) == end)
                return h;
            h = cw_rotated_halfedge(h);
        } while (h != hh);
    }

    return {};
}

Edge SurfaceMesh::find_edge(Vertex a, Vertex b) const
{
    const Halfedge h = find_halfedge(a, b);
    return h.is_valid() ? edge(h) : Edge();
}

void SurfaceMesh::adjust_outgoing_halfedge(Vertex v)
{
    Halfedge h = halfedge(v);
    const Halfedge hh = h;

    if (h.is_valid())
    {
        do
        {
            if (is_boundary(h))
            {
                set_halfedge(v, h);
                return;
            }
            h = cw_rotated_halfedge(h);
        } while (h != hh);
    }
}

Vertex SurfaceMesh::add_vertex(const Point& p)
{
    Vertex v = new_vertex();
    if (v.is_valid())
        vpoint_[v] = p;
    return v;
}

Vertex SurfaceMesh::add_vertex_unique(const Point& p)
{
    Vertex v;
    unique_vertices_point_ = p;
    auto it = unique_vertices_set_.lower_bound(Vertex());

    if (it == unique_vertices_set_.end() ||
        p != position(*it) || is_deleted(*it))
    {
        v = add_vertex(p);  // unique point
        unique_vertices_set_.insert(it, v);
    }
    else
        v = *it;  // duplicate
    return v;
}

Face SurfaceMesh::add_triangle(Vertex v0, Vertex v1, Vertex v2)
{
    add_face_vertices_.resize(3);
    add_face_vertices_[0] = v0;
    add_face_vertices_[1] = v1;
    add_face_vertices_[2] = v2;
    return add_face(add_face_vertices_);
}

Face SurfaceMesh::add_quad(Vertex v0, Vertex v1, Vertex v2, Vertex v3)
{
    add_face_vertices_.resize(4);
    add_face_vertices_[0] = v0;
    add_face_vertices_[1] = v1;
    add_face_vertices_[2] = v2;
    add_face_vertices_[3] = v3;
    return add_face(add_face_vertices_);
}

Face SurfaceMesh::add_face(const std::vector<Vertex>& vertices)
{
    const size_t n(vertices.size());
    assert(n > 2);

    Vertex v;
    size_t i, ii, id;
    Halfedge inner_next, inner_prev, outer_next, outer_prev, boundary_next,
        boundary_prev, patch_start, patch_end;

    // use global arrays to avoid new/delete of local arrays!!!
    std::vector<Halfedge>& halfedges = add_face_halfedges_;
    std::vector<bool>& is_new = add_face_is_new_;
    std::vector<bool>& needs_adjust = add_face_needs_adjust_;
    NextCache& next_cache = add_face_next_cache_;
    halfedges.clear();
    halfedges.resize(n);
    is_new.clear();
    is_new.resize(n);
    needs_adjust.clear();
    needs_adjust.resize(n, false);
    next_cache.clear();
    next_cache.reserve(3 * n);

    // test for topological errors
    for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
    {
        if (!is_boundary(vertices[i]))
        {
            auto what = "SurfaceMesh::add_face: Complex vertex.";
            throw TopologyException(what);
        }

        halfedges[i] = find_halfedge(vertices[i], vertices[ii]);
        is_new[i] = !halfedges[i].is_valid();

        if (!is_new[i] && !is_boundary(halfedges[i]))
        {
            auto what = "SurfaceMesh::add_face: Complex edge.";
            throw TopologyException(what);
        }
    }

    // re-link patches if necessary
    for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
    {
        if (!is_new[i] && !is_new[ii])
        {
            inner_prev = halfedges[i];
            inner_next = halfedges[ii];

            if (next_halfedge(inner_prev) != inner_next)
            {
                // here comes the ugly part... we have to relink a whole patch

                // search a free gap
                // free gap will be between boundaryPrev and boundaryNext
                outer_prev = opposite_halfedge(inner_next);
                outer_next = opposite_halfedge(inner_prev);
                boundary_prev = outer_prev;
                do
                {
                    boundary_prev =
                        opposite_halfedge(next_halfedge(boundary_prev));
                } while (!is_boundary(boundary_prev) ||
                         boundary_prev == inner_prev);
                boundary_next = next_halfedge(boundary_prev);
                assert(is_boundary(boundary_prev));
                assert(is_boundary(boundary_next));

                // ok ?
                if (boundary_next == inner_next)
                {
                    auto what =
                        "SurfaceMesh::add_face: Patch re-linking failed.";
                    throw TopologyException(what);
                }

                // other halfedges' handles
                patch_start = next_halfedge(inner_prev);
                patch_end = prev_halfedge(inner_next);

                // relink
                next_cache.emplace_back(boundary_prev, patch_start);
                next_cache.emplace_back(patch_end, boundary_next);
                next_cache.emplace_back(inner_prev, inner_next);
            }
        }
    }

    // create missing edges
    for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
    {
        if (is_new[i])
        {
            halfedges[i] = new_edge(vertices[i], vertices[ii]);
        }
    }

    // create the face
    Face f(new_face());
    set_halfedge(f, halfedges[n - 1]);

    // setup halfedges
    for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
    {
        v = vertices[ii];
        inner_prev = halfedges[i];
        inner_next = halfedges[ii];

        id = 0;
        if (is_new[i])
            id |= 1;
        if (is_new[ii])
            id |= 2;

        if (id)
        {
            outer_prev = opposite_halfedge(inner_next);
            outer_next = opposite_halfedge(inner_prev);

            // set outer links
            switch (id)
            {
                case 1: // prev is new, next is old
                    boundary_prev = prev_halfedge(inner_next);
                    next_cache.emplace_back(boundary_prev, outer_next);
                    set_halfedge(v, outer_next);
                    break;

                case 2: // next is new, prev is old
                    boundary_next = next_halfedge(inner_prev);
                    next_cache.emplace_back(outer_prev, boundary_next);
                    set_halfedge(v, boundary_next);
                    break;

                case 3: // both are new
                    if (!halfedge(v).is_valid())
                    {
                        set_halfedge(v, outer_next);
                        next_cache.emplace_back(outer_prev, outer_next);
                    }
                    else
                    {
                        boundary_next = halfedge(v);
                        boundary_prev = prev_halfedge(boundary_next);
                        next_cache.emplace_back(boundary_prev, outer_next);
                        next_cache.emplace_back(outer_prev, boundary_next);
                    }
                    break;
            }

            // set inner link
            next_cache.emplace_back(inner_prev, inner_next);
        }
        else
            needs_adjust[ii] = (halfedge(v) == inner_next);

        // set face handle
        set_face(halfedges[i], f);
    }

    // process next halfedge cache
    for (const auto& [first, second] : next_cache)
        set_next_halfedge(first, second);

    // adjust vertices' halfedge handle
    for (i = 0; i < n; ++i)
    {
        if (needs_adjust[i])
        {
            adjust_outgoing_halfedge(vertices[i]);
        }
    }

    return f;
}

size_t SurfaceMesh::valence(Vertex v) const
{
    auto vv = vertices(v);
    return std::distance(vv.begin(), vv.end());
}

size_t SurfaceMesh::valence(Face f) const
{
    auto vv = vertices(f);
    return std::distance(vv.begin(), vv.end());
}

bool SurfaceMesh::is_triangle_mesh() const
{
    for (auto f : faces())
        if (valence(f) != 3)
            return false;

    return true;
}

bool SurfaceMesh::is_quad_mesh() const
{
    for (auto f : faces())
        if (valence(f) != 4)
            return false;

    return true;
}

void SurfaceMesh::split(Face f, Vertex v)
{
    // Split an arbitrary face into triangles by connecting each vertex of face
    // f to vertex v . Face f will remain valid (it will become one of the
    // triangles). The halfedge handles of the new triangles will point to the
    // old halfedges.

    const Halfedge hend = halfedge(f);
    Halfedge h = next_halfedge(hend);

    Halfedge hold = new_edge(to_vertex(hend), v);

    set_next_halfedge(hend, hold);
    set_face(hold, f);

    hold = opposite_halfedge(hold);

    while (h != hend)
    {
        const Halfedge hnext = next_halfedge(h);

        const Face fnew = new_face();
        set_halfedge(fnew, h);

        const Halfedge hnew = new_edge(to_vertex(h), v);

        set_next_halfedge(hnew, hold);
        set_next_halfedge(hold, h);
        set_next_halfedge(h, hnew);

        set_face(hnew, fnew);
        set_face(hold, fnew);
        set_face(h, fnew);

        hold = opposite_halfedge(hnew);

        h = hnext;
    }

    set_next_halfedge(hold, hend);
    set_next_halfedge(next_halfedge(hend), hold);

    set_face(hold, f);

    set_halfedge(v, hold);
}

Halfedge SurfaceMesh::split(Edge e, Vertex v)
{
    const Halfedge h0 = halfedge(e, 0);
    const Halfedge o0 = halfedge(e, 1);

    const Vertex v2 = to_vertex(o0);

    const Halfedge e1 = new_edge(v, v2);
    Halfedge t1 = opposite_halfedge(e1);

    const Face f0 = face(h0);
    const Face f3 = face(o0);

    set_halfedge(v, h0);
    set_vertex(o0, v);

    if (!is_boundary(h0))
    {
        const Halfedge h1 = next_halfedge(h0);
        const Halfedge h2 = next_halfedge(h1);

        const Vertex v1 = to_vertex(h1);

        const Halfedge e0 = new_edge(v, v1);
        const Halfedge t0 = opposite_halfedge(e0);

        const Face f1 = new_face();
        set_halfedge(f0, h0);
        set_halfedge(f1, h2);

        set_face(h1, f0);
        set_face(t0, f0);
        set_face(h0, f0);

        set_face(h2, f1);
        set_face(t1, f1);
        set_face(e0, f1);

        set_next_halfedge(h0, h1);
        set_next_halfedge(h1, t0);
        set_next_halfedge(t0, h0);

        set_next_halfedge(e0, h2);
        set_next_halfedge(h2, t1);
        set_next_halfedge(t1, e0);
    }
    else
    {
        set_next_halfedge(prev_halfedge(h0), t1);
        set_next_halfedge(t1, h0);
        // halfedge handle of vh already is h0
    }

    if (!is_boundary(o0))
    {
        const Halfedge o1 = next_halfedge(o0);
        const Halfedge o2 = next_halfedge(o1);

        const Vertex v3 = to_vertex(o1);

        const Halfedge e2 = new_edge(v, v3);
        const Halfedge t2 = opposite_halfedge(e2);

        const Face f2 = new_face();
        set_halfedge(f2, o1);
        set_halfedge(f3, o0);

        set_face(o1, f2);
        set_face(t2, f2);
        set_face(e1, f2);

        set_face(o2, f3);
        set_face(o0, f3);
        set_face(e2, f3);

        set_next_halfedge(e1, o1);
        set_next_halfedge(o1, t2);
        set_next_halfedge(t2, e1);

        set_next_halfedge(o0, e2);
        set_next_halfedge(e2, o2);
        set_next_halfedge(o2, o0);
    }
    else
    {
        set_next_halfedge(e1, next_halfedge(o0));
        set_next_halfedge(o0, e1);
        set_halfedge(v, e1);
    }

    if (halfedge(v2) == h0)
        set_halfedge(v2, t1);

    return t1;
}

Halfedge SurfaceMesh::insert_vertex(Halfedge h0, Vertex v)
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

    const Halfedge h2 = next_halfedge(h0);
    const Halfedge o0 = opposite_halfedge(h0);
    const Halfedge o2 = prev_halfedge(o0);
    const Vertex v2 = to_vertex(h0);
    const Face fh = face(h0);
    const Face fo = face(o0);

    const Halfedge h1 = new_edge(v, v2);
    Halfedge o1 = opposite_halfedge(h1);

    // adjust halfedge connectivity
    set_next_halfedge(h1, h2);
    set_next_halfedge(h0, h1);
    set_vertex(h0, v);
    set_vertex(h1, v2);
    set_face(h1, fh);

    set_next_halfedge(o1, o0);
    set_next_halfedge(o2, o1);
    set_vertex(o1, v);
    set_face(o1, fo);

    // adjust vertex connectivity
    set_halfedge(v2, o1);
    adjust_outgoing_halfedge(v2);
    set_halfedge(v, h1);
    adjust_outgoing_halfedge(v);

    // adjust face connectivity
    if (fh.is_valid())
        set_halfedge(fh, h0);
    if (fo.is_valid())
        set_halfedge(fo, o1);

    return o1;
}

Halfedge SurfaceMesh::insert_edge(Halfedge h0, Halfedge h1)
{
    assert(face(h0) == face(h1));
    assert(face(h0).is_valid());

    const Vertex v0 = to_vertex(h0);
    const Vertex v1 = to_vertex(h1);

    const Halfedge h2 = next_halfedge(h0);
    const Halfedge h3 = next_halfedge(h1);

    Halfedge h4 = new_edge(v0, v1);
    const Halfedge h5 = opposite_halfedge(h4);

    const Face f0 = face(h0);
    const Face f1 = new_face();

    set_halfedge(f0, h0);
    set_halfedge(f1, h1);

    set_next_halfedge(h0, h4);
    set_next_halfedge(h4, h3);
    set_face(h4, f0);

    set_next_halfedge(h1, h5);
    set_next_halfedge(h5, h2);
    Halfedge h = h2;
    do
    {
        set_face(h, f1);
        h = next_halfedge(h);
    } while (h != h2);

    return h4;
}

bool SurfaceMesh::is_flip_ok(Edge e) const
{
    // boundary edges cannot be flipped
    if (is_boundary(e))
        return false;

    // check if the flipped edge is already present in the mesh
    const Halfedge h0 = halfedge(e, 0);
    const Halfedge h1 = halfedge(e, 1);

    const Vertex v0 = to_vertex(next_halfedge(h0));
    const Vertex v1 = to_vertex(next_halfedge(h1));

    if (v0 == v1) // this is generally a bad sign !!!
        return false;

    if (find_halfedge(v0, v1).is_valid())
        return false;

    return true;
}

void SurfaceMesh::flip(Edge e)
{
    //let's make it sure it is actually checked
    assert(is_flip_ok(e));

    const Halfedge a0 = halfedge(e, 0);
    const Halfedge b0 = halfedge(e, 1);

    const Halfedge a1 = next_halfedge(a0);
    const Halfedge a2 = next_halfedge(a1);

    const Halfedge b1 = next_halfedge(b0);
    const Halfedge b2 = next_halfedge(b1);

    const Vertex va0 = to_vertex(a0);
    const Vertex va1 = to_vertex(a1);

    const Vertex vb0 = to_vertex(b0);
    const Vertex vb1 = to_vertex(b1);

    const Face fa = face(a0);
    const Face fb = face(b0);

    set_vertex(a0, va1);
    set_vertex(b0, vb1);

    set_next_halfedge(a0, a2);
    set_next_halfedge(a2, b1);
    set_next_halfedge(b1, a0);

    set_next_halfedge(b0, b2);
    set_next_halfedge(b2, a1);
    set_next_halfedge(a1, b0);

    set_face(a1, fb);
    set_face(b1, fa);

    set_halfedge(fa, a0);
    set_halfedge(fb, b0);

    if (halfedge(va0) == b0)
        set_halfedge(va0, a1);
    if (halfedge(vb0) == a0)
        set_halfedge(vb0, b1);
}

bool SurfaceMesh::is_collapse_ok(Halfedge v0v1) const
{
    const Halfedge v1v0(opposite_halfedge(v0v1));
    const Vertex v0(to_vertex(v1v0));
    const Vertex v1(to_vertex(v0v1));
    Vertex vl, vr;
    Halfedge h1, h2;

    // the edges v1-vl and vl-v0 must not be both boundary edges
    if (!is_boundary(v0v1))
    {
        vl = to_vertex(next_halfedge(v0v1));
        h1 = next_halfedge(v0v1);
        h2 = next_halfedge(h1);
        if (is_boundary(opposite_halfedge(h1)) &&
            is_boundary(opposite_halfedge(h2)))
            return false;
    }

    // the edges v0-vr and vr-v1 must not be both boundary edges
    if (!is_boundary(v1v0))
    {
        vr = to_vertex(next_halfedge(v1v0));
        h1 = next_halfedge(v1v0);
        h2 = next_halfedge(h1);
        if (is_boundary(opposite_halfedge(h1)) &&
            is_boundary(opposite_halfedge(h2)))
            return false;
    }

    // if vl and vr are equal or both invalid -> fail
    if (vl == vr)
        return false;

    // edge between two boundary vertices should be a boundary edge
    if (is_boundary(v0) && is_boundary(v1) && !is_boundary(v0v1) &&
        !is_boundary(v1v0))
        return false;

    // test intersection of the one-rings of v0 and v1
    for (auto vv : vertices(v0))
    {
        if (vv != v1 && vv != vl && vv != vr)
            if (find_halfedge(vv, v1).is_valid())
                return false;
    }

    // passed all tests
    return true;
}

bool SurfaceMesh::is_removal_ok(Edge e) const
{
    const Halfedge h0 = halfedge(e, 0);
    const Halfedge h1 = halfedge(e, 1);
    const Vertex v0 = to_vertex(h0);
    const Vertex v1 = to_vertex(h1);
    const Face f0 = face(h0);
    const Face f1 = face(h1);

    // boundary?
    if (!f0.is_valid() || !f1.is_valid())
        return false;

    // same face?
    if (f0 == f1)
        return false;

    // are the two faces connect through another vertex?
    for (auto v : vertices(f0))
        if (v != v0 && v != v1)
            for (auto f : faces(v))
                if (f == f1)
                    return false;

    return true;
}

bool SurfaceMesh::remove_edge(Edge e)
{
    if (!is_removal_ok(e))
        return false;

    const Halfedge h0 = halfedge(e, 0);
    const Halfedge h1 = halfedge(e, 1);

    const Vertex v0 = to_vertex(h0);
    const Vertex v1 = to_vertex(h1);

    const Face f0 = face(h0);
    const Face f1 = face(h1);

    const Halfedge h0_prev = prev_halfedge(h0);
    const Halfedge h0_next = next_halfedge(h0);
    const Halfedge h1_prev = prev_halfedge(h1);
    const Halfedge h1_next = next_halfedge(h1);

    // adjust vertex->halfedge
    if (halfedge(v0) == h1)
        set_halfedge(v0, h0_next);
    if (halfedge(v1) == h0)
        set_halfedge(v1, h1_next);

    // adjust halfedge->face
    for (auto h : halfedges(f0))
        set_face(h, f1);

    // adjust halfedge->halfedge
    set_next_halfedge(h1_prev, h0_next);
    set_next_halfedge(h0_prev, h1_next);

    // adjust face->halfedge
    if (halfedge(f1) == h1)
        set_halfedge(f1, h1_next);

    // delete face f0 and edge e
    fdeleted_[f0] = true;
    ++deleted_faces_;
    edeleted_[e] = true;
    ++deleted_edges_;
    has_garbage_ = true;

    return true;
}

void SurfaceMesh::collapse(Halfedge h)
{
    const Halfedge h0 = h;
    const Halfedge h1 = prev_halfedge(h0);
    const Halfedge o0 = opposite_halfedge(h0);
    const Halfedge o1 = next_halfedge(o0);

    // remove edge
    remove_edge_helper(h0);

    // remove loops
    if (next_halfedge(next_halfedge(h1)) == h1)
    {
        remove_loop_helper(h1);
    }

    if (next_halfedge(next_halfedge(o1)) == o1)
    {
        remove_loop_helper(o1);
    }
}

void SurfaceMesh::remove_edge_helper(Halfedge h)
{
    const Halfedge hn = next_halfedge(h);
    const Halfedge hp = prev_halfedge(h);

    const Halfedge o = opposite_halfedge(h);
    const Halfedge on = next_halfedge(o);
    const Halfedge op = prev_halfedge(o);

    const Face fh = face(h);
    const Face fo = face(o);

    const Vertex vh = to_vertex(h);
    const Vertex vo = to_vertex(o);

    // halfedge -> vertex
    for (const auto hc : halfedges(vo))
    {
        set_vertex(opposite_halfedge(hc), vh);
    }

    // halfedge -> halfedge
    set_next_halfedge(hp, hn);
    set_next_halfedge(op, on);

    // face -> halfedge
    if (fh.is_valid())
        set_halfedge(fh, hn);
    if (fo.is_valid())
        set_halfedge(fo, on);

    // vertex -> halfedge
    if (halfedge(vh) == o)
        set_halfedge(vh, hn);
    adjust_outgoing_halfedge(vh);
    set_halfedge(vo, Halfedge());

    // delete stuff
    vdeleted_[vo] = true;
    ++deleted_vertices_;
    edeleted_[edge(h)] = true;
    ++deleted_edges_;
    has_garbage_ = true;
}

void SurfaceMesh::remove_loop_helper(Halfedge h)
{
    const Halfedge h0 = h;
    const Halfedge h1 = next_halfedge(h0);

    const Halfedge o0 = opposite_halfedge(h0);
    const Halfedge o1 = opposite_halfedge(h1);

    const Vertex v0 = to_vertex(h0);
    const Vertex v1 = to_vertex(h1);

    const Face fh = face(h0);
    const Face fo = face(o0);

    // is it a loop ?
    assert((next_halfedge(h1) == h0) && (h1 != o0));

    // halfedge -> halfedge
    set_next_halfedge(h1, next_halfedge(o0));
    set_next_halfedge(prev_halfedge(o0), h1);

    // halfedge -> face
    set_face(h1, fo);

    // vertex -> halfedge
    set_halfedge(v0, h1);
    adjust_outgoing_halfedge(v0);
    set_halfedge(v1, o1);
    adjust_outgoing_halfedge(v1);

    // face -> halfedge
    if (fo.is_valid() && halfedge(fo) == o0)
        set_halfedge(fo, h1);

    // delete stuff
    if (fh.is_valid())
    {
        fdeleted_[fh] = true;
        ++deleted_faces_;
    }
    edeleted_[edge(h)] = true;
    ++deleted_edges_;
    has_garbage_ = true;
}

void SurfaceMesh::delete_vertex(Vertex v)
{
    if (is_deleted(v))
        return;

    // collect incident faces
    std::vector<Face> incident_faces;
    incident_faces.reserve(6);

    for (auto f : faces(v))
        incident_faces.push_back(f);

    // delete incident faces
    for (auto f : incident_faces)
        delete_face(f);

    // mark v as deleted if not yet done by delete_face()
    if (!vdeleted_[v])
    {
        vdeleted_[v] = true;
        deleted_vertices_++;
        has_garbage_ = true;
    }
}

void SurfaceMesh::delete_edge(Edge e)
{
    if (is_deleted(e))
        return;

    const Face f0 = face(halfedge(e, 0));
    const Face f1 = face(halfedge(e, 1));

    if (f0.is_valid())
        delete_face(f0);
    if (f1.is_valid())
        delete_face(f1);
}

void SurfaceMesh::delete_face(Face f)
{
    if (fdeleted_[f])
        return;

    // mark face deleted
    if (!fdeleted_[f])
    {
        fdeleted_[f] = true;
        deleted_faces_++;
    }

    // boundary edges of face f to be deleted
    std::vector<Edge> deleted_edges;
    deleted_edges.reserve(3);

    // vertices of face f for updating their outgoing halfedge
    std::vector<Vertex> vertices;
    vertices.reserve(3);

    // for all halfedges of face f do:
    //   1) invalidate face handle.
    //   2) collect all boundary halfedges, set them deleted
    //   3) store vertex handles
    for (auto hc : halfedges(f))
    {
        set_face(hc, Face());

        if (is_boundary(opposite_halfedge(hc)))
            deleted_edges.push_back(edge(hc));

        vertices.push_back(to_vertex(hc));
    }

    // delete all collected (half)edges
    // delete isolated vertices
    if (!deleted_edges.empty())
    {
        for (const auto& e : deleted_edges)
        {
            const auto h0 = halfedge(e, 0);
            const auto v0 = to_vertex(h0);
            const auto next0 = next_halfedge(h0);
            const auto prev0 = prev_halfedge(h0);

            const auto h1 = halfedge(e, 1);
            const auto v1 = to_vertex(h1);
            const auto next1 = next_halfedge(h1);
            const auto prev1 = prev_halfedge(h1);

            // adjust next and prev handles
            set_next_halfedge(prev0, next1);
            set_next_halfedge(prev1, next0);

            // mark edge deleted
            if (!edeleted_[e])
            {
                edeleted_[e] = true;
                deleted_edges_++;
            }

            // update v0
            if (halfedge(v0) == h1)
            {
                if (next0 == h1)
                {
                    if (!vdeleted_[v0])
                    {
                        vdeleted_[v0] = true;
                        deleted_vertices_++;
                    }
                }
                else
                    set_halfedge(v0, next0);
            }

            // update v1
            if (halfedge(v1) == h0)
            {
                if (next1 == h0)
                {
                    if (!vdeleted_[v1])
                    {
                        vdeleted_[v1] = true;
                        deleted_vertices_++;
                    }
                }
                else
                    set_halfedge(v1, next1);
            }
        }
    }

    // update outgoing halfedge handles of remaining vertices
    for (auto v : vertices)
        adjust_outgoing_halfedge(v);

    has_garbage_ = true;
}

void SurfaceMesh::garbage_collection()
{
    if (!has_garbage_)
        return;

    auto nv = vertices_size();
    auto ne = edges_size();
    auto nh = halfedges_size();
    auto nf = faces_size();

    // setup handle mapping
    VertexProperty<Vertex> vmap =
        add_vertex_property<Vertex>("v:garbage-collection");
    HalfedgeProperty<Halfedge> hmap =
        add_halfedge_property<Halfedge>("h:garbage-collection");
    FaceProperty<Face> fmap = add_face_property<Face>("f:garbage-collection");
    for (size_t i = 0; i < nv; ++i)
        vmap[Vertex(i)] = Vertex(i);
    for (size_t i = 0; i < nh; ++i)
        hmap[Halfedge(i)] = Halfedge(i);
    for (size_t i = 0; i < nf; ++i)
        fmap[Face(i)] = Face(i);

    // remove deleted vertices from unique_vertices_set_
    for (auto it = unique_vertices_set_.begin();
             it != unique_vertices_set_.end(); )
    {
        if ((*it).idx() >= nv || is_deleted(*it))
            it = unique_vertices_set_.erase(it);
        else
            ++it;
    }

    // remove deleted vertices
    if (nv > 0)
    {
        size_t i0 = 0;
        size_t i1 = nv - 1;

        while (true)
        {
            // find first deleted and last un-deleted
            while (!vdeleted_[Vertex(i0)] && i0 < i1)
                ++i0;
            while (vdeleted_[Vertex(i1)] && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;

            // swap
            vprops_.swap(i0, i1);
        }

        // remember new size
        nv = vdeleted_[Vertex(i0)] ? i0 : i0 + 1;
    }

    // reload unique vertices
    for (auto it = unique_vertices_set_.begin();
             it != unique_vertices_set_.end(); )
    {
        if ((*it).idx() >= vmap[*it].idx())
        {
            Vertex v = vmap[*it];
            it = unique_vertices_set_.erase(it);
            unique_vertices_set_.insert(it, v);
        }
        else
            ++it;
    }

    // remove deleted edges
    if (ne > 0)
    {
        size_t i0 = 0;
        size_t i1 = ne - 1;

        while (true)
        {
            // find first deleted and last un-deleted
            while (!edeleted_[Edge(i0)] && i0 < i1)
                ++i0;
            while (edeleted_[Edge(i1)] && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;

            // swap
            eprops_.swap(i0, i1);
            hprops_.swap(2 * i0, 2 * i1);
            hprops_.swap(2 * i0 + 1, 2 * i1 + 1);
        }

        // remember new size
        ne = edeleted_[Edge(i0)] ? i0 : i0 + 1;
        nh = 2 * ne;
    }

    // remove deleted faces
    if (nf > 0)
    {
        size_t i0 = 0;
        size_t i1 = nf - 1;

        while (true)
        {
            // find 1st deleted and last un-deleted
            while (!fdeleted_[Face(i0)] && i0 < i1)
                ++i0;
            while (fdeleted_[Face(i1)] && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;

            // swap
            fprops_.swap(i0, i1);
        }

        // remember new size
        nf = fdeleted_[Face(i0)] ? i0 : i0 + 1;
    }

    // update vertex connectivity
    for (size_t i = 0; i < nv; ++i)
    {
        auto v = Vertex(i);
        if (!is_isolated(v))
            set_halfedge(v, hmap[halfedge(v)]);
    }

    // update halfedge connectivity
    for (size_t i = 0; i < nh; ++i)
    {
        auto h = Halfedge(i);
        set_vertex(h, vmap[to_vertex(h)]);
        set_next_halfedge(h, hmap[next_halfedge(h)]);
        if (!is_boundary(h))
            set_face(h, fmap[face(h)]);
    }

    // update handles of faces
    for (size_t i = 0; i < nf; ++i)
    {
        auto f = Face(i);
        set_halfedge(f, hmap[halfedge(f)]);
    }

    // remove handle maps
    remove_vertex_property(vmap);
    remove_halfedge_property(hmap);
    remove_face_property(fmap);

    // finally resize arrays
    vprops_.resize(nv);
    vprops_.free_memory();
    hprops_.resize(nh);
    hprops_.free_memory();
    eprops_.resize(ne);
    eprops_.free_memory();
    fprops_.resize(nf);
    fprops_.free_memory();

    deleted_vertices_ = deleted_edges_ = deleted_faces_ = 0;
    has_garbage_ = false;
}

} // namespace pmp
