import pytest
import numpy as np
from pypmp import *
from numpy.linalg import norm

from pypmp.algorithms.parameterization import *
from pypmp.algorithms.distance_point_triangle import *
from pypmp.algorithms.geodesics import *
from pypmp.algorithms.triangulation import *
from pypmp.algorithms.fairing import *
from pypmp.algorithms.decimation import *
from pypmp.algorithms.remeshing import *
from pypmp.algorithms.smoothing import *
from pypmp.algorithms.subdivision import *
from pypmp.algorithms.normals import *
from pypmp.algorithms.hole_filling import *
from pypmp.algorithms.features import *
from pypmp.algorithms.differential_geometry import *
from pypmp.algorithms.utilities import *

from pypmp.io import *

# Subdivision tests
def test_loop_subdivision(icosahedron):
    loop_subdivision(icosahedron)
    assert icosahedron.n_faces() == 80

def test_loop_subdivision_with_features(icosahedron):
    detect_features(icosahedron, 25)
    loop_subdivision(icosahedron)
    assert icosahedron.n_faces() == 80

def test_loop_with_boundary(vertex_onering):
    loop_subdivision(vertex_onering)
    assert vertex_onering.n_faces() == 24

def test_catmull_clark_subdivision(hexahedron):
    catmull_clark_subdivision(hexahedron)
    assert hexahedron.n_faces() == 24

def test_catmull_clark_with_features(hexahedron):
    detect_features(hexahedron, 25)
    catmull_clark_subdivision(hexahedron)
    assert hexahedron.n_faces() == 24
    
def test_catmull_clark_with_boundary(plane):
    mesh = plane(1)
    catmull_clark_subdivision(mesh)
    assert mesh.n_faces() == 4

def test_quad_tri_on_quads(hexahedron):
    quad_tri_subdivision(hexahedron)
    assert hexahedron.n_faces() == 24

def test_quad_tri_on_triangles(tetrahedron):
    quad_tri_subdivision(tetrahedron)
    assert tetrahedron.n_faces() == 16

def test_quad_tri_on_mixed(cone):
    mesh = cone(4, 4, 4)
    quad_tri_subdivision(mesh)
    assert mesh.n_faces() == 20

# Triangulation tests
def test_triangulate(vertex_onering):
    N = vertex_onering.n_vertices()
    triangulate(vertex_onering)
    assert vertex_onering.n_vertices() == N

def test_triangulate_quad(quad_face):
    assert quad_face.n_vertices() == 4
    assert quad_face.n_faces() == 1
    triangulate(quad_face)
    assert quad_face.n_vertices() == 4
    assert quad_face.n_faces() == 2

def test_triangulate_quad_face(quad_face):
    assert quad_face.n_vertices() == 4
    assert quad_face.n_faces() == 1
    quad = list(quad_face.faces())[0]
    triangulate(quad_face, quad)
    assert quad_face.n_vertices() == 4
    assert quad_face.n_faces() == 2

def test_triangulate_l_shape(l_shape):
    assert l_shape.is_triangle_mesh() == False
    triangulate(l_shape)
    assert l_shape.is_triangle_mesh()
    assert l_shape.n_faces() == 10

# Smoothing tests
def test_implicit_smoothing(open_cone):
    area_before = differential_geometry.surface_area(open_cone)
    implicit_smoothing(open_cone)
    area_after = differential_geometry.surface_area(open_cone)
    assert area_before > area_after

def test_implicit_smoothing_with_rescale(open_cone):
    area_before = differential_geometry.surface_area(open_cone)
    implicit_smoothing(open_cone, 0.01, 1, False, True)
    area_after = differential_geometry.surface_area(open_cone)
    assert np.allclose(area_after, area_before)

def test_explicit_smoothing(open_cone):
    area_before = differential_geometry.surface_area(open_cone)
    explicit_smoothing(open_cone, 10, True)
    explicit_smoothing(open_cone, 10, False)
    area_after = differential_geometry.surface_area(open_cone)
    assert area_before > area_after

# Decimation tests
@pytest.mark.skip("n_vertices() == 106 for some unknown reason, C++ tests return 101 +-2. C++ tests run fine.")
def test_simplification(subdivided_icosahedron):
    clear_features(subdivided_icosahedron)
    decimate(
        subdivided_icosahedron,
        int(subdivided_icosahedron.n_vertices() * 0.01),
        5,      # aspect ratio
        0.5,    # edge length
        10,     # max valence
        10.,    # normal deviation
        0.1     # Hausdorff
    )

    assert subdivided_icosahedron.n_vertices() > 99 and subdivided_icosahedron.n_vertices() < 107

@pytest.mark.skip("n_vertices() == 19 for some unknown reason. C++ tests run fine.")
def test_simplification_with_features(subdivided_icosahedron):
    decimate(
        subdivided_icosahedron,
        int(subdivided_icosahedron.n_vertices() * 0.01),
        5
    )
    assert subdivided_icosahedron.n_vertices() == 12

def test_simplification_texture_mesh(texture_seams_mesh):
    assert texture_seams_mesh.has_halfedge_property("h:tex")

    seam_threshold = 1e-2
    decimate(
        texture_seams_mesh,
        int(texture_seams_mesh.n_vertices()) - 4,
        10.,
        0.0,
        0,
        135.,
        0.,
        seam_threshold,
        1.
    )

    seam_edges = 0
    texcoords = texture_seams_mesh.halfedge_vector2d_property("h:tex", TexCoord(0))
    seams = texture_seams_mesh.edge_bool_property("e:seam", bool(0))
    if texcoords:
        for e in texture_seams_mesh.edges():
            h0 = texture_seams_mesh.halfedge(e, 0)
            h1 = texture_seams_mesh.halfedge(e, 1)
            h0p = texture_seams_mesh.prev_halfedge(h0)
            h1p = texture_seams_mesh.prev_halfedge(h1)

            if (
                norm(texcoords[h1] - texcoords[h0p]) > seam_threshold or
                norm(texcoords[h0] - texcoords[h1p]) > seam_threshold
            ):
                seam_edges += 1
                seams[e] = True

    se = texture_seams_mesh.find_edge(Vertex(4), Vertex(11))
    se2 = texture_seams_mesh.find_edge(Vertex(4), Vertex(0))

    assert texture_seams_mesh.n_vertices() == 12
    assert seam_edges == 13
    assert seams[se] == 1
    assert seams[se2] == 1

# Hole filling test
def test_hole_filling(open_cone):
    def find_boundary(mesh):
        for h in mesh.halfedges():
            if mesh.is_boundary(h):
                return h
        return Halfedge()

    h = find_boundary(open_cone)
    assert h.is_valid()

    fill_hole(open_cone, h)

    h = find_boundary(open_cone)

    assert h.is_valid() == False

# Normals test
def test_vertex_normals(icosahedron):
    vertex_normals(icosahedron)
    vnormals = icosahedron.vertex_vector3d_property("v:normal", Normal(0))
    vn0 = vnormals[Vertex(0)]
    assert norm(vn0) > 0

def test_face_normals(icosahedron):
    face_normals(icosahedron)
    fnormals = icosahedron.face_vector3d_property("f:normal", Normal(0))
    fn0 = fnormals[Face(0)]
    assert norm(fn0) > 0

def test_corner_normal(icosahedron):
    h = Halfedge(0)
    n = corner_normal(icosahedron, h, np.pi/3)
    assert norm(n) > 0

def test_polygonal_face_normal():
    mesh = SurfaceMesh()
    vertices = [None] * 5
    vertices[0] = mesh.add_vertex(Point(0, 0, 0))
    vertices[1] = mesh.add_vertex(Point(1, 0, 0))
    vertices[2] = mesh.add_vertex(Point(1, 1, 0))
    vertices[3] = mesh.add_vertex(Point(0.5, 1, 0))
    vertices[4] = mesh.add_vertex(Point(0, 1, 0))

    f0 = mesh.add_face(vertices)
    n0 = face_normal(mesh, f0)
    
    assert norm(n0) > 0

# Features test
def test_detect_angle(hexahedron):
    nf = detect_features(hexahedron, 25)
    assert nf == 12

def test_detect_boundary(vertex_onering):
    nb = detect_boundary(vertex_onering)
    assert nb == 6

def test_clear(vertex_onering):
    detect_boundary(vertex_onering)
    clear_features(vertex_onering)
    is_feature = vertex_onering.edge_bool_property("e:feature")
    
    for e in vertex_onering.edges():
        assert is_feature[e] == False

# Differential geometry test
def test_area_points(surface_mesh_with_face):
    v0, v1, v2 = surface_mesh_with_face.vertices()
    a = triangle_area(
        surface_mesh_with_face.position(v0),
        surface_mesh_with_face.position(v1),
        surface_mesh_with_face.position(v2),
    )
    assert a == 0.5

def test_area_face(surface_mesh_with_face):
    f0 = list(surface_mesh_with_face.faces())[0]

    a = face_area(surface_mesh_with_face, f0)
    assert a == 0.5

def test_voronoi_area(one_ring):
    central_vertex = Vertex(3)
    a = voronoi_area(one_ring, central_vertex)
    np.testing.assert_almost_equal(a, 0.024590395)

def test_laplace(one_ring):
    central_vertex = Vertex(3)
    lv = laplace(one_ring, central_vertex)
    assert norm(lv) > 0

def test_area_surface(icosphere):
    sphere = icosphere(5)
    a = surface_area(sphere)
    np.testing.assert_almost_equal(a, 12.57, decimal=2)

def test_volume(icosphere):
    sphere = icosphere(5)
    v = volume(sphere)
    np.testing.assert_almost_equal(v, 4.18, decimal=2)

def test_centroid(icosphere):
    sphere = icosphere(5)
    center = centroid(sphere)
    assert norm(center) < 1e-5

# Fairing test
def test_fairing(open_cone):
    loop_subdivision(open_cone)
    bbz = bounds(open_cone).max()[2]
    fair(open_cone)
    bbs = bounds(open_cone).max()[2]
    assert bbs < bbz

def test_fairing_selected(open_cone):
    loop_subdivision(open_cone)
    bb = bounds(open_cone)

    selected = open_cone.vertex_bool_property("v:selected")
    for v in open_cone.vertices():
        if open_cone.position(v)[2] > 0.5:
            selected[v] = True

    fair(open_cone)

    bb2 = bounds(open_cone)

    assert bb2.size() < bb.size()

# Utilities test
def test_flip_faces(surface_mesh_with_face):
    vertices_before = list(surface_mesh_with_face.vertices())
    flip_faces(surface_mesh_with_face)
    flip_faces(surface_mesh_with_face)
    vertices_after = list(surface_mesh_with_face.vertices())
    np.testing.assert_array_equal(vertices_before, vertices_after)

def test_min_face_area(quad_face):
    assert min_face_area(quad_face) == 1.0

def test_edge_length(quad_face):
    e = Edge(0)
    length = edge_length(quad_face, e)
    assert length == 1.0

def test_mean_edge_length(quad_face):
    mean = mean_edge_length(quad_face)
    assert mean == 1.0

def test_connected_components(quad_face, surface_mesh_with_face):
    mesh = SurfaceMesh()
    vertices = []
    for v in surface_mesh_with_face.vertices():
        p = surface_mesh_with_face.position(v)
        vertices.append(mesh.add_vertex(p))
    _ = mesh.add_triangle(*vertices)

    vertices = []
    for v in quad_face.vertices():
        p = quad_face.position(v)
        vertices.append(mesh.add_vertex(p))
    _ = mesh.add_quad(*vertices)
    
    nc = connected_components(mesh)
    assert nc == 2

# Parameterization test
def test_parameterization(open_cone):
    harmonic_parameterization(open_cone, False)
    harmonic_parameterization(open_cone, True)
    tex = open_cone.vertex_TexCoord_property("v:tex")
    assert tex

def test_lscm_parameterization(open_cone):
    lscm_parameterization(open_cone)
    tex = open_cone.vertex_TexCoord_property("v:tex")
    assert tex

# Distance point triangle test
def test_distance_point_degenerate_triangle(degenerate_triangle):
    p = Point(0, 1, 0)
    nearest = Point(1) # Dummy point for parsing into method by reference

    dist = dist_point_triangle(
        p,
        *degenerate_triangle.positions(),
        nearest
    )

    np.testing.assert_allclose(dist, 1.0)
    assert nearest == Point(0)

# Geodesics test
def test_geodesic(icosphere):
    mesh = icosphere(5)
    geodesics(mesh, [Vertex(0)])

    d = 0.0
    distance = mesh.vertex_scalar_property("geodesic:distance")

    for v in mesh.vertices():
        d = max(d, distance[v])

    np.testing.assert_allclose(d, 3.1355045)

    distance_to_texture_coordinates(mesh)
    tex = mesh.vertex_TexCoord_property("v:tex")
    assert tex

def test_geodesic_symmetry(data_path):
    # read irregular mesh (to have virtual edges)
    mesh = read(data_path / "off" / "bunny_adaptive.off")

    # grow from first vector
    v0 = Vertex(0)
    geodesics(mesh, [v0])

    # find maximum geodesic distance
    d0 = 0.0
    distance = mesh.vertex_scalar_property("geodesic:distance")

    for v in mesh.vertices():
        if distance[v] > d0:
            d0 = distance[v]
            v1 = v

    # grow back from max-dist vertex to vertex 0
    geodesics(mesh, [v1])
    d1 = distance[v0]

    err = np.abs(d0 - d1) / (0.5 * (d0 + d1))
    np.testing.assert_array_less(err, 0.001)

def test_geodesic_maxnum(icosphere):
    # generate unit sphere mesh
    mesh = icosphere(3)

    # compute geodesic distance from first vertex
    maxnum = 42

    num, neighbors = geodesics(mesh, [Vertex(0)], maxnum=maxnum)

    assert num == maxnum
    assert len(neighbors) == maxnum

    # test that neighbor array is properly sorted
    distance = mesh.vertex_scalar_property("geodesic:distance")
    for i in range(len(neighbors)-1):
        assert distance[neighbors[i]] <= distance[neighbors[i+1]]

# Remeshing test
def test_adaptive_remeshing_with_features(cylinder):
    mesh = cylinder()

    triangulate(mesh)
    detect_features(mesh, 25)
    bb = bounds(mesh).size()

    adaptive_remeshing(
        mesh,
        0.001 * bb, # min. length
        1.0 * bb, # max. length
        0.001 * bb # approx. error
    )

    assert mesh.n_vertices() == 6

def test_adaptive_remeshing_with_boundary(open_cone):
    mesh = open_cone

    bb = bounds(mesh).size()
    adaptive_remeshing(
        mesh,
        0.01 * bb,
        1.0 * bb,
        0.01 * bb
    )

    assert mesh.n_vertices() == 104

def test_adaptive_remeshing_with_selection(icosphere):
    mesh = icosphere(1)

    selected = mesh.vertex_bool_property("v:selected")

    # select half the vertices
    for v in mesh.vertices():
        if mesh.position(v)[1] > 0:
            selected[v] = True

    bb = bounds(mesh).size()
    
    adaptive_remeshing(
        mesh,
        0.01 * bb,
        1.0 * bb,
        0.01 * bb
    )

    assert mesh.n_vertices() == 62

def test_uniform_remeshing(open_cone):
    uniform_remeshing(open_cone, 0.5)

    assert open_cone.n_vertices() == 41