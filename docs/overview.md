# Overview {#overview}

This section provides a high-level overview of the pmp-library. We describe its
design as well as the capabilities provided by the library. The pmp-library is
organized into different modules. At the core of the library is the @ref core
module providing a data structure for polygonal surface meshes. On top of the
@ref core module the @ref algorithms module provides implementations of
canonical geometry processing algorithms such as remeshing, simplification,
subdivision, and smoothing. The optional @ref visualization module
provides OpenGL&reg;-based viewers and visualization tools.

## Core

The @ref core of the library provides a simple and efficient data structure for
representing polygon surface meshes, pmp::SurfaceMesh. It also defines basic
types such as 3D points and vectors as well as a basic utility classes for
timing and memory profiling.

## Algorithms

The @ref algorithms module provides implementations of canonical geometry
processing algorithms such as remeshing or mesh simplification. The class
structure and naming follows a simple and straightforward scheme: Provide one
class for one type of tasks and name it accordingly. Example: the
pmp::SurfaceRemeshing class provides remeshing algorithms operating on surface
meshes.

## Visualization

In order to easily create visualizations the library contains an optional @ref
visualization module including basic viewers, e.g., pmp::MeshViewer. The
corresponding OpenGL&reg; code for rendering the data is contained in
pmp::SurfaceMeshGL. For simplicity, the this class inherits from
pmp::SurfaceMesh.
