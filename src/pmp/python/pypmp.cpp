#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <sstream>
#include <any>
#include <typeinfo>
#include "pmp/surface_mesh.h"
#include "pmp/types.h"

namespace py = pybind11;
using namespace pybind11::literals;

// Compile using: c++ -I /Users/nemectad/Documents/Programming/pmp-library/external/pybind11/include -I /usr/local/Cellar/python@3.12/3.12.3/Frameworks/Python.framework/Headers -I /Users/nemectad/Documents/Programming/pmp-library/src -O3 -Wall -shared -std=c++17 -undefined dynamic_lookup pypmp.cpp -L /Users/nemectad/Documents/Programming/pmp-library/build -Wl,-rpath,/Users/nemectad/Documents/Programming/pmp-library/build -lpmp -o src/pypmp/pypmp$(python3-config --extension-suffix)
// note: the rpath is required for runtime search path for the library

void check_sequence(const std::string &name, const int allowed_size, const std::string &element_names, const py::sequence &seq) {
    if (seq.size() != allowed_size) {
        std::stringstream ss;
        ss << name << " requires sequence of " << allowed_size << " " << element_names;
        throw py::index_error(ss.str());
    }
}

template <typename Mat, typename Scalar>
auto get_matrix(py::module_& m, const std::string& name) {
    return py::class_<Mat>(m, name.c_str())
        .def(py::init<>())
        .def(py::init<Scalar>(), "fill"_a)

        .def("__getitem__", [](const Mat& mat, std::pair<size_t, size_t> ij) {
            if (ij.first >= mat.rows() || ij.second >= mat.cols()) throw py::index_error();
            return mat(ij.first, ij.second);
        })
        .def("__setitem__", [](Mat& mat, std::pair<size_t, size_t> ij, Scalar val) {
            if (ij.first >= mat.rows() || ij.second >= mat.cols()) throw py::index_error();
            mat(ij.first, ij.second) = val;
        })
        .def("__getitem__", [](const Mat& mat, size_t i) {
            if (i >= mat.rows() * mat.cols()) throw py::index_error();
            return mat[i];
        })
        .def("__setitem__", [](Mat& mat, size_t i, Scalar val) {
            if (i >= mat.rows() * mat.cols()) throw py::index_error();
            mat[i] = val;
        })
        .def("__len__", [](const Mat& mat) { return mat.cols() * mat.rows(); })
        // Basic math
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * Scalar())
        .def(Scalar() * py::self)
        .def(py::self / Scalar())
        .def(py::self == py::self)
        // Utility
        .def("normalize", &Mat::normalize)
        .def("__repr__", [name](const Mat& mat) {
            std::ostringstream oss;
            oss << name << "(";
            for (int i = 0; i < mat.rows(); ++i) {
                for (int j = 0; j < mat.cols(); ++j) {
                    oss << mat(i,j);
                    if (i != mat.rows()-1 || j != mat.cols()-1) oss << ", ";
                }
            }
            oss << ")";
            return oss.str();
        });
}

template <typename Mat, typename Scalar, int M, int N>
auto bind_matrix(py::module_& m, const std::string& name)
{
    auto matrix = get_matrix<Mat, Scalar>(m, name);

    if constexpr (M == 2 && N == 1) {
        matrix.def(py::init<Scalar, Scalar>(), "x"_a, "y"_a,
                "Construct 2D vector");
        matrix.def(py::init([name](const py::sequence &seq) {
            check_sequence(name, M*N, "scalars", seq);
            return Mat(seq[0].cast<Scalar>(), seq[1].cast<Scalar>());
        }),
            "array"_a
            "Construct 2D vector");
    }
    if constexpr (M == 3 && N == 1) {
        matrix.def(py::init<Scalar, Scalar, Scalar>(),
                "x"_a, "y"_a, "z"_a,
                "Construct 3D vector");
        matrix.def(py::init([name](const py::sequence &seq) {
            check_sequence(name, M*N, "scalars", seq);
            return pmp::Point(seq[0].cast<Scalar>(), seq[1].cast<Scalar>(), seq[2].cast<Scalar>());
        }),
            "array"_a
            "Construct 3D vector");
    }
    return matrix;
    /*if constexpr (M == 4 && N == 1) {
        matrix.def(py::init<Scalar, Scalar, Scalar, Scalar>(),
                py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"),
                "Construct 4D vector");
    }
    if constexpr (M == 3 && N == 3) {
        matrix.def(py::init<pmp::Point, pmp::Point, pmp::Point>(),
                py::arg("p0"), py::arg("p1"), py::arg("p2"),
                "Construct 3x3 matrix");
    }*/
}
/*

template <typename Derived, typename Base, typename Scalar>
void bind_vector_alias(py::module_ &m, const std::string &name) {
    py::class_<Derived, Base>(m, name.c_str())
        //.def(py::init<>())
        //.def(py::init<Scalar>(), "fill"_a)
        .def(py::init<Scalar,
                      Scalar,
                      Scalar>(),
             "x"_a, "y"_a, "z"_a)
        .def(py::init([](const py::sequence &seq) {
            if (seq.size() != 3) throw py::value_error("Expected 3 elements");
            return Derived(seq[0].cast<Scalar>(),
                           seq[1].cast<Scalar>(),
                           seq[2].cast<Scalar>());
        }));
}
*/
template <typename Circulator, typename HandleOut, typename HandleIn>
struct CirculatorWrapper {
    Circulator circ;
    Circulator start;
    bool first = true;

    CirculatorWrapper<Circulator>(const pmp::SurfaceMesh* mesh, HandleIn h)
        : circ(mesh, h), start(mesh, h) {}

    HandleOut next() {
        if (!first && circ == start)
            throw py::stop_iteration();

        first = false;
        HandleOut v = *circ;
        ++circ;
        return v;
    }
    
    HandleOut prev() {
        if (!first && circ == start)
            throw py::stop_iteration();

        first = false;
        HandleOut v = *circ;
        --circ;
        return v;
    }

};

template <typename Circulator, typename HandleOut, typename HandleIn>
void bind_circulator(py::module_& m, const std::string& name) {
    py::class_<CirculatorWrapper<Circulator, HandleOut, HandleIn>>(m, name.c_str())
        .def(py::init<const pmp::SurfaceMesh*, HandleIn>(), "mesh"_a, "handle"_a)
        .def("__iter__", [](CirculatorWrapper<Circulator, HandleOut, HandleIn> &self) {
            return self;
        })
        .def("__next__", &CirculatorWrapper<Circulator, HandleOut, HandleIn>::next)
        .def("previous", &CirculatorWrapper<Circulator, HandleOut, HandleIn>::prev);
}

//struct PropertyWrapper {
    //typename T;
    /*Property<T> property;
    const std::string property_name;

    PropertyWrapper(typename T, const std::string &property_name) 
        : T(T), property_name(property_name), property(Property<T>()) {}
    
    std::vector<T>& vector() { return property.vector(); }
    */
/*
    std::any property;
    const std::string property_name;

    template <typename T>
    PropertyWrapper(T property, const std::string &property_name)
        : T(property), property_name(property_name)

    auto vector() { return property.vector(); }
}*/

template <typename Property, typename T>
auto get_property(py::module_& m, const std::string& name) {
    const std::string name_type = name + typeid(T).name();
    return py::class_<Property>(m, name_type.c_str())
        //.def(py::init<>())
        //.def(py::init<pmp::PropertyArray<T>>(), "p"_a)
        .def("__getitem__", [](Property& prop, uint i) {
            if (i >= prop.vector().size()) throw py::index_error("Property data out of range.");
            return prop[i];
        })
        /*.def("__getitem__", [](Property& prop, pmp::Vertex &i) {
            if (i.idx() >= prop.vector().size()) throw py::index_error("Property data out of range.");
            return prop[i];
        })*/
        .def("__setitem__", [](Property& prop, uint i, T value) {
            if (i >= prop.vector().size()) throw py::index_error("Property data out of range.");
            prop[i] = value;
        })
        /*.def("__setitem__", [](Property& prop, pmp::Vertex &i, T value) {
            if (i.idx() >= prop.vector().size()) throw py::index_error("Property data out of range.");
            prop[i] = value;
        })*/
        .def("vector", &Property::vector,
            "Get reference to the underlying vector.");
}

template <typename Property, typename Handle, typename T>
auto get_property_handle(py::module_& m, const std::string& name) {
    // necessary to distinguish between the different templates
    const std::string name_type = name + typeid(T).name();
    return py::class_<Property, pmp::Property<T>>(m, name_type.c_str())
        //.def(py::init<>())
        //.def(py::init<pmp::PropertyArray<T>>(), "p"_a)
        .def("__getitem__", [](Property& prop, Handle &i) {
            if (i.idx() >= prop.vector().size()) throw py::index_error("Property data out of range.");
            return prop[i];
        })
        .def("__setitem__", [](Property& prop, Handle &i, T value) {
            if (i.idx() >= prop.vector().size()) throw py::index_error("Property data out of range.");
            prop[i] = value;
        });
        //.def("vector", &Property::vector,
        //    "Get reference to the underlying vector.");
}

template <template <typename> class Property, typename... Types>
void bind_property(py::module_& m, const std::string& name) {
    (get_property<Property<Types>, Types>(m, name), ...);
}

template <template <typename> class Property, typename Handle, typename... Types>
void bind_property_handle(py::module_& m, const std::string& name) {
    (get_property_handle<Property<Types>, Handle, Types>(m, name), ...);
}
        
/*
template <class Property>
void bind_property_wrapper(py::module_& m, const std::string& name) {
    py::class_<PropertyWrapper<Property>>(m, name.c_str())
    */
    /*
    .def(py::init([](typename T, const std::string& property_name) {
        return PropertyWrapper(T, property_name).property;
        })
        )
        */
        /*
        .def("__getitem__", [](const PropertyWrapper<Property>& prop, size_t i) {
            if (i > prop.property.data_.size()) throw py::index_error("Property data out of range.");
            return prop.property[i];
        })
        .def("vector", &PropertyWrapper<Property>::vector,
        "Get reference to the underlying vector.");
    }
    */



// Recursively detect list dimensionality
/*
int list_dim(const py::list& list) {
    py::list l = list.cast<py::list>();
    if (l.empty())
    return 1;  // empty list counts as 1D
    
    // Recurse into first element
    return 1 + list_dim(l[0]);
}

pmp::Matrix matrix_from_list(py::list list) {
    if (!py::isinstance<py::list>(list))
    throw py::value_error();
    
    int dim = list_dim(list);
    int M, N;
    
    if (dim > 2)
    throw py::value_error();
    
    if (list.size() == 0)
    throw py::value_error();
    
    for (int d = 0; i < dim; d++) {
        M = list.size();
        
    }
}

*/



PYBIND11_MODULE(pypmp, m, py::mod_gil_not_used()) {
    m.doc() = R"pbdoc(
        PyPMP - Python PMP library Binding
    )pbdoc";

    using namespace pmp;

    // Base Matrix types
    //bind_matrix<Matrix<Scalar,3,1>, Scalar, 3, 1>(m, "Point");
    auto vector3d = bind_matrix<Matrix<Scalar,3,1>, Scalar, 3, 1>(m, "Vector3D");
    //bind_matrix<Matrix<Scalar,3,1>, Scalar, 3, 1>(m, "Color");
    //bind_matrix<Matrix<Scalar,3,1>, Scalar, 3, 1>(m, "Normal");
    auto vector2d = bind_matrix<Matrix<Scalar,2,1>, Scalar, 2, 1>(m, "Vector2D");

    m.attr("Point") = vector3d;
    m.attr("Color") = vector3d;
    m.attr("Normal") = vector3d;
    m.attr("TexCoord") = vector2d;

    /*
    Later use something like this

    py::object Point = m.attr("Vector3D");
    Point.attr("__doc__") = "Alias of Vector3D used to represent 3D points.";
    m.attr("Point") = Point;
    */

    //bind_vector_alias<Point, Matrix<Scalar,3,1>, Scalar>(m, "Point");
    //py::class_<Point, Matrix<Scalar,3,1>>(m, "Point");
    /*py::class_<Normal, Point>(m, "Normal");
    py::class_<TexCoord, Point>(m, "TextCoord");
    */
    //bind_matrix<Matrix<Scalar,3,3>, Scalar, 3, 3>(m, "Mat3");

    // Circulators
    bind_circulator<SurfaceMesh::VertexAroundVertexCirculator, Vertex, Vertex>(m, "VertexAroundVertexCirculator");
    using VertexAroundVertexCirculator = CirculatorWrapper<SurfaceMesh::VertexAroundVertexCirculator, Vertex, Vertex>;
    bind_circulator<SurfaceMesh::HalfedgeAroundVertexCirculator, Halfedge, Vertex>(m, "HalfedgeAroundVertexCirculator");
    using HalfedgeAroundVertexCirculator = CirculatorWrapper<SurfaceMesh::HalfedgeAroundVertexCirculator, Halfedge, Vertex>;
    bind_circulator<SurfaceMesh::EdgeAroundVertexCirculator, Edge, Vertex>(m, "EdgeAroundVertexCirculator");
    using EdgeAroundVertexCirculator = CirculatorWrapper<SurfaceMesh::VertexAroundVertexCirculator, Edge, Vertex>;
    bind_circulator<SurfaceMesh::FaceAroundVertexCirculator, Face, Vertex>(m, "FaceAroundVertexCirculator");
    using FaceAroundVertexCirculator = CirculatorWrapper<SurfaceMesh::FaceAroundVertexCirculator, Face, Vertex>;
    bind_circulator<SurfaceMesh::VertexAroundFaceCirculator, Vertex, Face>(m, "VertexAroundFaceCirculator");
    using VertexAroundFaceCirculator = CirculatorWrapper<SurfaceMesh::VertexAroundFaceCirculator, Vertex, Face>;
    bind_circulator<SurfaceMesh::HalfedgeAroundFaceCirculator, Halfedge, Face>(m, "HalfedgeAroundFaceCirculator");
    using HalfedgeAroundFaceCirculator = CirculatorWrapper<SurfaceMesh::HalfedgeAroundFaceCirculator, Halfedge, Face>;

    // Explicit declaration of possible properties and their types
    bind_property<Property, Scalar, int, bool, uint, Matrix<Scalar,3,1>>(m, "Property"); //
    bind_property_handle<VertexProperty, Vertex, Scalar, int, bool, uint, Matrix<Scalar,3,1>>(m, "VertexProperty");
    bind_property_handle<EdgeProperty, Edge, Scalar, int, bool, uint, Matrix<Scalar,3,1>>(m, "EdgeProperty");
    bind_property_handle<HalfedgeProperty, Halfedge, Scalar, int, bool, uint, Matrix<Scalar,3,1>>(m, "HalfedgeProperty");
    bind_property_handle<FaceProperty, Face, Scalar, int, bool, uint, Matrix<Scalar,3,1>>(m, "FaceProperty");

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
        //.def("add_vertex_property", []())
        // TODO: Add properties for V, E, H, F

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

    // TODO: bind add_vertex_property and others using cls and void 

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
