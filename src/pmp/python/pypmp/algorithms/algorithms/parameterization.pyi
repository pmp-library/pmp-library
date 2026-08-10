"""
Module for computing mesh parameterization
"""
from __future__ import annotations
__all__: list[str] = ['harmonic_parameterization', 'lscm_parameterization']
def harmonic_parameterization(mesh: ..., use_uniform_weights: bool = False) -> None:
    """
                harmonic_parameterization(mesh: SurfaceMesh, use_uniform_weights: bool = False)
    
                Compute discrete harmonic parameterization.
    
                Extended Summary
                ----------------
                See [14] for details.
                .. [14] Mathieu Desbrun, Mark Meyer, and Pierre Alliez. Intrinsic parameterizations of surface meshes. Computer Graphics Forum, 21(3):209–218, 2002.
    
                Notes
                -----
                This algorithm works on general polygon meshes. 
    
                Warnings
                --------
                The mesh needs a boundary.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The mesh for computing the harmonic parameterization.
                use_uniform_weights : bool = False
                    Use uniform weights for the harmonic parameterization.
                
                Raises
                ------
                InvalidInputException
                    if the input precondition is violated.
                SolverException
                    in case of failure to solve the linear system.
    """
def lscm_parameterization(mesh: ...) -> None:
    """
                lscm_parameterization(mesh: SurfaceMesh)
    
                Compute parameterization based on least squares conformal mapping.
    
                Extended Summary
                ----------------
                See [15] for details.
                .. [15] Bruno Lévy, Sylvain Petitjean, Nicolas Ray, and Jérome Maillot. Least squares conformal maps for automatic texture atlas generation. ACM Transaction on Graphics, 21(3):362–371, 2002.
    
                Notes
                -----
                This algorithm works on triangular meshes. 
    
                Warnings
                --------
                The mesh needs a boundary.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The mesh for computing the least squares parameterization.
                
                Raises
                ------
                InvalidInputException
                    if the input precondition is violated.
                SolverException
                    in case of failure to solve the linear system.
    """
