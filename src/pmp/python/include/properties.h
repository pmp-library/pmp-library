#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <sstream>

#include "pmp/surface_mesh.h"

namespace py = pybind11;
using namespace pybind11::literals;

template <typename Property, typename T>
auto get_property(py::module_& m, const std::string& name, const std::unordered_map<std::type_index, std::string>& type_map) {
    const std::string name_type = name + type_map.find(std::type_index(typeid(T)))->second;
    return py::class_<Property>(m, name_type.c_str())
        .def("__getitem__", [](Property& prop, uint i) {
            if (i >= prop.vector().size()) throw py::index_error("Property data out of range.");
            return prop[i];
        })
        .def("__setitem__", [](Property& prop, uint i, T value) {
            if (i >= prop.vector().size()) throw py::index_error("Property data out of range.");
            prop[i] = value;
        })
        .def("vector", &Property::vector,
            "Get reference to the underlying vector.");
}

template <typename Property, typename Handle, typename T>
auto get_property_handle(py::module_& m, const std::string& name, const std::unordered_map<std::type_index, std::string>& type_map) {
    // necessary to distinguish between the different templates
    const std::string name_type = name + type_map.find(std::type_index(typeid(T)))->second;
    return py::class_<Property, pmp::Property<T>>(m, name_type.c_str())
        .def("__getitem__", [](Property& prop, Handle &i) {
            if (i.idx() >= prop.vector().size()) throw py::index_error("Property data out of range.");
            return prop[i];
        })
        .def("__setitem__", [](Property& prop, Handle &i, T value) {
            if (i.idx() >= prop.vector().size()) throw py::index_error("Property data out of range.");
            prop[i] = value;
        });
}

template <template <typename> class Property, typename... Types>
void bind_property(py::module_& m, const std::string& name, const std::unordered_map<std::type_index, std::string>& type_map) {
    (get_property<Property<Types>, Types>(m, name, type_map), ...);
}

template <template <typename> class Property, typename Handle, typename... Types>
void bind_property_handle(py::module_& m, const std::string& name, const std::unordered_map<std::type_index, std::string>& type_map) {
    (get_property_handle<Property<Types>, Handle, Types>(m, name, type_map), ...);
}

template <typename T>
void vertex_property(py::class_<pmp::SurfaceMesh>& cls) {
    cls.def("vertex_property", [](pmp::SurfaceMesh& self, const std::string& name, const T t = T()){
        return self.vertex_property<T>(name, t);
    },
        "name"_a, "T"_a,
        "If a vertex property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("remove_vertex_property", [](pmp::SurfaceMesh& self, pmp::VertexProperty<T>& p) {
        self.remove_vertex_property(p);
    }, 
        "p"_a,
        "Remove the vertex property p"  
    );
    cls.def("has_vertex_property", &pmp::SurfaceMesh::has_vertex_property,
        "name"_a, "does the mesh has a vertex property with name name?"
    );
    cls.def("vertex_properties", &pmp::SurfaceMesh::vertex_properties,
        "return the names of all vertex properties"
    );
}

template <typename ... Types>
void bind_vertex_property(py::class_<pmp::SurfaceMesh>& cls) {
    (vertex_property<Types>(cls), ...);
}

template <typename T>
void get_edge_property(py::class_<pmp::SurfaceMesh>& cls) {
    cls.def("edge_property", [](pmp::SurfaceMesh& self, const std::string& name, const T t = T()){
        return self.edge_property<T>(name, t);
    },
        "name"_a, "T"_a,
        "If an edge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("remove_edge_property", [](pmp::SurfaceMesh& self, pmp::EdgeProperty<T>& p) {
        self.remove_edge_property(p);
    }, 
        "p"_a,
        "Remove the edge property p"  
    );
    cls.def("has_edge_property", &pmp::SurfaceMesh::has_edge_property,
        "name"_a, "does the mesh has an edge property with name name?"
    );
    cls.def("edge_properties", &pmp::SurfaceMesh::vertex_properties,
        "return the names of all edge properties"
    );
}

template <typename ... Types>
void bind_edge_property(py::class_<pmp::SurfaceMesh>& cls) {
    (get_edge_property<Types>(cls), ...);
}

template <typename T>
void get_halfedge_property(py::class_<pmp::SurfaceMesh>& cls) {
    cls.def("halfedge_property", [](pmp::SurfaceMesh& self, const std::string& name, const T t = T()){
        return self.halfedge_property<T>(name, t);
    },
        "name"_a, "T"_a,
        "If a halfedge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("remove_halfedge_property", [](pmp::SurfaceMesh& self, pmp::HalfedgeProperty<T>& p) {
        self.remove_halfedge_property(p);
    }, 
        "p"_a,
        "Remove the halfedge property p"  
    );
    cls.def("has_halfedge_property", &pmp::SurfaceMesh::has_halfedge_property,
        "name"_a, "does the mesh has a halfedge property with name name?"
    );
    cls.def("halfedge_properties", &pmp::SurfaceMesh::vertex_properties,
        "return the names of all halfedge properties"
    );
}

template <typename ... Types>
void bind_halfedge_property(py::class_<pmp::SurfaceMesh>& cls) {
    (get_halfedge_property<Types>(cls), ...);
}

template <typename T>
void get_face_property(py::class_<pmp::SurfaceMesh>& cls) {
    cls.def("face_property", [](pmp::SurfaceMesh& self, const std::string& name, const T t = T()){
        return self.face_property<T>(name, t);
    },
        "name"_a, "T"_a,
        "If a face property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("remove_face_property", [](pmp::SurfaceMesh& self, pmp::FaceProperty<T>& p) {
        self.remove_face_property(p);
    }, 
        "p"_a,
        "Remove the face property p"  
    );
    cls.def("has_face_property", &pmp::SurfaceMesh::has_face_property,
        "name"_a, "does the mesh has a face property with name name?"
    );
    cls.def("face_properties", &pmp::SurfaceMesh::vertex_properties,
        "return the names of all face properties"
    );
}

template <typename ... Types>
void bind_face_property(py::class_<pmp::SurfaceMesh>& cls) {
    (get_face_property<Types>(cls), ...);
}
