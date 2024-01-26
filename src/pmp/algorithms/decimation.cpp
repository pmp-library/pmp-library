// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/decimation.h"

#include <iterator>
#include <limits>

#include "pmp/algorithms/distance_point_triangle.h"
#include "pmp/algorithms/normals.h"

namespace pmp {
namespace {

template <class HeapEntry, class HeapInterface>
class Heap : private std::vector<HeapEntry>
{
public:
    using This = Heap<HeapEntry, HeapInterface>;

    // Constructor
    Heap() : HeapVector() {}

    // Construct with a given \p HeapInterface.
    Heap(const HeapInterface& i) : HeapVector(), interface_(i) {}

    // Destructor.
    ~Heap() = default;

    // clear the heap
    void clear() { HeapVector::clear(); }

    // is heap empty?
    bool empty() { return HeapVector::empty(); }

    // returns the size of heap
    unsigned int size() { return (unsigned int)HeapVector::size(); }

    // reserve space for N entries
    void reserve(unsigned int n) { HeapVector::reserve(n); }

    // reset heap position to -1 (not in heap)
    void reset_heap_position(HeapEntry h)
    {
        interface_.set_heap_position(h, -1);
    }

    // is an entry in the heap?
    bool is_stored(HeapEntry h)
    {
        return interface_.get_heap_position(h) != -1;
    }

    // insert the entry h
    void insert(HeapEntry h)
    {
        This::push_back(h);
        upheap(size() - 1);
    }

    // get the first entry
    HeapEntry front()
    {
        assert(!empty());
        return entry(0);
    }

    // delete the first entry
    void pop_front()
    {
        assert(!empty());
        interface_.set_heap_position(entry(0), -1);
        if (size() > 1)
        {
            entry(0, entry(size() - 1));
            HeapVector::resize(size() - 1);
            downheap(0);
        }
        else
            HeapVector::resize(size() - 1);
    }

    // remove an entry
    void remove(HeapEntry h)
    {
        int pos = interface_.get_heap_position(h);
        interface_.set_heap_position(h, -1);

        assert(pos != -1);
        assert((unsigned int)pos < size());

        // last item ?
        if ((unsigned int)pos == size() - 1)
            HeapVector::resize(size() - 1);

        else
        {
            entry(pos, entry(size() - 1)); // move last elem to pos
            HeapVector::resize(size() - 1);
            downheap(pos);
            upheap(pos);
        }
    }

    // update an entry: change the key and update the position to
    // reestablish the heap property.
    void update(HeapEntry h)
    {
        int pos = interface_.get_heap_position(h);
        assert(pos != -1);
        assert((unsigned int)pos < size());
        downheap(pos);
        upheap(pos);
    }

    // Check heap condition. true if heap condition is satisfied, false if not.
    bool check()
    {
        bool ok(true);
        unsigned int i, j;
        for (i = 0; i < size(); ++i)
        {
            if (((j = left(i)) < size()) &&
                interface_.greater(entry(i), entry(j)))
            {
                ok = false;
            }
            if (((j = right(i)) < size()) &&
                interface_.greater(entry(i), entry(j)))
            {
                ok = false;
            }
        }
        return ok;
    }

private:
    using HeapVector = std::vector<HeapEntry>;

    // Upheap. Establish heap property.
    void upheap(unsigned int idx)
    {
        HeapEntry h = entry(idx);
        unsigned int parentIdx;

        while ((idx > 0) && interface_.less(h, entry(parentIdx = parent(idx))))
        {
            entry(idx, entry(parentIdx));
            idx = parentIdx;
        }

        entry(idx, h);
    }

    // Downheap. Establish heap property.
    void downheap(unsigned int idx)
    {
        HeapEntry h = entry(idx);
        unsigned int childIdx;
        unsigned int s = size();

        while (idx < s)
        {
            childIdx = left(idx);
            if (childIdx >= s)
                break;

            if ((childIdx + 1 < s) &&
                (interface_.less(entry(childIdx + 1), entry(childIdx))))
                ++childIdx;

            if (interface_.less(h, entry(childIdx)))
                break;

            entry(idx, entry(childIdx));
            idx = childIdx;
        }

        entry(idx, h);
    }

    // Get the entry at index idx
    inline HeapEntry entry(unsigned int idx)
    {
        assert(idx < size());
        return (This::operator[](idx));
    }

    // Set entry H to index idx and update H's heap position.
    inline void entry(unsigned int idx, HeapEntry h)
    {
        assert(idx < size());
        This::operator[](idx) = h;
        interface_.set_heap_position(h, idx);
    }

    // Get parent's index
    inline unsigned int parent(unsigned int i) { return (i - 1) >> 1; }

    // Get left child's index
    inline unsigned int left(unsigned int i) { return (i << 1) + 1; }

    // Get right child's index
    inline unsigned int right(unsigned int i) { return (i << 1) + 2; }

    // Instance of HeapInterface
    HeapInterface interface_;
};

// Store a quadric as a symmetric 4x4 matrix.
class Quadric
{
public: // clang-format off

    // construct quadric from upper triangle of symmetric 4x4 matrix
    Quadric(double a, double b, double c, double d,
            double e, double f, double g,
            double h, double i,
            double j)
        : a_(a), b_(b), c_(c), d_(d),
          e_(e), f_(f), g_(g),
          h_(h), i_(i),
          j_(j)
    {}

    // constructor quadric from given plane equation: ax+by+cz+d=0
    Quadric(double a=0.0, double b=0.0, double c=0.0, double d=0.0)
        :  a_(a*a), b_(a*b), c_(a*c),  d_(a*d),
           e_(b*b), f_(b*c), g_(b*d),
           h_(c*c), i_(c*d),
           j_(d*d)
    {}

    // construct from point and normal specifying a plane
    Quadric(const Normal& n, const Point& p)
    {
        *this = Quadric(n[0], n[1], n[2], -dot(n,p));
    }

    // set all matrix entries to zero
    void clear() { a_ = b_ = c_ = d_ = e_ = f_ = g_ = h_ = i_ = j_ = 0.0; }

    // add given quadric to this quadric
    Quadric& operator+=(const Quadric& q)
    {
        a_ += q.a_; b_ += q.b_; c_ += q.c_; d_ += q.d_;
        e_ += q.e_; f_ += q.f_; g_ += q.g_;
        h_ += q.h_; i_ += q.i_;
        j_ += q.j_;
        return *this;
    }

    // multiply quadric by a scalar
    Quadric& operator*=(double s)
    {
        a_ *= s; b_ *= s; c_ *= s;  d_ *= s;
        e_ *= s; f_ *= s; g_ *= s;
        h_ *= s; i_ *= s;
        j_ *= s;
        return *this;
    }

    // evaluate quadric Q at position p by computing (p^T * Q * p)
    double operator()(const Point& p) const
    {
        const double x(p[0]), y(p[1]), z(p[2]);
        return a_*x*x + 2.0*b_*x*y + 2.0*c_*x*z + 2.0*d_*x
            +  e_*y*y + 2.0*f_*y*z + 2.0*g_*y
            +  h_*z*z + 2.0*i_*z
            +  j_;
    }

private:

    double a_, b_, c_, d_,
        e_, f_, g_,
        h_, i_,
        j_;
}; // clang-format on

class NormalCone
{
public:
    NormalCone() = default;

    // Initialize cone with center (unit vector) and angle (radius in radians)
    NormalCone(const Normal& normal, Scalar angle = 0.0)
        : center_normal_(normal), angle_(angle)
    {
    }

    // returns center normal
    const Normal& center_normal() const { return center_normal_; }

    // returns size of cone (radius in radians)
    Scalar angle() const { return angle_; }

    // merge *this with n.
    NormalCone& merge(const Normal& n) { return merge(NormalCone(n)); }

    // merge *this with nc. *this will then enclose both cones.
    NormalCone& merge(const NormalCone& nc)
    {
        const Scalar dp = dot(center_normal_, nc.center_normal_);

        // axes point in same direction
        if (dp > 0.99999)
        {
            angle_ = std::max(angle_, nc.angle_);
        }

        // axes point in opposite directions
        else if (dp < -0.99999)
        {
            angle_ = Scalar(2 * M_PI);
        }

        else
        {
            // new angle
            Scalar center_angle = std::acos(dp);
            Scalar min_angle = std::min(-angle_, center_angle - nc.angle_);
            Scalar max_angle = std::max(angle_, center_angle + nc.angle_);
            angle_ = Scalar(0.5) * (max_angle - min_angle);

            // axis by SLERP
            Scalar axis_angle = Scalar(0.5) * (min_angle + max_angle);
            center_normal_ =
                ((center_normal_ * std::sin(center_angle - axis_angle) +
                  nc.center_normal_ * std::sin(axis_angle)) /
                 std::sin(center_angle));
        }

        return *this;
    }

private:
    Normal center_normal_;
    Scalar angle_;
};

class Decimation
{
public:
    Decimation(SurfaceMesh& mesh);
    ~Decimation();
    void initialize(Scalar aspect_ratio = 0.0, Scalar edge_length = 0.0,
                    unsigned int max_valence = 0, Scalar normal_deviation = 0.0,
                    Scalar hausdorff_error = 0.0, Scalar seam_threshold = 1e-2,
                    Scalar seam_angle_deviation = 1);
    void decimate(unsigned int n_vertices);

private:
    // Store data for an halfedge collapse
    struct CollapseData
    {
        CollapseData(SurfaceMesh& sm, Halfedge h);

        SurfaceMesh& mesh;

        /*        vl
         *        *
         *       / \
         *      /   \
         *     / fl  \
         * v0 *------>* v1
         *     \ fr  /
         *      \   /
         *       \ /
         *        *
         *        vr
         */
        Halfedge v0v1; // Halfedge to be collapsed
        Halfedge v1v0; // Reverse halfedge
        Vertex v0;     // Vertex to be removed
        Vertex v1;     // Remaining vertex
        Face fl;       // Left face
        Face fr;       // Right face
        Vertex vl;     // Left vertex
        Vertex vr;     // Right vertex
        Halfedge v1vl, vlv0, v0vr, vrv1;
    };

    // Heap interface
    class HeapInterface
    {
    public:
        HeapInterface(VertexProperty<float> prio, VertexProperty<int> pos)
            : prio_(prio), pos_(pos)
        {
        }

        bool less(Vertex v0, Vertex v1) { return prio_[v0] < prio_[v1]; }
        bool greater(Vertex v0, Vertex v1) { return prio_[v0] > prio_[v1]; }
        int get_heap_position(Vertex v) { return pos_[v]; }
        void set_heap_position(Vertex v, int pos) { pos_[v] = pos; }

    private:
        VertexProperty<float> prio_;
        VertexProperty<int> pos_;
    };

    using PriorityQueue = Heap<Vertex, HeapInterface>;

    using Points = std::vector<Point>;

    // put the vertex v in the priority queue
    void enqueue_vertex(PriorityQueue& queue, Vertex v);

    // is collapsing the halfedge h allowed?
    bool is_collapse_legal(const CollapseData& cd);

    // are texture seams preserved if h is collapsed?
    bool texcoord_check(Halfedge h);

    // what is the priority of collapsing the halfedge h
    float priority(const CollapseData& cd);

    // preprocess halfedge collapse
    void preprocess_collapse(const CollapseData& cd);

    // postprocess halfedge collapse
    void postprocess_collapse(const CollapseData& cd);

    // compute aspect ratio for face f
    Scalar aspect_ratio(Face f) const;

    // compute distance from point p to triangle f
    Scalar distance(Face f, const Point& p) const;

    SurfaceMesh& mesh_;

    bool initialized_{false};

    VertexProperty<float> vpriority_;
    VertexProperty<Halfedge> vtarget_;
    VertexProperty<int> heap_pos_;
    VertexProperty<Quadric> vquadric_;
    FaceProperty<NormalCone> normal_cone_;
    FaceProperty<Points> face_points_;

    VertexProperty<Point> vpoint_;
    FaceProperty<Point> fnormal_;
    VertexProperty<bool> vselected_;
    VertexProperty<bool> vfeature_;
    EdgeProperty<bool> efeature_;
    EdgeProperty<bool> texture_seams_;

    bool has_selection_{false};
    bool has_features_{false};
    Scalar normal_deviation_;
    Scalar hausdorff_error_;
    Scalar aspect_ratio_;
    Scalar edge_length_;
    Scalar seam_threshold_;
    Scalar seam_angle_deviation_;
    unsigned int max_valence_;
};

Decimation::Decimation(SurfaceMesh& mesh) : mesh_(mesh)
{
    if (!mesh_.is_triangle_mesh())
        throw InvalidInputException("Input is not a triangle mesh!");

    aspect_ratio_ = 0;
    edge_length_ = 0;
    max_valence_ = 0;
    normal_deviation_ = 0;
    hausdorff_error_ = 0;

    seam_threshold_ = 1e-2;
    seam_angle_deviation_ = 0.99;

    // add properties
    vquadric_ = mesh_.add_vertex_property<Quadric>("v:quadric");
    texture_seams_ = mesh_.edge_property<bool>("e:seam", false);

    // get properties
    vpoint_ = mesh_.vertex_property<Point>("v:point");

    // compute face normals
    face_normals(mesh_);
    fnormal_ = mesh_.face_property<Normal>("f:normal");
}

Decimation::~Decimation()
{
    // remove added properties
    mesh_.remove_vertex_property(vquadric_);
    mesh_.remove_face_property(normal_cone_);
    mesh_.remove_face_property(face_points_);
    mesh_.remove_edge_property(texture_seams_);
}

void Decimation::initialize(Scalar aspect_ratio, Scalar edge_length,
                            unsigned int max_valence, Scalar normal_deviation,
                            Scalar hausdorff_error, Scalar seam_threshold,
                            Scalar seam_angle_deviation)
{
    // store parameters
    aspect_ratio_ = aspect_ratio;
    max_valence_ = max_valence;
    edge_length_ = edge_length;
    normal_deviation_ = normal_deviation / 180.0 * M_PI;
    hausdorff_error_ = hausdorff_error;
    seam_threshold_ = seam_threshold;
    seam_angle_deviation_ = (180.0 - seam_angle_deviation) / 180.0;

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

    // detect texture seams
    auto texcoords = mesh_.get_halfedge_property<TexCoord>("h:tex");
    if (texcoords)
    {
        for (auto e : mesh_.edges())
        {
            // texcoords are stored in halfedge pointing towards a vertex
            Halfedge h0 = mesh_.halfedge(e, 0);
            Halfedge h1 = mesh_.halfedge(e, 1);     //opposite halfedge
            Halfedge h0p = mesh_.prev_halfedge(h0); // start point edge 0
            Halfedge h1p = mesh_.prev_halfedge(h1); // start point edge 1

            // if start or end points differ more than seam_threshold
            // the corresponding edge is a texture seam
            if (norm(texcoords[h1] - texcoords[h0p]) > seam_threshold_ ||
                norm(texcoords[h0] - texcoords[h1p]) > seam_threshold_)
            {
                texture_seams_[e] = true;
            }
            else
            {
                texture_seams_[e] = false;
            }
        }
    }

    initialized_ = true;
}

void Decimation::decimate(unsigned int n_vertices)
{
    // make sure the decimater is initialized
    if (!initialized_)
        initialize();

    std::vector<Vertex> one_ring;

    // add properties for priority queue
    vpriority_ = mesh_.add_vertex_property<float>("v:prio");
    heap_pos_ = mesh_.add_vertex_property<int>("v:heap");
    vtarget_ = mesh_.add_vertex_property<Halfedge>("v:target");

    // build priority queue
    HeapInterface hi(vpriority_, heap_pos_);
    PriorityQueue queue(hi);
    queue.reserve(mesh_.n_vertices());
    for (auto v : mesh_.vertices())
    {
        queue.reset_heap_position(v);
        enqueue_vertex(queue, v);
    }

    auto nv = mesh_.n_vertices();
    while (nv > n_vertices && !queue.empty())
    {
        // get 1st element
        auto v = queue.front();
        queue.pop_front();
        auto h = vtarget_[v];
        CollapseData cd(mesh_, h);

        // check this (again)
        if (!mesh_.is_collapse_ok(h))
            continue;

        // are texture seams preserved?
        if (!texcoord_check(cd.v0v1))
            continue;

        // store one-ring
        one_ring.clear();
        for (auto vv : mesh_.vertices(cd.v0))
        {
            one_ring.push_back(vv);
        }

        // preprocessing -> adjust texcoords
        preprocess_collapse(cd);

        // perform collapse
        mesh_.collapse(h);
        --nv;

        // postprocessing, e.g., update quadrics
        postprocess_collapse(cd);

        // update queue
        for (auto vv : one_ring)
            enqueue_vertex(queue, vv);
    }

    // clean up
    mesh_.garbage_collection();
    mesh_.remove_vertex_property(vpriority_);
    mesh_.remove_vertex_property(heap_pos_);
    mesh_.remove_vertex_property(vtarget_);
}

void Decimation::enqueue_vertex(PriorityQueue& queue, Vertex v)
{
    float prio, min_prio(std::numeric_limits<float>::max());
    Halfedge min_h;

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

        if (queue.is_stored(v))
            queue.update(v);
        else
            queue.insert(v);
    }

    // not valid -> remove from heap
    else
    {
        if (queue.is_stored(v))
            queue.remove(v);

        vpriority_[v] = -1;
        vtarget_[v] = min_h;
    }
}

bool Decimation::is_collapse_legal(const CollapseData& cd)
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

    // are texture seams preserved?
    if (!texcoord_check(cd.v0v1))
        return false;

    // check maximal valence
    if (max_valence_ > 0)
    {
        auto val0 = mesh_.valence(cd.v0);
        auto val1 = mesh_.valence(cd.v1);
        auto val = val0 + val1 - 1;
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
                Normal n1 = face_normal(mesh_, f);
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
                nc.merge(face_normal(mesh_, f));

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

bool Decimation::texcoord_check(Halfedge h)
{
    auto texcoords = mesh_.get_halfedge_property<TexCoord>("h:tex");
    if (!texcoords)
    {
        // no texture coordinates -> skip texture seam tests
        return true;
    }

    auto texture_seams = mesh_.edge_property<bool>("e:seam");
    if (!texture_seams)
    {
        // no seams found -> skip seam tests
        return true;
    }

    Halfedge o(mesh_.opposite_halfedge(h));
    Vertex v0(mesh_.to_vertex(o));

    if (!texture_seams[mesh_.edge(h)])
    {
        // v0v1 is not a texture seam
        for (auto he : mesh_.halfedges(v0))
        {
            if (he == h)
                continue;
            // Check if v0 is part of a texture seam
            // If yes, v0 must not be moved
            if (texture_seams[mesh_.edge(he)])
            {
                return false;
            }
        }

        return true;
    }

    // count number of adjacent texture seam edges
    int nr_seam_edges = 0;
    for (auto he : mesh_.halfedges(v0))
    {
        if (texture_seams[mesh_.edge(he)])
        {
            nr_seam_edges++;
        }
    }

    // if there are more than 2 seam edges at point v0
    // -> v0 must not be moved
    if (nr_seam_edges > 2)
    {
        return false;
    }

    Halfedge seam1 = h, seam2 = mesh_.prev_halfedge(h);
    while (seam2.idx() != o.idx())
    {
        if (texture_seams[mesh_.edge(seam2)])
        {
            auto s1 = normalize(texcoords[seam1] -
                                texcoords[mesh_.prev_halfedge(seam1)]);
            auto s2 = normalize(texcoords[seam2] -
                                texcoords[mesh_.prev_halfedge(seam2)]);

            // opposite uvs
            Halfedge o_seam1 = mesh_.opposite_halfedge(seam1);
            Halfedge o_seam2 = mesh_.opposite_halfedge(seam2);
            auto o1 = normalize(texcoords[o_seam1] -
                                texcoords[mesh_.prev_halfedge(o_seam1)]);
            auto o2 = normalize(texcoords[o_seam2] -
                                texcoords[mesh_.prev_halfedge(o_seam2)]);

            // check if the angle between the seam edge to be collapsed and the
            // seam edge prolonged is smaller than the allowed deviation
            if (dot(s1, s2) < seam_angle_deviation_ ||
                dot(o1, o2) < seam_angle_deviation_)
            {
                // angle is too large -> don't collapse this edge
                return false;
            }
        }
        seam2 = mesh_.prev_halfedge(mesh_.opposite_halfedge(seam2));
    }

    // passed all tests
    return true;
}

float Decimation::priority(const CollapseData& cd)
{
    // computer quadric error metric
    Quadric Q = vquadric_[cd.v0];
    Q += vquadric_[cd.v1];
    return Q(vpoint_[cd.v1]);
}

void Decimation::preprocess_collapse(const CollapseData& cd)
{
    Halfedge h = cd.v0v1;
    Halfedge o = mesh_.opposite_halfedge(h);
    Halfedge v1v2, v2v1, v0v2;

    // move texcoords in correct halfedge before collapsing an edge
    auto texcoords = mesh_.get_halfedge_property<TexCoord>("h:tex");
    if (texcoords)
    {
        auto texture_seams = mesh_.edge_property<bool>("e:seam", false);
        Halfedge hit = h;
        bool is_first_side = true;

        // which texcoord must be saved depends
        // on the side of the texture seam
        for (size_t i = 0; i < mesh_.valence(mesh_.to_vertex(o)) - 1; ++i)
        {
            hit = mesh_.prev_halfedge(hit);
            if (is_first_side)
                texcoords[hit] = texcoords[h];
            else if (!is_first_side)
                texcoords[hit] = texcoords[mesh_.prev_halfedge(o)];
            if (texture_seams[mesh_.edge(hit)])
            {
                is_first_side = false;

                // loop case 1
                if (mesh_.to_vertex(mesh_.next_halfedge(h)) ==
                    mesh_.from_vertex(hit))
                {
                    v1v2 = mesh_.next_halfedge(h);
                    texcoords[mesh_.opposite_halfedge(v1v2)] = texcoords[hit];
                    texcoords[v1v2] = texcoords[mesh_.opposite_halfedge(hit)];
                    texture_seams[mesh_.edge(v1v2)] = true;
                }

                // loop case 2
                if (mesh_.to_vertex(mesh_.next_halfedge(o)) ==
                    mesh_.from_vertex(hit))
                {
                    v2v1 = mesh_.prev_halfedge(o);
                    v0v2 = mesh_.opposite_halfedge(hit);
                    texcoords[mesh_.opposite_halfedge(v2v1)] = texcoords[v0v2];
                    texcoords[v2v1] = texcoords[hit];
                    texture_seams[mesh_.edge(v2v1)] = true;
                }
            }
            hit = mesh_.opposite_halfedge(hit);
        }
    }
}

void Decimation::postprocess_collapse(const CollapseData& cd)
{
    // update error quadrics
    vquadric_[cd.v1] += vquadric_[cd.v0];

    // update normal cones
    if (normal_deviation_)
    {
        for (auto f : mesh_.faces(cd.v1))
        {
            normal_cone_[f].merge(face_normal(mesh_, f));
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

Scalar Decimation::aspect_ratio(Face f) const
{
    // min height is area/maxLength
    // aspect ratio = length / height
    //              = length * length / area

    auto fvit = mesh_.vertices(f);

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

Scalar Decimation::distance(Face f, const Point& p) const
{
    auto fvit = mesh_.vertices(f);

    const Point p0 = vpoint_[*fvit];
    const Point p1 = vpoint_[*(++fvit)];
    const Point p2 = vpoint_[*(++fvit)];

    Point n;

    return dist_point_triangle(p, p0, p1, p2, n);
}

Decimation::CollapseData::CollapseData(SurfaceMesh& sm, Halfedge h) : mesh(sm)
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
} // namespace

void decimate(SurfaceMesh& mesh, unsigned int n_vertices, Scalar aspect_ratio,
              Scalar edge_length, unsigned int max_valence,
              Scalar normal_deviation, Scalar hausdorff_error,
              Scalar seam_threshold, Scalar seam_angle_deviation)
{
    Decimation decimator(mesh);
    decimator.initialize(aspect_ratio, edge_length, max_valence,
                         normal_deviation, hausdorff_error, seam_threshold,
                         seam_angle_deviation);
    decimator.decimate(n_vertices);
}

} // namespace pmp
