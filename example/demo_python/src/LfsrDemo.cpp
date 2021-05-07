#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "LfsrIo.h"

namespace py = pybind11;

PYBIND11_MODULE(lfsr_demo, m) {
    py::class_<LfsrIo>(m, "LfsrIo")
        .def(py::init<const std::string&>())
        .def("start", &LfsrIo::start)
        .def("stop", &LfsrIo::stop)
        .def("read", &LfsrIo::read);
}
