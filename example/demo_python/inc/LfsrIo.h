#pragma once

#include <string>
#include <vector>
#include <cstddef>

#include <pybind11/numpy.h>

#include "udmaio/UioAxiDmaIf.hpp"
#include "udmaio/UioMemSgdma.hpp"
#include "udmaio/DmaBufferAbstract.hpp"
#include "udmaio/DataHandlerSync.hpp"

#include "DmaMode.hpp"
#include "UioTrafficGen.hpp"

namespace py = pybind11;

class LfsrIo {
    static void _checkDDR4Init(DmaMode mode, const std::string& dev_path);

    std::unique_ptr<udmaio::UioAxiDmaIf> _axi_dma;
    std::unique_ptr<udmaio::UioMemSgdma> _mem_sgdma;
    std::unique_ptr<udmaio::DmaBufferAbstract> _udmabuf;
    std::unique_ptr<udmaio::DataHandlerSync> _dataHandler;
    std::unique_ptr<UioTrafficGen> _traffic_gen;

public:
    enum class LogLevel {
        fatal = boost::log::trivial::severity_level::fatal,
        info = boost::log::trivial::severity_level::info,
        debug = boost::log::trivial::severity_level::debug,
        trace = boost::log::trivial::severity_level::trace
    };

    LfsrIo(boost::log::trivial::severity_level log_lvl, DmaMode mode, const std::string& dev_path = "");

    void start(uint32_t pkt_len, uint16_t nr_pkts, uint16_t pkt_pause);
    void stop();

    py::array_t<uint16_t> read(uint32_t ms_timeout=0);
};
