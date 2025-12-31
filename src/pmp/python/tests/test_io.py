import numpy as np
import os
from numpy.linalg import norm
from pathlib import Path

from pypmp import *
from pypmp.io import *
from pypmp.algorithms import normals

vertex_normals = normals.vertex_normals
face_normals = normals.face_normals

def test_obj_io_read_to_mesh(surface_mesh_with_face, tmp_path):
    vertex_normals(surface_mesh_with_face)
    _ = surface_mesh_with_face.halfedge_TexCoord_property("h:tex")
    
    filename = os.path.join(str(tmp_path), "test.obj")
    write(surface_mesh_with_face, filename)

    surface_mesh_with_face.clear()
    assert surface_mesh_with_face.is_empty()

    read_to_mesh(surface_mesh_with_face, filename)
    assert surface_mesh_with_face.n_vertices() == 3
    assert surface_mesh_with_face.n_faces() == 1

def test_obj_io_read(surface_mesh_with_face, tmp_path):
    vertex_normals(surface_mesh_with_face)
    _ = surface_mesh_with_face.halfedge_TexCoord_property("h:tex")
    
    filename = os.path.join(str(tmp_path), "test.obj")
    write(surface_mesh_with_face, filename)

    mesh = read(filename)
    assert mesh.n_vertices() == 3
    assert mesh.n_faces() == 1

def test_off_io(surface_mesh_with_face, tmp_path):
    surface_mesh_with_face.vertex_TexCoord_property("v:texcoord", TexCoord(0))
    surface_mesh_with_face.vertex_Color_property("v:color", Color(0))

    filename = os.path.join(str(tmp_path), "test.off")
    write(
        surface_mesh_with_face,
        filename,
        use_binary=False,
        use_vertex_normals=True,
        use_vertex_colors=True,
        use_vertex_texcoords=True
    )

    surface_mesh_with_face.clear()

    mesh = read(filename)
    assert mesh.n_vertices() == 3
    assert mesh.n_faces() == 1

def test_off_io_binary(surface_mesh_with_face, tmp_path):
    filename = os.path.join(str(tmp_path), "binary.off")

    write(surface_mesh_with_face, filename, use_binary=True)

    surface_mesh_with_face.clear()
    assert surface_mesh_with_face.is_empty()

    read_to_mesh(surface_mesh_with_face, filename)

    assert surface_mesh_with_face.n_vertices() == 3
    assert surface_mesh_with_face.n_faces() == 1

def test_pmp_io(surface_mesh_with_face, tmp_path):
    filename = os.path.join(str(tmp_path), "test.pmp")
    write(surface_mesh_with_face, filename)
    surface_mesh_with_face.clear()

    read_to_mesh(surface_mesh_with_face, filename)

    assert surface_mesh_with_face.n_vertices() == 3
    assert surface_mesh_with_face.n_faces() == 1

    np.testing.assert_raises(
        RuntimeError,
        write,
        *(surface_mesh_with_face, os.path.join(tmp_path, "testpolyly"))
    )

def test_read_stl_ascii(data_path):
    mesh = read(data_path / "stl" / "icosahedron_ascii.stl")
    assert mesh.n_vertices() == 12
    assert mesh.n_faces() == 20
    assert mesh.n_edges() == 30

def test_write_stl_binary(surface_mesh_with_face, tmp_path):
    face_normals(surface_mesh_with_face)
    write(surface_mesh_with_face, tmp_path / "binary.stl", use_binary=True)

    # inject solid keyword to test for robustness
    with open(tmp_path / "binary.stl", "r+b") as f:
        f.write(b"solid")

    mesh = read(tmp_path / "binary.stl")

    assert mesh.n_vertices() == 3
    assert mesh.n_faces() == 1

def test_write_stl_no_normals(surface_mesh_with_face, tmp_path):
    # try writing mesh without normals
    np.testing.assert_raises(
        ValueError,
        write,
        *(surface_mesh_with_face, tmp_path / "test.stl"),
    )

def test_write_stl_with_normals(surface_mesh_with_face, tmp_path):
    face_normals(surface_mesh_with_face)
    np.testing.assert_no_warnings(
        write,
        *(surface_mesh_with_face, tmp_path / "test.stl"),
    )

def test_write_stl_no_triangles(quad_face, tmp_path):
    # try writing non-triangle mesh
    np.testing.assert_raises(
        ValueError,
        write,
        *(quad_face, tmp_path / "test.stl"),
    )
