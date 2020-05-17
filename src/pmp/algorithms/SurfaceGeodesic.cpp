// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/SurfaceGeodesic.h"

namespace pmp {

SurfaceGeodesic::SurfaceGeodesic(SurfaceMesh& mesh, bool use_virtual_edges)
    : mesh_(mesh), use_virtual_edges_(use_virtual_edges)
{
    distance_ = mesh_.add_vertex_property<Scalar>("geodesic:distance");
    processed_ = mesh_.add_vertex_property<bool>("geodesic:processed");

    if (use_virtual_edges_)
        find_virtual_edges();
}

SurfaceGeodesic::~SurfaceGeodesic()
{
    mesh_.remove_vertex_property(distance_);
    mesh_.remove_vertex_property(processed_);
}

void SurfaceGeodesic::find_virtual_edges()
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

    std::clog << "[Geodesic] Found " << virtual_edges_.size()
              << " virtual edges\n";
}

unsigned int SurfaceGeodesic::compute(const std::vector<Vertex>& seed,
                                      Scalar maxdist, unsigned int maxnum,
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

unsigned int SurfaceGeodesic::init_front(const std::vector<Vertex>& seed,
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

unsigned int SurfaceGeodesic::propagate_front(Scalar maxdist,
                                              unsigned int maxnum,
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

void SurfaceGeodesic::heap_vertex(Vertex v)
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

bool valid_triangle(double a, double b, double c)
{
    return (a + b > c && a + c > b && b + c > a);
}

Scalar SurfaceGeodesic::distance(Vertex v0, Vertex v1, Vertex v2, Scalar r0,
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

    // Dykstra: propagate along edges
    const double dykstra = std::min(TA + b, TB + a);

    // obtuse angle -> fall back to Dykstra
    const double c = dot(normalize(A - C), normalize(B - C)); // cosine
    if (c < 0.0)
        return dykstra;

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

    // use Dykstra as fall-back
    return dykstra;
}

void SurfaceGeodesic::distance_to_texture_coordinates()
{
    // find maximum distance
    Scalar maxdist(0);
    for (auto v : mesh_.vertices())
    {
        if (distance_[v] < std::numeric_limits<Scalar>::max())
        {
            maxdist = std::max(maxdist, distance_[v]);
        }
    }

    auto tex = mesh_.vertex_property<TexCoord>("v:tex");
    for (auto v : mesh_.vertices())
    {
        if (distance_[v] < std::numeric_limits<Scalar>::max())
        {
            tex[v] = TexCoord(distance_[v] / maxdist, 0.0);
        }
        else
        {
            tex[v] = TexCoord(1.0, 0.0);
        }
    }
}

} // namespace pmp
