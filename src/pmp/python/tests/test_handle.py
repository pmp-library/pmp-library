from pypmp import *
import pytest

@pytest.mark.parametrize(
    ["handle"],
    [
        (Handle(),),
        (Vertex(),),
        (Face(),),
        (Edge(),),
        (Halfedge(),)
    ]
)
def test_handle_base(handle):
    assert not handle.is_valid()

@pytest.mark.parametrize(
    ["handle"],
    [
        (Handle(1),),
        (Vertex(1),),
        (Face(1),),
        (Edge(1),),
        (Halfedge(1),)
    ]
)
def test_handle_index(handle):
    assert handle.idx() == 1
    assert handle.is_valid()
    handle_type = type(handle)
    assert handle_type(0) < handle
    assert handle_type(2) > handle
    assert handle_type(0) != handle
    assert handle_type(1) <= handle
    assert handle_type(1) >= handle
    assert handle_type(1) == handle
    