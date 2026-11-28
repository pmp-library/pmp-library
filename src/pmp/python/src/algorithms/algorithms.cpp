#include "algorithms.h"

using namespace pybind11::literals;

void bind_algorithms(py::module_& algorithms) {
    bind_fairing(algorithms);
    bind_triangulation(algorithms);
    bind_decimation(algorithms);
    bind_remeshing(algorithms);
    bind_smoothing(algorithms);
    bind_subdivision(algorithms);
    bind_normals(algorithms);
    bind_hole_filling(algorithms);
    bind_features(algorithms);
    bind_differential_geometry(algorithms);
    bind_utilities(algorithms);
    bind_parameterization(algorithms);
    bind_distance_point_triangle(algorithms);
    bind_geodesics(algorithms);
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
    py::module_ decimation = algorithms.def_submodule(
        "decimation",
        "Module for decimation"
    );

    decimation.def("decimate", &pmp::decimate,
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

void bind_smoothing(py::module_& algorithms) {
    py::module_ smoothing = algorithms.def_submodule(
        "smoothing",
        "Module for mesh Laplacian smoothing"
    );

    smoothing.def("explicit_smoothing", &pmp::explicit_smoothing,
        R"pbdoc(
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

        )pbdoc",
        "mesh"_a, "iterations"_a = 10, "use_uniform_laplace"_a = true
    );
    
    smoothing.def("implicit_smoothing", &pmp::implicit_smoothing,
        R"pbdoc(
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

        )pbdoc",
        "mesh"_a, "timestep"_a = 0.001, "iterations"_a = 1, "use_uniform_laplace"_a = true,
        "rescale"_a = true
    );
}

void bind_subdivision(py::module_& algorithms) {
    py::module_ subdivision = algorithms.def_submodule(
        "subdivision",
        "Module for mesh subdivision"
    );

    py::enum_<pmp::BoundaryHandling>(subdivision, "BoundaryHandling")
        .value("Interpolate", pmp::BoundaryHandling::Interpolate)
        .value("Preserve", pmp::BoundaryHandling::Preserve);

    subdivision.def("catmull_clark_subdivision", &pmp::catmull_clark_subdivision,
        R"pbdoc(
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

        )pbdoc",
        "mesh"_a, "boundary_handling"_a=pmp::BoundaryHandling::Interpolate
    );
    
    subdivision.def("loop_subdivision", &pmp::loop_subdivision,
        R"pbdoc(
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
        )pbdoc",
        "mesh"_a, "boundary_handling"_a=pmp::BoundaryHandling::Interpolate
    );
    
    subdivision.def("quad_tri_subdivision", &pmp::quad_tri_subdivision,
        R"pbdoc(
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

        )pbdoc",
        "mesh"_a, "boundary_handling"_a=pmp::BoundaryHandling::Interpolate
    );
    
    subdivision.def("linear_subdivision", &pmp::linear_subdivision,
        R"pbdoc(
            linear_subdivision(mesh: SurfaceMesh) -> None

            Perform one step of linear quad-tri subdivision.

            Extended Summary
            ----------------
            Suitable for mixed quad/triangle meshes.

            Parameters
            ----------
            mesh : SurfaceMesh
                The input mesh. Modified in place.

        )pbdoc",
        "mesh"_a
    );
    
}

void bind_normals(py::module_& algorithms) {
    py::module_ normals = algorithms.def_submodule(
        "normals",
        "Module for mesh normals computation"
    );

    normals.def("vertex_normals", &pmp::vertex_normals,
        R"pbdoc(
            vertex_normals(mesh: SurfaceMesh) -> None

            Compute vertex normals for the whole `mesh`.

            Extended Summary
            ----------------
            Calls vertex_normal() for each vertex and adds a new 
            vertex property of type `Normal` named "v:normal".

            Notes
            -----
            This algorithm works on general polygon meshes.

            Parameters
            ----------
            mesh : SurfaceMesh
                The input mesh. Modified in place.

        )pbdoc",
        "mesh"_a
    );
    
    normals.def("face_normals", &pmp::face_normals,
        R"pbdoc(
            face_normals(mesh: SurfaceMesh) -> None

            Compute face normals for the whole `mesh`.

            Extended Summary
            ----------------
            Calls face_normal() for each face and adds a new 
            face property of type `Normal` named "f:normal".

            Notes
            -----
            This algorithm works on general polygon meshes.

            Parameters
            ----------
            mesh : SurfaceMesh
                The input mesh. Modified in place.

        )pbdoc",
        "mesh"_a
    );
    
    normals.def("vertex_normal", &pmp::vertex_normal,
        R"pbdoc(
            vertex_normal(mesh: SurfaceMesh, v: Vertex) -> Normal

            Compute the normal vector of vertex `v`.

            Notes
            -----
            This algorithm works on general polygon meshes.

            Parameters
            ----------
            mesh : SurfaceMesh
                The input mesh. 
            v : Vertex
                The input vertex.

            Returns
            -------
            Normal
                Normal vector

        )pbdoc",
        "mesh"_a, "v"_a
    );
    
    normals.def("face_normal", &pmp::face_normal,
        R"pbdoc(
            face_normal(mesh: SurfaceMesh, f: Face) -> Normal

            Compute the normal vector of face `f`.

            Extended Summary
            ----------------
            Normal is computed as (normalized) sum of per-corner
            cross products of the two incident edges. This corresponds to
            the normalized vector area in [11].
            .. [11] Marc Alexa and Max Wardetzky. Discrete laplacians on general polygonal meshes. ACM Transactions on Graphics, 30(4), 2011.

            Notes
            -----
            This algorithm works on general polygon meshes.

            Parameters
            ----------
            mesh : SurfaceMesh
                The input mesh. 
            f : Face
                The input face.

            Returns
            -------
            Normal
                Normal vector

        )pbdoc",
        "mesh"_a, "f"_a
    );
    
    normals.def("corner_normal", &pmp::corner_normal,
        R"pbdoc(
            corner_normal(mesh: SurfaceMesh, h: Halfedge, crease_angle: float) -> Normal

            Compute the normal vector of the polygon corner specified by the
            target vertex of halfedge `h`.

            Extended Summary
            ----------------
            Averages incident corner normals if they are within `crease_angle`
            of the face normal. `crease_angle` is in radians, not degrees.

            Notes
            -----
            This algorithm works on general polygon meshes.

            Parameters
            ----------
            mesh : SurfaceMesh
                The input mesh. 
            h : Halfedge
                The input halfedge.
            crease_angle : float
                The crease angle.

            Returns
            -------
            Normal
                Normal vector

        )pbdoc",
        "mesh"_a, "h"_a, "crease_angle"_a
    );

}

void bind_hole_filling(py::module_& algorithms) {
    py::module_ hole_filling = algorithms.def_submodule(
        "hole_filling",
        "Module for mesh hole filling"
    );

    hole_filling.def("fill_hole", &pmp::fill_hole,
        R"pbdoc(
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

        )pbdoc",
        "mesh"_a, "h"_a
    );
}

void bind_features(py::module_& algorithms) {
    py::module_ features = algorithms.def_submodule(
        "features",
        "Module for feature detection"
    );

    features.def("detect_features", &pmp::detect_features,
        R"pbdoc(
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
            
        )pbdoc",
        "mesh"_a, "angle"_a
    );
    
    features.def("detect_boundary", &pmp::detect_boundary,
        R"pbdoc(
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
            
        )pbdoc",
        "mesh"_a
    );
    
    features.def("clear_features", &pmp::clear_features,
        R"pbdoc(
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
            
        )pbdoc",
        "mesh"_a
    );
}
            
void bind_differential_geometry(py::module_& algorithms) {
    py::module_ differential_geometry = algorithms.def_submodule(
        "differential_geometry",
        "Module for mesh analysis"
    );

    differential_geometry.def("triangle_area", &pmp::triangle_area,
        R"pbdoc(
            triangle_area(p0: Point, p1: Point, p2: Point) -> float

            Compute the area of a triangle given by three points.

            Returns
            -------
            float
                The surface area.
            
        )pbdoc",
        "p0"_a, "p1"_a, "p2"_a
    );
    
    differential_geometry.def("face_area", &pmp::face_area,
        R"pbdoc(
            face_area(mesh: SurfaceMesh, f: Face) -> float

            Compute area of face `f`.

            Extended Summary
            ----------------
            Computes standard area for triangles and norm of vector area for other polygons.

            Parameters
            ----------
            mesh : SurfaceMesh
                Mesh corresponding to face `f`.
            face : Face
                The face for computing the surface area

            Returns
            -------
            float
                The surface area
            
        )pbdoc",
        "mesh"_a, "f"_a
    );
    
    differential_geometry.def("surface_area", &pmp::surface_area,
        R"pbdoc(
            surface_area(mesh: SurfaceMesh) -> float

            Compute the surface area of `mesh` as the sum of face areas.

            Parameters
            ----------
            mesh : SurfaceMesh
                The mesh for computing the surface area.

            Returns
            -------
            float
                The surface area
            
        )pbdoc",
        "mesh"_a
    );

    differential_geometry.def("volume", &pmp::volume,
        R"pbdoc(
            volume(mesh: SurfaceMesh) -> float

            Compute the volume of a mesh.

            Extended Summary
            ----------------
            See [13] for details
            .. [13] Cha Zhang and Tsuhan Chen. Efficient feature extraction for 2d/3d objects in mesh representation. In Proceedings 2001 International Conference on Image Processing (Cat. No.01CH37205), 2002.

            Warnings
            --------
            Input mesh needs to be a triangle mesh.

            Parameters
            ----------
            mesh : SurfaceMesh
                The mesh for computing the volume.

            Returns
            -------
            float
                The mesh volume

            Raises
            ------
                InvalidInputException if the input precondition is violated.
            
        )pbdoc",
        "mesh"_a
    );
    
    differential_geometry.def("centroid", [](const pmp::SurfaceMesh& mesh){
        return centroid(mesh);
    },
        R"pbdoc(
            centroid(mesh: SurfaceMesh) -> float

            Compute the barycenter (centroid) of the `mesh`.

            Extended Summary
            ----------------
            Computed as area-weighted mean of vertices.

            Parameters
            ----------
            mesh : SurfaceMesh
                The mesh for computing the centroid.

            Returns
            -------
            float
                The mesh centroid
            
        )pbdoc",
        "mesh"_a
    );

    differential_geometry.def("dual", &pmp::dual,
        R"pbdoc(
            dual(mesh: SurfaceMesh) -> None

            Compute dual of a `mesh`.

            Warnings
            --------
            Changes the mesh in place. All properties are cleared.

            Parameters
            ----------
            mesh : SurfaceMesh
                The mesh for computing the dual. Modified in place.
            
        )pbdoc",
        "mesh"_a
    );
 
    differential_geometry.def("voronoi_area", &pmp::voronoi_area,
        R"pbdoc(
            dual(mesh: SurfaceMesh, v: Vertex) -> float

            Compute the (barycentric) Voronoi area of vertex `v`.

            Parameters
            ----------
            mesh : SurfaceMesh
                The mesh for computing the voronoi_area.
            v : Vertex
                Voronoi vertex.

            Returns
            -------
            float
                The voronoi area
            
        )pbdoc",
        "mesh"_a, "v"_a
    );
    
    differential_geometry.def("laplace", &pmp::laplace,
        R"pbdoc(
            laplace(mesh: SurfaceMesh, v: Vertex) -> Point

            Compute the Laplace vector for vertex `v`, normalized by Voronoi area.

            Parameters
            ----------
            mesh : SurfaceMesh
                The mesh for computing the Laplace vector.
            v : Vertex
                Voronoi vertex.

            Returns
            -------
            Point
                The Laplace vector

            Notes
            -----
            Input mesh needs to be a triangle mesh.
            
        )pbdoc",
        "mesh"_a, "v"_a
    );

}

void bind_utilities(py::module_& algorithms) {
    py::module_ utilities = algorithms.def_submodule(
        "utilities",
        "Module for mesh utilities"
    );

    py::class_<pmp::BoundingBox>(utilities, "BoundingBox")
        .def(py::init<pmp::Point, pmp::Point>(),
            "Construct from min and max points.",
            "min"_a, "max"_a
        )
        .def(py::self += pmp::Point())
        .def(py::self += py::self)
        .def("min", &pmp::BoundingBox::min,
            "Get min point."
        )
        .def("max", &pmp::BoundingBox::max,
            "Get max point."
        )
        .def("center", &pmp::BoundingBox::center,
            "Get center point."
        )
        .def("is_empty", &pmp::BoundingBox::is_empty,
            "Indicate if the bounding box is empty."    
        )
        .def("size", &pmp::BoundingBox::size,
            "Get the size of the bounding box."
        );
    
    utilities.def("bounds", &pmp::bounds,
        "Compute bounding box of `mesh`.",
        "mesh"_a
    );

    utilities.def("flip_faces", &pmp::flip_faces,
        "Flip the orientation of all faces in `mesh`.",
        "mesh"_a
    );

    utilities.def("min_face_area", &pmp::min_face_area,
        "Compute the minimum area of all faces in `mesh`.",
        "mesh"_a
    );

    utilities.def("edge_length", &pmp::edge_length,
        "Compute length of an edge `e` in `mesh`.",
        "mesh"_a, "e"_a
    );

    utilities.def("mean_edge_length", &pmp::mean_edge_length,
        "Compute mean edge length of `mesh`.",
        "mesh"_a
    );

    utilities.def("connected_components", &pmp::connected_components,
        R"pbdoc(
            connected_components(mesh: SurfaceMesh) -> int

            Compute connected components in `mesh`.

            Extended Summary
            ----------------
            Adds a new vertex property `v:component` containing the component index.

            Parameters
            ----------
            mesh : SurfaceMesh
                The mesh for computing the connected components.
            
            Returns
            -------
            int
                The number of connected components.
        )pbdoc",
        "mesh"_a
    );

}

void bind_parameterization(py::module_& algorithms) {
    py::module_ parameterization = algorithms.def_submodule(
        "parameterization",
        "Module for computing mesh parameterization"
    );

    parameterization.def("harmonic_parameterization", &pmp::harmonic_parameterization,
        R"pbdoc(
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
        )pbdoc",
        "mesh"_a, "use_uniform_weights"_a=false
    );
    
    parameterization.def("lscm_parameterization", &pmp::lscm_parameterization,
        R"pbdoc(
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
        )pbdoc",
        "mesh"_a
    );
}

void bind_distance_point_triangle(py::module_& algorithms) {
    py::module_ distance_point_triangle = algorithms.def_submodule(
        "distance_point_triangle",
        "Module for computing the distance of point and triangle/line segment"
    );

    distance_point_triangle.def("dist_point_line_segment", &pmp::dist_point_line_segment,
        "Compute the distance of a point `p` to a line segment given by points `v0`, `v1`.",
        "p"_a, "v0"_a, "v1"_a, "nearest_point"_a
    );
    
    distance_point_triangle.def("dist_point_triangle", &pmp::dist_point_triangle,
        "Compute the distance of a point `p` to the triangle given by points `v0`, `v1` and `v2`.",
        "p"_a, "v0"_a, "v1"_a, "v2"_a, "nearest_point"_a
    );
}

void bind_geodesics(py::module_& algorithms) {
    py::module_ geodesics = algorithms.def_submodule(
        "geodesics",
        "Module for computing the geodesic distance from a set of vertices"
    );

    geodesics.def("geodesics", [](
        pmp::SurfaceMesh& mesh, 
        const std::vector<pmp::Vertex>& seeds,
        pmp::Scalar maxdist = std::numeric_limits<pmp::Scalar>::max(),
        unsigned int maxnum = std::numeric_limits<unsigned int>::max()
    ){
        std::vector<pmp::Vertex> neighbors;
        auto num = pmp::geodesics(
            mesh,
            seeds,
            maxdist,
            maxnum,
            &neighbors
        );
        return py::make_tuple(num, neighbors);
    },
        R"pbdoc(
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
        )pbdoc",
        "mesh"_a, "seeds"_a, "maxdist"_a=std::numeric_limits<pmp::Scalar>::max(),
        "maxnum"_a=std::numeric_limits<unsigned int>::max()
    );
    
    geodesics.def("geodesics_heat", &pmp::geodesics_heat,
        R"pbdoc(
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
        )pbdoc",
        "mesh"_a, "seeds"_a
    );

    geodesics.def("distance_to_texture_coordinates", &pmp::distance_to_texture_coordinates,
        R"pbdoc(
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
        )pbdoc",
        "mesh"_a
    );
}