# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Fixed

- Fixed a bug SurfaceGeodesic that lead to dist(v0,v1) != dist(v1,v0). As a
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
