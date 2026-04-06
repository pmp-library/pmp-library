#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <sstream>

#include "pmp/surface_mesh.h"
#include "pmp/types.h"

namespace py = pybind11;

void bind_surface_mesh(py::module_& m);