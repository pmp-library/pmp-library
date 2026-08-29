import pytest
import numpy as np
from pypmp import *
from numpy.linalg import norm

def vertices_on_sphere(mesh: SurfaceMesh):
    for v in mesh.vertices():
        n = norm(mesh.position(v))
        if n < 0.999 or n > 1.001:
            return False
        
    return True

def test_tetrahedron(tetrahedron):
    assert tetrahedron.n_vertices() == 4
    assert tetrahedron.n_edges() == 6
    assert tetrahedron.n_faces() == 4
    assert tetrahedron.is_triangle_mesh()
    assert vertices_on_sphere(tetrahedron)

def test_octahedron(octahedron):
    assert octahedron.n_vertices() == 6
    assert octahedron.n_edges() == 12
    assert octahedron.n_faces() == 8
    assert octahedron.is_triangle_mesh()
    assert vertices_on_sphere(octahedron)

def test_hexahedron(hexahedron):
    assert hexahedron.n_vertices() == 8
    assert hexahedron.n_edges() == 12
    assert hexahedron.n_faces() == 6
    assert hexahedron.is_quad_mesh()
    assert vertices_on_sphere(hexahedron)

def test_icosahedron(icosahedron):
    assert icosahedron.n_vertices() == 12
    assert icosahedron.n_edges() == 30
    assert icosahedron.n_faces() == 20
    assert icosahedron.is_triangle_mesh()
    assert vertices_on_sphere(icosahedron)

def test_dodecahedron(dodecahedron):
    assert dodecahedron.n_vertices() == 20
    assert dodecahedron.n_edges() == 30
    assert dodecahedron.n_faces() == 12
    assert dodecahedron.is_quad_mesh() == False
    assert dodecahedron.is_triangle_mesh() == False
    assert vertices_on_sphere(dodecahedron)

def test_icosphere(icosphere):
    icosphere = icosphere()
    assert icosphere.n_vertices() == 642
    assert icosphere.n_edges() == 1920
    assert icosphere.n_faces() == 1280
    assert icosphere.is_triangle_mesh()
    assert vertices_on_sphere(icosphere)

def test_quad_sphere(quad_sphere):
    quad_sphere = quad_sphere()
    assert quad_sphere.n_vertices() == 386
    assert quad_sphere.n_edges() == 768
    assert quad_sphere.n_faces() == 384
    assert quad_sphere.is_quad_mesh()
    assert vertices_on_sphere(quad_sphere)

def test_uv_sphere(uv_sphere):
    uv_sphere = uv_sphere()
    assert uv_sphere.n_vertices() == 212
    assert uv_sphere.n_edges() == 435
    assert uv_sphere.n_faces() == 225
    assert uv_sphere.is_quad_mesh() == False
    assert uv_sphere.is_triangle_mesh() == False
    assert vertices_on_sphere(uv_sphere)

def test_plane_minimal(plane):
    mesh = plane(1)
    assert mesh.n_vertices() == 4
    assert mesh.n_edges() == 4
    assert mesh.n_faces() == 1
    assert mesh.is_quad_mesh()

def test_plane_planar(plane):
    mesh = plane()
    z_sum = 0
    for v in mesh.vertices():
        z_sum += mesh.position(v)[2]
    assert z_sum == 0

def test_plane_default(plane):
    mesh = plane()
    assert mesh.n_vertices() == 25
    assert mesh.n_edges() == 40
    assert mesh.n_faces() == 16

def test_cylinder(cylinder):
    mesh = cylinder(3)
    assert mesh.n_vertices() == 6
    assert mesh.n_edges() == 9
    assert mesh.n_faces() == 5

def test_torus(torus):
    mesh = torus()
    assert mesh.n_vertices() == 800
    assert mesh.n_edges() == 1600
    assert mesh.n_faces() == 800
    assert mesh.is_quad_mesh()
