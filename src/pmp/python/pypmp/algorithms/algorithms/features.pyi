"""
Module for feature detection
"""
from __future__ import annotations
import typing
__all__: list[str] = ['clear_features', 'detect_boundary', 'detect_features']
def clear_features(mesh: ...) -> None:
    """
                clear_features(mesh: SurfaceMesh) -> None
    
                Clear feature and boundary edges.
    
                Extended Summary
                ----------------
                Sets all `"e:feature"` and `"v:feature"` properties to `false`.
    
                Notes
                -----
                This does not remove the corresponding property arrays.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. 
    """
def detect_boundary(mesh: ...) -> int:
    """
                detect_boundary(mesh: SurfaceMesh) -> int
    
                Mark all boundary edges as features.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. 
    
                Returns
                -------
                int
                    The number of boundary edges detected.
    """
def detect_features(mesh: ..., angle: typing.SupportsFloat) -> int:
    """
                detect_features(mesh: SurfaceMesh, angle: float) -> int
    
                Mark edges with dihedral angle larger than `angle` as feature.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. 
                angle : float
                    Angle threshold for marking as feature
    
                Returns
                -------
                int
                    The number of feature edges detected.
    """
