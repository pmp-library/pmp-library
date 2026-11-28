"""
Module for computing the geodesic distance from a set of vertices
"""
from __future__ import annotations
import collections.abc
import typing
__all__: list[str] = ['distance_to_texture_coordinates', 'geodesics', 'geodesics_heat']
def distance_to_texture_coordinates(mesh: ...) -> None:
    """
                distance_to_texture_coordinates(mesh: SurfaceMesh)
    
                Use the normalized distances as texture coordinates
    
                Extended Summary
                ----------------
                Stores the normalized distances in a vertex property of type
                TexCoord named "v:tex". Reuses any existing vertex property of the
                same type and name.
                
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh, modified in place.
    """
def geodesics(mesh: ..., seeds: collections.abc.Sequence[...], maxdist: typing.SupportsFloat = 3.4028234663852886e+38, maxnum: typing.SupportsInt = 4294967295) -> tuple:
    """
                geodesics(mesh: SurfaceMesh, seeds: List[Vertex], maxdist: float = sys.float_info.max, 
                    maxnum: int = 4294967295)
    
                Compute geodesic distance from a set of seed vertices
    
                Extended Summary
                ----------------
                The method works by a Dijkstra-like breadth first traversal from
                the seed vertices, implemented by a heap structure.
                
                See [16] for details.
                .. [16] Ron Kimmel and James Albert Sethian. Computing geodesic paths on manifolds. Proceedings of the National Academy of Sciences, 95(15):8431–8435, 1998.
    
                Notes
                -----
                This algorithm works on triangular meshes. 
    
                Warnings
                --------
                The mesh needs a boundary.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh, modified in place.
                seeds : List[Vertex]
                    The vector of seed vertices.
                maxdist : float = sys.float_info.max
                    The maximum distance up to which to compute the
                    geodesic distances.
                maxnum : int = 4294967295
                    The maximum number of neighbors up to which to
                    compute the geodesic distances.
    
                Returns
                -------
                tuple
                    The number of neighbors that have been found and corresponding neighbors list
    """
def geodesics_heat(mesh: ..., seeds: collections.abc.Sequence[...]) -> None:
    """
                geodesics_heat(mesh: SurfaceMesh, seeds: List[Vertex])
    
                Compute geodesic distance from a set of seed vertices
    
                Extended Summary
                ----------------
                Compute geodesic distances based on the heat method,
                by solving two Poisson systems. Works on general polygon meshes.
                
                See [17] for details.
                .. [17] Keenan Crane, Clarisse Weischedel, and Max Wardetzky. Geodesics in heat: A new approach to computing distance based on heat flow. ACM Transactions on Graphics, 32(5), 2013.
    
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                Warnings
                --------
                The mesh needs a boundary.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh, modified in place.
                seeds : List[Vertex]
                    The vector of seed vertices.
    """
