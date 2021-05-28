#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "udmaio/UioConfig.hpp"

#include "LfsrIo.h"

namespace py = pybind11;

class UioIf_PyOverrideHelper : public udmaio::UioIf {
public:
    using udmaio::UioIf::UioIf;

    const std::string_view _log_name() const override {
        PYBIND11_OVERRIDE_PURE(
            std::string_view, /* Return type */
            udmaio::UioIf,    /* Parent class */
            _log_name,        /* Name of function */
        );
    };
};

class UioIf_PyPublishHelper : public udmaio::UioIf {
public:
    using udmaio::UioIf::UioIf;
    using udmaio::UioIf::_log_name;
    using udmaio::UioIf::_rd32;
    using udmaio::UioIf::_wr32;
};

PYBIND11_MODULE(lfsr_demo, m) {
    py::class_<udmaio::UioRegion>(m, "UioRegion")
        .def(py::init<uintptr_t, size_t>())
        .def_readwrite("addr", &udmaio::UioRegion::addr)
        .def_readwrite("size", &udmaio::UioRegion::size);

    py::class_<udmaio::UioDeviceInfo>(m, "UioDeviceInfo")
        .def(py::init<>())
        .def_readwrite("dev_path", &udmaio::UioDeviceInfo::dev_path)
        .def_readwrite("evt_path", &udmaio::UioDeviceInfo::evt_path)
        .def_readwrite("region", &udmaio::UioDeviceInfo::region)
        .def_readwrite("mmap_offs", &udmaio::UioDeviceInfo::mmap_offs);
    
    py::class_<udmaio::UioDeviceLocation>(m, "UioDeviceLocation")
        .def(py::init<std::string, UioRegion, std::string>(),
             py::arg("uioname"),
             py::arg("xdmaregion"),
             py::arg("xdmaevtdev") = std::string(""))
        .def_readwrite("uio_name", &udmaio::UioDeviceLocation::uio_name)
        .def_readwrite("xdma_region", &udmaio::UioDeviceLocation::xdma_region)
        .def_readwrite("xdma_evt_dev", &udmaio::UioDeviceLocation::xdma_evt_dev);

    py::class_<udmaio::UioConfigBase, std::shared_ptr<udmaio::UioConfigBase>>(m, "ConfigBase");

    py::class_<udmaio::UioConfigUio, udmaio::UioConfigBase, std::shared_ptr<udmaio::UioConfigUio>>(m, "ConfigUio")
        .def(py::init<>())
        // We have to expose overloaded functions explicitly
        .def("__call__",
             static_cast<udmaio::UioDeviceInfo (udmaio::UioConfigUio::*)(UioDeviceLocation)>(&udmaio::UioConfigUio::operator())
        )
        .def("__call__",
             static_cast<udmaio::UioDeviceInfo (udmaio::UioConfigUio::*)(std::string)>(&udmaio::UioConfigUio::operator())
        );

    py::class_<udmaio::UioConfigXdma, udmaio::UioConfigBase, std::shared_ptr<udmaio::UioConfigXdma>>(m, "ConfigXdma")
        .def(py::init<std::string, uintptr_t>())
        // We have to expose overloaded functions explicitly
        .def("__call__",
             static_cast<udmaio::UioDeviceInfo (udmaio::UioConfigXdma::*)(UioDeviceLocation)>(&udmaio::UioConfigXdma::operator())
        )
        .def("__call__",
             static_cast<udmaio::UioDeviceInfo (udmaio::UioConfigXdma::*)(udmaio::UioRegion, std::string)>(&udmaio::UioConfigXdma::operator()),
             // We have to expose default arguments explicitly
             py::arg("dev_region"),
             py::arg("evt_dev") = ""
        );

    py::class_<udmaio::UioIf, UioIf_PyOverrideHelper, std::shared_ptr<udmaio::UioIf>> (m, "UioIf")
        .def(py::init<UioDeviceInfo>())
        .def("_log_name", &UioIf_PyPublishHelper::_log_name)
        .def("_rd32", &UioIf_PyPublishHelper::_rd32)
        .def("_wr32", &UioIf_PyPublishHelper::_wr32);

    py::class_<LfsrIo> lfsr_io(m, "LfsrIo");

    lfsr_io.def(py::init<boost::log::trivial::severity_level, std::shared_ptr<udmaio::UioConfigBase>>())
           .def("start", &LfsrIo::start)
           .def("stop", &LfsrIo::stop)
           .def("read", &LfsrIo::read);

    py::enum_<udmaio::DmaMode>(lfsr_io, "DmaMode")
        .value("xdma", udmaio::DmaMode::XDMA)
        .value("uio", udmaio::DmaMode::UIO)
        .export_values();

    py::enum_<boost::log::trivial::severity_level>(lfsr_io, "LogLevel")
        .value("fatal", boost::log::trivial::severity_level::fatal)
        .value("info", boost::log::trivial::severity_level::info)
        .value("debug", boost::log::trivial::severity_level::debug)
        .value("trace", boost::log::trivial::severity_level::trace)
        .export_values();
}
