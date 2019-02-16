# Overview {#overview}

This section provides a high-level overview of the pmp-library. We describe its
design as well as the capabilities provided by the library. The pmp-library is
organized into different modules. At the core of the library is the @ref core
module providing a data structure for polygonal surface meshes. On top of the
@ref core module the @ref algorithms module provides implementations of
canonical geometry processing algorithms such as remeshing, simplification,
subdivision, and smoothing. The @ref io module provides classes for reading and
writing meshes from / to common file formats. The optional @ref gl module
provides OpenGL&reg;-based viewers and visualization tools.

## The `core` Module

The @ref core of the library provides a simple and efficient data structure for
representing polygon surface meshes, pmp::SurfaceMesh. It also defines basic
types such as 3D points and vectors as well as a basic utility classes for
timing and memory profiling.

## The `algorithms` Module

The @ref algorithms module provides implementations of canonical geometry
processing algorithms such as remeshing or mesh simplification. The class
structure and naming follows a simple and straightforward scheme: Provide one
class for one type of tasks and name it accordingly. Example: the
pmp::SurfaceRemeshing class provides remeshing algorithms operating on surface
meshes.

## The `io` Module

This module provides readers and writers for common file formats. For
convenience, the IO functionality is easily accessible using the `read()` and
`write()` member functions. The only required argument is a file name. The
corresponding file format is automatically determined by the file
extension. Additional pmp::IOOptions can be used to control _what_ data is
written, e.g., including vertex normals or not, and _how_ it is written, i.e.,
as plain text ASCII or binary files. See the individual IO classes for more
information on supported file formats and capabilities.

## The `gl` Module

In order to easily create visualizations the library contains an optional @ref
gl module including basic viewers, e.g., pmp::MeshViewer. The corresponding
OpenGL&reg; code for rendering the data is contained in pmp::SurfaceMeshGL. For
simplicity, the this class inherits from pmp::SuraceMesh.
