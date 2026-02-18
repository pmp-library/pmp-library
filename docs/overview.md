# Overview {#overview}

This section provides a high-level overview of PMP. We describe its design as well as the capabilities provided by the library. PMP is organized into different components. At the core of the library is the @ref core component providing a data structure for polygonal surface meshes. On top of the @ref core component the @ref algorithms component provides implementations of canonical geometry processing algorithms such as remeshing, decimation, subdivision, and smoothing. The optional @ref viewers component provides OpenGL&reg;-based viewers.

## Core

The @ref core of the library provides a simple and efficient data structure for representing polygon surface meshes, pmp::SurfaceMesh. It also defines basic types such as 3D points and vectors as well as a basic utility classes for timing and memory profiling. See the @ref tutorial for how to use the core pmp::SurfaceMesh data structure.

You can find more details on the design and implementation of the core surface mesh data structure in our original paper \cite sieger_2011_design .

## Algorithms

The @ref algorithms component provides implementations of canonical geometry processing algorithms such as remeshing or mesh decimation. For each type of algorithms, we provide simple functions that take a pmp::SurfaceMesh as an argument, eventually followed by optional parameters to control algorithm behavior.

## Viewers

In order to easily create viewers the library contains an optional @ref viewers component including basic viewers, e.g., pmp::MeshViewer. The corresponding OpenGL&reg; code for rendering the data is contained in pmp::Renderer class.
