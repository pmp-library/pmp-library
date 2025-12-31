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
    const std::string name_type = name + type_map.find(std::type_index(typeid(T)))->second + "Property";
    return py::class_<Property>(m, name_type.c_str())
        .def("__getitem__", [](Property& prop, uint i) {
            if (i >= prop.vector().size()) throw py::index_error("Property data out of range.");
            if constexpr (std::is_same_v<T, bool>) {
                return static_cast<bool>(prop[i]);
            } else {
                return prop[i];
            }
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
            if constexpr (std::is_same_v<T, bool>) {
                return static_cast<bool>(prop[i]);
            } else {
                return prop[i];
            }        
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
void remove_vertex_property(py::class_<pmp::SurfaceMesh>& cls) {
    cls.def("remove_vertex_property", [](pmp::SurfaceMesh& self, pmp::VertexProperty<T>& p) {
        self.remove_vertex_property(p);
    }, 
        "p"_a,
        "Remove the vertex property p"  
    );
}

template <typename ... Types>
void bind_vertex_property(py::class_<pmp::SurfaceMesh>& cls) {
    (remove_vertex_property<Types>(cls), ...);
    cls.def("has_vertex_property", &pmp::SurfaceMesh::has_vertex_property,
        "name"_a, "does the mesh has a vertex property with name name?"
    );
    cls.def("vertex_properties", &pmp::SurfaceMesh::vertex_properties,
        "return the names of all vertex properties"
    );
    cls.def("vertex_int_property", [](pmp::SurfaceMesh& self, const std::string& name, const int t = int()){
        return self.vertex_property<int>(name, t);
    },
        "name"_a, "T"_a,
        "If a vertex property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("vertex_bool_property", [](pmp::SurfaceMesh& self, const std::string& name, const bool t = bool()){
        return self.vertex_property<bool>(name, t);
    },
        "name"_a, "T"_a,
        "If a vertex property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("vertex_uint_property", [](pmp::SurfaceMesh& self, const std::string& name, const uint t = uint()){
        return self.vertex_property<uint>(name, t);
    },
        "name"_a, "T"_a,
        "If a vertex property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("vertex_scalar_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Scalar t = pmp::Scalar()){
        return self.vertex_property<pmp::Scalar>(name, t);
    },
        "name"_a, "t"_a=pmp::Scalar(),
        "If a vertex property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("vertex_vector3d_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Matrix<pmp::Scalar,3,1> t = pmp::Matrix<pmp::Scalar,3,1>(0)){
        return self.vertex_property<pmp::Matrix<pmp::Scalar,3,1>>(name, t);
    },
        "name"_a, "t"_a=pmp::Matrix<pmp::Scalar,3,1>(0),
        "If a vertex property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.attr("vertex_Normal_property") = cls.attr("vertex_vector3d_property");
    cls.attr("vertex_Point_property") = cls.attr("vertex_vector3d_property");
    cls.attr("vertex_Color_property") = cls.attr("vertex_vector3d_property");    cls.def("vertex_vector2d_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Matrix<pmp::Scalar,2,1> t = pmp::Matrix<pmp::Scalar,2,1>(0)){
        return self.vertex_property<pmp::Matrix<pmp::Scalar,2,1>>(name, t);
    },
        "name"_a, "t"_a=pmp::Matrix<pmp::Scalar,2,1>(0),
        "If a vertex property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.attr("vertex_TexCoord_property") = cls.attr("vertex_vector2d_property");
}

template <typename T>
void remove_edge_property(py::class_<pmp::SurfaceMesh>& cls) {
    cls.def("remove_edge_property", [](pmp::SurfaceMesh& self, pmp::EdgeProperty<T>& p) {
        self.remove_edge_property(p);
    }, 
        "p"_a,
        "Remove the edge property p"  
    );
}

template <typename ... Types>
void bind_edge_property(py::class_<pmp::SurfaceMesh>& cls) {
    (remove_edge_property<Types>(cls), ...);
    cls.def("has_edge_property", &pmp::SurfaceMesh::has_edge_property,
        "name"_a, "does the mesh has an edge property with name name?"
    );
    cls.def("edge_properties", &pmp::SurfaceMesh::edge_properties,
        "return the names of all edge properties"
    );
    cls.def("edge_int_property", [](pmp::SurfaceMesh& self, const std::string& name, const int t = int()){
        return self.edge_property<int>(name, t);
    },
        "name"_a, "t"_a=int(),
        "If an edge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("edge_bool_property", [](pmp::SurfaceMesh& self, const std::string& name, const bool t = bool()){
        return self.edge_property<bool>(name, t);
    },
        "name"_a, "t"_a=false,
        "If an edge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("edge_uint_property", [](pmp::SurfaceMesh& self, const std::string& name, const uint t = uint()){
        return self.edge_property<uint>(name, t);
    },
        "name"_a, "t"_a=uint(),
        "If an edge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("edge_scalar_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Scalar t = pmp::Scalar()){
        return self.edge_property<pmp::Scalar>(name, t);
    },
        "name"_a, "t"_a=pmp::Scalar(),
        "If a edge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("edge_vector3d_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Matrix<pmp::Scalar,3,1> t = pmp::Matrix<pmp::Scalar,3,1>(0)){
        return self.edge_property<pmp::Matrix<pmp::Scalar,3,1>>(name, t);
    },
        "name"_a, "t"_a=pmp::Matrix<pmp::Scalar,3,1>(0),
        "If a edge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.attr("edge_Normal_property") = cls.attr("edge_vector3d_property");
    cls.attr("edge_Point_property") = cls.attr("edge_vector3d_property");
    cls.attr("edge_Color_property") = cls.attr("edge_vector3d_property");
    cls.def("edge_vector2d_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Matrix<pmp::Scalar,2,1> t = pmp::Matrix<pmp::Scalar,2,1>(0)){
        return self.edge_property<pmp::Matrix<pmp::Scalar,2,1>>(name, t);
    },
        "name"_a, "t"_a=pmp::Matrix<pmp::Scalar,2,1>(0),
        "If a edge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.attr("edge_TexCoord_property") = cls.attr("edge_vector2d_property");
}

template <typename T>
void remove_halfedge_property(py::class_<pmp::SurfaceMesh>& cls) {
    cls.def("remove_halfedge_property", [](pmp::SurfaceMesh& self, pmp::HalfedgeProperty<T>& p) {
        self.remove_halfedge_property(p);
    }, 
        "p"_a,
        "Remove the halfedge property p"  
    );
}

template <typename ... Types>
void bind_halfedge_property(py::class_<pmp::SurfaceMesh>& cls) {
    (remove_halfedge_property<Types>(cls), ...);
    cls.def("has_halfedge_property", &pmp::SurfaceMesh::has_halfedge_property,
        "name"_a, "does the mesh has a halfedge property with name name?"
    );
    cls.def("halfedge_properties", &pmp::SurfaceMesh::halfedge_properties,
        "return the names of all halfedge properties"
    );
    cls.def("halfedge_int_property", [](pmp::SurfaceMesh& self, const std::string& name, const int t = int()){
        return self.halfedge_property<int>(name, t);
    },
        "name"_a, "T"_a,
        "If a halfedge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("halfedge_bool_property", [](pmp::SurfaceMesh& self, const std::string& name, const bool t = bool()){
        return self.halfedge_property<bool>(name, t);
    },
        "name"_a, "T"_a,
        "If a halfedge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("halfedge_uint_property", [](pmp::SurfaceMesh& self, const std::string& name, const uint t = uint()){
        return self.halfedge_property<uint>(name, t);
    },
        "name"_a, "T"_a,
        "If a halfedge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("halfedge_scalar_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Scalar t = pmp::Scalar()){
        return self.halfedge_property<pmp::Scalar>(name, t);
    },
        "name"_a, "t"_a=pmp::Scalar(),
        "If a halfedge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("halfedge_vector3d_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Matrix<pmp::Scalar,3,1> t = pmp::Matrix<pmp::Scalar,3,1>(0)){
        return self.halfedge_property<pmp::Matrix<pmp::Scalar,3,1>>(name, t);
    },
        "name"_a, "t"_a=pmp::Matrix<pmp::Scalar,3,1>(0),
        "If a halfedge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.attr("halfedge_Normal_property") = cls.attr("halfedge_vector3d_property");
    cls.attr("halfedge_Point_property") = cls.attr("halfedge_vector3d_property");
    cls.attr("halfedge_Color_property") = cls.attr("halfedge_vector3d_property");
    cls.def("halfedge_vector2d_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Matrix<pmp::Scalar,2,1> t = pmp::Matrix<pmp::Scalar,2,1>(0)){
        return self.halfedge_property<pmp::Matrix<pmp::Scalar,2,1>>(name, t);
    },
        "name"_a, "t"_a=pmp::Matrix<pmp::Scalar,2,1>(0),
        "If a halfedge property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.attr("halfedge_TexCoord_property") = cls.attr("halfedge_vector2d_property");
}

template <typename T>
void remove_face_property(py::class_<pmp::SurfaceMesh>& cls) {
    cls.def("remove_face_property", [](pmp::SurfaceMesh& self, pmp::FaceProperty<T>& p) {
        self.remove_face_property(p);
    }, 
        "p"_a,
        "Remove the face property p"  
    );
}

template <typename ... Types>
void bind_face_property(py::class_<pmp::SurfaceMesh>& cls) {
    (remove_face_property<Types>(cls), ...);
    cls.def("has_face_property", &pmp::SurfaceMesh::has_face_property,
        "name"_a, "does the mesh has a face property with name name?"
    );
    cls.def("face_properties", &pmp::SurfaceMesh::face_properties,
        "return the names of all face properties"
    );
    cls.def("face_int_property", [](pmp::SurfaceMesh& self, const std::string& name, const int t = int()){
        return self.face_property<int>(name, t);
    },
        "name"_a, "T"_a,
        "If a face property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("face_bool_property", [](pmp::SurfaceMesh& self, const std::string& name, const bool t = bool()){
        return self.face_property<bool>(name, t);
    },
        "name"_a, "T"_a,
        "If a face property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("face_uint_property", [](pmp::SurfaceMesh& self, const std::string& name, const uint t = uint()){
        return self.face_property<uint>(name, t);
    },
        "name"_a, "T"_a,
        "If a face property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("face_scalar_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Scalar t = pmp::Scalar()){
        return self.face_property<pmp::Scalar>(name, t);
    },
        "name"_a, "t"_a=pmp::Scalar(),
        "If a face property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.def("face_vector3d_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Matrix<pmp::Scalar,3,1> t = pmp::Matrix<pmp::Scalar,3,1>(0)){
        return self.face_property<pmp::Matrix<pmp::Scalar,3,1>>(name, t);
    },
        "name"_a, "t"_a=pmp::Matrix<pmp::Scalar,3,1>(0),
        "If a face property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.attr("face_Normal_property") = cls.attr("face_vector3d_property");
    cls.attr("face_Point_property") = cls.attr("face_vector3d_property");
    cls.attr("face_Color_property") = cls.attr("face_vector3d_property");
    cls.def("face_vector2d_property", [](pmp::SurfaceMesh& self, const std::string& name, const pmp::Matrix<pmp::Scalar,2,1> t = pmp::Matrix<pmp::Scalar,2,1>(0)){
        return self.face_property<pmp::Matrix<pmp::Scalar,2,1>>(name, t);
    },
        "name"_a, "t"_a=pmp::Matrix<pmp::Scalar,2,1>(0),
        "If a face property of type T with name name exists, it is "
        "returned. Otherwise this property is added (with default value t)"
    );
    cls.attr("face_TexCoord_property") = cls.attr("face_vector2d_property");
}
