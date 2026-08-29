"""
Module for mesh utilities
"""
from __future__ import annotations
import typing
__all__: list[str] = ['BoundingBox', 'bounds', 'connected_components', 'edge_length', 'flip_faces', 'mean_edge_length', 'min_face_area']
class BoundingBox:
    @staticmethod
    @typing.overload
    def __iadd__(*args, **kwargs) -> BoundingBox:
        ...
    @staticmethod
    def __init__(*args, **kwargs) -> None:
        """
        Construct from min and max points.
        """
    @typing.overload
    def __iadd__(self, arg0: BoundingBox) -> BoundingBox:
        ...
    def center(self) -> ...:
        """
        Get center point.
        """
    def is_empty(self) -> bool:
        """
        Indicate if the bounding box is empty.
        """
    def max(self) -> ...:
        """
        Get max point.
        """
    def min(self) -> ...:
        """
        Get min point.
        """
    def size(self) -> float:
        """
        Get the size of the bounding box.
        """
def bounds(mesh: ...) -> BoundingBox:
    """
    Compute bounding box of `mesh`.
    """
def connected_components(mesh: ...) -> int:
    """
                connected_components(mesh: SurfaceMesh) -> int
    
                Compute connected components in `mesh`.
    
                Extended Summary
                ----------------
                Adds a new vertex property `v:component` containing the component index.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The mesh for computing the connected components.
                
                Returns
                -------
                int
                    The number of connected components.
    """
def edge_length(mesh: ..., e: ...) -> float:
    """
    Compute length of an edge `e` in `mesh`.
    """
def flip_faces(mesh: ...) -> None:
    """
    Flip the orientation of all faces in `mesh`.
    """
def mean_edge_length(mesh: ...) -> float:
    """
    Compute mean edge length of `mesh`.
    """
def min_face_area(mesh: ...) -> float:
    """
    Compute the minimum area of all faces in `mesh`.
    """
