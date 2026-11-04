"""
Module for decimation
"""
from __future__ import annotations
import typing
__all__: list[str] = ['decimate']
def decimate(mesh: ..., n_vertices: typing.SupportsInt, aspect_ratio: typing.SupportsFloat = 0.0, edge_length: typing.SupportsFloat = 0.0, max_valence: typing.SupportsInt = 0, normal_deviation: typing.SupportsFloat = 0.0, hausdorff_error: typing.SupportsFloat = 0.0, seam_threshold: typing.SupportsFloat = 0.01, seam_angle_deviation: typing.SupportsFloat = 1.0) -> None:
    """
                decimate(mesh: SurfaceMesh, n_vertices: int, aspect_ratio: float = 0.0,
                edge_length: float = 0.0, max_valence: int = 0, normal_deviation: float = 0.0,
                hausdorff_error: float = 0.0, seam_threshold: float = 1e-2,
                seam_angle_deviation: float = 1.0
                ) -> None
    
                Mesh decimation based on approximation error and fairness criteria.
    
                Extended Summary
                ----------------
                Performs incremental greedy mesh decimation based on halfedge
                collapses. See [3]_ and [4]_ for details.
                .. [3] Leif Kobbelt, Swen Campagna, and Hans-Peter Seidel. A general framework for mesh decimation. In Proceedings of Graphics Interface, pages 43–50, 1998.
                .. [4] Michael Garland and Paul Seagrave Heckbert. Surface simplification using quadric error metrics. In Proceedings of the 24th Annual Conference on Computer Graphics and Interactive Techniques, SIGGRAPH '97, pages 209–216, 1997.
    
                Parameters
                ----------
                mesh : SurfaceMesh
                    Target mesh. Modified in place.
                n_vertices : int
                    Target number of vertices.
                aspect_ratio : float
                    Minimum aspect ratio of the triangles.
                edge_length : float
                    Minimum target edge length.
                max_valence : int
                    Maximum number of incident edges per vertex.
                normal_deviation : float
                    Maximum deviation of face normals.
                hausdorff_error : float
                    Maximum deviation from the original surface.
                seam_threshold : float 
                    Threshold for texture seams.
                seam_angle_deviation : float
                    Maximum texture seam deviation.
    
                Warnings
                --------
                Input mesh needs to be a triangle mesh.
    
                Raises
                ------
                InvalidInputException
                    if the input precondition is violated.
    """
