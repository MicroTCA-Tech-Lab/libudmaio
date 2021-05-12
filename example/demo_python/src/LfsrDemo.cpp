#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "DmaMode.hpp"
#include "LfsrIo.h"

namespace py = pybind11;

PYBIND11_MODULE(lfsr_demo, m) {
    py::class_<LfsrIo> lfsr_io(m, "LfsrIo");

    lfsr_io.def(py::init<boost::log::trivial::severity_level, DmaMode, const std::string&>())
           .def("start", &LfsrIo::start)
           .def("stop", &LfsrIo::stop)
           .def("read", &LfsrIo::read);

    py::enum_<DmaMode>(lfsr_io, "DmaMode")
        .value("xdma", DmaMode::XDMA)
        .value("uio", DmaMode::UIO)
        .export_values();

    py::enum_<boost::log::trivial::severity_level>(lfsr_io, "LogLevel")
        .value("fatal", boost::log::trivial::severity_level::fatal)
        .value("info", boost::log::trivial::severity_level::info)
        .value("debug", boost::log::trivial::severity_level::debug)
        .value("trace", boost::log::trivial::severity_level::trace)
        .export_values();
}
