#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "DmaMode.hpp"
#include "LfsrIo.h"

namespace py = pybind11;

PYBIND11_MODULE(lfsr_demo, m) {
    py::class_<LfsrIo> lfsr_io(m, "LfsrIo");

    lfsr_io.def(py::init<DmaMode, const std::string&>())
           .def("start", &LfsrIo::start)
           .def("stop", &LfsrIo::stop)
           .def("read", &LfsrIo::read);

    py::enum_<DmaMode>(lfsr_io, "DmaMode")
        .value("xdma", DmaMode::XDMA)
        .value("uio", DmaMode::UIO)
        .export_values();
}
