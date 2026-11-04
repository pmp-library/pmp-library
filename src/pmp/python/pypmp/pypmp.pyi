"""

        PyPMP - Python binding for Polygon Mesh Processing (PMP) library
    
"""
from __future__ import annotations
import collections.abc
import typing
__all__: list[str] = ['BoolProperty', 'Color', 'Edge', 'EdgeAroundVertexCirculator', 'EdgeBoolProperty', 'EdgeIntProperty', 'EdgeScalarProperty', 'EdgeUintProperty', 'EdgeVector2DProperty', 'EdgeVector3DProperty', 'Face', 'FaceAroundVertexCirculator', 'FaceBoolProperty', 'FaceIntProperty', 'FaceScalarProperty', 'FaceUintProperty', 'FaceVector2DProperty', 'FaceVector3DProperty', 'Halfedge', 'HalfedgeAroundFaceCirculator', 'HalfedgeAroundVertexCirculator', 'HalfedgeBoolProperty', 'HalfedgeIntProperty', 'HalfedgeScalarProperty', 'HalfedgeUintProperty', 'HalfedgeVector2DProperty', 'HalfedgeVector3DProperty', 'Handle', 'IntProperty', 'Normal', 'Point', 'ScalarProperty', 'SurfaceMesh', 'TexCoord', 'UintProperty', 'Vector2D', 'Vector2DProperty', 'Vector3D', 'Vector3DProperty', 'Vertex', 'VertexAroundFaceCirculator', 'VertexAroundVertexCirculator', 'VertexBoolProperty', 'VertexIntProperty', 'VertexScalarProperty', 'VertexUintProperty', 'VertexVector2DProperty', 'VertexVector3DProperty']
class BoolProperty:
    def __getitem__(self, arg0: typing.SupportsInt) -> bool:
        ...
    def __setitem__(self, arg0: typing.SupportsInt, arg1: bool) -> None:
        ...
    def vector(self) -> list[bool]:
        """
        Get reference to the underlying vector.
        """
class Edge(Handle):
    @typing.overload
    def __init__(self) -> None:
        """
        default constructor with invalid index
        """
    @typing.overload
    def __init__(self, idx: typing.SupportsInt) -> None:
        """
        constructor with index
        """
class EdgeAroundVertexCirculator:
    def __init__(self, mesh: ..., handle: ...) -> None:
        ...
    def __iter__(self) -> EdgeAroundVertexCirculator:
        ...
    def __next__(self) -> ...:
        ...
    def prev(self) -> ...:
        ...
class EdgeBoolProperty(BoolProperty):
    def __getitem__(self, arg0: Edge) -> bool:
        ...
    def __setitem__(self, arg0: Edge, arg1: bool) -> None:
        ...
class EdgeIntProperty(IntProperty):
    def __getitem__(self, arg0: Edge) -> int:
        ...
    def __setitem__(self, arg0: Edge, arg1: typing.SupportsInt) -> None:
        ...
class EdgeScalarProperty(ScalarProperty):
    def __getitem__(self, arg0: Edge) -> float:
        ...
    def __setitem__(self, arg0: Edge, arg1: typing.SupportsFloat) -> None:
        ...
class EdgeUintProperty(UintProperty):
    def __getitem__(self, arg0: Edge) -> int:
        ...
    def __setitem__(self, arg0: Edge, arg1: typing.SupportsInt) -> None:
        ...
class EdgeVector2DProperty(Vector2DProperty):
    def __getitem__(self, arg0: Edge) -> Vector2D:
        ...
    def __setitem__(self, arg0: Edge, arg1: Vector2D) -> None:
        ...
class EdgeVector3DProperty(Vector3DProperty):
    def __getitem__(self, arg0: Edge) -> Vector3D:
        ...
    def __setitem__(self, arg0: Edge, arg1: Vector3D) -> None:
        ...
class Face(Handle):
    @typing.overload
    def __init__(self) -> None:
        """
        default constructor with invalid index
        """
    @typing.overload
    def __init__(self, idx: typing.SupportsInt) -> None:
        """
        constructor with index
        """
class FaceAroundVertexCirculator:
    def __init__(self, mesh: ..., handle: ...) -> None:
        ...
    def __iter__(self) -> FaceAroundVertexCirculator:
        ...
    def __next__(self) -> ...:
        ...
    def prev(self) -> ...:
        ...
class FaceBoolProperty(BoolProperty):
    def __getitem__(self, arg0: Face) -> bool:
        ...
    def __setitem__(self, arg0: Face, arg1: bool) -> None:
        ...
class FaceIntProperty(IntProperty):
    def __getitem__(self, arg0: Face) -> int:
        ...
    def __setitem__(self, arg0: Face, arg1: typing.SupportsInt) -> None:
        ...
class FaceScalarProperty(ScalarProperty):
    def __getitem__(self, arg0: Face) -> float:
        ...
    def __setitem__(self, arg0: Face, arg1: typing.SupportsFloat) -> None:
        ...
class FaceUintProperty(UintProperty):
    def __getitem__(self, arg0: Face) -> int:
        ...
    def __setitem__(self, arg0: Face, arg1: typing.SupportsInt) -> None:
        ...
class FaceVector2DProperty(Vector2DProperty):
    def __getitem__(self, arg0: Face) -> Vector2D:
        ...
    def __setitem__(self, arg0: Face, arg1: Vector2D) -> None:
        ...
class FaceVector3DProperty(Vector3DProperty):
    def __getitem__(self, arg0: Face) -> Vector3D:
        ...
    def __setitem__(self, arg0: Face, arg1: Vector3D) -> None:
        ...
class Halfedge(Handle):
    @typing.overload
    def __init__(self) -> None:
        """
        default constructor with invalid index
        """
    @typing.overload
    def __init__(self, idx: typing.SupportsInt) -> None:
        """
        constructor with index
        """
class HalfedgeAroundFaceCirculator:
    def __init__(self, mesh: ..., handle: ...) -> None:
        ...
    def __iter__(self) -> HalfedgeAroundFaceCirculator:
        ...
    def __next__(self) -> ...:
        ...
    def prev(self) -> ...:
        ...
class HalfedgeAroundVertexCirculator:
    def __init__(self, mesh: ..., handle: ...) -> None:
        ...
    def __iter__(self) -> HalfedgeAroundVertexCirculator:
        ...
    def __next__(self) -> ...:
        ...
    def prev(self) -> ...:
        ...
class HalfedgeBoolProperty(BoolProperty):
    def __getitem__(self, arg0: Halfedge) -> bool:
        ...
    def __setitem__(self, arg0: Halfedge, arg1: bool) -> None:
        ...
class HalfedgeIntProperty(IntProperty):
    def __getitem__(self, arg0: Halfedge) -> int:
        ...
    def __setitem__(self, arg0: Halfedge, arg1: typing.SupportsInt) -> None:
        ...
class HalfedgeScalarProperty(ScalarProperty):
    def __getitem__(self, arg0: Halfedge) -> float:
        ...
    def __setitem__(self, arg0: Halfedge, arg1: typing.SupportsFloat) -> None:
        ...
class HalfedgeUintProperty(UintProperty):
    def __getitem__(self, arg0: Halfedge) -> int:
        ...
    def __setitem__(self, arg0: Halfedge, arg1: typing.SupportsInt) -> None:
        ...
class HalfedgeVector2DProperty(Vector2DProperty):
    def __getitem__(self, arg0: Halfedge) -> Vector2D:
        ...
    def __setitem__(self, arg0: Halfedge, arg1: Vector2D) -> None:
        ...
class HalfedgeVector3DProperty(Vector3DProperty):
    def __getitem__(self, arg0: Halfedge) -> Vector3D:
        ...
    def __setitem__(self, arg0: Halfedge, arg1: Vector3D) -> None:
        ...
class Handle:
    __hash__: typing.ClassVar[None] = None
    def __eq__(self, arg0: Handle) -> bool:
        ...
    def __ge__(self, arg0: Handle) -> bool:
        ...
    def __gt__(self, arg0: Handle) -> bool:
        ...
    @typing.overload
    def __init__(self) -> None:
        """
        default constructor with invalid index
        """
    @typing.overload
    def __init__(self, idx: typing.SupportsInt) -> None:
        """
        constructor with index
        """
    def __le__(self, arg0: Handle) -> bool:
        ...
    def __lt__(self, arg0: Handle) -> bool:
        ...
    def __ne__(self, arg0: Handle) -> bool:
        ...
    def idx(self) -> int:
        """
        Get the underlying index of this handle
        """
    def is_valid(self) -> bool:
        """
        Return whether the handle is valid, i.e., the index is not equal to PMP_MAX_INDEX.
        """
class IntProperty:
    def __getitem__(self, arg0: typing.SupportsInt) -> int:
        ...
    def __setitem__(self, arg0: typing.SupportsInt, arg1: typing.SupportsInt) -> None:
        ...
    def vector(self) -> list[int]:
        """
        Get reference to the underlying vector.
        """
class ScalarProperty:
    def __getitem__(self, arg0: typing.SupportsInt) -> float:
        ...
    def __setitem__(self, arg0: typing.SupportsInt, arg1: typing.SupportsFloat) -> None:
        ...
    def vector(self) -> list[float]:
        """
        Get reference to the underlying vector.
        """
class SurfaceMesh:
    @staticmethod
    def edge_Color_property(*args, **kwargs):
        """
        edge_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::EdgeProperty<pmp::Matrix<float, 3, 1> >
        
        If a edge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def edge_Normal_property(*args, **kwargs):
        """
        edge_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::EdgeProperty<pmp::Matrix<float, 3, 1> >
        
        If a edge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def edge_Point_property(*args, **kwargs):
        """
        edge_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::EdgeProperty<pmp::Matrix<float, 3, 1> >
        
        If a edge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def edge_TexCoord_property(*args, **kwargs):
        """
        edge_vector2d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector2D = Vector2D(0, 0)) -> pmp::EdgeProperty<pmp::Matrix<float, 2, 1> >
        
        If a edge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def face_Color_property(*args, **kwargs):
        """
        face_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::FaceProperty<pmp::Matrix<float, 3, 1> >
        
        If a face property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def face_Normal_property(*args, **kwargs):
        """
        face_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::FaceProperty<pmp::Matrix<float, 3, 1> >
        
        If a face property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def face_Point_property(*args, **kwargs):
        """
        face_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::FaceProperty<pmp::Matrix<float, 3, 1> >
        
        If a face property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def face_TexCoord_property(*args, **kwargs):
        """
        face_vector2d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector2D = Vector2D(0, 0)) -> pmp::FaceProperty<pmp::Matrix<float, 2, 1> >
        
        If a face property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def halfedge_Color_property(*args, **kwargs):
        """
        halfedge_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::HalfedgeProperty<pmp::Matrix<float, 3, 1> >
        
        If a halfedge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def halfedge_Normal_property(*args, **kwargs):
        """
        halfedge_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::HalfedgeProperty<pmp::Matrix<float, 3, 1> >
        
        If a halfedge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def halfedge_Point_property(*args, **kwargs):
        """
        halfedge_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::HalfedgeProperty<pmp::Matrix<float, 3, 1> >
        
        If a halfedge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def halfedge_TexCoord_property(*args, **kwargs):
        """
        halfedge_vector2d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector2D = Vector2D(0, 0)) -> pmp::HalfedgeProperty<pmp::Matrix<float, 2, 1> >
        
        If a halfedge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    @typing.overload
    def remove_edge_property(*args, **kwargs) -> None:
        """
        Remove the edge property p
        """
    @staticmethod
    @typing.overload
    def remove_edge_property(*args, **kwargs) -> None:
        """
        Remove the edge property p
        """
    @staticmethod
    @typing.overload
    def remove_face_property(*args, **kwargs) -> None:
        """
        Remove the face property p
        """
    @staticmethod
    @typing.overload
    def remove_face_property(*args, **kwargs) -> None:
        """
        Remove the face property p
        """
    @staticmethod
    @typing.overload
    def remove_halfedge_property(*args, **kwargs) -> None:
        """
        Remove the halfedge property p
        """
    @staticmethod
    @typing.overload
    def remove_halfedge_property(*args, **kwargs) -> None:
        """
        Remove the halfedge property p
        """
    @staticmethod
    @typing.overload
    def remove_vertex_property(*args, **kwargs) -> None:
        """
        Remove the vertex property p
        """
    @staticmethod
    @typing.overload
    def remove_vertex_property(*args, **kwargs) -> None:
        """
        Remove the vertex property p
        """
    @staticmethod
    def vertex_Color_property(*args, **kwargs):
        """
        vertex_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::VertexProperty<pmp::Matrix<float, 3, 1> >
        
        If a vertex property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def vertex_Normal_property(*args, **kwargs):
        """
        vertex_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::VertexProperty<pmp::Matrix<float, 3, 1> >
        
        If a vertex property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def vertex_Point_property(*args, **kwargs):
        """
        vertex_vector3d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector3D = Vector3D(0, 0, 0)) -> pmp::VertexProperty<pmp::Matrix<float, 3, 1> >
        
        If a vertex property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @staticmethod
    def vertex_TexCoord_property(*args, **kwargs):
        """
        vertex_vector2d_property(self: pypmp.SurfaceMesh, name: str, t: pypmp.Vector2D = Vector2D(0, 0)) -> pmp::VertexProperty<pmp::Matrix<float, 2, 1> >
        
        If a vertex property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def __init__(self) -> None:
        ...
    def add_face(self, vertices: collections.abc.Sequence[...]) -> ...:
        """
                        add_face(vertices: List[Vertex]) -> Face
        
                        Add a new face with vertex list `vertices`
        
                        Raises
                        ------
                        TopologyException in case a topological error occurs.            
        
                        See Also
                        --------
                        add_triangle
                        add_quad
        """
    @typing.overload
    def add_quad(self, v0: ..., v1: ..., v2: ..., v3: ...) -> ...:
        """
        Add a new quad connecting vertices in array
        """
    @typing.overload
    def add_quad(self, array: collections.abc.Sequence) -> ...:
        """
        Add a new quad connecting vertices in array
        """
    @typing.overload
    def add_triangle(self, v0: ..., v1: ..., v2: ...) -> ...:
        """
        Add a new triangle connecting vertices v0, v1, v2
        """
    @typing.overload
    def add_triangle(self, array: collections.abc.Sequence) -> ...:
        """
        Add a new triangle connecting vertices v0, v1, v2
        """
    def add_vertex(self, p: Vector3D) -> ...:
        """
        Add a new vertex with position p
        """
    def clear(self) -> None:
        """
        clear mesh: remove all vertices, edges, faces
        """
    def delete_edge(self, e: ...) -> None:
        """
                        delete_edge(e: Edge) -> None
        
                        Delete edge `e` from the mesh.
        
                        Notes
                        -----
                        Only marks the edge as deleted. Call `garbage_collection()` to finally remove deleted entities.
        """
    def delete_face(self, f: ...) -> None:
        """
                        delete_face(f: Face) -> None
        
                        Delete face `f` from the mesh.
        
                        Notes
                        -----
                        Only marks the face as deleted. Call `garbage_collection()` to finally remove deleted entities.
        """
    def delete_vertex(self, v: ...) -> None:
        """
                        delete_vertex(v: Vertex) -> None
        
                        Delete vertex `v` from the mesh.
        
                        Notes
                        -----
                        Only marks the vertex as deleted. Call `garbage_collection()` to finally remove deleted entities.
        """
    def edge_bool_property(self, name: str, t: bool = False) -> ...:
        """
        If an edge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def edge_int_property(self, name: str, t: typing.SupportsInt = 0) -> ...:
        """
        If an edge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def edge_properties(self) -> list[str]:
        """
        return the names of all edge properties
        """
    def edge_scalar_property(self, name: str, t: typing.SupportsFloat = 0.0) -> ...:
        """
        If a edge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def edge_uint_property(self, name: str, t: typing.SupportsInt = 0) -> ...:
        """
        If an edge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def edge_vector2d_property(self, name: str, t: Vector2D = ...) -> ...:
        """
        If a edge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def edge_vector3d_property(self, name: str, t: Vector3D = ...) -> ...:
        """
        If a edge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @typing.overload
    def edges(self) -> collections.abc.Iterator[...]:
        """
        Return edge container
        """
    @typing.overload
    def edges(self, v: ...) -> ...:
        ...
    def face_bool_property(self, name: str, T: bool = False) -> ...:
        """
        If a face property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def face_int_property(self, name: str, T: typing.SupportsInt = 0) -> ...:
        """
        If a face property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def face_properties(self) -> list[str]:
        """
        return the names of all face properties
        """
    def face_scalar_property(self, name: str, t: typing.SupportsFloat = 0.0) -> ...:
        """
        If a face property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def face_uint_property(self, name: str, T: typing.SupportsInt = 0) -> ...:
        """
        If a face property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def face_vector2d_property(self, name: str, t: Vector2D = ...) -> ...:
        """
        If a face property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def face_vector3d_property(self, name: str, t: Vector3D = ...) -> ...:
        """
        If a face property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @typing.overload
    def faces(self) -> collections.abc.Iterator[...]:
        """
        Return face container
        """
    @typing.overload
    def faces(self, v: ...) -> FaceAroundVertexCirculator:
        ...
    def find_edge(self, a: ..., b: ...) -> ...:
        """
        Find the edge of two vertices (a,b)
        """
    def garbage_collection(self) -> None:
        """
                        garbage_collection() -> None
        
                        Remove deleted elements.
        """
    @typing.overload
    def halfedge(self, v: ...) -> ...:
        """
        Return an outgoing halfedge of vertex `v`. if `v` is a boundary vertex this will be a boundary halfedge.
        """
    @typing.overload
    def halfedge(self, v: ..., i: typing.SupportsInt) -> ...:
        """
        Return the `i`'th halfedge of edge `e`. `i` has to be 0 or 1.
        """
    def halfedge_bool_property(self, name: str, T: bool = False) -> ...:
        """
        If a halfedge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def halfedge_int_property(self, name: str, T: typing.SupportsInt = 0) -> ...:
        """
        If a halfedge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def halfedge_properties(self) -> list[str]:
        """
        return the names of all halfedge properties
        """
    def halfedge_scalar_property(self, name: str, t: typing.SupportsFloat = 0.0) -> ...:
        """
        If a halfedge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def halfedge_uint_property(self, name: str, T: typing.SupportsInt = 0) -> ...:
        """
        If a halfedge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def halfedge_vector2d_property(self, name: str, t: Vector2D = ...) -> ...:
        """
        If a halfedge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def halfedge_vector3d_property(self, name: str, t: Vector3D = ...) -> ...:
        """
        If a halfedge property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @typing.overload
    def halfedges(self) -> collections.abc.Iterator[...]:
        """
        Return halfedge container
        """
    @typing.overload
    def halfedges(self, v: ...) -> HalfedgeAroundVertexCirculator:
        ...
    @typing.overload
    def halfedges(self, f: ...) -> HalfedgeAroundFaceCirculator:
        ...
    def has_edge_property(self, name: str) -> bool:
        """
        does the mesh has an edge property with name name?
        """
    def has_face_property(self, name: str) -> bool:
        """
        does the mesh has a face property with name name?
        """
    def has_halfedge_property(self, name: str) -> bool:
        """
        does the mesh has a halfedge property with name name?
        """
    def has_vertex_property(self, name: str) -> bool:
        """
        does the mesh has a vertex property with name name?
        """
    @typing.overload
    def is_boundary(self, v: ...) -> bool:
        """
        Return whether v is a boundary vertex
        """
    @typing.overload
    def is_boundary(self, h: ...) -> bool:
        """
        Return whether h is a boundary halfedge
        """
    @typing.overload
    def is_boundary(self, e: ...) -> bool:
        """
        Return whether e is a boundary edge
        """
    @typing.overload
    def is_boundary(self, f: ...) -> bool:
        """
        Return whether f is a boundary face
        """
    @typing.overload
    def is_deleted(self, v: ...) -> bool:
        """
                        is_deleted(v: Vertex) -> bool
                        
                        Returns
                        -------
                        Whether vertex `v` is deleted.
        
                        See Also
                        --------
                        garbage_collection
        """
    @typing.overload
    def is_deleted(self, h: ...) -> bool:
        """
                        is_deleted(h: Halfedge) -> bool
                        
                        Returns
                        -------
                        Whether halfedge `h` is deleted.
        
                        See Also
                        --------
                        garbage_collection
        """
    @typing.overload
    def is_deleted(self, e: ...) -> bool:
        """
                        is_deleted(e: Edge) -> bool
                        
                        Returns
                        -------
                        Whether edge `e` is deleted.
        
                        See Also
                        --------
                        garbage_collection
        """
    @typing.overload
    def is_deleted(self, f: ...) -> bool:
        """
                        is_deleted(f: Face) -> bool
                        
                        Returns
                        -------
                        Whether face `f` is deleted.
        
                        See Also
                        --------
                        garbage_collection
        """
    def is_empty(self) -> bool:
        """
        Return true if the mesh is empty, i.e., has no vertices
        """
    def is_quad_mesh(self) -> bool:
        """
                        is_quad_mesh() -> bool
        
                        Returns
                        -------
                        bool 
                            whether the mesh a quad mesh. this function simply tests
                            each face, and therefore is not very efficient.
        """
    def is_triangle_mesh(self) -> bool:
        """
                        is_triangle_mesh() -> bool
        
                        Returns
                        -------
                        bool 
                            whether the mesh a triangle mesh. this function simply tests
                            each face, and therefore is not very efficient.
        """
    @typing.overload
    def is_valid(self, v: ...) -> bool:
        """
        Return whether vertex v is valid
        """
    @typing.overload
    def is_valid(self, h: ...) -> bool:
        """
        Return whether halfedge h is valid
        """
    @typing.overload
    def is_valid(self, e: ...) -> bool:
        """
        Return whether edge e is valid
        """
    @typing.overload
    def is_valid(self, f: ...) -> bool:
        """
        Return whether face f is valid
        """
    def n_edges(self) -> int:
        """
        Return number of edges in the mesh
        """
    def n_faces(self) -> int:
        """
        Return number of faces in the mesh
        """
    def n_halfedges(self) -> int:
        """
        Return number of halfedge in the mesh
        """
    def n_vertices(self) -> int:
        """
        Return number of vertices in the mesh
        """
    def opposite_halfedge(self, h: ...) -> ...:
        """
        Return the opposite halfedge of `h`
        """
    def position(self, v: ...) -> Vector3D:
        """
        Return mutable position of a vertex
        """
    def positions(self) -> list[Vector3D]:
        """
        Return vector of mutable point positions
        """
    def prev_halfedge(self, h: ...) -> ...:
        """
        Return the previous halfedge withing the incident face.
        """
    @typing.overload
    def remove_edge_property(self, p: ...) -> None:
        """
        Remove the edge property p
        """
    @typing.overload
    def remove_edge_property(self, p: ...) -> None:
        """
        Remove the edge property p
        """
    @typing.overload
    def remove_edge_property(self, p: ...) -> None:
        """
        Remove the edge property p
        """
    @typing.overload
    def remove_edge_property(self, p: ...) -> None:
        """
        Remove the edge property p
        """
    @typing.overload
    def remove_face_property(self, p: ...) -> None:
        """
        Remove the face property p
        """
    @typing.overload
    def remove_face_property(self, p: ...) -> None:
        """
        Remove the face property p
        """
    @typing.overload
    def remove_face_property(self, p: ...) -> None:
        """
        Remove the face property p
        """
    @typing.overload
    def remove_face_property(self, p: ...) -> None:
        """
        Remove the face property p
        """
    @typing.overload
    def remove_halfedge_property(self, p: ...) -> None:
        """
        Remove the halfedge property p
        """
    @typing.overload
    def remove_halfedge_property(self, p: ...) -> None:
        """
        Remove the halfedge property p
        """
    @typing.overload
    def remove_halfedge_property(self, p: ...) -> None:
        """
        Remove the halfedge property p
        """
    @typing.overload
    def remove_halfedge_property(self, p: ...) -> None:
        """
        Remove the halfedge property p
        """
    @typing.overload
    def remove_vertex_property(self, p: ...) -> None:
        """
        Remove the vertex property p
        """
    @typing.overload
    def remove_vertex_property(self, p: ...) -> None:
        """
        Remove the vertex property p
        """
    @typing.overload
    def remove_vertex_property(self, p: ...) -> None:
        """
        Remove the vertex property p
        """
    @typing.overload
    def remove_vertex_property(self, p: ...) -> None:
        """
        Remove the vertex property p
        """
    @typing.overload
    def valence(self, v: ...) -> int:
        """
        Compute the valence of vertex `v` (number of incident edges).
        """
    @typing.overload
    def valence(self, v: ...) -> int:
        """
        Compute the valence of face `f` (its number of vertices).
        """
    def vertex_bool_property(self, name: str, T: bool = False) -> ...:
        """
        If a vertex property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def vertex_int_property(self, name: str, T: typing.SupportsInt = 0) -> ...:
        """
        If a vertex property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def vertex_properties(self) -> list[str]:
        """
        return the names of all vertex properties
        """
    def vertex_scalar_property(self, name: str, t: typing.SupportsFloat = 0.0) -> ...:
        """
        If a vertex property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def vertex_uint_property(self, name: str, T: typing.SupportsInt = 0) -> ...:
        """
        If a vertex property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def vertex_vector2d_property(self, name: str, t: Vector2D = ...) -> ...:
        """
        If a vertex property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    def vertex_vector3d_property(self, name: str, t: Vector3D = ...) -> ...:
        """
        If a vertex property of type T with name name exists, it is returned. Otherwise this property is added (with default value t)
        """
    @typing.overload
    def vertices(self) -> collections.abc.Iterator[...]:
        """
        Return vertex container
        """
    @typing.overload
    def vertices(self, v: ...) -> VertexAroundVertexCirculator:
        ...
    @typing.overload
    def vertices(self, f: ...) -> VertexAroundFaceCirculator:
        ...
class UintProperty:
    def __getitem__(self, arg0: typing.SupportsInt) -> int:
        ...
    def __setitem__(self, arg0: typing.SupportsInt, arg1: typing.SupportsInt) -> None:
        ...
    def vector(self) -> list[int]:
        """
        Get reference to the underlying vector.
        """
class Vector2D:
    __hash__: typing.ClassVar[None] = None
    @staticmethod
    @typing.overload
    def __init__(*args, **kwargs) -> None:
        ...
    def __add__(self, arg0: Vector2D) -> Vector2D:
        ...
    def __eq__(self, arg0: Vector2D) -> bool:
        ...
    @typing.overload
    def __getitem__(self, arg0: tuple[typing.SupportsInt, typing.SupportsInt]) -> float:
        ...
    @typing.overload
    def __getitem__(self, arg0: typing.SupportsInt) -> float:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, fill: typing.SupportsFloat) -> None:
        ...
    @typing.overload
    def __init__(self, x: typing.SupportsFloat, y: typing.SupportsFloat) -> None:
        """
        Construct 2D vector
        """
    def __len__(self) -> int:
        ...
    def __mul__(self, arg0: typing.SupportsFloat) -> Vector2D:
        ...
    def __repr__(self) -> str:
        ...
    def __rmul__(self, arg0: typing.SupportsFloat) -> Vector2D:
        ...
    @typing.overload
    def __setitem__(self, arg0: tuple[typing.SupportsInt, typing.SupportsInt], arg1: typing.SupportsFloat) -> None:
        ...
    @typing.overload
    def __setitem__(self, arg0: typing.SupportsInt, arg1: typing.SupportsFloat) -> None:
        ...
    def __sub__(self, arg0: Vector2D) -> Vector2D:
        ...
    def __truediv__(self, arg0: typing.SupportsFloat) -> Vector2D:
        ...
    def normalize(self) -> None:
        ...
class Vector2DProperty:
    def __getitem__(self, arg0: typing.SupportsInt) -> Vector2D:
        ...
    def __setitem__(self, arg0: typing.SupportsInt, arg1: Vector2D) -> None:
        ...
    def vector(self) -> list[Vector2D]:
        """
        Get reference to the underlying vector.
        """
class Vector3D:
    __hash__: typing.ClassVar[None] = None
    @staticmethod
    @typing.overload
    def __init__(*args, **kwargs) -> None:
        ...
    def __add__(self, arg0: Vector3D) -> Vector3D:
        ...
    def __eq__(self, arg0: Vector3D) -> bool:
        ...
    @typing.overload
    def __getitem__(self, arg0: tuple[typing.SupportsInt, typing.SupportsInt]) -> float:
        ...
    @typing.overload
    def __getitem__(self, arg0: typing.SupportsInt) -> float:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, fill: typing.SupportsFloat) -> None:
        ...
    @typing.overload
    def __init__(self, x: typing.SupportsFloat, y: typing.SupportsFloat, z: typing.SupportsFloat) -> None:
        """
        Construct 3D vector
        """
    def __len__(self) -> int:
        ...
    def __mul__(self, arg0: typing.SupportsFloat) -> Vector3D:
        ...
    def __repr__(self) -> str:
        ...
    def __rmul__(self, arg0: typing.SupportsFloat) -> Vector3D:
        ...
    @typing.overload
    def __setitem__(self, arg0: tuple[typing.SupportsInt, typing.SupportsInt], arg1: typing.SupportsFloat) -> None:
        ...
    @typing.overload
    def __setitem__(self, arg0: typing.SupportsInt, arg1: typing.SupportsFloat) -> None:
        ...
    def __sub__(self, arg0: Vector3D) -> Vector3D:
        ...
    def __truediv__(self, arg0: typing.SupportsFloat) -> Vector3D:
        ...
    def normalize(self) -> None:
        ...
class Vector3DProperty:
    def __getitem__(self, arg0: typing.SupportsInt) -> Vector3D:
        ...
    def __setitem__(self, arg0: typing.SupportsInt, arg1: Vector3D) -> None:
        ...
    def vector(self) -> list[Vector3D]:
        """
        Get reference to the underlying vector.
        """
class Vertex(Handle):
    @typing.overload
    def __init__(self) -> None:
        """
        default constructor with invalid index
        """
    @typing.overload
    def __init__(self, idx: typing.SupportsInt) -> None:
        """
        constructor with index
        """
class VertexAroundFaceCirculator:
    def __init__(self, mesh: ..., handle: ...) -> None:
        ...
    def __iter__(self) -> VertexAroundFaceCirculator:
        ...
    def __next__(self) -> ...:
        ...
    def prev(self) -> ...:
        ...
class VertexAroundVertexCirculator:
    def __init__(self, mesh: ..., handle: ...) -> None:
        ...
    def __iter__(self) -> VertexAroundVertexCirculator:
        ...
    def __next__(self) -> ...:
        ...
    def prev(self) -> ...:
        ...
class VertexBoolProperty(BoolProperty):
    def __getitem__(self, arg0: Vertex) -> bool:
        ...
    def __setitem__(self, arg0: Vertex, arg1: bool) -> None:
        ...
class VertexIntProperty(IntProperty):
    def __getitem__(self, arg0: Vertex) -> int:
        ...
    def __setitem__(self, arg0: Vertex, arg1: typing.SupportsInt) -> None:
        ...
class VertexScalarProperty(ScalarProperty):
    def __getitem__(self, arg0: Vertex) -> float:
        ...
    def __setitem__(self, arg0: Vertex, arg1: typing.SupportsFloat) -> None:
        ...
class VertexUintProperty(UintProperty):
    def __getitem__(self, arg0: Vertex) -> int:
        ...
    def __setitem__(self, arg0: Vertex, arg1: typing.SupportsInt) -> None:
        ...
class VertexVector2DProperty(Vector2DProperty):
    def __getitem__(self, arg0: Vertex) -> Vector2D:
        ...
    def __setitem__(self, arg0: Vertex, arg1: Vector2D) -> None:
        ...
class VertexVector3DProperty(Vector3DProperty):
    def __getitem__(self, arg0: Vertex) -> Vector3D:
        ...
    def __setitem__(self, arg0: Vertex, arg1: Vector3D) -> None:
        ...
Color = Vector3D
Normal = Vector3D
Point = Vector3D
TexCoord = Vector2D
