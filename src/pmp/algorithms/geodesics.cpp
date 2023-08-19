// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/geodesics.h"
#include "pmp/algorithms/laplace.h"

#include <cassert>
#include <set>
#include <map>

namespace pmp {
namespace {

class Geodesics
{
public:
    Geodesics(SurfaceMesh& mesh, bool use_virtual_edges = true);
    ~Geodesics();
    unsigned int compute(
        const std::vector<Vertex>& seed,
        Scalar maxdist = std::numeric_limits<Scalar>::max(),
        unsigned int maxnum = std::numeric_limits<unsigned int>::max(),
        std::vector<Vertex>* neighbors = nullptr);

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
    using PriorityQueue = std::set<Vertex, VertexCmp>;

    // virtual edges for walking through obtuse triangles
    struct VirtualEdge
    {
        VirtualEdge(Vertex v, Scalar l) : vertex(v), length(l) {}
        Vertex vertex;
        Scalar length;
    };

    // set for storing virtual edges
    using VirtualEdges = std::map<Halfedge, VirtualEdge>;

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

Geodesics::Geodesics(SurfaceMesh& mesh, bool use_virtual_edges)
    : mesh_(mesh), use_virtual_edges_(use_virtual_edges)
{
    distance_ = mesh_.vertex_property<Scalar>("geodesic:distance");
    processed_ = mesh_.add_vertex_property<bool>("geodesic:processed");

    if (use_virtual_edges_)
        find_virtual_edges();
}

Geodesics::~Geodesics()
{
    mesh_.remove_vertex_property(processed_);
}

void Geodesics::find_virtual_edges()
{
    Halfedge hh, hhh;
    Vertex vh0, vh1, vhn, start_vh0, start_vh1;
    Point pp, p0, p1, pn, p, d0, d1;
    Point X, Y;
    vec2 v0, v1, vn, v, d;
    Scalar f, alpha, beta, tan_beta;

    const Scalar one(1.0), minus_one(-1.0);
    const Scalar max_angle = 90.0 / 180.0 * M_PI;
    const Scalar max_angle_cos = cos(max_angle);

    virtual_edges_.clear();

    for (auto vv : mesh_.vertices())
    {
        pp = mesh_.position(vv);

        for (auto h : mesh_.halfedges(vv))
        {
            if (!mesh_.is_boundary(h))
            {
                vh0 = mesh_.to_vertex(h);
                hh = mesh_.next_halfedge(h);
                vh1 = mesh_.to_vertex(hh);

                p0 = mesh_.position(vh0);
                p1 = mesh_.position(vh1);
                d0 = normalize(p0 - pp);
                d1 = normalize(p1 - pp);

                // obtuse angle ?
                if (dot(d0, d1) < max_angle_cos)
                {
                    // compute angles
                    alpha = 0.5 * acos(std::min(
                                      one, std::max(minus_one, dot(d0, d1))));
                    beta = max_angle - alpha;
                    tan_beta = tan(beta);

                    // coord system
                    X = normalize(d0 + d1);
                    Y = normalize(cross(cross(d0, d1), X));

                    // 2D coords
                    d0 = p0 - pp;
                    d1 = p1 - pp;
                    v0[0] = dot(d0, X);
                    v0[1] = dot(d0, Y);
                    v1[0] = dot(d1, X);
                    v1[1] = dot(d1, Y);

                    start_vh0 = vh0;
                    start_vh1 = vh1;
                    hhh = mesh_.opposite_halfedge(hh);

                    // unfold ...
                    while (((vh0 == start_vh0) || (vh1 == start_vh1)) &&
                           (!mesh_.is_boundary(hhh)))
                    {
                        // get next point
                        vhn = mesh_.to_vertex(mesh_.next_halfedge(hhh));
                        pn = mesh_.position(vhn);
                        d0 = (p1 - p0);
                        d1 = (pn - p0);
                        d = (v1 - v0);
                        f = dot(d0, d1) / sqrnorm(d0);
                        p = p0 + f * d0;
                        v = v0 + f * d;
                        d = normalize(vec2(d[1], -d[0]));
                        vn = v + d * norm(p - pn);

                        // point in tolerance?
                        if ((fabs(vn[1]) / fabs(vn[0])) < tan_beta)
                        {
                            virtual_edges_.insert(
                                std::pair<Halfedge, VirtualEdge>(
                                    h, VirtualEdge(vhn, norm(vn))));
                            break;
                        }

                        // prepare next edge
                        if (vn[1] > 0.0)
                        {
                            hh = mesh_.opposite_halfedge(hh);
                            hh = mesh_.next_halfedge(hh);
                            vh1 = vhn;
                            p1 = pn;
                            v1 = vn;
                        }
                        else
                        {
                            hh = mesh_.opposite_halfedge(hh);
                            hh = mesh_.next_halfedge(hh);
                            hh = mesh_.next_halfedge(hh);
                            vh0 = vhn;
                            p0 = pn;
                            v0 = vn;
                        }
                        hhh = mesh_.opposite_halfedge(hh);
                    }
                }
            }
        }
    }
}

unsigned int Geodesics::compute(const std::vector<Vertex>& seed, Scalar maxdist,
                                unsigned int maxnum,
                                std::vector<Vertex>* neighbors)
{
    unsigned int num(0);

    // generate front
    front_ = new PriorityQueue(VertexCmp(distance_));

    // initialize front with given seed
    num = init_front(seed, neighbors);

    // sort one-ring neighbors of seed vertices
    if (neighbors)
    {
        std::sort(neighbors->begin(), neighbors->end(), VertexCmp(distance_));
    }

    // correct if seed vertices have more than maxnum neighbors
    if (num > maxnum)
    {
        num = maxnum;
        if (neighbors)
            neighbors->resize(maxnum);
    }

    // propagate up to max distance or max number of neighbors
    if (num < maxnum)
        num += propagate_front(maxdist, maxnum - num, neighbors);

    // clean up
    delete front_;

    return num;
}

unsigned int Geodesics::init_front(const std::vector<Vertex>& seed,
                                   std::vector<Vertex>* neighbors)
{
    unsigned int num(0);

    if (seed.empty())
        return num;

    // reset all vertices
    for (auto v : mesh_.vertices())
    {
        processed_[v] = false;
        distance_[v] = std::numeric_limits<Scalar>::max();
    }

    // initialize neighbor array
    if (neighbors)
        neighbors->clear();

    // initialize seed vertices
    for (auto v : seed)
    {
        processed_[v] = true;
        distance_[v] = 0.0;
    }

    // initialize seed's one-ring
    for (auto v : seed)
    {
        for (auto vv : mesh_.vertices(v))
        {
            const Scalar dist =
                pmp::distance(mesh_.position(v), mesh_.position(vv));
            if (dist < distance_[vv])
            {
                distance_[vv] = dist;
                processed_[vv] = true;
                ++num;
                if (neighbors)
                    neighbors->push_back(vv);
            }
        }
    }

    // init marching front
    front_->clear();
    for (auto v : seed)
    {
        for (auto vv : mesh_.vertices(v))
        {
            for (auto vvv : mesh_.vertices(vv))
            {
                if (!processed_[vvv])
                {
                    heap_vertex(vvv);
                }
            }
        }
    }

    return num;
}

unsigned int Geodesics::propagate_front(Scalar maxdist, unsigned int maxnum,
                                        std::vector<Vertex>* neighbors)
{
    unsigned int num(0);

    while (!front_->empty())
    {
        // find minimum vertex, remove it from queue
        auto v = *front_->begin();
        front_->erase(front_->begin());
        assert(!processed_[v]);
        processed_[v] = true;
        ++num;
        if (neighbors)
            neighbors->push_back(v);

        // did we reach maximum distance?
        if (distance_[v] > maxdist)
            break;

        // did we reach maximum number of neighbors
        if (num >= maxnum)
            break;

        // update front
        for (auto vv : mesh_.vertices(v))
        {
            if (!processed_[vv])
            {
                heap_vertex(vv);
            }
        }
    }

    return num;
}

void Geodesics::heap_vertex(Vertex v)
{
    assert(!processed_[v]);

    Vertex v0, v1, vv, v0_min, v1_min;
    Scalar dist, dist_min(std::numeric_limits<Scalar>::max()), d;
    typename VirtualEdges::const_iterator ve_it, ve_end(virtual_edges_.end());
    bool found(false);

    for (auto h : mesh_.halfedges(v))
    {
        if (!mesh_.is_boundary(h))
        {
            ve_it = virtual_edges_.find(h);

            // no virtual edge
            if (ve_it == ve_end)
            {
                v0 = mesh_.to_vertex(h);
                v1 = mesh_.to_vertex(mesh_.next_halfedge(h));

                if (processed_[v0] && processed_[v1])
                {
                    dist = distance(v0, v1, v);
                    if (dist < dist_min)
                    {
                        dist_min = dist;
                        found = true;
                    }
                }
            }

            // virtual edge
            else
            {
                v0 = mesh_.to_vertex(h);
                v1 = mesh_.to_vertex(mesh_.next_halfedge(h));
                vv = ve_it->second.vertex;
                d = ve_it->second.length;

                if (processed_[v0] && processed_[vv])
                {
                    dist = distance(v0, vv, v,
                                    std::numeric_limits<Scalar>::max(), d);
                    if (dist < dist_min)
                    {
                        dist_min = dist;
                        found = true;
                    }
                }

                if (processed_[v1] && processed_[vv])
                {
                    dist = distance(vv, v1, v, d,
                                    std::numeric_limits<Scalar>::max());
                    if (dist < dist_min)
                    {
                        dist_min = dist;
                        found = true;
                    }
                }
            }
        }
    }

    // update priority queue
    if (found)
    {
        if (distance_[v] != std::numeric_limits<Scalar>::max())
        {
            auto it = front_->find(v);
            assert(it != front_->end());
            front_->erase(it);
        }

        distance_[v] = dist_min;
        front_->insert(v);
    }
    else
    {
        if (distance_[v] != std::numeric_limits<Scalar>::max())
        {
            front_->erase(v);
            distance_[v] = std::numeric_limits<Scalar>::max();
        }
    }
}

Scalar Geodesics::distance(Vertex v0, Vertex v1, Vertex v2, Scalar r0,
                           Scalar r1)
{
    Point A, B, C;
    double TA, TB;
    double a, b;

    // choose points such that TB>TA and hence u>0
    if (distance_[v0] < distance_[v1])
    {
        A = mesh_.position(v0);
        B = mesh_.position(v1);
        C = mesh_.position(v2);
        TA = distance_[v0];
        TB = distance_[v1];
        a = r1 == std::numeric_limits<Scalar>::max() ? pmp::distance(B, C) : r1;
        b = r0 == std::numeric_limits<Scalar>::max() ? pmp::distance(A, C) : r0;
    }
    else
    {
        A = mesh_.position(v1);
        B = mesh_.position(v0);
        C = mesh_.position(v2);
        TA = distance_[v1];
        TB = distance_[v0];
        a = r0 == std::numeric_limits<Scalar>::max() ? pmp::distance(B, C) : r0;
        b = r1 == std::numeric_limits<Scalar>::max() ? pmp::distance(A, C) : r1;
    }

    // Dijkstra: propagate along edges
    const double dijkstra = std::min(TA + b, TB + a);

    // obtuse angle -> fall back to Dijkstra
    const double c = dot(normalize(A - C), normalize(B - C)); // cosine
    if (c < 0.0)
        return dijkstra;

    // Kimmel: solve quadratic equation
    const double u = TB - TA;
    const double aa = a * a + b * b - 2.0 * a * b * c;
    const double bb = 2.0 * b * u * (a * c - b);
    const double cc = b * b * (u * u - a * a * (1.0 - c * c));
    const double dd = bb * bb - 4.0 * aa * cc;
    if (dd > 0.0)
    {
        const double t1 = (-bb + sqrt(dd)) / (2.0 * aa);
        const double t2 = (-bb - sqrt(dd)) / (2.0 * aa);
        const double t = std::max(t1, t2);
        const double q = b * (t - u) / t;
        if ((u < t) && (a * c < q) && (q < a / c))
        {
            return TA + t;
        }
    }

    // use Dijkstra as fall-back
    return dijkstra;
}

Scalar max_diagonal_length(const SurfaceMesh& mesh)
{
    Scalar maxdiag(0.0), length;
    for (auto f : mesh.faces())
    {
        for (auto v : mesh.vertices(f))
        {
            for (auto vv : mesh.vertices(f))
            {
                length = distance(mesh.position(v), mesh.position(vv));
                if (length > maxdiag)
                {
                    maxdiag = length;
                }
            }
        }
    }
    return maxdiag;
}

} // namespace

void distance_to_texture_coordinates(SurfaceMesh& mesh)
{
    auto distance = mesh.get_vertex_property<Scalar>("geodesic:distance");
    assert(distance);

    // find maximum distance
    Scalar maxdist(0);
    for (auto v : mesh.vertices())
    {
        if (distance[v] < std::numeric_limits<Scalar>::max())
        {
            maxdist = std::max(maxdist, distance[v]);
        }
    }

    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    for (auto v : mesh.vertices())
    {
        if (distance[v] < std::numeric_limits<Scalar>::max())
        {
            tex[v] = TexCoord(distance[v] / maxdist, 0.0);
        }
        else
        {
            tex[v] = TexCoord(1.0, 0.0);
        }
    }
}

unsigned int geodesics(SurfaceMesh& mesh, const std::vector<Vertex>& seed,
                       Scalar maxdist, unsigned int maxnum,
                       std::vector<Vertex>* neighbors)
{
    return Geodesics(mesh, true /*virtual edges*/)
        .compute(seed, maxdist, maxnum, neighbors);
}

void geodesics_heat(SurfaceMesh& mesh, const std::vector<Vertex>& seed)
{
    const unsigned int n = mesh.n_vertices();

    // setup all matrices
    SparseMatrix G, D, L;
    DiagonalMatrix M;
    gradient_matrix(mesh, G);
    divergence_matrix(mesh, D);
    mass_matrix(mesh, M);
    L = D * G;

    // diffusion time step (squared mean edge length)
    double h = max_diagonal_length(mesh);
    const double dt = h * h;

    // solve heat diffusion from seed points
    SparseMatrix A = SparseMatrix(M) - dt * L;
    Eigen::SparseVector<double> b(n);
    for (auto s : seed)
    {
        b.coeffRef(s.idx()) = 1.0;
    }
    Eigen::VectorXd heat = cholesky_solve(A, b);

    // compute and normalize heat gradient
    Eigen::VectorXd grad = G * heat;
    for (int i = 0; i < grad.rows(); i += 3)
    {
        dvec3& g = *reinterpret_cast<dvec3*>(&grad[i]);
        double ng = norm(g);
        if (ng > std::numeric_limits<double>::min())
        {
            g /= ng;
        }
    }

    // solve Poisson system for distances
    Eigen::VectorXd dist = cholesky_solve(L, D * (-grad));

    // shift distances value such that min dist is zero
    double mindist = dist.minCoeff();
    for (int i = 0; i < dist.rows(); ++i)
    {
        dist[i] -= mindist;
    }

    // copy result
    auto distance = mesh.vertex_property<Scalar>("geodesic:distance");
    for (auto v : mesh.vertices())
    {
        distance[v] = dist[v.idx()];
    }
}

} // namespace pmp
