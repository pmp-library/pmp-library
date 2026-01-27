"""
Module for mesh subdivision
"""
from __future__ import annotations
import typing
__all__: list[str] = ['BoundaryHandling', 'catmull_clark_subdivision', 'linear_subdivision', 'loop_subdivision', 'quad_tri_subdivision']
class BoundaryHandling:
    """
    Members:
    
      Interpolate
    
      Preserve
    """
    Interpolate: typing.ClassVar[BoundaryHandling]  # value = <BoundaryHandling.Interpolate: 0>
    Preserve: typing.ClassVar[BoundaryHandling]  # value = <BoundaryHandling.Preserve: 1>
    __members__: typing.ClassVar[dict[str, BoundaryHandling]]  # value = {'Interpolate': <BoundaryHandling.Interpolate: 0>, 'Preserve': <BoundaryHandling.Preserve: 1>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __ne__(self, other: typing.Any) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    def __setstate__(self, state: typing.SupportsInt) -> None:
        ...
    def __str__(self) -> str:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def value(self) -> int:
        ...
def catmull_clark_subdivision(mesh: ..., boundary_handling: BoundaryHandling = ...) -> None:
    """
                catmull_clark_subdivision(mesh: SurfaceMesh, boundary_handling: BoundaryHandling
                ) -> None
    
                Perform one step of Catmull-Clark subdivision.
    
                Extended Summary
                ----------------
                See [8]_ for details.
                .. [8] Edwin Catmull and James Clark. Recursively generated b-spline surfaces on arbitrary topological meshes. Computer-Aided Design, 10(6):350–355, 1978.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. Modified in place.
                boundary_handling : BoundaryHandling = BoundaryHandling.Interpolate
                    Specify to interpolate or preserve boundary edges. Available options: `BoundaryHandling.Interpolate` or `BoundaryHandling.Preserve`.
    """
def linear_subdivision(mesh: ...) -> None:
    """
                linear_subdivision(mesh: SurfaceMesh) -> None
    
                Perform one step of linear quad-tri subdivision.
    
                Extended Summary
                ----------------
                Suitable for mixed quad/triangle meshes.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. Modified in place.
    """
def loop_subdivision(mesh: ..., boundary_handling: BoundaryHandling = ...) -> None:
    """
                loop_subdivision(mesh: SurfaceMesh, boundary_handling: BoundaryHandling
                ) -> None
    
                Perform one step of Loop subdivision.
    
                Extended Summary
                ----------------
                See [9]_ for details.
                .. [9] Charles Teorell Loop. Smooth subdivision surfaces based on triangles. Master's thesis, University of Utah, Department of Mathematics, 1987.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. Modified in place.
                boundary_handling : BoundaryHandling = BoundaryHandling.Interpolate
                    Specify to interpolate or preserve boundary edges. Available options: `BoundaryHandling.Interpolate` or `BoundaryHandling.Preserve`.
    
                Warnings
                --------
                Requires a triangle mesh as input.
    
                Raises
                ------
                InvalidInputException in case the input violates the precondition.
    """
def quad_tri_subdivision(mesh: ..., boundary_handling: BoundaryHandling = ...) -> None:
    """
                quad_tri_subdivision(mesh: SurfaceMesh, boundary_handling: BoundaryHandling
                ) -> None
    
                Perform one step of quad-tri subdivision.
    
                Extended Summary
                ----------------
                Suitable for mixed quad/triangle meshes. See [10]_ for details.
                .. [10] Jos Stam and Charles Loop. Quad/triangle subdivision. Computer Graphics Forum, 22(1), 2003.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. Modified in place.
                boundary_handling : BoundaryHandling = BoundaryHandling.Interpolate
                    Specify to interpolate or preserve boundary edges. Available options: `BoundaryHandling.Interpolate` or `BoundaryHandling.Preserve`.
    """
