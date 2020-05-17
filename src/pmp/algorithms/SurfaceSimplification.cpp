// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/SurfaceSimplification.h"

#include <iterator>
#include <limits>

#include "pmp/algorithms/DistancePointTriangle.h"
#include "pmp/algorithms/SurfaceNormals.h"

namespace pmp {

SurfaceSimplification::SurfaceSimplification(SurfaceMesh& mesh)
    : mesh_(mesh), initialized_(false), queue_(nullptr)

{
    aspect_ratio_ = 0;
    edge_length_ = 0;
    max_valence_ = 0;
    normal_deviation_ = 0;
    hausdorff_error_ = 0;

    // add properties
    vquadric_ = mesh_.add_vertex_property<Quadric>("v:quadric");

    // get properties
    vpoint_ = mesh_.vertex_property<Point>("v:point");

    // compute face normals
    SurfaceNormals::compute_face_normals(mesh_);
    fnormal_ = mesh_.face_property<Normal>("f:normal");
}

SurfaceSimplification::~SurfaceSimplification()
{
    // remove added properties
    mesh_.remove_vertex_property(vquadric_);
    mesh_.remove_face_property(normal_cone_);
    mesh_.remove_face_property(face_points_);
}

void SurfaceSimplification::initialize(Scalar aspect_ratio, Scalar edge_length,
                                       unsigned int max_valence,
                                       Scalar normal_deviation,
                                       Scalar hausdorff_error)
{
    if (!mesh_.is_triangle_mesh())
        return;

    // store parameters
    aspect_ratio_ = aspect_ratio;
    max_valence_ = max_valence;
    edge_length_ = edge_length;
    normal_deviation_ = normal_deviation / 180.0 * M_PI;
    hausdorff_error_ = hausdorff_error;

    // properties
    if (normal_deviation_ > 0.0)
        normal_cone_ = mesh_.face_property<NormalCone>("f:normalCone");
    else
        mesh_.remove_face_property(normal_cone_);
    if (hausdorff_error > 0.0)
        face_points_ = mesh_.face_property<Points>("f:points");
    else
        mesh_.remove_face_property(face_points_);

    // vertex selection
    has_selection_ = false;
    vselected_ = mesh_.get_vertex_property<bool>("v:selected");
    if (vselected_)
    {
        for (auto v : mesh_.vertices())
        {
            if (vselected_[v])
            {
                has_selection_ = true;
                break;
            }
        }
    }

    // feature vertices/edges
    has_features_ = false;
    vfeature_ = mesh_.get_vertex_property<bool>("v:feature");
    efeature_ = mesh_.get_edge_property<bool>("e:feature");
    if (vfeature_ && efeature_)
    {
        for (auto v : mesh_.vertices())
        {
            if (vfeature_[v])
            {
                has_features_ = true;
                break;
            }
        }
    }

    // initialize quadrics
    for (auto v : mesh_.vertices())
    {
        vquadric_[v].clear();

        if (!mesh_.is_isolated(v))
        {
            for (auto f : mesh_.faces(v))
            {
                vquadric_[v] += Quadric(fnormal_[f], vpoint_[v]);
            }
        }
    }

    // initialize normal cones
    if (normal_deviation_)
    {
        for (auto f : mesh_.faces())
        {
            normal_cone_[f] = NormalCone(fnormal_[f]);
        }
    }

    // initialize faces' point list
    if (hausdorff_error_)
    {
        for (auto f : mesh_.faces())
        {
            Points().swap(face_points_[f]); // free mem
        }
    }

    initialized_ = true;
}

void SurfaceSimplification::simplify(unsigned int n_vertices)
{
    if (!mesh_.is_triangle_mesh())
    {
        std::cerr << "Not a triangle mesh!" << std::endl;
        return;
    }

    // make sure the decimater is initialized
    if (!initialized_)
        initialize();

    unsigned int nv(mesh_.n_vertices());

    std::vector<Vertex> one_ring;
    std::vector<Vertex>::iterator or_it, or_end;
    Halfedge h;
    Vertex v;

    // add properties for priority queue
    vpriority_ = mesh_.add_vertex_property<float>("v:prio");
    heap_pos_ = mesh_.add_vertex_property<int>("v:heap");
    vtarget_ = mesh_.add_vertex_property<Halfedge>("v:target");

    // build priority queue
    HeapInterface hi(vpriority_, heap_pos_);
    queue_ = new PriorityQueue(hi);
    queue_->reserve(mesh_.n_vertices());
    for (auto v : mesh_.vertices())
    {
        queue_->reset_heap_position(v);
        enqueue_vertex(v);
    }

    while (nv > n_vertices && !queue_->empty())
    {
        // get 1st element
        v = queue_->front();
        queue_->pop_front();
        h = vtarget_[v];
        CollapseData cd(mesh_, h);

        // check this (again)
        if (!mesh_.is_collapse_ok(h))
            continue;

        // store one-ring
        one_ring.clear();
        for (auto vv : mesh_.vertices(cd.v0))
        {
            one_ring.push_back(vv);
        }

        // perform collapse
        mesh_.collapse(h);
        --nv;
        //if (nv % 1000 == 0) std::cerr << nv << "\r";

        // postprocessing, e.g., update quadrics
        postprocess_collapse(cd);

        // update queue
        for (or_it = one_ring.begin(), or_end = one_ring.end(); or_it != or_end;
             ++or_it)
            enqueue_vertex(*or_it);
    }

    // clean up
    delete queue_;
    mesh_.garbage_collection();
    mesh_.remove_vertex_property(vpriority_);
    mesh_.remove_vertex_property(heap_pos_);
    mesh_.remove_vertex_property(vtarget_);
}

void SurfaceSimplification::enqueue_vertex(Vertex v)
{
    float prio, min_prio(std::numeric_limits<float>::max());
    Halfedge min_h;

    // find best out-going halfedge
    for (auto h : mesh_.halfedges(v))
    {
        CollapseData cd(mesh_, h);
        if (is_collapse_legal(cd))
        {
            prio = priority(cd);
            if (prio != -1.0 && prio < min_prio)
            {
                min_prio = prio;
                min_h = h;
            }
        }
    }

    // target found -> put vertex on heap
    if (min_h.is_valid())
    {
        vpriority_[v] = min_prio;
        vtarget_[v] = min_h;

        if (queue_->is_stored(v))
            queue_->update(v);
        else
            queue_->insert(v);
    }

    // not valid -> remove from heap
    else
    {
        if (queue_->is_stored(v))
            queue_->remove(v);

        vpriority_[v] = -1;
        vtarget_[v] = min_h;
    }
}

bool SurfaceSimplification::is_collapse_legal(const CollapseData& cd)
{
    // test selected vertices
    if (has_selection_)
    {
        if (!vselected_[cd.v0])
            return false;
    }

    // test features
    if (has_features_)
    {
        if (vfeature_[cd.v0] && !efeature_[mesh_.edge(cd.v0v1)])
            return false;

        if (cd.vl.is_valid() && efeature_[mesh_.edge(cd.vlv0)])
            return false;

        if (cd.vr.is_valid() && efeature_[mesh_.edge(cd.v0vr)])
            return false;
    }

    // do not collapse boundary vertices to interior vertices
    if (mesh_.is_boundary(cd.v0) && !mesh_.is_boundary(cd.v1))
        return false;

    // there should be at least 2 incident faces at v0
    if (mesh_.cw_rotated_halfedge(mesh_.cw_rotated_halfedge(cd.v0v1)) ==
        cd.v0v1)
        return false;

    // topological check
    if (!mesh_.is_collapse_ok(cd.v0v1))
        return false;

    // check maximal valence
    if (max_valence_ > 0)
    {
        unsigned int val0 = mesh_.valence(cd.v0);
        unsigned int val1 = mesh_.valence(cd.v1);
        unsigned int val = val0 + val1 - 1;
        if (cd.fl.is_valid())
            --val;
        if (cd.fr.is_valid())
            --val;
        if (val > max_valence_ && !(val < std::max(val0, val1)))
            return false;
    }

    // remember the positions of the endpoints
    const Point p0 = vpoint_[cd.v0];
    const Point p1 = vpoint_[cd.v1];

    // check for maximum edge length
    if (edge_length_)
    {
        for (auto v : mesh_.vertices(cd.v0))
        {
            if (v != cd.v1 && v != cd.vl && v != cd.vr)
            {
                if (norm(vpoint_[v] - p1) > edge_length_)
                    return false;
            }
        }
    }

    // check for flipping normals
    if (normal_deviation_ == 0.0)
    {
        vpoint_[cd.v0] = p1;
        for (auto f : mesh_.faces(cd.v0))
        {
            if (f != cd.fl && f != cd.fr)
            {
                Normal n0 = fnormal_[f];
                Normal n1 = SurfaceNormals::compute_face_normal(mesh_, f);
                if (dot(n0, n1) < 0.0)
                {
                    vpoint_[cd.v0] = p0;
                    return false;
                }
            }
        }
        vpoint_[cd.v0] = p0;
    }

    // check normal cone
    else
    {
        vpoint_[cd.v0] = p1;

        Face fll, frr;
        if (cd.vl.is_valid())
            fll = mesh_.face(
                mesh_.opposite_halfedge(mesh_.prev_halfedge(cd.v0v1)));
        if (cd.vr.is_valid())
            frr = mesh_.face(
                mesh_.opposite_halfedge(mesh_.next_halfedge(cd.v1v0)));

        for (auto f : mesh_.faces(cd.v0))
        {
            if (f != cd.fl && f != cd.fr)
            {
                NormalCone nc = normal_cone_[f];
                nc.merge(SurfaceNormals::compute_face_normal(mesh_, f));

                if (f == fll)
                    nc.merge(normal_cone_[cd.fl]);
                if (f == frr)
                    nc.merge(normal_cone_[cd.fr]);

                if (nc.angle() > 0.5 * normal_deviation_)
                {
                    vpoint_[cd.v0] = p0;
                    return false;
                }
            }
        }

        vpoint_[cd.v0] = p0;
    }

    // check aspect ratio
    if (aspect_ratio_)
    {
        Scalar ar0(0), ar1(0);

        for (auto f : mesh_.faces(cd.v0))
        {
            if (f != cd.fl && f != cd.fr)
            {
                // worst aspect ratio after collapse
                vpoint_[cd.v0] = p1;
                ar1 = std::max(ar1, aspect_ratio(f));
                // worst aspect ratio before collapse
                vpoint_[cd.v0] = p0;
                ar0 = std::max(ar0, aspect_ratio(f));
            }
        }

        // aspect ratio is too bad, and it does also not improve
        if (ar1 > aspect_ratio_ && ar1 > ar0)
            return false;
    }

    // check Hausdorff error
    if (hausdorff_error_)
    {
        Points points;
        bool ok;

        // collect points to be tested
        for (auto f : mesh_.faces(cd.v0))
        {
            std::copy(face_points_[f].begin(), face_points_[f].end(),
                      std::back_inserter(points));
        }
        points.push_back(vpoint_[cd.v0]);

        // test points against all faces
        vpoint_[cd.v0] = p1;
        for (auto point : points)
        {
            ok = false;

            for (auto f : mesh_.faces(cd.v0))
            {
                if (f != cd.fl && f != cd.fr)
                {
                    if (distance(f, point) < hausdorff_error_)
                    {
                        ok = true;
                        break;
                    }
                }
            }

            if (!ok)
            {
                vpoint_[cd.v0] = p0;
                return false;
            }
        }
        vpoint_[cd.v0] = p0;
    }

    // collapse passed all tests -> ok
    return true;
}

float SurfaceSimplification::priority(const CollapseData& cd)
{
    // computer quadric error metric
    Quadric Q = vquadric_[cd.v0];
    Q += vquadric_[cd.v1];
    return Q(vpoint_[cd.v1]);
}

void SurfaceSimplification::postprocess_collapse(const CollapseData& cd)
{
    // update error quadrics
    vquadric_[cd.v1] += vquadric_[cd.v0];

    // update normal cones
    if (normal_deviation_)
    {
        for (auto f : mesh_.faces(cd.v1))
        {
            normal_cone_[f].merge(
                SurfaceNormals::compute_face_normal(mesh_, f));
        }

        if (cd.vl.is_valid())
        {
            Face f = mesh_.face(cd.v1vl);
            if (f.is_valid())
                normal_cone_[f].merge(normal_cone_[cd.fl]);
        }

        if (cd.vr.is_valid())
        {
            Face f = mesh_.face(cd.vrv1);
            if (f.is_valid())
                normal_cone_[f].merge(normal_cone_[cd.fr]);
        }
    }

    // update Hausdorff error
    if (hausdorff_error_)
    {
        Points points;

        // collect points to be distributed

        // points of v1's one-ring
        for (auto f : mesh_.faces(cd.v1))
        {
            std::copy(face_points_[f].begin(), face_points_[f].end(),
                      std::back_inserter(points));
            face_points_[f].clear();
        }

        // points of the 2 removed triangles
        if (cd.fl.is_valid())
        {
            std::copy(face_points_[cd.fl].begin(), face_points_[cd.fl].end(),
                      std::back_inserter(points));
            Points().swap(face_points_[cd.fl]); // free mem
        }
        if (cd.fr.is_valid())
        {
            std::copy(face_points_[cd.fr].begin(), face_points_[cd.fr].end(),
                      std::back_inserter(points));
            Points().swap(face_points_[cd.fr]); // free mem
        }

        // the removed vertex
        points.push_back(vpoint_[cd.v0]);

        // test points against all faces
        Scalar d, dd;
        Face ff;

        for (auto point : points)
        {
            dd = std::numeric_limits<Scalar>::max();

            for (auto f : mesh_.faces(cd.v1))
            {
                d = distance(f, point);
                if (d < dd)
                {
                    ff = f;
                    dd = d;
                }
            }

            face_points_[ff].push_back(point);
        }
    }
}

Scalar SurfaceSimplification::aspect_ratio(Face f) const
{
    // min height is area/maxLength
    // aspect ratio = length / height
    //              = length * length / area

    SurfaceMesh::VertexAroundFaceCirculator fvit = mesh_.vertices(f);

    const Point p0 = vpoint_[*fvit];
    const Point p1 = vpoint_[*(++fvit)];
    const Point p2 = vpoint_[*(++fvit)];

    const Point d0 = p0 - p1;
    const Point d1 = p1 - p2;
    const Point d2 = p2 - p0;

    const Scalar l0 = sqrnorm(d0);
    const Scalar l1 = sqrnorm(d1);
    const Scalar l2 = sqrnorm(d2);

    // max squared edge length
    const Scalar l = std::max(l0, std::max(l1, l2));

    // triangle area
    Scalar a = norm(cross(d0, d1));

    return l / a;
}

Scalar SurfaceSimplification::distance(Face f, const Point& p) const
{
    SurfaceMesh::VertexAroundFaceCirculator fvit = mesh_.vertices(f);

    const Point p0 = vpoint_[*fvit];
    const Point p1 = vpoint_[*(++fvit)];
    const Point p2 = vpoint_[*(++fvit)];

    Point n;

    return dist_point_triangle(p, p0, p1, p2, n);
}

SurfaceSimplification::CollapseData::CollapseData(SurfaceMesh& sm, Halfedge h)
    : mesh(sm)
{
    v0v1 = h;
    v1v0 = mesh.opposite_halfedge(v0v1);
    v0 = mesh.to_vertex(v1v0);
    v1 = mesh.to_vertex(v0v1);
    fl = mesh.face(v0v1);
    fr = mesh.face(v1v0);

    // get vl
    if (fl.is_valid())
    {
        v1vl = mesh.next_halfedge(v0v1);
        vlv0 = mesh.next_halfedge(v1vl);
        vl = mesh.to_vertex(v1vl);
    }

    // get vr
    if (fr.is_valid())
    {
        v0vr = mesh.next_halfedge(v1v0);
        vrv1 = mesh.prev_halfedge(v0vr);
        vr = mesh.from_vertex(vrv1);
    }
}

} // namespace pmp
