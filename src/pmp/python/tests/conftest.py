import pytest
from pypmp import *

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