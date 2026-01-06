"""
Module for mesh Laplacian smoothing
"""
from __future__ import annotations
import typing
__all__: list[str] = ['explicit_smoothing', 'implicit_smoothing']
def explicit_smoothing(mesh: ..., iterations: typing.SupportsInt = 10, use_uniform_laplace: bool = True) -> None:
    """
                explicit_smoothing(mesh: SurfaceMesh, iterations: int = 10, use_uniform_laplace : bool = True
                ) -> None
    
                Perform explicit Laplacian smoothing.
    
                Extended Summary
                ----------------
                See [7]_ for details.
                .. [7] Mathieu Desbrun, Mark Meyer, , Peter Schröder, and Alan H. Barr. Implicit fairing of irregular meshes using diffusion and curvature flow. In Proceedings of SIGGRAPH, pages 317–324, 1999.
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. Modified in place.
                iterations : int = 10
                    The number of iterations performed.
                use_uniform_laplace : bool = True
                    Use uniform or cotan Laplacian. Default: cotan.
    """
def implicit_smoothing(mesh: ..., timestep: typing.SupportsFloat = 0.001, iterations: typing.SupportsInt = 1, use_uniform_laplace: bool = True, rescale: bool = True) -> None:
    """
                implicit_smoothing(mesh: SurfaceMesh, timestep: float = 0.001, 
                iterations: int = 1, use_uniform_laplace : bool = True,
                rescale: bool = True
                ) -> None
    
                Perform implicit Laplacian smoothing.
    
                Extended Summary
                ----------------
                See [7]_ and [8]_ for details.
                .. [7] Mathieu Desbrun, Mark Meyer, , Peter Schröder, and Alan H. Barr. Implicit fairing of irregular meshes using diffusion and curvature flow. In Proceedings of SIGGRAPH, pages 317–324, 1999.
                .. [8] Misha Kazhdan, Justin Solomon, and Mirela Ben-Chen. Can mean-curvature flow be modified to be non-singular? Computer Graphics Forum, 31(5), 2012.
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. Modified in place.
                timestep : float
                    The time step taken.
                iterations : int = 1
                    The number of iterations performed.
                use_uniform_laplace : bool = True
                    Use uniform or cotan Laplacian. Default: cotan.
                rescale : bool = True
                    Re-center and re-scale model after smoothing.
    
                Raises
                ------
                SolverException in case of a failure to solve the linear system.
    """
