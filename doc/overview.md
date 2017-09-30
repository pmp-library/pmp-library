# Overview {#overview}

[TOC]

# Introduction {#introduction}

In general, a polygonal surface mesh is composed of vertices, edges and faces as
well as the incidence relationships between them. SurfaceMesh stores the
connectivity information based on halfedges, i.e., pairs of directed edges with
opposing direction. To be more precise:

- Each vertex stores an outgoing halfedge.
- Each face stores an incident halfedge.
- Each halfedge stores its incident face, its target vertex, and its previous
  and next halfedges within the face.

The halfedge connectivity is illustrated in the figure below:

![Halfedge connectivity.](halfedge-connectivity.png)

In the following sections we describe the basic usage of SurfaceMesh by means of
simple example programs and code excerpts.

# Basics {#basics}

The very basic usage of SurfaceMesh is demonstrated in the example below. The
program first instantiates a SurfaceMesh object as well as four vertex
handles. These handles, as well as the handles for the other mesh entities
`Halfedge`, `Edge` and `Face` basically indices. Four vertices are added to the
mesh, as well as four triangular faces composing a tetrahedron. Finally, the
number of vertices, edges, and faces is printed to standard output.

\snippet SurfaceMeshBasics.cpp basics

# File I/O {#fileio}

SurfaceMesh currently supports reading OFF, OBJ, and STL files. Write support is
currently limited to OFF files. All I/O operations are handled by the
pmp::SurfaceMesh::read() and pmp::SurfaceMesh::write() member functions, with
the target file name being their only argument. An example is given below.

\snippet SurfaceMeshIO.cpp io


# Iterators and Circulators {#iterators}

In order to sequentially access mesh entities SurfaceMesh provides iterators for
each entity type, namely pmp::PointSet::VertexIterator,
pmp::EdgeSet::HalfedgeIterator, pmp::EdgeSet::EdgeIterator, and
pmp::SurfaceMesh::FaceIterator. Similar to iterators, SurfaceMesh also provides
circulators for the ordered enumeration of all incident vertices, halfedges, or
faces around a given face or vertex. The example below demonstrates the use of
iterators and circulators for computing the mean valence of a mesh.

\snippet SurfaceMeshIterators.cpp iterators

# Dynamic Properties {#properties}

Attaching additional attributes to mesh entities is important for many
applications. SurfaceMesh supports properties by means of synchronized
arrays that can be (de-)allocated dynamically at run-time. Property arrays
are also used internally, e.g., to store vertex coordinates. The example
program below shows how to access vertex coordinates through the
(pre-defined) point property.

\snippet SurfaceMeshBarycenter.cpp barycenter

The dynamic (de-)allocation of properties at run-time is managed by a set
of four different functions:

- add[_EntityType_]Property<_PropertyType_>(_PropertyName_) allocates a new
  property for the given _EntityType_ of the type _PropertyType_ labeled by the
  _PropertyName_ string.
- get[_EntityType_]Property<_PropertyType_>(_PropertyName_) returns a handle
  to an existing property.
- _EntityType_Property<_PropertyType_>(_PropertyName_) returns a handle to
  an existing property if the specified property already exists. If not, a new
  property is allocated and its handle is returned.
- remove[_EntityType_]Property(_PropertyHandle_) removes and the vertex
  property referenced by _PropertyHandle_.

Functions that allocate a new property take a default value for the property as
an optional second argument. The code excerpt below demonstrates how to
allocate, use and remove a custom edge property.

    SurfaceMesh mesh;

    // allocate property storing a point per edge
    auto edgePoints = mesh.addEdgeProperty<Point>("propertyName");

    // access the edge property like an array
    SurfaceMesh::Edge e;
    edgePoints[e] = Point(x,y,z);

    // remove property and free memory
    mesh.removeEdgeProperty(edgePoints);


# Connectivity Queries {#connectivity}

Commonly used connectivity queries such as retrieving the next
halfedge or the target vertex of an halfedge are illustrated below.

    SurfaceMesh::Halfedge h;
    auto h0 = mesh.nextHalfedge(h);
    auto h1 = mesh.prevHalfedge(h);
    auto h2 = mesh.oppositeHalfedge(h);
    auto f  = mesh.face(h);
    auto v0 = mesh.fromVertex(h);
    auto v1 = mesh.toVertex(h);

![Connectivity queries](connectivity-queries.png)

# Topological Operations {#topology}

SurfaceMesh also offers higher-level topological operations, such as performing
edge flips, edge splits, face splits, or halfedge collapses. The figure below
illustrates some of these operations.

![High-level operations changing the topology.](topology-changes.png)

The corresponding member functions and their syntax is demonstrated in the
pseudo-code below.

    SurfaceMesh::Vertex   v;
    SurfaceMesh::Edge     e;
    SurfaceMesh::Halfedge h;
    SurfaceMesh::Face     f;

    mesh.split(f, v);
    mesh.split(e, v);
    mesh.flip(e);
    mesh.collapse(h);

When entities are removed from the mesh due to topological changes, the member
function `garbageCollection()` has to be called in order to ensure the
consistency of the data structure.
