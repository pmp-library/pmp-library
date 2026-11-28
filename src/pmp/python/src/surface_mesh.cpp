#include "surface_mesh.h"
#include "circulators.h"
#include "properties.h"
#include "matrices.h"

namespace py = pybind11;
using namespace pybind11::literals;

// Compile using: c++ -I /Users/nemectad/Documents/Programming/pmp-library/external/pybind11/include -I /usr/local/Cellar/python@3.12/3.12.3/Frameworks/Python.framework/Headers -I /Users/nemectad/Documents/Programming/pmp-library/src -I /Users/nemectad/Documents/Programming/pmp-library/src/pmp/python/include -O3 -Wall -shared -std=c++17 -undefined dynamic_lookup src/*.cpp -L /Users/nemectad/Documents/Programming/pmp-library/build -Wl,-rpath,/Users/nemectad/Documents/Programming/pmp-library/build -lpmp -o pypmp/pypmp$(python3-config --extension-suffix)
// note: the rpath is required for runtime search path for the library

using VertexAroundVertexCirculator = CirculatorWrapper<pmp::SurfaceMesh::VertexAroundVertexCirculator, pmp::Vertex, pmp::Vertex>;
using HalfedgeAroundVertexCirculator = CirculatorWrapper<pmp::SurfaceMesh::HalfedgeAroundVertexCirculator, pmp::Halfedge, pmp::Vertex>;
using EdgeAroundVertexCirculator = CirculatorWrapper<pmp::SurfaceMesh::VertexAroundVertexCirculator, pmp::Edge, pmp::Vertex>;
using FaceAroundVertexCirculator = CirculatorWrapper<pmp::SurfaceMesh::FaceAroundVertexCirculator, pmp::Face, pmp::Vertex>;
using VertexAroundFaceCirculator = CirculatorWrapper<pmp::SurfaceMesh::VertexAroundFaceCirculator, pmp::Vertex, pmp::Face>;
using HalfedgeAroundFaceCirculator = CirculatorWrapper<pmp::SurfaceMesh::HalfedgeAroundFaceCirculator, pmp::Halfedge, pmp::Face>;

void bind_surface_mesh(py::module_& m) {
    using namespace pmp;

    py::class_<SurfaceMesh> cls(m, "SurfaceMesh");
    cls
        .def(py::init<>())
        // Mesh construction
        .def("add_vertex", &SurfaceMesh::add_vertex, 
            "Add a new vertex with position p", 
            "p"_a
        )
        .def("add_triangle", &SurfaceMesh::add_triangle, 
            "Add a new triangle connecting vertices v0, v1, v2", 
            "v0"_a, "v1"_a, "v2"_a
        )
        .def("add_triangle", [](SurfaceMesh &self, const py::sequence &seq){
            check_sequence("SurfaceMesh method add_triangle", 3, "vertices", seq);

            return self.add_triangle(seq[0].cast<Vertex>(), seq[1].cast<Vertex>(), seq[2].cast<Vertex>());
        },
            "Add a new triangle connecting vertices v0, v1, v2",
            "array"_a
        )
        .def("add_quad", &SurfaceMesh::add_quad,
            "Add a new quad connecting vertices in array",
            "v0"_a, "v1"_a, "v2"_a, "v3"_a
        )
        .def("add_quad", [](SurfaceMesh &self, const py::sequence &seq){
            check_sequence("SurfaceMesh method add_quad", 4, "vertices", seq);

            return self.add_quad(seq[0].cast<Vertex>(), seq[1].cast<Vertex>(), seq[2].cast<Vertex>(), seq[3].cast<Vertex>());
        },
            "Add a new quad connecting vertices in array",
            "array"_a
        )
        .def("add_face", [](SurfaceMesh &self, const std::vector<Vertex>& vertices) {
            return self.add_face(vertices);
        },
            R"pbdoc(
                add_face(vertices: List[Vertex]) -> Face

                Add a new face with vertex list `vertices`

                Raises
                ------
                TopologyException in case a topological error occurs.            

                See Also
                --------
                add_triangle
                add_quad
            )pbdoc",
            "vertices"_a
            )

        // Mesh properties
        .def("n_vertices", &SurfaceMesh::n_vertices,
            "Return number of vertices in the mesh"
        )
        .def("n_halfedges", &SurfaceMesh::n_halfedges,
            "Return number of halfedge in the mesh"
        )
        .def("n_edges", &SurfaceMesh::n_edges, 
            "Return number of edges in the mesh"
        )
        .def("n_faces", &SurfaceMesh::n_faces, 
            "Return number of faces in the mesh"
        )
        .def("is_empty", &SurfaceMesh::is_empty,
            "Return true if the mesh is empty, i.e., has no vertices"
        )
        .def("is_boundary", [](const SurfaceMesh &self, Vertex v) {
            return self.is_boundary(v);
        },
            "Return whether v is a boundary vertex",
            "v"_a
        )
        .def("is_boundary", [](const SurfaceMesh &self, Halfedge h) {
            return self.is_boundary(h);
        },
            "Return whether h is a boundary halfedge",
            "h"_a
        )
        .def("is_boundary", [](const SurfaceMesh &self, Edge e) {
            return self.is_boundary(e);
        },
            "Return whether e is a boundary edge",
            "e"_a
        )
        .def("is_boundary", [](const SurfaceMesh &self, Face f) {
            return self.is_boundary(f);
        },
            "Return whether f is a boundary face",
            "f"_a
        )
        .def("is_valid", [](const SurfaceMesh &self, Vertex v){
            return self.is_valid(v);
        },
            "Return whether vertex v is valid",
            "v"_a
        )
        .def("is_valid", [](const SurfaceMesh &self, Halfedge h){
            return self.is_valid(h);
        },
            "Return whether halfedge h is valid",
            "h"_a
        )
        .def("is_valid", [](const SurfaceMesh &self, Edge e){
            return self.is_valid(e);
        },
            "Return whether edge e is valid",
            "e"_a
        )
        .def("is_valid", [](const SurfaceMesh &self, Face f){
            return self.is_valid(f);
        },
            "Return whether face f is valid",
            "f"_a
        )
        .def("halfedge", [](const SurfaceMesh &self, Vertex v){
            return self.halfedge(v);
        },
            "Return an outgoing halfedge of vertex `v`. if `v` is a boundary vertex this will be a boundary halfedge.",
            "v"_a
        )
        .def("halfedge", [](const SurfaceMesh &self, Edge e, unsigned int i){
            if (i > 1 || i < 0) throw py::value_error("Value can be only 0 or 1");
            return self.halfedge(e, i);
        },
            "Return the `i`'th halfedge of edge `e`. `i` has to be 0 or 1.",
            "v"_a, "i"_a
        )
        .def("opposite_halfedge", [](const SurfaceMesh &self, Halfedge h){
            return self.opposite_halfedge(h);
        },
            "Return the opposite halfedge of `h`",
            "h"_a
        )
        .def("prev_halfedge", &SurfaceMesh::prev_halfedge,
            "Return the previous halfedge withing the incident face.",
            "h"_a
        )
        .def("find_edge", &SurfaceMesh::find_edge,
            "Find the edge of two vertices (a,b)",
            "a"_a, "b"_a
        )
        .def("position",
            static_cast<Point& (SurfaceMesh::*)(Vertex)>(&SurfaceMesh::position),
            py::return_value_policy::reference_internal,
            "Return mutable position of a vertex",
            "v"_a
        )        
        .def("positions", &SurfaceMesh::positions,
            py::return_value_policy::reference_internal,
            "Return vector of mutable point positions"
        )
        .def("valence", [](SurfaceMesh &self, Vertex v){
            return self.valence(v);
        },
            "Compute the valence of vertex `v` (number of incident edges).",
            "v"_a
        )
        .def("valence", [](SurfaceMesh &self, Face f){
            return self.valence(f);
        },
            "Compute the valence of face `f` (its number of vertices).",
            "v"_a
        )
        .def("delete_vertex", [](SurfaceMesh &self, Vertex v){
            self.delete_vertex(v);
        },
            R"pbdoc(
                delete_vertex(v: Vertex) -> None

                Delete vertex `v` from the mesh.

                Notes
                -----
                Only marks the vertex as deleted. Call `garbage_collection()` to finally remove deleted entities.
            )pbdoc",
            "v"_a
        )
        .def("delete_edge", [](SurfaceMesh &self, Edge e){
            self.delete_edge(e);
        },
            R"pbdoc(
                delete_edge(e: Edge) -> None

                Delete edge `e` from the mesh.

                Notes
                -----
                Only marks the edge as deleted. Call `garbage_collection()` to finally remove deleted entities.
            )pbdoc",
            "e"_a
        )
        .def("delete_face", [](SurfaceMesh &self, Face f){
            self.delete_face(f);
        },
            R"pbdoc(
                delete_face(f: Face) -> None

                Delete face `f` from the mesh.

                Notes
                -----
                Only marks the face as deleted. Call `garbage_collection()` to finally remove deleted entities.
            )pbdoc",
            "f"_a
        )
        .def("garbage_collection", [](SurfaceMesh &self){
            self.garbage_collection();
        },
            R"pbdoc(
                garbage_collection() -> None

                Remove deleted elements.
            )pbdoc"
        )
        .def("clear", &SurfaceMesh::clear,
            "clear mesh: remove all vertices, edges, faces"
        )
        .def("is_deleted", [](SurfaceMesh &self, Vertex v){
            return self.is_deleted(v);
        },
            R"pbdoc(
                is_deleted(v: Vertex) -> bool
                
                Returns
                -------
                Whether vertex `v` is deleted.

                See Also
                --------
                garbage_collection

            )pbdoc",
            "v"_a
        )
        .def("is_deleted", [](SurfaceMesh &self, Halfedge h){
            return self.is_deleted(h);
        },
            R"pbdoc(
                is_deleted(h: Halfedge) -> bool
                
                Returns
                -------
                Whether halfedge `h` is deleted.

                See Also
                --------
                garbage_collection

            )pbdoc",
            "h"_a
        )
        .def("is_deleted", [](SurfaceMesh &self, Edge e){
            return self.is_deleted(e);
        },
            R"pbdoc(
                is_deleted(e: Edge) -> bool
                
                Returns
                -------
                Whether edge `e` is deleted.

                See Also
                --------
                garbage_collection

            )pbdoc",
            "e"_a
        )
        .def("is_deleted", [](SurfaceMesh &self, Face f){
            return self.is_deleted(f);
        },
            R"pbdoc(
                is_deleted(f: Face) -> bool
                
                Returns
                -------
                Whether face `f` is deleted.

                See Also
                --------
                garbage_collection

            )pbdoc",
            "f"_a
        )
        .def("is_triangle_mesh", &SurfaceMesh::is_triangle_mesh,
            R"pbdoc(
                is_triangle_mesh() -> bool

                Returns
                -------
                bool 
                    whether the mesh a triangle mesh. this function simply tests
                    each face, and therefore is not very efficient.
            )pbdoc"
        )
        .def("is_quad_mesh", &SurfaceMesh::is_quad_mesh,
            R"pbdoc(
                is_quad_mesh() -> bool

                Returns
                -------
                bool 
                    whether the mesh a quad mesh. this function simply tests
                    each face, and therefore is not very efficient.
            )pbdoc"
        )

        // Iterators
        .def("vertices", [](const SurfaceMesh &self) {
            auto begin = self.vertices_begin();
            auto end = self.vertices_end();
            return py::make_iterator(begin, end);
        }, py::keep_alive<0, 1>(),
            "Return vertex container")
        .def("faces", [](const SurfaceMesh &self) {
            auto begin = self.faces_begin();
            auto end = self.faces_end();
            return py::make_iterator(begin, end);
        }, py::keep_alive<0, 1>(),
            "Return face container")
        .def("halfedges", [](const SurfaceMesh &self) {
            auto begin = self.halfedges_begin();
            auto end = self.halfedges_end();
            return py::make_iterator(begin, end);
        }, py::keep_alive<0, 1>(),
            "Return halfedge container")
        .def("edges", [](const SurfaceMesh &self) {
            auto begin = self.edges_begin();
            auto end = self.edges_end();
            return py::make_iterator(begin, end);
        }, py::keep_alive<0, 1>(),
            "Return edge container")

        // Circulators
        .def("vertices", [](const SurfaceMesh &self, Vertex v) {
            return VertexAroundVertexCirculator(&self, v);
        }, py::keep_alive<0, 1>(), "v"_a)
        .def("edges", [](const SurfaceMesh &self, Vertex v) {
            return EdgeAroundVertexCirculator(&self, v);
        }, py::keep_alive<0, 1>(), "v"_a)
        .def("halfedges", [](const SurfaceMesh &self, Vertex v) {
            return HalfedgeAroundVertexCirculator(&self, v);
        }, py::keep_alive<0, 1>(), "v"_a)
        .def("faces", [](const SurfaceMesh &self, Vertex v) {
            return FaceAroundVertexCirculator(&self, v);
        }, py::keep_alive<0, 1>(), "v"_a)
        .def("vertices", [](const SurfaceMesh &self, Face f) {
            return VertexAroundFaceCirculator(&self, f);
        }, py::keep_alive<0, 1>(), "f"_a)
        .def("halfedges", [](const SurfaceMesh &self, Face f) {
            return HalfedgeAroundFaceCirculator(&self, f);
        }, py::keep_alive<0, 1>(), "f"_a);

    bind_vertex_property<Scalar, int, bool, uint, Matrix<Scalar,3,1>, Matrix<Scalar,2,1>>(cls);
    bind_face_property<Scalar, int, bool, uint, Matrix<Scalar,3,1>, Matrix<Scalar,2,1>>(cls);
    bind_edge_property<Scalar, int, bool, uint, Matrix<Scalar,3,1>, Matrix<Scalar,2,1>>(cls);
    bind_halfedge_property<Scalar, int, bool, uint, Matrix<Scalar,3,1>, Matrix<Scalar,2,1>>(cls);

    py::class_<Handle>(m, "Handle")
        .def(py::init<>(),
            "default constructor with invalid index")
        .def(py::init<IndexType>(),
            "idx"_a,
            "constructor with index")
        .def("idx", &Handle::idx,
            "Get the underlying index of this handle")
        .def("is_valid", &Handle::is_valid,
            "Return whether the handle is valid, i.e., the index is not equal to PMP_MAX_INDEX.")
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self)
        .def(py::self <= py::self)
        .def(py::self > py::self)
        .def(py::self >= py::self);

    py::class_<Vertex, Handle>(m, "Vertex")
        .def(py::init<>(),
            "default constructor with invalid index")
        .def(py::init<IndexType>(),
            "idx"_a,
            "constructor with index");

    py::class_<Face, Handle>(m, "Face")
        .def(py::init<>(),
            "default constructor with invalid index")
        .def(py::init<IndexType>(),
            "idx"_a,
            "constructor with index");

    py::class_<Edge, Handle>(m, "Edge")
        .def(py::init<>(),
            "default constructor with invalid index")
        .def(py::init<IndexType>(),
            "idx"_a,
            "constructor with index");

    py::class_<Halfedge, Handle>(m, "Halfedge")
        .def(py::init<>(),
            "default constructor with invalid index")
        .def(py::init<IndexType>(),
            "idx"_a,
            "constructor with index");
}
