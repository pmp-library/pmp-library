#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <sstream>

#include "pmp/surface_mesh.h"
#include "pmp/types.h"

namespace py = pybind11;
using namespace pybind11::literals;

void check_sequence(const std::string &name, const int allowed_size, const std::string &element_names, const py::sequence &seq);
void bind_matrices(py::module_& m);


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
        .def(py::self * pmp::Scalar())
        .def(pmp::Scalar() * py::self)
        .def(py::self / pmp::Scalar())
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

