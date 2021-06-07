//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "udmaio/UioConfig.hpp"
#include "udmaio/FpgaMemBufferOverAxi.hpp"
#include "udmaio/FpgaMemBufferOverXdma.hpp"
#include "udmaio/UDmaBuf.hpp"

#include "DataHandlerPython.hpp"

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

PYBIND11_MODULE(binding, m) {
    // set default to info, with a function provided to set the level dynamically
    boost::log::core::get()->set_filter(blt::severity >= blt::info);

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
        .def(py::init<std::string, udmaio::UioRegion, std::string>(),
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
             static_cast<udmaio::UioDeviceInfo (udmaio::UioConfigUio::*)(udmaio::UioDeviceLocation)>(&udmaio::UioConfigUio::operator())
        )
        .def("__call__",
             static_cast<udmaio::UioDeviceInfo (udmaio::UioConfigUio::*)(std::string)>(&udmaio::UioConfigUio::operator())
        );

    py::class_<udmaio::UioConfigXdma, udmaio::UioConfigBase, std::shared_ptr<udmaio::UioConfigXdma>>(m, "ConfigXdma")
        .def(py::init<std::string, uintptr_t>())
        // We have to expose overloaded functions explicitly
        .def("__call__",
             static_cast<udmaio::UioDeviceInfo (udmaio::UioConfigXdma::*)(udmaio::UioDeviceLocation)>(&udmaio::UioConfigXdma::operator())
        )
        .def("__call__",
             static_cast<udmaio::UioDeviceInfo (udmaio::UioConfigXdma::*)(udmaio::UioRegion, std::string)>(&udmaio::UioConfigXdma::operator()),
             // We have to expose default arguments explicitly
             py::arg("dev_region"),
             py::arg("evt_dev") = ""
        );

    py::class_<udmaio::UioIf, UioIf_PyOverrideHelper, std::shared_ptr<udmaio::UioIf>> (m, "UioIf")
        .def(py::init<udmaio::UioDeviceInfo>())
        .def("_log_name", &UioIf_PyPublishHelper::_log_name)
        .def("_rd32", &UioIf_PyPublishHelper::_rd32)
        .def("_wr32", &UioIf_PyPublishHelper::_wr32);

    py::class_<udmaio::DmaBufferAbstract, std::shared_ptr<udmaio::DmaBufferAbstract>>(m, "DmaBufferAbstract");

    py::class_<udmaio::FpgaMemBufferOverAxi, udmaio::DmaBufferAbstract, udmaio::UioIf, std::shared_ptr<udmaio::FpgaMemBufferOverAxi>>(m, "FpgaMemBufferOverAxi")
        .def(py::init<udmaio::UioDeviceInfo>());

    py::class_<udmaio::FpgaMemBufferOverXdma, udmaio::DmaBufferAbstract, std::shared_ptr<udmaio::FpgaMemBufferOverXdma>>(m, "FpgaMemBufferOverXdma")
        .def(py::init<const std::string &, uintptr_t>());

    py::class_<udmaio::UDmaBuf, udmaio::DmaBufferAbstract, std::shared_ptr<udmaio::UDmaBuf>>(m, "UDmaBuf")
        .def(py::init<int>(), py::arg("buf_idx") = 0);

    py::class_<udmaio::UioAxiDmaIf, udmaio::UioIf, std::shared_ptr<udmaio::UioAxiDmaIf>>(m, "UioAxiDmaIf")
         .def(py::init<udmaio::UioDeviceInfo>());

    py::class_<udmaio::UioMemSgdma, udmaio::UioIf, std::shared_ptr<udmaio::UioMemSgdma>>(m, "UioMemSgdma")
         .def(py::init<udmaio::UioDeviceInfo>());

    py::class_<udmaio::DataHandlerPython> data_handler(m, "DataHandler");

    data_handler.def(py::init<std::shared_ptr<udmaio::UioAxiDmaIf>,
                              std::shared_ptr<udmaio::UioMemSgdma>,
                              std::shared_ptr<udmaio::DmaBufferAbstract>>())
                .def("start", &udmaio::DataHandlerPython::start)
                .def("stop", &udmaio::DataHandlerPython::stop)
                .def("read", &udmaio::DataHandlerPython::numpy_read);

    py::enum_<boost::log::trivial::severity_level>(m, "LogLevel")
        .value("FATAL", boost::log::trivial::severity_level::fatal)
        .value("INFO", boost::log::trivial::severity_level::info)
        .value("DEBUG", boost::log::trivial::severity_level::debug)
        .value("TRACE", boost::log::trivial::severity_level::trace)
        .export_values();

    m.def("set_logging_level", [](boost::log::trivial::severity_level level) {
        boost::log::core::get()->set_filter(blt::severity >= level);
    });

}
