#include "matrices.h"

void bind_matrices(py::module_& m) {
    // Base Matrix types
    auto vector3d = bind_matrix<pmp::Matrix<pmp::Scalar,3,1>, pmp::Scalar, 3, 1>(m, "Vector3D");
    auto vector2d = bind_matrix<pmp::Matrix<pmp::Scalar,2,1>, pmp::Scalar, 2, 1>(m, "Vector2D");

    // Derive basic vector types from Matrix
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
}

void check_sequence(const std::string &name, const int allowed_size, const std::string &element_names, const py::sequence &seq) {
    if (seq.size() != allowed_size) {
        std::stringstream ss;
        ss << name << " requires sequence of " << allowed_size << " " << element_names;
        throw py::index_error(ss.str());
    }
}