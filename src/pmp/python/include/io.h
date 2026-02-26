#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/stl/filesystem.h>
#include <sstream>

#include "pmp/surface_mesh.h"
#include "pmp/io/io.h"
#include "pmp/io/io_flags.h"

namespace py = pybind11;
void bind_io(py::module_ &io);