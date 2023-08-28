# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

This project aims to adhere to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased

### Fixed

- Fix GLFW include path for ImGui when using PMP as a sub-project (use relative path)

## [3.0.0] 2023-08-24

### Added

- Add `mesh_to_matrices()` and `matrices_to_mesh()` functions to convert to and from Eigen matrices.
- Add support to write binary STL files.
- Added `geodesics_heat()` for computing geodesics (based on the heat method) on general polygon meshes.
- Generalize curvature computation in `curvature()` to polygon meshes.
- Add `mean_edge_length(SurfaceMesh)`.
- Add `min_face_area()` to compute minimum face area of all faces.
- Add `edge_area()` to compute the area associated to an edge.
- Added circulator enumerating edges around vertex: `SurfaceMesh::edges(Vertex)`.
- Functions `laplace_matrix()`, `mass_matrix()`, `gradient_matrix()`, and `divergence_matrix()` that compute matrix representations of those discrete differential operators. Works for both triangle meshes and general polygon meshes, based on the paper Bunge et al, "Polygon Laplacian made simple", Eurographics 2020.
- Smoothing, parameterization, and fairing are now implemented based on sparse Laplacian matrices, which generalizes to general polygon meshes.
- Add `PMP_STRICT_COMPILATION` CMake option to control treating warnings as errors. Default: On.
- Add function `flip_faces()` to reverse face orientation in a mesh (#123)
- Meshes and textures can be loaded by dropping the files onto the window, both in normal GLFW windows and browser windows (using emscripten). Thanks to Stephan Wenninger!

### Changed

- Get rid of git submodules, vendor all third-party libraries for simplicity. See also #167.
- The function `voronoi_area()` now computes the barycentric Voronoi area, because this version generalizes better to polygon meshes. The mixed Voronoi area (for triangle meshes) is now computed by `voronoi_area_mixed()`.
- Consistently use lowercase file names
- Rename `Timer` to `StopWatch`
- `SurfaceMeshGL` renamed to `Renderer` and de-coupled from `SurfaceMesh`
- Replace `triangle_area(Face)` by `face_area(Face)`, which now supports general polygons. `surface_area(SurfaceMesh)` now also works for general polygon meshes.
- Remove max angle triangulation objective leading to fold-overs.
- Breaking change: Re-design algorithms API to use a simple function interface.
- Breaking change: Make helper classes `Heap`, `Quadric`, `NormalCone`, `TriangleKdTree` private.
- Remove `vertex_curvature()`, `VertexCurvature`, and `angle_sum()`.
- Support CMake's `BUILD_SHARED_LIBS` option to control building shared libraries
- Fix `Mat3::inverse()` to use `fabs()`
- Remove `Matrix::allFinite()`.
- Make `SurfaceMesh::bounds()` a free function `pmp::bounds()`.
- Make `SurfaceMesh::edge_length()` a free function `pmp::edge_length()`.
- Remove object properties.
- Remove support for PLY format, see 1ec912b for why.
- Remove support for XYZ and AGI point set formats.
- Move `SurfaceMesh::read()` and `SurfaceMesh::write()` to free functions `pmp::read()` and `pmp::write()`.
- Move IO functionality into its own module
- Upgrade C++ standard to C++17
- Remove `PMP_ASSERT()` macro.
- Remove `get_*_property_type()` functions from `SurfaceMesh`
- Move example apps from `src/apps` to `examples`. Remove `PMP_BUILD_APPS` CMake option.
- Rename `Simplification` to `Decimation`
- Rename `Factory` to `Shapes`
- Drop `Surface` prefix from algorithms
- Update glew to version 2.2.0.
- Update stb_image to version 2.28 and stb_image_write to version 1.16.
- Upgrade ImGui to version 1.89.8

## [2.0.1] 2022-08-26

### Fixed

- Fix shared library version
- Fix compilation with PMP_SCALAR_TYPE=64
- Use correct C++ standard (C++14) in public target compile options
- Fix crash in smoothing demo app

### Changed

- Upgrade ImGui to version 1.88

## [2.0.0] 2022-08-14

### Added

- Add support for texture seams in `SurfaceSimplification`, thanks to Timo Menzel.
- Add quad/tri subdivision, thanks to Hendrik Meyer.
- Add `SurfaceFactory` class to generate basic shapes. Thanks to [u/lycium](https://www.reddit.com/user/lycium/) on Reddit for suggesting a simplification of the `icosahedron()` function.
- Add support to render colors per face
- Add support to render colors per vertex, thanks to Xiewei Zhong
- Add function to compute volume of a mesh, thanks to Xiewei Zhong
- Add post-increment iterators and make low level functions to add elements public. This makes it possible to use CGAL algorithms on a PMP `SurfaceMesh`. Thanks to Andreas Fabri for contributing the changes!
- Add PMP_INSTALL option to CMake.
- Add PMP_BUILD_VIS CMake option to enable / disable building the pmp_vis library and its dependencies.

### Changed

- Bump C++ standard to C++14
- `TriangleKdTree`: no longer record number of tests, remove `NearestNeighbor::tests` member
- Remove `SurfaceSubdivision::sqrt3()`
- Remove `PMP_SHOW` macro
- Remove `pmp::Version` class and `Version.h`
- Build shared libraries on all platforms by default (except pmp_vis , see #87)
- Bump CMake minimum required version to 3.16.3
- Make `SurfaceTriangulation::Objective` a scoped enum
- Remove `SurfaceMesh::triangulate()` functions to avoid redundancy with `SurfaceTriangulation`
- Improve API by reporting errors using exceptions (see #16).
- `SurfaceFeatures` now returns the number of boundary / feature edges detected.
- Update Eigen to version 3.4.0
- Update GoogleTest to version 1.12.1
- Update stb_image to version 2.26 and stb_image_writer to version 1.15.
- Update GLFW to version 3.3.8
- Change CMake policy CMP0072 to "NEW" behavior preferring GLVND

### Fixed

- Fix `SurfaceMesh::get_object_propertyType()` to `get_object_property_type()`
- `SurfaceRemeshing`: Don't leak feature properties. Spotted by Pierre Buyssens.
- Fixed a rare bug regarding ImGui scaling on Linux. Thanks to Jan Erik Swiadek for reporting and fixing this!
- Fix crash in `SurfaceRemeshing` when dealing with low quality meshes (#90, #89)
- `SurfaceMesh::bounds()` no longer includes deleted vertices. Thanks to Paul Du.
- Fix crash in `SurfaceHoleFilling` when a vertex selection already exists.
- Fix bug in `SurfaceHoleFilling` leading to artifacts in the filled surface patch. See #64.
- Change sign of `vec2::perp()` to correctly model CCW rotation by 90 degrees.
- Fix bug in OpenGL buffer clean-up in `SurfaceMeshGL`.

## [1.2.1] 2020-05-10

### Fixed

- Fix rendering issues around sharp edges
- Fix bug in adaptive remeshing leading to over-refinement of corner vertices.
- Fix bug in bounding box computation. Thanks a lot to Jascha Achenbach for reporting this bug!

## [1.2.0] 2020-03-15

### External libraries

- Upgrade Eigen to current master version to get rid of a compile error on Windows
- Upgrade ImGui to version 1.75
- Upgrade stb_image to current master version

### Added

- Add constructors using initializer lists to Matrix/Vector classes
- Add assignment from and cast from Eigen matrices and vectors
- Improved rendering of general polygons, avoiding erroneous tessellation into overlapping/flipped triangles in case of non-convex polygons.
- Added support for rendering using MatCaps. Thanks to Alec Jacobson for suggesting this!

### Fixed

- Fix erroneous header install path (visualization)
- Fix bug hole filling (when filling single-triangle holes). Thanks a lot to Pierre Buyssens for reporting this bug!
- Fix bug when fairing a mesh w/o boundary constraints. Thanks a lot to Pierre Buyssens for reporting this bug!

## [1.1.0] 2019-05-30

### Added

- Compile-time switch `PMP_SCALAR_TYPE` to choose between float/double as Scalar
- Support point set rendering for surface meshes without faces
- Add hole filling based on Liepa algorithm
- Add explicit warranty and liability disclaimer

### Changed

- Breaking change: Public members in Window and TrackballViewer classes made
  private and encapsulated through access functions
- Change SurfaceSmoothing to avoid model shrinking
- Improve normal computation for polygonal faces
- Upgrade GLFW to version 3.3
- Upgrade ImGui to version 1.70
- Upgrade Eigen to version 3.3.7
- Documentation updates

### Fixed

- Fix a bug in OFF reader when loading faces with high valence
- Fix a bug SurfaceGeodesic that lead to dist(v0,v1) != dist(v1,v0). As a consequence, the Novotni method has been removed from SurfaceGeodesic.

## [1.0.0] 2019-02-18

### Added

- Algorithms module
- Visualization tools
- Support for range-based for loops
- Configurable index type
- Global object properties
- Support for emscripten
- Unit test suite
- Continuous integration
- Coding standard
- Version API

### Changed

- Switched to MIT license
- Library scope handle and property types
- Enhanced IO capabilities
