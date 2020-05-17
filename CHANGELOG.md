# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

This project aims to adhere to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.1] 2020-05-10

### Fixed

- Fix rendering issues around sharp edges
- Fix bug in adaptive remeshing leading to over-refinement of corner vertices.
- Fix bug in bounding box computation.
  Thanks a lot to Jascha Achenbach for reporting this bug!

## [1.2.0] 2020-03-15

### External libraries

- Upgrade Eigen to current master version to get rid of a compile error on Windows
- Upgrade ImGui to version 1.75
- Upgrade stb_image to current master version

### Added

- Add constructors using initializer lists to Matrix/Vector classes
- Add assignment from and cast from Eigen matrices and vectors
- Improved rendering of general polygons, avoiding erroneous
  tessellation into overlapping/flipped triangles in case of
  non-convex polygons.
- Added support for rendering using MatCaps.
  Thanks to Alec Jacobson for suggesting this!

### Fixed

- Fix erroneous header install path (visualization)
- Fix bug hole filling (when filling single-triangle holes).
  Thanks a lot to Pierre Buyssens for reporting this bug!
- Fix bug when fairing a mesh w/o boundary constraints.
  Thanks a lot to Pierre Buyssens for reporting this bug!

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
- Fix a bug SurfaceGeodesic that lead to dist(v0,v1) != dist(v1,v0). As a
  consequence, the Novotni method has been removed from SurfaceGeodesic.

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
