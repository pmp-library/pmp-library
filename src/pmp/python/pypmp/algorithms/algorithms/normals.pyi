"""
Module for mesh normals computation
"""
from __future__ import annotations
import typing
__all__: list[str] = ['corner_normal', 'face_normal', 'face_normals', 'vertex_normal', 'vertex_normals']
def corner_normal(mesh: ..., h: ..., crease_angle: typing.SupportsFloat) -> ...:
    """
                corner_normal(mesh: SurfaceMesh, h: Halfedge, crease_angle: float) -> Normal
    
                Compute the normal vector of the polygon corner specified by the
                target vertex of halfedge `h`.
    
                Extended Summary
                ----------------
                Averages incident corner normals if they are within `crease_angle`
                of the face normal. `crease_angle` is in radians, not degrees.
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. 
                h : Halfedge
                    The input halfedge.
                crease_angle : float
                    The crease angle.
    
                Returns
                -------
                Normal
                    Normal vector
    """
def face_normal(mesh: ..., f: ...) -> ...:
    """
                face_normal(mesh: SurfaceMesh, f: Face) -> Normal
    
                Compute the normal vector of face `f`.
    
                Extended Summary
                ----------------
                Normal is computed as (normalized) sum of per-corner
                cross products of the two incident edges. This corresponds to
                the normalized vector area in [11].
                .. [11] Marc Alexa and Max Wardetzky. Discrete laplacians on general polygonal meshes. ACM Transactions on Graphics, 30(4), 2011.
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. 
                f : Face
                    The input face.
    
                Returns
                -------
                Normal
                    Normal vector
    """
def face_normals(mesh: ...) -> None:
    """
                face_normals(mesh: SurfaceMesh) -> None
    
                Compute face normals for the whole `mesh`.
    
                Extended Summary
                ----------------
                Calls face_normal() for each face and adds a new 
                face property of type `Normal` named "f:normal".
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. Modified in place.
    """
def vertex_normal(mesh: ..., v: ...) -> ...:
    """
                vertex_normal(mesh: SurfaceMesh, v: Vertex) -> Normal
    
                Compute the normal vector of vertex `v`.
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. 
                v : Vertex
                    The input vertex.
    
                Returns
                -------
                Normal
                    Normal vector
    """
def vertex_normals(mesh: ...) -> None:
    """
                vertex_normals(mesh: SurfaceMesh) -> None
    
                Compute vertex normals for the whole `mesh`.
    
                Extended Summary
                ----------------
                Calls vertex_normal() for each vertex and adds a new 
                vertex property of type `Normal` named "v:normal".
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. Modified in place.
    """
