"""
Module for minimizing surface features
"""
from __future__ import annotations
import typing
__all__: list[str] = ['fair', 'minimize_area', 'minimize_curvature']
def fair(mesh: ..., k: typing.SupportsInt = 2) -> None:
    """
                fair(mesh: SurfaceMesh, k: int = 2) -> None
    
                Implicit surface fairing.
    
                Extended Summary
                ----------------
                Computes a surface by solving k-harmonic equation. See also [1]_
                .. [1] Mathieu Desbrun, Mark Meyer, , Peter Schröder, and Alan H. Barr. Implicit fairing of irregular meshes using diffusion and curvature flow. In Proceedings of SIGGRAPH, pages 317–324, 1999.
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                Raises
                ------
                SolverException
                    in case of failure to solve the linear system
                InvalidInputException
                    in case of missing boundary constraints
    """
def minimize_area(mesh: ...) -> None:
    """
                minimize_area(mesh: SurfaceMesh) -> None
    
                Minimize surface area.
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                See Also
                --------
                fair
    """
def minimize_curvature(mesh: ...) -> None:
    """
                minimize_curvature(mesh: SurfaceMesh) -> None
    
                Minimize surface curvature.
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                See Also
                --------
                fair
    """
