#include "algorithms.h"

using namespace pybind11::literals;

void bind_algorithms(py::module_& algorithms) {
    bind_fairing(algorithms);
    bind_triangulation(algorithms);
    bind_decimation(algorithms);
    bind_remeshing(algorithms);
}
    

void bind_fairing(py::module_& algorithms) {
    py::module_ fairing = algorithms.def_submodule(
        "fairing",
        "Module for minimizing surface features"
    );

    fairing.def("minimize_area", &pmp::minimize_area,
        R"pbdoc(
            minimize_area(mesh: SurfaceMesh) -> None

            Minimize surface area.

            Notes
            -----
            This algorithm works on general polygon meshes.

            See Also
            --------
            fair
        )pbdoc",
        "mesh"_a
    );

    fairing.def("minimize_curvature", &pmp::minimize_curvature,
        R"pbdoc(
            minimize_curvature(mesh: SurfaceMesh) -> None

            Minimize surface curvature.

            Notes
            -----
            This algorithm works on general polygon meshes.

            See Also
            --------
            fair
        )pbdoc",
        "mesh"_a
    );
    
    fairing.def("fair", &pmp::fair,
        R"pbdoc(
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

        )pbdoc",
        "mesh"_a, "k"_a = 2
    );
}

void bind_triangulation(py::module_& algorithms) {
    py::module_ triangulate = algorithms.def_submodule(
        "triangulation",
        "Module for triangulation"
    );

    triangulate.def("triangulate", [](pmp::SurfaceMesh& mesh) {
        pmp::triangulate(mesh);
    },
        R"pbdoc(
            triangulate(mesh: SurfaceMesh) -> None

            Triangulate all faces in `mesh` by applying triangulate().
        )pbdoc",
        "mesh"_a
    );
    
    triangulate.def("triangulate", [](pmp::SurfaceMesh& mesh, pmp::Face f) {
        pmp::triangulate(mesh, f);
    },
        R"pbdoc(
            triangulate(mesh: SurfaceMesh, f: Face) -> None

            Triangulate the Face `f`.

            Extended Summary
            ----------------
            Triangulate n-gons into n-2 triangles. Finds the triangulation that
            minimizes the sum of squared triangle areas.
            See [2]_ for details.
            .. [2] Peter Liepa. Filling holes in meshes. In Proceedings of Eurographics Symposium on Geometry Processing, pages 200–205, 2003.

            Warnings
            --------
            The input face is manifold.

            Notes
            -----
            This algorithm works on general polygon meshes.

            Raises
            ------
            InvalidInputException
                in case the input precondition is violated
        )pbdoc",
        "mesh"_a, "f"_a
    );

}

void bind_decimation(py::module_& algorithms) {
    py::module_ triangulate = algorithms.def_submodule(
        "decimation",
        "Module for decimation"
    );

    triangulate.def("decimate", &pmp::decimate,
        R"pbdoc(
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
        )pbdoc",
        "mesh"_a, "n_vertices"_a, "aspect_ratio"_a = 0.0,
        "edge_length"_a = 0.0, "max_valence"_a = 0, "normal_deviation"_a = 0.0,
        "hausdorff_error"_a = 0.0, "seam_threshold"_a = 1e-2, 
        "seam_angle_deviation"_a = 1.0
    );
}

void bind_remeshing(py::module_& algorithms) {
    py::module_ remeshing = algorithms.def_submodule(
        "remeshing",
        "Module for remeshing"
    );

    remeshing.def("uniform_remeshing", &pmp::uniform_remeshing,
        R"pbdoc(
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
        )pbdoc",
        "mesh"_a, "edge_length"_a, "iterations"_a = 10,
        "use_projection"_a = true
    );
    
    remeshing.def("adaptive_remeshing", &pmp::adaptive_remeshing,
        R"pbdoc(
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
        )pbdoc",
        "mesh"_a, "min_edge_length"_a, "max_edge_length"_a,
        "approx_error"_a, "iterations"_a = 10, "use_projection"_a = true
    );
}