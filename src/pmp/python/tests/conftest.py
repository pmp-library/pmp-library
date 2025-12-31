import pytest
import math
import numpy as np

from pypmp import *
from pypmp.algorithms import subdivision
from pypmp.algorithms import features

@pytest.fixture
def vertex():
    return Vertex(3)

@pytest.fixture
def point_scalar():
    return Point(1)

@pytest.fixture
def surface_mesh():
    return SurfaceMesh()

@pytest.fixture()
def surface_mesh_with_face():
    mesh = SurfaceMesh()
    points = [
        Point(0),
        Point(1,0,0),
        Point(0,1,0)
    ]
    vertices = [mesh.add_vertex(p) for p in points]

    mesh.add_triangle(vertices)
    return mesh

@pytest.fixture
def four_points():
    return [
        Point(0),
        Point(1,0,0),
        Point(0,1,0),
        Point(0,0,1)
    ]

@pytest.fixture
def quad_face(surface_mesh):
    points = [
        Point(0),
        Point(1,0,0),
        Point(0,1,0),
        Point(1,1,0)
    ]
    vertices = [surface_mesh.add_vertex(p) for p in points]
    _ = surface_mesh.add_quad(vertices)
    return surface_mesh

@pytest.fixture
def vertex_onering():
    mesh = SurfaceMesh()

    v0 = mesh.add_vertex(Point(0.4499998093, 0.5196152329, 0.0000000000))
    v1 = mesh.add_vertex(Point(0.2999998033, 0.5196152329, 0.0000000000))
    v2 = mesh.add_vertex(Point(0.5249998569, 0.3897114396, 0.0000000000))
    v3 = mesh.add_vertex(Point(0.3749998510, 0.3897114396, 0.0000000000))
    v4 = mesh.add_vertex(Point(0.2249998450, 0.3897114396, 0.0000000000))
    v5 = mesh.add_vertex(Point(0.4499999285, 0.2598076165, 0.0000000000))
    v6 = mesh.add_vertex(Point(0.2999999225, 0.2598076165, 0.0000000000))

    mesh.add_triangle(v3, v0, v1)
    mesh.add_triangle(v3, v2, v0)
    mesh.add_triangle(v4, v3, v1)
    mesh.add_triangle(v5, v2, v3)
    mesh.add_triangle(v6, v5, v3)
    mesh.add_triangle(v6, v3, v4)

    return mesh

@pytest.fixture
def edge_onering():
    mesh = SurfaceMesh()

    v0 = mesh.add_vertex(Point(0.5999997854, 0.5196152329, 0.0000000000))
    v1 = mesh.add_vertex(Point(0.4499998093, 0.5196152329, 0.0000000000))
    v2 = mesh.add_vertex(Point(0.2999998033, 0.5196152329, 0.0000000000))
    v3 = mesh.add_vertex(Point(0.6749998331, 0.3897114396, 0.0000000000))
    v4 = mesh.add_vertex(Point(0.5249998569, 0.3897114396, 0.0000000000))
    v5 = mesh.add_vertex(Point(0.3749998510, 0.3897114396, 0.0000000000))
    v6 = mesh.add_vertex(Point(0.2249998450, 0.3897114396, 0.0000000000))
    v7 = mesh.add_vertex(Point(0.5999999046, 0.2598076165, 0.0000000000))
    v8 = mesh.add_vertex(Point(0.4499999285, 0.2598076165, 0.0000000000))
    v9 = mesh.add_vertex(Point(0.2999999225, 0.2598076165, 0.0000000000))

    mesh.add_triangle(v4, v0, v1)
    mesh.add_triangle(v4, v3, v0)
    mesh.add_triangle(v5, v1, v2)
    mesh.add_triangle(v5, v4, v1)
    mesh.add_triangle(v6, v5, v2)
    mesh.add_triangle(v7, v3, v4)
    mesh.add_triangle(v8, v7, v4)
    mesh.add_triangle(v8, v4, v5)
    mesh.add_triangle(v9, v8, v5)
    mesh.add_triangle(v9, v5, v6)

    return mesh

def project_to_unit_sphere(mesh: SurfaceMesh):
    for v in mesh.vertices():
        p = mesh.position(v)
        n = np.linalg.norm(p)
        p = (1.0 / n) * p

@pytest.fixture
def edge_onering():
    mesh = SurfaceMesh()

    v0 = mesh.add_vertex(Point(0.5999997854, 0.5196152329, 0.0))
    v1 = mesh.add_vertex(Point(0.4499998093, 0.5196152329, 0.0))
    v2 = mesh.add_vertex(Point(0.2999998033, 0.5196152329, 0.0))
    v3 = mesh.add_vertex(Point(0.6749998331, 0.3897114396, 0.0))
    v4 = mesh.add_vertex(Point(0.5249998569, 0.3897114396, 0.0))
    v5 = mesh.add_vertex(Point(0.3749998510, 0.3897114396, 0.0))
    v6 = mesh.add_vertex(Point(0.2249998450, 0.3897114396, 0.0))
    v7 = mesh.add_vertex(Point(0.5999999046, 0.2598076165, 0.0))
    v8 = mesh.add_vertex(Point(0.4499999285, 0.2598076165, 0.0))
    v9 = mesh.add_vertex(Point(0.2999999225, 0.2598076165, 0.0))

    mesh.add_triangle(v4, v0, v1)
    mesh.add_triangle(v4, v3, v0)
    mesh.add_triangle(v5, v1, v2)
    mesh.add_triangle(v5, v4, v1)
    mesh.add_triangle(v6, v5, v2)
    mesh.add_triangle(v7, v3, v4)
    mesh.add_triangle(v8, v7, v4)
    mesh.add_triangle(v8, v4, v5)
    mesh.add_triangle(v9, v8, v5)
    mesh.add_triangle(v9, v5, v6)

    return mesh

@pytest.fixture
def icosahedron():
    mesh = SurfaceMesh()

    phi = (1.0 + math.sqrt(5.0)) * 0.5  # golden ratio
    a = 1.0
    b = 1.0 / phi

    v1  = mesh.add_vertex(Point(0,  b, -a))
    v2  = mesh.add_vertex(Point(b,  a,  0))
    v3  = mesh.add_vertex(Point(-b, a,  0))
    v4  = mesh.add_vertex(Point(0,  b,  a))
    v5  = mesh.add_vertex(Point(0, -b,  a))
    v6  = mesh.add_vertex(Point(-a, 0,  b))
    v7  = mesh.add_vertex(Point(0, -b, -a))
    v8  = mesh.add_vertex(Point(a,  0, -b))
    v9  = mesh.add_vertex(Point(a,  0,  b))
    v10 = mesh.add_vertex(Point(-a, 0, -b))
    v11 = mesh.add_vertex(Point(b, -a,  0))
    v12 = mesh.add_vertex(Point(-b, -a, 0))

    project_to_unit_sphere(mesh)

    mesh.add_triangle(v3,  v2,  v1)
    mesh.add_triangle(v2,  v3,  v4)
    mesh.add_triangle(v6,  v5,  v4)
    mesh.add_triangle(v5,  v9,  v4)
    mesh.add_triangle(v8,  v7,  v1)
    mesh.add_triangle(v7,  v10, v1)
    mesh.add_triangle(v12, v11, v5)
    mesh.add_triangle(v11, v12, v7)
    mesh.add_triangle(v10, v6,  v3)
    mesh.add_triangle(v6,  v10, v12)
    mesh.add_triangle(v9,  v8,  v2)
    mesh.add_triangle(v8,  v9,  v11)
    mesh.add_triangle(v3,  v6,  v4)
    mesh.add_triangle(v9,  v2,  v4)
    mesh.add_triangle(v10, v3,  v1)
    mesh.add_triangle(v2,  v8,  v1)
    mesh.add_triangle(v12, v10, v7)
    mesh.add_triangle(v8,  v11, v7)
    mesh.add_triangle(v6,  v12, v5)
    mesh.add_triangle(v11, v9,  v5)

    return mesh


@pytest.fixture
def subdivided_icosahedron(icosahedron):
    mesh = icosahedron

    # select all edges as features
    features.detect_features(mesh, 25)

    # feature-preserving subdivision
    subdivision.loop_subdivision(mesh)
    subdivision.loop_subdivision(mesh)
    subdivision.loop_subdivision(mesh)

    return mesh

@pytest.fixture
def l_shape():
    mesh = SurfaceMesh()

    vertices = []

    vertices.append(mesh.add_vertex(Point(0.0, 0.0, 0.0)))
    vertices.append(mesh.add_vertex(Point(0.5, 0.0, 0.0)))
    vertices.append(mesh.add_vertex(Point(1.0, 0.0, 0.0)))
    vertices.append(mesh.add_vertex(Point(1.0, 0.5, 0.0)))
    vertices.append(mesh.add_vertex(Point(0.5, 0.5, 0.0)))
    vertices.append(mesh.add_vertex(Point(0.5, 1.0, 0.0)))
    vertices.append(mesh.add_vertex(Point(0.5, 1.5, 0.0)))
    vertices.append(mesh.add_vertex(Point(0.5, 2.0, 0.0)))
    vertices.append(mesh.add_vertex(Point(0.0, 2.0, 0.0)))
    vertices.append(mesh.add_vertex(Point(0.0, 1.5, 0.0)))
    vertices.append(mesh.add_vertex(Point(0.0, 1.0, 0.0)))
    vertices.append(mesh.add_vertex(Point(0.0, 0.5, 0.0)))

    mesh.add_face(vertices)

    return mesh

# Factory cone fixture
@pytest.fixture
def cone():
    def _cone(
        n_subdivisions: int,
        radius: float,
        height: float
    ):
        if n_subdivisions < 3:
            raise ValueError("n_subdivisions must be >= 3.")
        mesh = SurfaceMesh()

        base_vertices = []

        for i in range(n_subdivisions):
            ratio = i/n_subdivisions
            r = ratio * 2 * math.pi
            x = math.cos(r) * radius
            y = math.sin(r) * radius
            v = mesh.add_vertex(Point(x, y, 0))
            base_vertices.append(v)

        # add the tip of the cone
        v0 = mesh.add_vertex(Point(0., 0., height))

        # generate triangular faces
        for i in range(n_subdivisions):
            ii = (i + 1) % n_subdivisions
            mesh.add_triangle(v0, Vertex(i), Vertex(ii))

        # reverse order for consistent face orientation
        base_vertices.reverse()

        # add polygonal base face
        mesh.add_face(base_vertices)

        return mesh
        
    return _cone

@pytest.fixture
def open_cone(cone):
    mesh = cone(8, 1, 1.5)

    for f in mesh.faces():
        if mesh.valence(f) > 3:
            mesh.delete_face(f)
            mesh.garbage_collection()
            break
    
    return mesh

@pytest.fixture
def texture_seams_mesh():
    mesh = SurfaceMesh()

    v0  = mesh.add_vertex(Point(0.5999997854, 0.5196152329, 0.0))
    v1  = mesh.add_vertex(Point(0.4499998093, 0.5196152329, -0.001))
    v2  = mesh.add_vertex(Point(0.2999998033, 0.5196152329, 0.0))
    v3  = mesh.add_vertex(Point(0.6749998331, 0.3897114396, -0.001))
    v4  = mesh.add_vertex(Point(0.5249998569, 0.3897114396, 0.0))
    v5  = mesh.add_vertex(Point(0.3749998510, 0.3897114396, 0.0))
    v6  = mesh.add_vertex(Point(0.2249998450, 0.3897114396, 0.0))
    v7  = mesh.add_vertex(Point(0.5999999046, 0.2598076165, 0.0))
    v8  = mesh.add_vertex(Point(0.4499999285, 0.2598076165, 0.0))
    v9  = mesh.add_vertex(Point(0.2999999225, 0.2598076165, 0.0))
    v10 = mesh.add_vertex(Point(0.749999285, 0.2598076165, 0.0))
    v11 = mesh.add_vertex(Point(0.8249998331, 0.3897114396, 0.0))
    v12 = mesh.add_vertex(Point(0.749999285, 0.5196152329, 0.0))
    v13 = mesh.add_vertex(Point(0.6749998331, 0.6496152329, 0.0))
    v14 = mesh.add_vertex(Point(0.5249998569, 0.6496152329, 0.0))
    v15 = mesh.add_vertex(Point(0.3749998510, 0.6496152329, 0.0))

    mesh.add_triangle(v4,  v0,  v1)
    mesh.add_triangle(v4,  v3,  v0)
    mesh.add_triangle(v15, v4,  v1)
    mesh.add_triangle(v2,  v5,  v4)
    mesh.add_triangle(v6,  v5,  v2)
    mesh.add_triangle(v7,  v11, v4)
    mesh.add_triangle(v8,  v7,  v4)
    mesh.add_triangle(v8,  v4,  v5)
    mesh.add_triangle(v9,  v8,  v5)
    mesh.add_triangle(v9,  v5,  v6)
    mesh.add_triangle(v7,  v10, v11)
    mesh.add_triangle(v4,  v11, v3)
    mesh.add_triangle(v3,  v11, v12)
    mesh.add_triangle(v3,  v12, v0)
    mesh.add_triangle(v0,  v12, v13)
    mesh.add_triangle(v0,  v13, v14)
    mesh.add_triangle(v0,  v14, v1)
    mesh.add_triangle(v1,  v14, v15)
    mesh.add_triangle(v2,  v4,  v15)

    # add test texcoords
    texcoords = mesh.halfedge_vector2d_property("h:tex", TexCoord(0))

    for v in mesh.vertices():
        p = mesh.position(v)
        for h in mesh.halfedges(v):
            if mesh.is_boundary(mesh.opposite_halfedge(h)):
                continue
            texcoords[mesh.opposite_halfedge(h)] = TexCoord(p[0], p[1])

    faces = [
        Face(0), 
        Face(1), 
        Face(12), 
        Face(13),
        Face(14), 
        Face(15), 
        Face(16), 
        Face(17)
    ]

    for f in faces:
        for h in mesh.halfedges(f):
            texcoords[h] += TexCoord(0.1, 0.1)

    return mesh

@pytest.fixture
def hexahedron():
    mesh = SurfaceMesh()

    a = 1.0 / math.sqrt(3.0)

    v0 = mesh.add_vertex(Point(-a, -a, -a))
    v1 = mesh.add_vertex(Point(a, -a, -a))
    v2 = mesh.add_vertex(Point(a, a, -a))
    v3 = mesh.add_vertex(Point(-a, a, -a))
    v4 = mesh.add_vertex(Point(-a, -a, a))
    v5 = mesh.add_vertex(Point(a, -a, a))
    v6 = mesh.add_vertex(Point(a, a, a))
    v7 = mesh.add_vertex(Point(-a, a, a))

    mesh.add_quad(v3, v2, v1, v0)
    mesh.add_quad(v2, v6, v5, v1)
    mesh.add_quad(v5, v6, v7, v4)
    mesh.add_quad(v0, v4, v7, v3)
    mesh.add_quad(v3, v7, v6, v2)
    mesh.add_quad(v1, v5, v4, v0)

    return mesh

@pytest.fixture
def tetrahedron():
    mesh = SurfaceMesh()

    a = 1.0 / 3.0
    b = math.sqrt(8.0 / 9.0)
    c = math.sqrt(2.0 / 9.0)
    d = math.sqrt(2.0 / 3.0)

    v0 = mesh.add_vertex(Point(0, 0, 1))
    v1 = mesh.add_vertex(Point(-c, d, -a))
    v2 = mesh.add_vertex(Point(-c, -d, -a))
    v3 = mesh.add_vertex(Point(b, 0, -a))

    mesh.add_triangle(v0, v1, v2)
    mesh.add_triangle(v0, v2, v3)
    mesh.add_triangle(v0, v3, v1)
    mesh.add_triangle(v3, v2, v1)

    return mesh

@pytest.fixture
def plane():
    def _plane(resolution: int):
        if resolution < 1:
            raise ValueError("resolution must be >= 1")
        
        mesh = SurfaceMesh()

        # Generate vertices
        p = Point(0)
        for i in range(resolution + 1):
            for j in range(resolution + 1):
                mesh.add_vertex(p)
                p[1] += 1/resolution
            p[1] = 0
            p[0] += 1/resolution

        # Generate faces
        for i in range(resolution):
            for j in range(resolution):
                v0 = Vertex(j + i * (resolution + 1))
                v1 = Vertex(v0.idx() + resolution + 1)
                v2 = Vertex(v0.idx() + resolution + 2)
                v3 = Vertex(v0.idx() + 1)
                mesh.add_quad(v0, v1, v2, v3)

        return mesh
    
    return _plane
    

# Add more shapes from algorithms::shapes