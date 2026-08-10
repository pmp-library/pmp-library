"""
Module for remeshing
"""
from __future__ import annotations
import typing
__all__: list[str] = ['adaptive_remeshing', 'uniform_remeshing']
def adaptive_remeshing(mesh: ..., min_edge_length: typing.SupportsFloat, max_edge_length: typing.SupportsFloat, approx_error: typing.SupportsFloat, iterations: typing.SupportsInt = 10, use_projection: bool = True) -> None:
    """
                adaptive_remeshing(mesh: SurfaceMesh, min_edge_length: float, max_edge_length: float, 
                approx_error: float, iterations: int = 10, use_projection: bool = True
                ) -> None
    
                Perform adaptive remeshing.
    
                Extended Summary
                ----------------
                Performs incremental remeshing based
                on edge collapse, split, flip, and tangential relaxation.
                See [5]_ and [6]_ for details.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    Target mesh. Modified in place.
                min_edge_length : float
                    The minimum edge length.
                max_edge_length : float
                    The maximum edge length.
                approx_error : float
                    The maximum approximation error.
                iterations : int = 10 
                    The number of iterations
                use_projection : bool = True
                    Use back-projection to the input surface.
    
                Warnings
                --------
                Input mesh needs to be a triangle mesh.
    
                Raises
                ------
                InvalidInputException
                    if the input precondition is violated.
    """
def uniform_remeshing(mesh: ..., edge_length: typing.SupportsFloat, iterations: typing.SupportsInt = 10, use_projection: bool = True) -> None:
    """
                uniform_remeshing(mesh: SurfaceMesh, edge_length: float, iterations: int = 10,
                use_projection: bool = True
                ) -> None
    
                Perform uniform remeshing.
    
                Extended Summary
                ----------------
                Performs incremental remeshing based
                on edge collapse, split, flip, and tangential relaxation.
                See [5]_ and [6]_ for details.
                .. [5] Mario Botsch and Leif Kobbelt. A remeshing approach to multiresolution modeling. In Proceedings of Eurographics Symposium on Geometry Processing, pages 189–96, 2004.
                .. [6] Marion Dunyach, David Vanderhaeghe, Loïc Barthe, and Mario Botsch. Adaptive remeshing for real-time mesh deformation. In Eurographics 2013 - Short Papers, pages 29–32, 2013.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    Target mesh. Modified in place.
                edge_length : float
                    The target edge length.
                iterations : int = 10 
                    The number of iterations
                use_projection : bool = True
                    Use back-projection to the input surface.
    
                Warnings
                --------
                Input mesh needs to be a triangle mesh.
    
                Raises
                ------
                InvalidInputException
                    if the input precondition is violated.
    """
