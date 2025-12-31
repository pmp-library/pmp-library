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
#include "pmp/algorithms/smoothing.h"
#include "pmp/algorithms/subdivision.h"
#include "pmp/algorithms/normals.h"
#include "pmp/algorithms/hole_filling.h"
#include "pmp/algorithms/features.h"

namespace py = pybind11;

void bind_algorithms(py::module_& algorithms);
void bind_fairing(py::module_& algorithms);
void bind_triangulation(py::module_& algorithms);
void bind_decimation(py::module_& algorithms);
void bind_remeshing(py::module_& algorithms);
void bind_smoothing(py::module_& algorithms);
void bind_subdivision(py::module_& algorithms);
void bind_normals(py::module_& algorithms);
void bind_hole_filling(py::module_& algorithms);
void bind_features(py::module_& algorithms);