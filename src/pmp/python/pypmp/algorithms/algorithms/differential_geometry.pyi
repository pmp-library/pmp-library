"""
Module for mesh analysis
"""
from __future__ import annotations
__all__: list[str] = ['centroid', 'dual', 'face_area', 'laplace', 'surface_area', 'triangle_area', 'volume', 'voronoi_area']
def centroid(mesh: ...) -> ...:
    """
                centroid(mesh: SurfaceMesh) -> float
    
                Compute the barycenter (centroid) of the `mesh`.
    
                Extended Summary
                ----------------
                Computed as area-weighted mean of vertices.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The mesh for computing the centroid.
    
                Returns
                -------
                float
                    The mesh centroid
    """
def dual(mesh: ...) -> None:
    """
                dual(mesh: SurfaceMesh) -> None
    
                Compute dual of a `mesh`.
    
                Warnings
                --------
                Changes the mesh in place. All properties are cleared.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The mesh for computing the dual. Modified in place.
    """
def face_area(mesh: ..., f: ...) -> float:
    """
                face_area(mesh: SurfaceMesh, f: Face) -> float
    
                Compute area of face `f`.
    
                Extended Summary
                ----------------
                Computes standard area for triangles and norm of vector area for other polygons.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    Mesh corresponding to face `f`.
                face : Face
                    The face for computing the surface area
    
                Returns
                -------
                float
                    The surface area
    """
def laplace(mesh: ..., v: ...) -> ...:
    """
                laplace(mesh: SurfaceMesh, v: Vertex) -> Point
    
                Compute the Laplace vector for vertex `v`, normalized by Voronoi area.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The mesh for computing the Laplace vector.
                v : Vertex
                    Voronoi vertex.
    
                Returns
                -------
                Point
                    The Laplace vector
    
                Notes
                -----
                Input mesh needs to be a triangle mesh.
    """
def surface_area(mesh: ...) -> float:
    """
                surface_area(mesh: SurfaceMesh) -> float
    
                Compute the surface area of `mesh` as the sum of face areas.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The mesh for computing the surface area.
    
                Returns
                -------
                float
                    The surface area
    """
def triangle_area(*args, **kwargs) -> float:
    """
                triangle_area(p0: Point, p1: Point, p2: Point) -> float
    
                Compute the area of a triangle given by three points.
    
                Returns
                -------
                float
                    The surface area.
    """
def volume(mesh: ...) -> float:
    """
                volume(mesh: SurfaceMesh) -> float
    
                Compute the volume of a mesh.
    
                Extended Summary
                ----------------
                See [13] for details
                .. [13] Cha Zhang and Tsuhan Chen. Efficient feature extraction for 2d/3d objects in mesh representation. In Proceedings 2001 International Conference on Image Processing (Cat. No.01CH37205), 2002.
    
                Warnings
                --------
                Input mesh needs to be a triangle mesh.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The mesh for computing the volume.
    
                Returns
                -------
                float
                    The mesh volume
    
                Raises
                ------
                    InvalidInputException if the input precondition is violated.
    """
def voronoi_area(mesh: ..., v: ...) -> float:
    """
                dual(mesh: SurfaceMesh, v: Vertex) -> float
    
                Compute the (barycentric) Voronoi area of vertex `v`.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The mesh for computing the voronoi_area.
                v : Vertex
                    Voronoi vertex.
    
                Returns
                -------
                float
                    The voronoi area
    """
