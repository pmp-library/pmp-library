import pytest
from pypmp import *

@pytest.mark.parametrize(
    ["handle_iterator", "n_handle"],
    [
        ("vertices", "n_vertices"),
        ("faces", "n_faces"),
        ("edges", "n_edges"),
        ("halfedges", "n_halfedges")
    ]
)
def test_iterators(surface_mesh_with_face, handle_iterator, n_handle):
    assert surface_mesh_with_face
    iterator = getattr(surface_mesh_with_face, handle_iterator)
    n_handle = getattr(surface_mesh_with_face, n_handle)
    assert len(list(iterator())) == n_handle()
    assert all([surface_mesh_with_face.is_valid(h) for h in iterator()])

def test_vertex_circulator_next(vertex, vertex_onering):
    vertex_circulator = vertex_onering.vertices(vertex)

    vv = next(vertex_circulator)
    assert isinstance(vv, Vertex)
    assert vv.idx() == 4
    
def test_vertex_circulator_prev(vertex, vertex_onering):
    vertex_circulator = vertex_onering.vertices(vertex)

    # Calling custom method previous as Python stdlib lacks __prev__
    vv = vertex_circulator.previous()
    assert isinstance(vv, Vertex)
    assert vv.idx() == 4

def test_vertex_circulator_distance_forwards(vertex, vertex_onering):
    vertex_circulator = vertex_onering.vertices(vertex)
    count = 0
    try:
        while next(vertex_circulator) != vertex:
            count+=1
    except StopIteration:
        pass

    assert count == 6
    vertex_circulator = vertex_onering.vertices(vertex)
    assert len(list(vertex_circulator)) == 6

def test_vertex_circulator_distance_backwards(vertex, vertex_onering):
    vertex_circulator = vertex_onering.vertices(vertex)
    count = 0
    try:
        while vertex_circulator.previous() != vertex:
            count+=1
    except StopIteration:
        pass

    assert count == 6