"""
Module for triangulation
"""
from __future__ import annotations
import typing
__all__: list[str] = ['triangulate']
@typing.overload
def triangulate(mesh: ...) -> None:
    """
                triangulate(mesh: SurfaceMesh) -> None
    
                Triangulate all faces in `mesh` by applying triangulate().
    """
@typing.overload
def triangulate(mesh: ..., f: ...) -> None:
    """
                triangulate(mesh: SurfaceMesh, f: Face) -> None
    
                Triangulate the Face `f`.
    
                Extended Summary
                ----------------
                Triangulate n-gons into n-2 triangles. Finds the triangulation that
                minimizes the sum of squared triangle areas.
                See [2]_ for details.
                .. [2] Peter Liepa. Filling holes in meshes. In Proceedings of Eurographics Symposium on Geometry Processing, pages 200–205, 2003.
    
                Warnings
                --------
                The input face is manifold.
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                Raises
                ------
                InvalidInputException
                    in case the input precondition is violated
    """
