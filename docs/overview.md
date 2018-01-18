# Overview {#overview}

This section provides a high-level overview of the pmp-library. We describe its
design as well as the capabilities provided by the library. The pmp-library is
organized into different modules. At the core of the library is the @ref
geometry module providing data structures for point sets, edge sets, and
polygonal surface meshes. On top of the @ref geometry module the @ref algorithms
module provides implementations of canonical geometry processing algorithms such
as remeshing, simplification, subdivision, and smoothing. The @ref io module
provides classes for reading and writing data structures of the @ref geometry
module to common file formats. The optional @ref gl module provides
OpenGL&reg;-based viewers and visualization tools.

## The `geometry` Module

The core of the library is the geometry module providing a set of data
structures for common geometry processing tasks. At this point, pmp-library
provides data structures pmp::PointSet, pmp::EdgeSet, and
pmp::SurfaceMesh. These classes form a simple inheritance hierarchy. The
pmp::GeometryObject base class contains functionality common to all types of
geometry representations, such as having certain spatial bounds and giving
access to the underlying points. Furthermore, it also defines the base class for
entity handles and functions for handling global object properties.

## The `algorithms` Module

The @ref algorithms module provides implementations of canonical geometry
processing algorithms such as remeshing or mesh simplification. The class
structure and naming follows a simple and straightforward scheme: Provide one
class for one type of tasks, and name it according to the data structure it is
operating on. Example: the pmp::SurfaceRemeshing class provides remeshing
algorithms operating on surface meshes. Similarly, the pmp::PointSetSmoothing
class can be used to smooth point sets.

## The `io` Module

This module provides readers and writers for common file formats. For each data
structure of the @ref geometry module a corresponding IO class is provided,
e.g., pmp::SurfaceMeshIO and pmp::PointSetIO. For convenience, the IO
functionality is easily accessible using the `read()` and `write()` member
functions. The only required argument is a file name. The corresponding file
format is automatically determined by the file extension. Additional
pmp::IOOptions can be used to control _what_ data is written, e.g., including
vertex normals or not, and _how_ it is written, i.e., as plain text ASCII or
binary files. See the individual IO classes for more information on supported
file formats and capabilities.

## The `gl` Module

In order to easily create visualizations the library contains an optional @ref
gl module including basic viewers for all @ref geometry structures, i.e., a
pmp::PointSetViewer, pmp::EdgeSetViewer, and a pmp::MeshViewer. Similar to the
@ref io module, the corresponding OpenGL&reg; code for rendering the data is
implemented using one class for each data type, e.g., pmp::EdgeSetGL. For
simplicity, the @ref gl classes inherit from their corresponding @ref geometry
classes in order to access internal data structures.
