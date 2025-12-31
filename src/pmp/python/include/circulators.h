#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <sstream>

#include "pmp/surface_mesh.h"

namespace py = pybind11;
using namespace pybind11::literals;

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
        .def("prev", &CirculatorWrapper<Circulator, HandleOut, HandleIn>::prev);
}