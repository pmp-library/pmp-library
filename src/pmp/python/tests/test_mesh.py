import pytest
import operator as op
from pypmp import *
import numpy as np

@pytest.mark.parametrize(
    ["point"],
    [
        (Vector3D(1, 2, 3), ),
        (Normal(1, 2, 3), ),
        (Color(1, 2, 3), ),
        (Point(1, 2, 3), ),
        (Point([1, 2, 3]), ),
        (Point(np.array([1, 2, 3])), )
    ]
)
def test_point(point):
    assert all([point[i] == i+1 for i in range(2)])
    assert len(point) == 3
    assert point == Point(1, 2, 3)
    point[0] = 3
    point[1] = 3
    assert point == Point(3)
    assert point/3 == Point(1)
    point.normalize()
    assert point == Point(1)/np.sqrt(3)

@pytest.mark.parametrize(
    ["args", "error"],
    [
        ((1, 2), TypeError),
        (([1, 2, 3, 4],), IndexError),
        (([1, 2],), IndexError),
        (([1, 2, "a"],), RuntimeError)
    ]
)
def test_point_errors(args, error):
    try:
        Point(*args)
    except Exception as e:
        assert isinstance(e, error)

def test_point_scalar(point_scalar):
    assert len(point_scalar) == 3
    assert point_scalar == Point(1, 1, 1)

@pytest.mark.parametrize(
    ["scalar", "point", "result"],
    [
        (1, Point(1, 2, 3), Point(1, 2, 3)),
        (2, Point(1, 2, 3), Point(2, 4, 6)),
        (0, Point(1, 2, 3), Point(0, 0, 0)),
        (-1, Point(1, 2, 3), Point(-1, -2, -3)),
        (0, Point(1, 2, 3), Point(0))
    ]
)
def test_point_scalar(scalar, point, result):
    assert point*scalar == result
    assert scalar*point == result

@pytest.mark.parametrize(
    ["p1", "p2", "operator", "p3"],
    [
        (Point(1), Point(2), op.add, Point(3)),
        (Point(2), Point(1), op.sub, Point(1))
    ]
)
def test_point_arithmetic(p1, p2, operator, p3):
    assert operator(p1, p2) == p3

def test_surface_mesh(surface_mesh):
    assert surface_mesh.n_edges() == 0
    assert surface_mesh.n_faces() == 0
    assert surface_mesh.n_vertices() == 0
    assert surface_mesh.is_empty()

@pytest.mark.parametrize(
    ["points", "n_vertices"],
    [
        ([Point(1)], 1),
        ([Point(1), Point(0)], 2)
    ]
)
def test_add_vertex(surface_mesh, points, n_vertices):
    for i, p in enumerate(points):
        v = surface_mesh.add_vertex(p)
        assert type(v) is Vertex
        assert v.idx() == i
    assert surface_mesh.n_vertices() == n_vertices

@pytest.mark.parametrize(
    ["points"],
    [
        ([
            Point(0),
            Point(1,0,0),
            Point(0,1,0)
        ],)
    ]
)
def test_add_triangle_unwrap_points(surface_mesh, points):
    vertices = [surface_mesh.add_vertex(p) for p in points]
    
    f = surface_mesh.add_triangle(*vertices)
    assert type(f) is Face
    assert f.idx() == 0
    assert surface_mesh.is_valid(f)
    assert all([surface_mesh.is_valid(v) for v in vertices])

@pytest.mark.parametrize(
    ["points"],
    [
        ([
            Point(0),
            Point(1,0,0),
            Point(0,1,0)
        ],)
    ]
)
def test_add_triangle(surface_mesh, points):
    vertices = [surface_mesh.add_vertex(p) for p in points]
    
    f = surface_mesh.add_triangle(vertices)
    assert type(f) is Face
    assert f.idx() == 0
    assert surface_mesh.position(Vertex(0)) == Point(0)
    assert np.linalg.norm(surface_mesh.position(Vertex(1))) == 1.
    assert surface_mesh.positions() == points

def test_add_quad_unwrap_points(surface_mesh, four_points):
    vertices = [surface_mesh.add_vertex(p) for p in four_points]
    quad = surface_mesh.add_quad(*vertices)
    assert quad
    assert type(quad) is Face
    assert surface_mesh.n_faces() == 1
    assert surface_mesh.n_vertices() == 4
    assert surface_mesh.n_edges() == 4
    assert surface_mesh.n_halfedges() == 8
    assert all([surface_mesh.is_boundary(v) for v in vertices])

def test_add_quad(surface_mesh, four_points):
    vertices = [surface_mesh.add_vertex(p) for p in four_points]
    quad = surface_mesh.add_quad(vertices)
    assert quad
    assert type(quad) is Face
    assert surface_mesh.n_faces() == 1
    assert surface_mesh.n_vertices() == 4
    assert surface_mesh.n_edges() == 4
    assert surface_mesh.n_halfedges() == 8
    assert all([surface_mesh.is_boundary(v) for v in vertices])

def test_add_tetrahedron(four_points, surface_mesh):
    v0, v1, v2, v3 = [surface_mesh.add_vertex(p) for p in four_points]

    triangles = [
        (v0, v1, v3),
        (v1, v2, v3),
        (v2, v0, v3),
        (v0, v2, v1)
    ]

    faces = [
        surface_mesh.add_triangle(trias) for trias in triangles
    ]

    assert type(faces[0]) is Face
    assert surface_mesh.n_vertices() == 4
    assert surface_mesh.n_edges() == 6
    assert surface_mesh.n_faces() == 4
    mesh = surface_mesh
    assert mesh.n_vertices() == 4
    assert mesh.n_edges() == 6
    assert mesh.n_faces() == 4

# Region mesh properties

def test_point_property(vertex_onering):
    assert vertex_onering.has_vertex_property("v:point")
    v_point = vertex_onering.vertex_vector3d_property("v:point", Point(0))
    assert v_point[Vertex(0)] == Point(0.4499998093, 0.5196152329, 0.0000000000)

@pytest.mark.parametrize(
    ["property_name", "type_name", "property_value_type"],
    [
        ("quad_tri:new_position", "vector3d", Point),
        ("quad_tri:new_position", "Point", Point),
        ("is_selected", "bool", bool),
        ("index", "int", int),
        ("scalar", "scalar", float),
        ("texcoord", "vector2d", TexCoord),
        ("texcoord", "TexCoord", TexCoord),
        ("vector3d", "vector3d", Vector3D),
        ("normal", "Normal", Normal),
        ("color", "Color", Color)
    ]
)
def test_vertex_property(vertex_onering, property_name, type_name, property_value_type):
    assert not vertex_onering.has_vertex_property(property_name)
    new_pos = getattr(vertex_onering, "vertex_"+type_name+"_property")(property_name, property_value_type(0))
    assert vertex_onering.has_vertex_property(property_name)

    for i, v in enumerate(vertex_onering.vertices()):
        new_pos[v] = property_value_type(i)

    for i, v in enumerate(vertex_onering.vertices()):
        assert new_pos[v] == property_value_type(i)

    assert property_name in vertex_onering.vertex_properties()

    vertex_onering.remove_vertex_property(new_pos)
    assert not vertex_onering.has_vertex_property(property_name)

@pytest.mark.parametrize(
    ["property_name", "type_name", "property_value_type"],
    [
        ("quad_tri:new_position", "vector3d", Point),
        ("quad_tri:new_position", "Point", Point),
        ("is_selected", "bool", bool),
        ("index", "int", int),
        ("scalar", "scalar", float),
        ("texcoord", "vector2d", TexCoord),
        ("texcoord", "TexCoord", TexCoord),
        ("vector3d", "vector3d", Vector3D),
        ("normal", "Normal", Normal),
        ("color", "Color", Color)
    ]
)
def test_face_property(vertex_onering, property_name, type_name, property_value_type):
    assert not vertex_onering.has_face_property(property_name)
    new_pos = getattr(vertex_onering, "face_"+type_name+"_property")(property_name, property_value_type(0))
    assert vertex_onering.has_face_property(property_name)

    for i, f in enumerate(vertex_onering.faces()):
        new_pos[f] = property_value_type(i)

    for i, f in enumerate(vertex_onering.faces()):
        assert new_pos[f] == property_value_type(i)

    vertex_onering.remove_face_property(new_pos)
    assert not vertex_onering.has_face_property(property_name)

@pytest.mark.parametrize(
    ["property_name", "type_name", "property_value_type"],
    [
        ("quad_tri:new_position", "vector3d", Point),
        ("quad_tri:new_position", "Point", Point),
        ("is_selected", "bool", bool),
        ("index", "int", int),
        ("scalar", "scalar", float),
        ("texcoord", "vector2d", TexCoord),
        ("texcoord", "TexCoord", TexCoord),
        ("vector3d", "vector3d", Vector3D),
        ("normal", "Normal", Normal),
        ("color", "Color", Color)
    ]
)
def test_edge_property(vertex_onering, property_name, type_name, property_value_type):
    assert not vertex_onering.has_edge_property(property_name)
    new_pos = getattr(vertex_onering, "edge_"+type_name+"_property")(property_name, property_value_type(0))
    assert vertex_onering.has_edge_property(property_name)

    for i, e in enumerate(vertex_onering.edges()):
        new_pos[e] = property_value_type(i)

    for i, e in enumerate(vertex_onering.edges()):
        assert new_pos[e] == property_value_type(i)

    vertex_onering.remove_edge_property(new_pos)
    assert not vertex_onering.has_edge_property(property_name)

@pytest.mark.parametrize(
    ["property_name", "type_name", "property_value_type"],
    [
        ("quad_tri:new_position", "vector3d", Point),
        ("quad_tri:new_position", "Point", Point),
        ("is_selected", "bool", bool),
        ("index", "int", int),
        ("scalar", "scalar", float),
        ("texcoord", "vector2d", TexCoord),
        ("texcoord", "TexCoord", TexCoord),
        ("vector3d", "vector3d", Vector3D),
        ("normal", "Normal", Normal),
        ("color", "Color", Color)
    ]
)
def test_halfedge_property(vertex_onering, property_name, type_name, property_value_type):
    assert not vertex_onering.has_halfedge_property(property_name)
    new_pos = getattr(vertex_onering, "halfedge_"+type_name+"_property")(property_name, property_value_type(0))
    assert vertex_onering.has_halfedge_property(property_name)

    for i, h in enumerate(vertex_onering.halfedges()):
        new_pos[h] = property_value_type(i)

    for i, h in enumerate(vertex_onering.halfedges()):
        assert new_pos[h] == property_value_type(i)

    vertex_onering.remove_halfedge_property(new_pos)
    assert not vertex_onering.has_halfedge_property(property_name)

def test_valence(surface_mesh_with_face):
    assert surface_mesh_with_face.valence(Vertex(0)) == 2
    assert surface_mesh_with_face.valence(Face(0)) == 3

@pytest.mark.parametrize(
    ["handle", "delete_handle", "n_handle", "handle_property"],
    [
        (Vertex(0), "delete_vertex", "n_vertices", "vertex_property"),
        (Edge(0), "delete_edge", "n_edges", "edge_property"),
        (Face(0), "delete_face", "n_faces", "face_property")
    ]
)
def test_delete(handle, delete_handle, n_handle, handle_property, surface_mesh_with_face):
    assert surface_mesh_with_face.is_valid(handle)
    assert getattr(surface_mesh_with_face, "has_"+handle_property)(
        handle_property[0] + ":deleted"
    )
    assert surface_mesh_with_face.is_deleted(handle) == False
    delete_handle = getattr(surface_mesh_with_face, delete_handle)(handle)
    assert surface_mesh_with_face.is_deleted(handle)
    
    assert getattr(surface_mesh_with_face, n_handle)() == 0
    surface_mesh_with_face.garbage_collection()
    
    
