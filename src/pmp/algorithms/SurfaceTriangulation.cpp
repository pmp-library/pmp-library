//=============================================================================

#include <pmp/algorithms/SurfaceTriangulation.h>

//=============================================================================

namespace pmp {

//=============================================================================

SurfaceTriangulation::SurfaceTriangulation(SurfaceMesh& mesh) : mesh_(mesh)
{
    points_    = mesh_.vertex_property<Point>("v:point");
}

//-----------------------------------------------------------------------------

void SurfaceTriangulation::triangulate(Objective o)
{
    for (auto f: mesh_.faces())
        triangulate(f, o);

    std::cout << "Triangle mesh? " << (mesh_.is_triangle_mesh() ? "yes" : "no") << std::endl;
}

//-----------------------------------------------------------------------------

void SurfaceTriangulation::triangulate(Face f, Objective o)
{
    // store objective
    objective_ = o;


    // collect polygon halfedges
    Halfedge h0 = mesh_.halfedge(f);
    halfedges_.clear();
    vertices_.clear();
    Halfedge h = h0;
    do
    {
        if (!mesh_.is_manifold(mesh_.to_vertex(h)))
        {
            //std::cout << "[SurfaceTriangulation] Non-manifold polygon\n";
            return;
        }

        halfedges_.push_back(h);
        vertices_.push_back(mesh_.to_vertex(h));
    } 
    while ((h = mesh_.next_halfedge(h)) != h0);

    // do we have at least four vertices?
    const int n = halfedges_.size();
    if (n <= 3) return;

    // compute minimal triangulation by dynamic programming
    weight_.clear();
    weight_.resize(n, std::vector<Weight>(n, Weight()));
    index_.clear();
    index_.resize(n, std::vector<int>(n, 0));

    int i, j, m, k, imin;
    Weight w, wmin;

    // initialize 2-gons
    for (i = 0; i < n - 1; ++i)
    {
        weight_[i][i + 1] = Weight(0,0);
        index_[i][i + 1] = -1;
    }

    // n-gons with n>2
    for (j = 2; j < n; ++j)
    {
        //PMP_SHOW(j);

        // for all n-gons [i,i+j]
        for (i = 0; i < n - j; ++i)
        {
            k = i + j;
            wmin = Weight();
            imin = -1;

            // find best split i < m < i+j
            for (m = i + 1; m < k; ++m)
            {
                w = weight_[i][m] + compute_weight(i, m, k) + weight_[m][k];
                if (w < wmin)
                {
                    wmin = w;
                    imin = m;
                }
            }

            weight_[i][k] = wmin;
            index_[i][k] = imin;
            //std::cout << "index[" << i << ", " << k << "] = " << imin << std::endl;
        }
    }

    // now add triangles to mesh
    std::vector<ivec2> todo;
    todo.reserve(n);
    todo.push_back(ivec2(0, n - 1));
    while (!todo.empty())
    {
        ivec2 tri = todo.back();
        todo.pop_back();
        int start = tri[0];
        int end = tri[1];
        if (end - start < 2)
            continue;
        int split = index_[start][end];

        insert_edge(start, split);
        insert_edge(split, end);

        todo.push_back(ivec2(start, split));
        todo.push_back(ivec2(split, end));
    }


    // clean up
    weight_.clear();
    index_.clear();
    halfedges_.clear();
    vertices_.clear();
}

//-----------------------------------------------------------------------------

SurfaceTriangulation::Weight SurfaceTriangulation::compute_weight(int i, int j, int k) const
{
    const Vertex a = vertices_[i];
    const Vertex b = vertices_[j];
    const Vertex c = vertices_[k];
    Vertex d;

    // if one of the potential edges already exists as NON-boundary edge
    // this would result in an invalid triangulation
    // -> prevent by giving infinite weight
    // (this happens for suzanne.obj!)
    //if (is_interior_edge(a, b) || 
        //is_interior_edge(b, c) ||
        //is_interior_edge(c, a))
        //return FLT_MAX;
    //if (is_edge(a,b) && is_edge(b,c) && is_edge(c,a))
        //return FLT_MAX;

    // compute area
    const Scalar area = compute_area(a, b, c);

    // compute dihedral angles with...
    Scalar angle(0);
    const Point n = compute_normal(a, b, c);

    // ...neighbor to (i,j)
    if (i+1 != j)
    {
        d = hole_vertex(index_[i][j]);
        angle = std::max(angle, compute_angle(n, compute_normal(a, d, b)));
    }

    // ...neighbor to (j,k)
    if (j+1 != k)
    {
        d = hole_vertex(index_[j][k]);
        angle = std::max(angle, compute_angle(n, compute_normal(b, d, c)));
    }

    return Weight(angle, area);
}

//-----------------------------------------------------------------------------

Scalar SurfaceTriangulation::compute_area(Vertex _a, Vertex _b, Vertex _c) const
{
    return sqrnorm(cross(points_[_b] - points_[_a], points_[_c] - points_[_a]));
}

//-----------------------------------------------------------------------------

Point SurfaceTriangulation::compute_normal(Vertex _a, Vertex _b, Vertex _c) const
{
    return normalize(
        cross(points_[_b] - points_[_a], points_[_c] - points_[_a]));
}

//-----------------------------------------------------------------------------

Scalar SurfaceTriangulation::compute_angle(const Point& _n1,
                                         const Point& _n2) const
{
    return (1.0 - dot(_n1, _n2));
}

//-----------------------------------------------------------------------------

bool SurfaceTriangulation::is_edge(Vertex a, Vertex b) const
{
    return mesh_.find_halfedge(a, b).is_valid();
}

//-----------------------------------------------------------------------------

bool SurfaceTriangulation::is_interior_edge(Vertex a, Vertex b) const
{
    Halfedge h = mesh_.find_halfedge(a, b);
    if (!h.is_valid())
        return false; // edge does not exist
    return (!mesh_.is_boundary(h) &&
            !mesh_.is_boundary(mesh_.opposite_halfedge(h)));
}

//-----------------------------------------------------------------------------

bool SurfaceTriangulation::insert_edge(int i, int j)
{
    Halfedge h0 = halfedges_[i];
    Halfedge h1 = halfedges_[j];
    Vertex   v0 = vertices_[i];
    Vertex   v1 = vertices_[j];

    // does edge already exist?
    if (mesh_.find_halfedge(v0, v1).is_valid())
    {
        return false;
    }

    // can we reach v1 from h0?
    {
        Halfedge h = h0;
        do {
            h = mesh_.next_halfedge(h);
            if (mesh_.to_vertex(h) == v1)
            {
                mesh_.insert_edge(h0, h);
                return true;
            }
        } while (h != h0);
    }

    // can we reach v0 from h1?
    {
        Halfedge h = h1;
        do {
            h = mesh_.next_halfedge(h);
            if (mesh_.to_vertex(h) == v0)
            {
                mesh_.insert_edge(h1, h);
                return true;
            }
        } while (h != h1);
    }

    std::cerr << "[SurfaceTriangulation] This should not happen...\n"; 
    return false;
}

//=============================================================================
}
//=============================================================================
