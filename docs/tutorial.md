# Tutorial {#tutorial}

This section provides a hands-on tutorial on the basic usage of the
pmp-library. For a thorough introduction into polygon mesh processing we refer
to the book of Botsch et al. \cite botsch_2010_polygon.

## Introduction

In general, a polygonal surface mesh is composed of vertices, edges and faces as
well as the incidence relationships between them. pmp::SurfaceMesh stores the
connectivity information based on halfedges, i.e., pairs of directed edges with
opposing direction. To be more precise:

- Each vertex stores an outgoing halfedge.
- Each face stores an incident halfedge.
- Each halfedge stores its incident face, its target vertex, and its previous
  and next halfedges within the face.

The halfedge connectivity is illustrated in the figure below:

![Halfedge connectivity.](./images/halfedge-connectivity.png)

In the following sections we describe the basic usage of pmp::SurfaceMesh by
means of simple example programs and code excerpts.

## Basics

The very basic usage of pmp::SurfaceMesh is demonstrated in the example below. The
program first instantiates a pmp::SurfaceMesh object as well as four vertex
handles. These handles, as well as the handles for the other mesh entities
`Halfedge`, `Edge` and `Face` basically indices. Four vertices are added to the
mesh, as well as four triangular faces composing a tetrahedron. Finally, the
number of vertices, edges, and faces is printed to standard output.

~~~~{.cpp}
    // instantiate a SurfaceMesh object
    SurfaceMesh mesh;

    // instantiate 4 vertex handles
    SurfaceMesh::Vertex v0,v1,v2,v3;

    // add 4 vertices
    v0 = mesh.addVertex(Point(0,0,0));
    v1 = mesh.addVertex(Point(1,0,0));
    v2 = mesh.addVertex(Point(0,1,0));
    v3 = mesh.addVertex(Point(0,0,1));

    // add 4 triangular faces
    mesh.addTriangle(v0,v1,v3);
    mesh.addTriangle(v1,v2,v3);
    mesh.addTriangle(v2,v0,v3);
    mesh.addTriangle(v0,v2,v1);

    std::cout << "vertices: " << mesh.nVertices() << std::endl;
    std::cout << "edges: "    << mesh.nEdges()    << std::endl;
    std::cout << "faces: "    << mesh.nFaces()    << std::endl;
~~~~

## Iterators and Circulators

In order to sequentially access mesh entities pmp::SurfaceMesh provides
iterators for each entity type, namely pmp::SurfaceMesh::VertexIterator,
pmp::SurfaceMesh::HalfedgeIterator, pmp::SurfaceMesh::EdgeIterator, and
pmp::SurfaceMesh::FaceIterator. Similar to iterators, pmp::SurfaceMesh also
provides circulators for the ordered enumeration of all incident vertices,
halfedges, or faces around a given face or vertex. The example below
demonstrates the use of iterators and circulators for computing the mean valence
of a mesh.

~~~~{.cpp}
    SurfaceMesh mesh;

    if (argc > 1)
        mesh.read(argv[1]);

    float meanValence = 0.0f;

    // loop over all vertices
    for (auto v : mesh.vertices())
    {
        // sum up vertex valences
        meanValence += mesh.valence(v);
    }

    meanValence /= mesh.nVertices();

    std::cout << "mean valence: " << meanValence << std::endl;
~~~~

## Dynamic Properties

Attaching additional attributes to mesh entities is important for many
applications. pmp::SurfaceMesh supports properties by means of synchronized arrays
that can be (de-)allocated dynamically at run-time. Property arrays are also
used internally, e.g., to store vertex coordinates. The example program below
shows how to access vertex coordinates through the (pre-defined) point property.

~~~~{.cpp}
    SurfaceMesh mesh;

    if (argc > 1)
        mesh.read(argv[1]);

    // get (pre-defined) property storing vertex positions
    auto points = mesh.getVertexProperty<Point>("v:point");

    Point p(0,0,0);

    for (auto v : mesh.vertices())
    {
        // access point property like an array
        p += points[v];
    }

    p /= mesh.nVertices();

    std::cout << "barycenter: " << p << std::endl;
~~~~

The dynamic (de-)allocation of properties at run-time is managed by a set
of four different functions:

- `addEntityTypeProperty<PropertyType>("PropertyName")` allocates a new property
  for the given _EntityType_ of the type _PropertyType_ labeled by the
  _PropertyName_ string.
- `getEntityTypeProperty<PropertyType>("PropertyName")` returns a handle to an
  existing property.
- `EntityTypeProperty<PropertyType>("PropertyName")` returns a handle to an
  existing property if the specified property already exists. If not, a new
  property is allocated and its handle is returned.
- `removeEntityTypeProperty(PropertyHandle)` removes and the property referenced
  by `PropertyHandle`.

Functions that allocate a new property take a default value for the property as
an optional second argument. The code excerpt below demonstrates how to
allocate, use and remove a custom edge property.

~~~~{.cpp}
    SurfaceMesh mesh;

    // allocate property storing a point per edge
    auto edgePoints = mesh.addEdgeProperty<Point>("propertyName");

    // access the edge property like an array
    SurfaceMesh::Edge e;
    edgePoints[e] = Point(x,y,z);

    // remove property and free memory
    mesh.removeEdgeProperty(edgePoints);
~~~~

## Connectivity Queries

Commonly used connectivity queries such as retrieving the next
halfedge or the target vertex of an halfedge are illustrated below.

~~~~{.cpp}
    SurfaceMesh::Halfedge h;
    auto h0 = mesh.nextHalfedge(h);
    auto h1 = mesh.prevHalfedge(h);
    auto h2 = mesh.oppositeHalfedge(h);
    auto f  = mesh.face(h);
    auto v0 = mesh.fromVertex(h);
    auto v1 = mesh.toVertex(h);
~~~~

![Connectivity queries](./images/connectivity-queries.png)

## Topological Operations

pmp::SurfaceMesh also offers higher-level topological operations, such as
performing edge flips, edge splits, face splits, or halfedge collapses. The
figure below illustrates some of these operations.

![High-level operations changing the topology.](./images/topology-changes.png)

The corresponding member functions and their syntax is demonstrated in the
pseudo-code below.

~~~~{.cpp}
    SurfaceMesh::Vertex   v;
    SurfaceMesh::Edge     e;
    SurfaceMesh::Halfedge h;
    SurfaceMesh::Face     f;

    mesh.split(f, v);
    mesh.split(e, v);
    mesh.flip(e);
    mesh.collapse(h);
~~~~

When entities are removed from the mesh due to topological changes, the member
function pmp::GeometryObject::garbageCollection() has to be called in order to
ensure the consistency of the data structure.

## File I/O

pmp::SurfaceMesh currently supports reading OFF, OBJ, and STL files. Write
support is currently limited to OFF files. All I/O operations are handled by the
pmp::SurfaceMesh::read() and pmp::SurfaceMesh::write() member functions, with
the target file name being their only argument. An example is given below.

~~~~{.cpp}
    // instantiate a SurfaceMesh object
    SurfaceMesh mesh;

    // read a mesh specified as the first command line argument
    if (argc > 1)
        mesh.read(argv[1]);

    // ...
    // do fancy stuff with the mesh
    // ...

    // write the mesh to the file specified as second argument
    if (argc > 2)
        mesh.write(argv[2]);
~~~~
