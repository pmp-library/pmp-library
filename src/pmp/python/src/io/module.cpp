#include "io.h"

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(pmp_io, m, py::mod_gil_not_used()) {
    m.doc() = R"pbdoc(
        IO - Methods for read/write geometry data in multiple formats
    )pbdoc";

    bind_io(m);
}
