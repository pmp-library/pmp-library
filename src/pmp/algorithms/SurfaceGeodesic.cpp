//=============================================================================
// Copyright (C) 2011-2018 The pmp-library developers
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

#include <pmp/algorithms/SurfaceGeodesic.h>
#include <pmp/MatVec.h>

//=============================================================================

namespace pmp {

//=============================================================================


SurfaceGeodesic::SurfaceGeodesic(SurfaceMesh& _mesh,
                                 std::vector<Vertex> _seed,
                                 Scalar _maxdist,
                                 bool _use_virtual_edges) :
    mesh_(_mesh),
    seed_(_seed),
    maxdist_(_maxdist),
    use_virtual_edges_(_use_virtual_edges)
{
    distance_  = mesh_.addVertexProperty<Scalar>("geodesic:distance");
    processed_ = mesh_.addVertexProperty<bool>("geodesic:processed");

    front_ = new PriorityQueue(VertexCmp(distance_));
    find_virtual_edges(); 
    init_front();
    propagate_front(); 
    delete front_;
}


//-----------------------------------------------------------------------------


SurfaceGeodesic::~SurfaceGeodesic()
{
    mesh_.removeVertexProperty(distance_);
    mesh_.removeVertexProperty(processed_);
}


//-----------------------------------------------------------------------------


void SurfaceGeodesic::find_virtual_edges()
{
    Halfedge hh, hhh;
    Vertex   vh0, vh1, vhn, start_vh0, start_vh1;
    Point    pp, p0, p1, pn, p, d0, d1;
    Point    X, Y;
    vec2     v0, v1, vn, v, d;
    Scalar   f, alpha, beta, tan_beta;

    const Scalar one(1.0), minus_one(-1.0);
    const Scalar max_angle     = 90.0 / 180.0 * M_PI;
    const Scalar max_angle_cos = cos(max_angle);


    virtual_edges_.clear();
    if (!use_virtual_edges_) return;


    for (auto vv: mesh_.vertices())
    {
        pp = mesh_.position(vv);

        for (auto h: mesh_.halfedges(vv))
        {
            if (!mesh_.isBoundary(h))
            {
                vh0 = mesh_.toVertex(h);
                hh  = mesh_.nextHalfedge(h);
                vh1 = mesh_.toVertex(hh);

                p0 = mesh_.position(vh0);
                p1 = mesh_.position(vh1);
                d0 = normalize(p0-pp);
                d1 = normalize(p1-pp);


                // obtuse angle ?
                if (dot(d0,d1) < max_angle_cos)
                {
                    // compute angles
                    alpha = 0.5 * acos(std::min(one, std::max(minus_one, dot(d0,d1))));
                    beta  = max_angle - alpha;
                    tan_beta = tan(beta);

                    // coord system
                    X = normalize(d0 + d1);
                    Y = normalize( cross( cross(d0, d1), X) );

                    // 2D coords
                    d0    = p0-pp;  
                    d1    = p1-pp;
                    v0[0] = dot(d0, X);
                    v0[1] = dot(d0, Y);
                    v1[0] = dot(d1, X);
                    v1[1] = dot(d1, Y);

                    start_vh0 = vh0;
                    start_vh1 = vh1;
                    hhh = mesh_.oppositeHalfedge(hh);


                    // unfold ...
                    while (((vh0==start_vh0) || (vh1==start_vh1)) && (!mesh_.isBoundary(hhh)))
                    {
                        // get next point
                        vhn = mesh_.toVertex(mesh_.nextHalfedge(hhh));
                        pn  = mesh_.position(vhn);
                        d0  = (p1-p0);
                        d1  = (pn-p0);
                        d   = (v1-v0);
                        f   = dot(d0, d1) / sqrnorm(d0);
                        p   = p0 + f*d0;
                        v   = v0 + f*d;
                        d   = normalize( vec2(d[1], -d[0]) );
                        vn  = v + d*norm(p-pn);


                        // point in tolerance?
                        if ((fabs(vn[1]) / fabs(vn[0])) < tan_beta)
                        {
                            virtual_edges_.insert(std::pair<Halfedge,VirtualEdge>(h, VirtualEdge(vhn, norm(vn))));
                            break;
                        }


                        // prepare next edge
                        if (vn[1] > 0.0)
                        {
                            hh  = mesh_.oppositeHalfedge(hh);
                            hh  = mesh_.nextHalfedge(hh);
                            vh1 = vhn;
                            p1  = pn;
                            v1  = vn;
                        }
                        else
                        {
                            hh  = mesh_.oppositeHalfedge(hh);
                            hh  = mesh_.nextHalfedge(hh);
                            hh  = mesh_.nextHalfedge(hh);
                            vh0 = vhn;
                            p0  = pn;
                            v0  = vn;
                        }
                        hhh = mesh_.oppositeHalfedge(hh);
                    }
                }
            }
        }
    }

    std::clog << "[Geodesic] Found " << virtual_edges_.size() << " virtual edges\n";
}


//-----------------------------------------------------------------------------


void SurfaceGeodesic::init_front()
{
    if (seed_.empty()) return;


    // reset all vertices
    for (auto v: mesh_.vertices())
    {
        processed_[v] = false;
        distance_[v]  = FLT_MAX;
    }


    // initialize seed vertices
    for (auto v: seed_)
    {
        processed_[v] = true;
        distance_[v]  = 0.0;
    }


    // initialize seed's one-ring
    for (auto v: seed_)
    {
        for (auto vv: mesh_.vertices(v))
        {
            const Scalar dist = pmp::distance(mesh_.position(v), mesh_.position(vv));
            if (dist < distance_[vv])
            {
                distance_[vv]  = dist;
                processed_[vv] = true;
            }
        }
    }



    // init marching front
    front_->clear();
    for (auto v: seed_)
    {
        for (auto vv: mesh_.vertices(v))
        {
            for (auto vvv: mesh_.vertices(vv))
            {
                if (!processed_[vvv])
                {
                    heap_vertex(vvv);
                }
            }
        }
    }
}


//-----------------------------------------------------------------------------


void SurfaceGeodesic::propagate_front()
{
    while (!front_->empty())
    {
        // find minimum vertex, remove it from queue
        auto v = *front_->begin();
        front_->erase(front_->begin());
        assert(!processed_[v]);
        processed_[v] = true;

        // did we reach maximum distance?
        if (distance_[v] > maxdist_) return;

        // update front
        for (auto vv: mesh_.vertices(v))
        {
            if (!processed_[vv])
            {
                heap_vertex(vv);
            }
        }
    }
}


//-----------------------------------------------------------------------------


void SurfaceGeodesic::heap_vertex(Vertex _v)
{
    assert(!processed_[_v]);

    Vertex    v0, v1, vv, v0_min, v1_min;
    Scalar    dist, dist_min(FLT_MAX), d;
    typename VirtualEdges::const_iterator  ve_it, ve_end(virtual_edges_.end());
    bool found(false);


    for (auto h: mesh_.halfedges(_v))
    {
        if (!mesh_.isBoundary(h))
        {
            ve_it = virtual_edges_.find(h);

            // no virtual edge
            if (ve_it == ve_end)
            {
                v0 = mesh_.toVertex(h);
                v1 = mesh_.toVertex(mesh_.nextHalfedge(h));

                if (processed_[v0] && processed_[v1])
                {
                    dist = distance(v0, v1, _v);
                    if (dist < dist_min)
                    {
                        dist_min = dist;
                        found    = true;
                    }
                }
            }


            // virtual edge
            else
            {
                v0 = mesh_.toVertex(h);
                v1 = mesh_.toVertex(mesh_.nextHalfedge(h));
                vv = ve_it->second.vertex;
                d  = ve_it->second.length;

                if (processed_[v0] && processed_[vv])
                {
                    dist = distance(v0, vv, _v, FLT_MAX, d);
                    if (dist < dist_min)
                    {
                        dist_min = dist;
                        found    = true;
                    }
                }

                if (processed_[v1] && processed_[vv])
                {
                    dist = distance(vv, v1, _v, d, FLT_MAX);
                    if (dist < dist_min)
                    {
                        dist_min = dist;
                        found    = true;
                    }
                }
            }
        }
    }



    // update priority queue
    if (found)
    {
        if (distance_[_v] != FLT_MAX)
        {
            auto it = front_->find(_v);
            assert(it != front_->end());
            front_->erase(it);
        }

        distance_[_v] = dist_min;
        front_->insert(_v);
    }
    else
    {
        if (distance_[_v] != FLT_MAX)
        {
            front_->erase(_v);
            distance_[_v] = FLT_MAX;
        }
    }
}


//-----------------------------------------------------------------------------


bool valid_triangle(double a, double b, double c) 
{ 
    return (a + b > c && a + c > b && b + c > a); 
}


//-----------------------------------------------------------------------------


Scalar SurfaceGeodesic::distance(Vertex _v0, Vertex _v1, Vertex _v2,
                                 Scalar _r0, Scalar _r1)
{
    Point  A, B, C;
    double TA, TB;
    double a, b;

    // choose points such that TB>TA and hence u>0
    if (distance_[_v0] < distance_[_v1])
    {
        A  = mesh_.position(_v0);
        B  = mesh_.position(_v1);
        C  = mesh_.position(_v2);
        TA = distance_[_v0];
        TB = distance_[_v1];
        a  = _r1 == FLT_MAX ? pmp::distance(B,C) : _r1;
        b  = _r0 == FLT_MAX ? pmp::distance(A,C) : _r0;
    }
    else
    {
        A  = mesh_.position(_v1);
        B  = mesh_.position(_v0);
        C  = mesh_.position(_v2);
        TA = distance_[_v1];
        TB = distance_[_v0];
        a  = _r0 == FLT_MAX ? pmp::distance(B,C) : _r0;
        b  = _r1 == FLT_MAX ? pmp::distance(A,C) : _r1;
    }


    // Dykstra: propagate along edges
    const double dykstra = std::min(TA+b, TB+a);


    // obtuse angle -> fall back to Dykstra
    const double c = dot(normalize(A-C), normalize(B-C)); // cosine
    if (c < 0.0) return dykstra;


    // Novotny: intersect two circles
    // use Novotny when distances are not too large
    const double l  = pmp::distance(A, B);
    if (std::max(TA,TB)/l < 10)
    {
        if (valid_triangle(l, a, b) && valid_triangle(l, TA, TB))
        {
            const double x2 = 0.5f * (l*l + b*b - a*a) / l;
            const double y2 = sqrt(b*b - x2*x2);
            const double x  = 0.5f * (l*l + TA*TA - TB*TB) / l;
            const double y  = -sqrt(TA*TA - x*x);
            return pmp::distance(dvec2(x2,y2), dvec2(x,y));
        }
    }


    // Kimmel: solve quadratic equation
    const double u  = TB - TA;
    const double aa = a*a + b*b - 2.0*a*b*c;
    const double bb = 2.0*b*u*(a*c-b);
    const double cc = b*b*(u*u-a*a*(1.0-c*c)); 
    const double dd = bb*bb - 4.0*aa*cc;
    if (dd > 0.0) 
    {
        const double t1 = (-bb + sqrt(dd)) / (2.0*aa);
        const double t2 = (-bb - sqrt(dd)) / (2.0*aa);
        const double  t = std::max(t1,t2);
        const double  q = b*(t-u)/t;
        if ( (u < t) && (a*c < q) && (q < a/c) )
        {
            return TA + t;
        }
    }


    // use Dykstra as fall-back
    return dykstra;
}


//-----------------------------------------------------------------------------


void SurfaceGeodesic::distanceToTextureCoordinates()
{
    // find maximum distance
    Scalar maxdist(0);
    for (auto v: mesh_.vertices())
    {
        if (distance_[v] <= maxdist_)
        {
            maxdist = std::max(maxdist, distance_[v]);
        }
    }

    auto tex = mesh_.vertexProperty<TextureCoordinate>("v:tex");
    for (auto v : mesh_.vertices())
    {
        if (distance_[v] <= maxdist_)
        {
            tex[v] = TextureCoordinate( distance_[v] / maxdist, 0.0 );
        }
        else
        {
            tex[v] = TextureCoordinate( 1.0, 0.0 );
        }
    }
}


//=============================================================================
} // namespace pmp
//=============================================================================
