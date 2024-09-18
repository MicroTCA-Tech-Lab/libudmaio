//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include <cstdint>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "DataHandlerPython.hpp"
#include "udmaio/FpgaMemBufferOverAxi.hpp"
#include "udmaio/FpgaMemBufferOverXdma.hpp"
#include "udmaio/FrameFormat.hpp"
#include "udmaio/Logging.hpp"
#include "udmaio/UDmaBuf.hpp"
#include "udmaio/UioConfig.hpp"

namespace py = pybind11;

class UioIf_PyOverrideHelper : public udmaio::UioIf {
  public:
    using udmaio::UioIf::UioIf;
};

class UioIf_PyPublishHelper : public udmaio::UioIf {
  public:
    using udmaio::UioIf::_rd32;
    using udmaio::UioIf::_wr32;
    using udmaio::UioIf::arm_interrupt;
    using udmaio::UioIf::UioIf;
    using udmaio::UioIf::wait_for_interrupt;
};

PYBIND11_MODULE(binding, m) {
    // set default to info, with a function provided to set the level dynamically
    udmaio::Logger::init(15);
    udmaio::Logger::set_level(udmaio::bls::info);

    py::class_<udmaio::UioRegion>(m, "UioRegion")
        .def(py::init<uintptr_t, size_t>())
        .def_readwrite("addr", &udmaio::UioRegion::addr)
        .def_readwrite("size", &udmaio::UioRegion::size);

    py::class_<udmaio::HwAccessor, std::shared_ptr<udmaio::HwAccessor>>(m, "HwAccessor");

    py::class_<udmaio::UioDeviceLocation>(m, "UioDeviceLocation")
        .def(py::init<std::string, udmaio::UioRegion, std::string>(),
             py::arg("uioname"),
             py::arg("xdmaregion") = udmaio::UioRegion({0, 0}),
             py::arg("xdmaevtdev") = std::string(""))
        .def_static("set_link_axi", &udmaio::UioDeviceLocation::set_link_axi)
        .def_static("set_link_xdma", &udmaio::UioDeviceLocation::set_link_xdma)
        .def_readwrite("uio_name", &udmaio::UioDeviceLocation::uio_name)
        .def_readwrite("xdma_region", &udmaio::UioDeviceLocation::xdma_region)
        .def_readwrite("xdma_evt_dev", &udmaio::UioDeviceLocation::xdma_evt_dev)
        .def("hw_acc", &udmaio::UioDeviceLocation::hw_acc);

    py::class_<udmaio::UioConfigBase, std::shared_ptr<udmaio::UioConfigBase>>(m, "ConfigBase")
        .def("hw_acc", &udmaio::UioConfigBase::hw_acc);

    py::class_<udmaio::UioConfigUio, udmaio::UioConfigBase, std::shared_ptr<udmaio::UioConfigUio>>(
        m,
        "ConfigUio")
        .def(py::init<>());

    py::class_<udmaio::UioConfigXdma,
               udmaio::UioConfigBase,
               std::shared_ptr<udmaio::UioConfigXdma>>(m, "ConfigXdma")
        .def(py::init<std::string, uintptr_t, bool>(),
             py::arg("xdma_path"),
             py::arg("pcie_offs"),
             py::arg("x7_series_mode") = bool(false));

    py::class_<udmaio::UioIf, UioIf_PyOverrideHelper, std::shared_ptr<udmaio::UioIf>>(m, "UioIf")
        .def(py::init<std::string, udmaio::UioDeviceLocation>())
        .def("_rd32", &UioIf_PyPublishHelper::_rd32)
        .def("_wr32", &UioIf_PyPublishHelper::_wr32)
        .def("arm_interrupt", &UioIf_PyPublishHelper::arm_interrupt)
        .def("wait_for_interrupt", &UioIf_PyPublishHelper::wait_for_interrupt);

    py::class_<udmaio::DmaBufferAbstract, std::shared_ptr<udmaio::DmaBufferAbstract>>(
        m,
        "DmaBufferAbstract")
        .def("get_phys_region", &udmaio::DmaBufferAbstract::get_phys_region);

    py::class_<udmaio::FpgaMemBufferOverAxi,
               udmaio::DmaBufferAbstract,
               udmaio::UioIf,
               std::shared_ptr<udmaio::FpgaMemBufferOverAxi>>(m, "FpgaMemBufferOverAxi")
        .def(py::init<udmaio::UioDeviceLocation>());

    py::class_<udmaio::FpgaMemBufferOverXdma,
               udmaio::DmaBufferAbstract,
               std::shared_ptr<udmaio::FpgaMemBufferOverXdma>>(m, "FpgaMemBufferOverXdma")
        .def(py::init<const std::string&, uintptr_t>());

    py::class_<udmaio::UDmaBuf, udmaio::DmaBufferAbstract, std::shared_ptr<udmaio::UDmaBuf>>(
        m,
        "UDmaBuf")
        .def(py::init<int>(), py::arg("buf_idx") = 0);

    py::class_<udmaio::UioAxiDmaIf, udmaio::UioIf, std::shared_ptr<udmaio::UioAxiDmaIf>>(
        m,
        "UioAxiDmaIf")
        .def(py::init<udmaio::UioDeviceLocation>())
        .def("dump_status", &udmaio::UioAxiDmaIf::dump_status);

    py::class_<udmaio::UioMemSgdma, udmaio::UioIf, std::shared_ptr<udmaio::UioMemSgdma>>(
        m,
        "UioMemSgdma")
        .def(py::init<udmaio::UioDeviceLocation>())
        .def("print_descs", &udmaio::UioMemSgdma::print_descs);

    py::class_<udmaio::DataHandlerPython> data_handler(m, "DataHandler");

    data_handler
        .def(py::init<std::shared_ptr<udmaio::UioAxiDmaIf>,
                      std::shared_ptr<udmaio::UioMemSgdma>,
                      std::shared_ptr<udmaio::DmaBufferAbstract>,
                      bool,
                      size_t>(),
             py::arg("dma"),
             py::arg("desc"),
             py::arg("mem"),
             py::arg("receive_packets") = bool(true),
             py::arg("queue_size") = size_t(64))
        .def("start",
             &udmaio::DataHandlerPython::start,
             py::arg("nr_pkts"),
             py::arg("pkt_size"),
             py::arg("init_only") = false)
        .def("stop", &udmaio::DataHandlerPython::stop)
        .def("read", &udmaio::DataHandlerPython::numpy_read, py::arg("ms_timeout"))
        .def("read_nb", &udmaio::DataHandlerPython::numpy_read_nb);

    py::enum_<boost::log::trivial::severity_level>(m, "LogLevel")
        .value("FATAL", boost::log::trivial::severity_level::fatal)
        .value("INFO", boost::log::trivial::severity_level::info)
        .value("DEBUG", boost::log::trivial::severity_level::debug)
        .value("TRACE", boost::log::trivial::severity_level::trace)
        .export_values();

    m.def("set_logging_level",
          [](boost::log::trivial::severity_level level) { udmaio::Logger::set_level(level); });

    py::class_<udmaio::FrameFormat> frame_format(m, "FrameFormat");

    py::enum_<udmaio::FrameFormat::PixelFormat>(frame_format, "PixelFormat")
        .value("unknown", udmaio::FrameFormat::PixelFormat::unknown)
        .value("Mono8", udmaio::FrameFormat::PixelFormat::Mono8)
        .value("Mono10", udmaio::FrameFormat::PixelFormat::Mono10)
        .value("Mono12", udmaio::FrameFormat::PixelFormat::Mono12)
        .value("Mono14", udmaio::FrameFormat::PixelFormat::Mono14)
        .value("Mono16", udmaio::FrameFormat::PixelFormat::Mono16)
        .value("BayerGR8", udmaio::FrameFormat::PixelFormat::BayerGR8)
        .value("BayerRG8", udmaio::FrameFormat::PixelFormat::BayerRG8)
        .value("BayerGB8", udmaio::FrameFormat::PixelFormat::BayerGB8)
        .value("BayerBG8", udmaio::FrameFormat::PixelFormat::BayerBG8)
        .value("BayerGR10", udmaio::FrameFormat::PixelFormat::BayerGR10)
        .value("BayerRG10", udmaio::FrameFormat::PixelFormat::BayerRG10)
        .value("BayerGB10", udmaio::FrameFormat::PixelFormat::BayerGB10)
        .value("BayerBG10", udmaio::FrameFormat::PixelFormat::BayerBG10)
        .value("BayerGR12", udmaio::FrameFormat::PixelFormat::BayerGR12)
        .value("BayerRG12", udmaio::FrameFormat::PixelFormat::BayerRG12)
        .value("BayerGB12", udmaio::FrameFormat::PixelFormat::BayerGB12)
        .value("BayerBG12", udmaio::FrameFormat::PixelFormat::BayerBG12)
        .value("RGB8", udmaio::FrameFormat::PixelFormat::RGB8)
        .value("BGR8", udmaio::FrameFormat::PixelFormat::BGR8)
        .value("RGBa8", udmaio::FrameFormat::PixelFormat::RGBa8)
        .value("BGRa8", udmaio::FrameFormat::PixelFormat::BGRa8)
        .value("RGB10V1Packed1", udmaio::FrameFormat::PixelFormat::RGB10V1Packed1)
        .value("RGB10p32", udmaio::FrameFormat::PixelFormat::RGB10p32)
        .value("RGB565p", udmaio::FrameFormat::PixelFormat::RGB565p)
        .value("BGR565p", udmaio::FrameFormat::PixelFormat::BGR565p)
        .value("YUV422_8_UYVY", udmaio::FrameFormat::PixelFormat::YUV422_8_UYVY)
        .value("YUV422_8", udmaio::FrameFormat::PixelFormat::YUV422_8)
        .value("YUV8_UYV", udmaio::FrameFormat::PixelFormat::YUV8_UYV)
        .value("YCbCr8_CbYCr", udmaio::FrameFormat::PixelFormat::YCbCr8_CbYCr)
        .value("YCbCr422_8", udmaio::FrameFormat::PixelFormat::YCbCr422_8)
        .value("YCbCr422_8_CbYCrY", udmaio::FrameFormat::PixelFormat::YCbCr422_8_CbYCrY)
        .value("YCbCr601_8_CbYCr", udmaio::FrameFormat::PixelFormat::YCbCr601_8_CbYCr)
        .value("YCbCr601_422_8", udmaio::FrameFormat::PixelFormat::YCbCr601_422_8)
        .value("YCbCr601_422_8_CbYCrY", udmaio::FrameFormat::PixelFormat::YCbCr601_422_8_CbYCrY)
        .value("YCbCr709_8_CbYCr", udmaio::FrameFormat::PixelFormat::YCbCr709_8_CbYCr)
        .value("YCbCr709_422_8", udmaio::FrameFormat::PixelFormat::YCbCr709_422_8)
        .value("YCbCr709_422_8_CbYCrY", udmaio::FrameFormat::PixelFormat::YCbCr709_422_8_CbYCrY)
        .value("RGB8_Planar", udmaio::FrameFormat::PixelFormat::RGB8_Planar)
        .export_values();

    py::class_<udmaio::FrameFormat::dim_t>(frame_format, "Dim")
        .def(py::init<uint16_t, uint16_t>())
        .def_readwrite("width", &udmaio::FrameFormat::dim_t::width)
        .def_readwrite("height", &udmaio::FrameFormat::dim_t::height);

    frame_format.def(py::init<>())
        .def("set_format",
             static_cast<void (
                 udmaio::FrameFormat::*)(udmaio::FrameFormat::dim_t, uint16_t, uint8_t)>(
                 &udmaio::FrameFormat::set_format),
             py::arg("dim"),
             py::arg("bytes_per_pixel"),
             py::arg("word_width") = 4)
        .def("set_format",
             static_cast<void (
                 udmaio::FrameFormat::*)(udmaio::FrameFormat::dim_t, std::string, uint8_t)>(
                 &udmaio::FrameFormat::set_format),
             py::arg("dim"),
             py::arg("pix_fmt_str"),
             py::arg("word_width") = 4)
        .def("set_format",
             static_cast<void (udmaio::FrameFormat::*)(udmaio::FrameFormat::dim_t,
                                                       udmaio::FrameFormat::PixelFormat,
                                                       uint8_t)>(&udmaio::FrameFormat::set_format),
             py::arg("dim"),
             py::arg("pix_fmt"),
             py::arg("word_width") = 4)
        .def("set_dim", &udmaio::FrameFormat::set_dim)
        .def("set_bpp", &udmaio::FrameFormat::set_bpp)
        .def("set_pix_fmt", &udmaio::FrameFormat::set_pix_fmt)
        .def("set_word_width", &udmaio::FrameFormat::set_word_width)
        .def("get_dim", &udmaio::FrameFormat::get_dim)
        .def("get_pixel_format", &udmaio::FrameFormat::get_pixel_format)
        .def("get_pixel_format_str", &udmaio::FrameFormat::get_pixel_format_str)
        .def("get_bytes_per_pixel", &udmaio::FrameFormat::get_bytes_per_pixel)
        .def("get_word_width", &udmaio::FrameFormat::get_word_width)
        .def("get_pixel_per_word", &udmaio::FrameFormat::get_pixel_per_word)
        .def("get_hsize", &udmaio::FrameFormat::get_hsize)
        .def("get_frm_size", &udmaio::FrameFormat::get_frm_size);
}
