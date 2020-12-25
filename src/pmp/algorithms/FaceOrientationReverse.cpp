// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/FaceOrientationReverse.h"

namespace pmp {

FaceOrientationReverse::FaceOrientationReverse(SurfaceMesh& mesh) : mesh_(mesh)
{
}

void FaceOrientationReverse::reverse()
{
    for (auto f : mesh_.faces())
    {
        reverse_halfedge(mesh_.halfedge(f));
    }

    // Fix halfedge on boundary which is parallel to opposite halfedge
    for (auto f : mesh_.faces())
    {
        for (auto h : mesh_.halfedges(f))
        {
            auto oh = mesh_.opposite_halfedge(h);
            if (mesh_.is_boundary(oh) &&
                mesh_.to_vertex(oh) == mesh_.to_vertex(h))
            {
                reverse_halfedge(oh);
            }
        }
    }
}

void FaceOrientationReverse::reverse_halfedge(Halfedge cur) 
{
    if (!cur.is_valid())
        return;

    auto pre = cur;
    auto start = cur;
    auto last = cur;
    cur = mesh_.next_halfedge(cur);
    auto v = mesh_.to_vertex(start);
    while (cur != last)
    {
        auto next_v = mesh_.to_vertex(cur);
        mesh_.set_vertex(cur, v);
        mesh_.set_halfedge(v, pre);
        auto next = mesh_.next_halfedge(cur);
        mesh_.set_next_halfedge(cur, pre);
        pre = cur;
        cur = next;
        v = next_v;
    }
    mesh_.set_vertex(start, v);
    mesh_.set_halfedge(v, pre);
    mesh_.set_next_halfedge(start, pre);
}

} // namespace pmp
