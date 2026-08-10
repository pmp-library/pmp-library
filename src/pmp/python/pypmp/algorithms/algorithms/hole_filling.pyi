"""
Module for mesh hole filling
"""
from __future__ import annotations
__all__: list[str] = ['fill_hole']
def fill_hole(mesh: ..., h: ...) -> None:
    """
                fill_hole(mesh: SurfaceMesh, h: Halfedge) -> None
    
                Fill the hole specified by halfedge `h`.
    
                Extended Summary
                ----------------
                Close simple holes (boundary loops of manifold vertices) by first
                filling the hole with an angle/area-minimizing triangulation, followed
                by isometric remeshing, and finished by curvature-minimizing fairing of the
                filled-in patch.
                See [12] for details.
                .. [12] Peter Liepa. Filling holes in meshes. In Proceedings of Eurographics Symposium on Geometry Processing, pages 200–205, 2003.
                
                Notes
                -----
                This algorithm works on general polygon meshes.
    
                Warnings
                --------
                The specified halfedge is valid. The specified halfedge is a boundary halfedge.
                The specified halfedge is not adjacent to a non-manifold hole.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    The input mesh. 
                h : Halfedge
                    The input halfedge.
    
                Raises
                ------
                InvalidInputException 
                    in case on of the input preconditions is violated
    """
