import pytest
import math
import numpy as np
import os
from pathlib import Path

from pypmp import *
from pypmp.algorithms import subdivision
from pypmp.algorithms import features
from pypmp.algorithms import differential_geometry

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
        Point(1,1,0),
        Point(0,1,0)
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
def one_ring(vertex_onering):
    # the central vertex
    central_vertex = Vertex(3)
    # lift central vertex
    vertex_onering.position(central_vertex)[2] = 0.1
    # Check the value is really written
    assert np.allclose(vertex_onering.positions()[central_vertex.idx()][2], 0.1)
    return vertex_onering

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
        mesh.position(v)[0]=p[0]
        mesh.position(v)[1]=p[1]
        mesh.position(v)[2]=p[2]
        assert np.allclose(p, mesh.position(v))

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
    features.detect_features(mesh, 25.)

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
    
@pytest.fixture
def octahedron(hexahedron):
    mesh = hexahedron
    differential_geometry.dual(mesh)
    project_to_unit_sphere(mesh)
    return mesh

@pytest.fixture
def dodecahedron(icosahedron):
    mesh = icosahedron
    differential_geometry.dual(mesh)
    project_to_unit_sphere(mesh)
    return mesh

@pytest.fixture
def icosphere(icosahedron):
    def _icosphere(n_subdivisions: int = 3):
        for i in range(n_subdivisions):
            subdivision.loop_subdivision(icosahedron)
            project_to_unit_sphere(icosahedron)
        return icosahedron
    
    return _icosphere

@pytest.fixture
def quad_sphere(hexahedron):
    def _quad_sphere(n_subdivisions: int = 3):
        for i in range(n_subdivisions):
            subdivision.catmull_clark_subdivision(hexahedron)
            project_to_unit_sphere(hexahedron)
        return hexahedron
    
    return _quad_sphere

@pytest.fixture
def uv_sphere():
    def _uv_sphere(
        center: Point = Point(0, 0, 0), 
        radius: float = 1.0, 
        n_slices: int = 15, 
        n_stacks: int = 15
    ):
        mesh = SurfaceMesh()
        # add top vertex
        top = Point(center[0], center[1] + radius, center[2])
        v0 = mesh.add_vertex(top)

        # generate vertices per stack / slice
        for i in range(n_stacks-1):
            phi = np.pi * (i+1) / n_stacks
            for j in range(n_slices):
                theta = 2 * np.pi * j / n_slices

                x = center[0] + radius * np.sin(phi) * np.cos(theta)
                y = center[1] + radius * np.cos(phi)
                z = center[2] + radius * np.sin(phi) * np.sin(theta)
                
                mesh.add_vertex(Point(x, y, z))

        # add bottom vertex
        bottom = Point(center[0], center[1] - radius, center[2])
        v1 = mesh.add_vertex(bottom)

        # add top / bottom triangles
        for i in range(n_slices):
            i0 = i + 1
            i1 = (i + 1) % n_slices + 1
            mesh.add_triangle(v0, Vertex(i1), Vertex(i0))

            i2 = i + n_slices * (n_stacks - 2) + 1
            i3 = (i + 1) % n_slices + n_slices * (n_stacks - 2) + 1
            mesh.add_triangle(v1, Vertex(i2), Vertex(i3))

        for j in range(n_stacks - 2):
            idx0 = j * n_slices + 1
            idx1 = (j + 1) * n_slices + 1
            for i in range(n_slices):
                i0 = idx0 + i
                i1 = idx0 + (i + 1) % n_slices
                i2 = idx1 + (i + 1) % n_slices
                i3 = idx1 + i
                mesh.add_quad(Vertex(i0), Vertex(i1), Vertex(i2), Vertex(i3))
        
        return mesh

    return _uv_sphere

@pytest.fixture
def plane():
    def _plane(resolution: int = 4):
        if resolution < 1:
            raise ValueError("Resolution must be greater than 1.")
        
        mesh = SurfaceMesh()

        # generate vertices
        p = Point(0)
        for i in range(resolution + 1):
            for j in range(resolution + 1):
                mesh.add_vertex(p)
                p[1] += 1. / resolution
            p[1] = 0
            p[0] += 1. /resolution

        # generate faces
        for i in range(resolution):
            for j in range(resolution):
                v0 = Vertex(j + i * (resolution + 1))
                v1 = Vertex(v0.idx() + resolution + 1)
                v2 = Vertex(v0.idx() + resolution + 2)
                v3 = Vertex(v0.idx() + 1)
                mesh.add_quad(v0, v1, v2, v3)

        return mesh
    
    return _plane

@pytest.fixture
def cone():
    def _cone(
        n_subdivisions: int = 30,
        radius: float = 1.0,
        height: float = 2.5
    ):
        if n_subdivisions < 3:
            raise ValueError("Minimum allowed subdivisions are 3.")
    
        mesh = SurfaceMesh()
        # add vertices subdividing a circle
        base_vertices = []

        for i in range(n_subdivisions):
            ratio = i/n_subdivisions
            r = ratio * (np.pi * 2)
            x = np.cos(r) * radius
            y = np.sin(r) * radius
            v = mesh.add_vertex(Point(x, y, 0.0))
            base_vertices.append(v)

        # add the tip of the cone
        v0 = mesh.add_vertex(Point(0, 0, height))

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
def cylinder():
    def _cylinder(
        n_subdivisions: int = 30,
        radius: float = 1.0,
        height: float = 2.5
    ):
        if n_subdivisions < 3:
            raise ValueError("Minimum allowed subdivisions are 3.")
        
        mesh = SurfaceMesh()

        # generate vertices
        bottom_vertices = []
        top_vertices = []

        for i in range(n_subdivisions):
            ratio = i / n_subdivisions
            r = ratio * (np.pi * 2)
            x = np.cos(r) * radius
            y = np.sin(r) * radius
            v = mesh.add_vertex(Point(x, y, 0.0))
            bottom_vertices.append(v)
            v = mesh.add_vertex(Point(x, y, height))
            top_vertices.append(v)

        # add faces around the cylinder
        for i in range(n_subdivisions):
            ii = i * 2
            jj = (ii + 2) % (n_subdivisions * 2)
            kk = (ii + 3) % (n_subdivisions * 2)
            ll = ii + 1
            mesh.add_quad(Vertex(ii), Vertex(jj), Vertex(kk), Vertex(ll))
        
        # add top polygon
        mesh.add_face(top_vertices)

        # reverse order for consistent face orientation
        bottom_vertices.reverse()

        mesh.add_face(bottom_vertices)

        return mesh
    
    return _cylinder

@pytest.fixture
def torus():
    def _torus(
        radial_resolution: int = 20,
        tubular_resolution: int = 40,
        radius: float = 1.0,
        thickness: float = 0.4
    ):
        if radial_resolution < 3:
            raise ValueError("Minimum allowed resolution is 3.")
        if tubular_resolution < 3:
            raise ValueError("Minimum allowed resolution is 3.")
        
        mesh = SurfaceMesh()

        # generate vertices
        for i in range(radial_resolution):
            for j in range(tubular_resolution):
                u = j / tubular_resolution * 2 * np.pi
                v = i / radial_resolution * 2 * np.pi

                x = (radius + thickness * np.cos(v)) * np.cos(u)
                y = (radius + thickness * np.cos(v)) * np.sin(u)
                z = thickness * np.sin(v)

                mesh.add_vertex(Point(x, y, z))

        # add quad faces
        for i in range(radial_resolution):
            i_next = (i + 1) % radial_resolution
            for j in range(tubular_resolution):
                j_next = (j + 1) % tubular_resolution
                i0 = i * tubular_resolution + j
                i1 = i * tubular_resolution + j_next
                i2 = i_next * tubular_resolution + j_next
                i3 = i_next * tubular_resolution + j
                mesh.add_quad(Vertex(i0), Vertex(i1), Vertex(i2), Vertex(i3))

        return mesh

    return _torus

@pytest.fixture
def degenerate_triangle():
    mesh = SurfaceMesh()
    points = [
        Point(0, 0, 0),
        Point(1, 0, 0),
        Point(0, 0, 0)
    ]
    vertices = [mesh.add_vertex(p) for p in points]
    _ = mesh.add_triangle(*vertices)

    return mesh

@pytest.fixture
def data_path():
    path = Path(os.path.abspath(__file__))
    return path.parent.parent.parent.parent.parent / "data"
