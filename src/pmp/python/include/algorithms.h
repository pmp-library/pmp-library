#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <sstream>

#include "pmp/surface_mesh.h"
#include "pmp/algorithms/triangulation.h"
#include "pmp/algorithms/fairing.h"
#include "pmp/algorithms/remeshing.h"
#include "pmp/algorithms/decimation.h"

namespace py = pybind11;

void bind_algorithms(py::module_& m);
void bind_fairing(py::module_& algorithms);
void bind_triangulation(py::module_& algorithms);
void bind_decimation(py::module_& algorithms);
void bind_remeshing(py::module_& algorithms);