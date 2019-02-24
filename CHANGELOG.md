# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Support point set rendering for surface meshes without faces

### Changed

- Improve normal computation for polygonal faces
- Upgrade ImGui to version 1.68
- Upgrade Eigen to version 3.3.7

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
