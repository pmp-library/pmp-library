#include "algorithms.h"

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(algorithms, m, py::mod_gil_not_used()) {
    m.doc() = R"pbdoc(
        Algorithms - algorithms for PMP mesh manipulation
    )pbdoc";

    bind_algorithms(m);
}
