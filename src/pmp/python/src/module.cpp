#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <sstream>

#include "pmp/surface_mesh.h"
#include "surface_mesh.h"
#include "circulators.h"
#include "properties.h"
#include "matrices.h"

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(pypmp, m, py::mod_gil_not_used()) {
    m.doc() = R"pbdoc(
        PyPMP - Python binding for Polygon Mesh Processing (PMP) library
    )pbdoc";

    using namespace pmp;

    // Matrices & vectors
    bind_matrices(m);
    
    // Circulators
    bind_circulator<SurfaceMesh::VertexAroundVertexCirculator, Vertex, Vertex>(m, "VertexAroundVertexCirculator");
    bind_circulator<SurfaceMesh::HalfedgeAroundVertexCirculator, Halfedge, Vertex>(m, "HalfedgeAroundVertexCirculator");
    bind_circulator<SurfaceMesh::EdgeAroundVertexCirculator, Edge, Vertex>(m, "EdgeAroundVertexCirculator");
    bind_circulator<SurfaceMesh::FaceAroundVertexCirculator, Face, Vertex>(m, "FaceAroundVertexCirculator");
    bind_circulator<SurfaceMesh::VertexAroundFaceCirculator, Vertex, Face>(m, "VertexAroundFaceCirculator");
    bind_circulator<SurfaceMesh::HalfedgeAroundFaceCirculator, Halfedge, Face>(m, "HalfedgeAroundFaceCirculator");
    
    // Surface mesh
    bind_surface_mesh(m);

    // Explicit declaration of possible properties and their types
    std::unordered_map<std::type_index, std::string> type_map = {
        { typeid(Scalar), "Scalar" },
        { typeid(int), "Int" },
        { typeid(bool), "Bool" },
        { typeid(uint), "Uint" },
        { typeid(Matrix<Scalar,3,1>), "Vector3D" },
        { typeid(Matrix<Scalar,2,1>), "Vector2D" }
    };

    // Properties
    bind_property<Property, Scalar, int, bool, uint, Matrix<Scalar,3,1>, Matrix<Scalar,2,1>>(m, "Property", type_map);
    bind_property_handle<VertexProperty, Vertex, Scalar, int, bool, uint, Matrix<Scalar,3,1>, Matrix<Scalar,2,1>>(m, "VertexProperty", type_map);
    bind_property_handle<EdgeProperty, Edge, Scalar, int, bool, uint, Matrix<Scalar,3,1>, Matrix<Scalar,2,1>>(m, "EdgeProperty", type_map);
    bind_property_handle<HalfedgeProperty, Halfedge, Scalar, int, bool, uint, Matrix<Scalar,3,1>, Matrix<Scalar,2,1>>(m, "HalfedgeProperty", type_map);
    bind_property_handle<FaceProperty, Face, Scalar, int, bool, uint, Matrix<Scalar,3,1>, Matrix<Scalar,2,1>>(m, "FaceProperty", type_map);
}