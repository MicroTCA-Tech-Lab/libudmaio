#pragma once

#include <string>
#include <vector>
#include <cstddef>

#include <pybind11/numpy.h>

#include "udmaio/UioAxiDmaIf.hpp"
#include "udmaio/UioConfig.hpp"
#include "udmaio/UioMemSgdma.hpp"
#include "udmaio/DmaBufferAbstract.hpp"
#include "udmaio/DataHandlerSync.hpp"

namespace py = pybind11;

class LfsrIo {
    std::unique_ptr<udmaio::UioAxiDmaIf> _axi_dma;
    std::unique_ptr<udmaio::UioMemSgdma> _mem_sgdma;
    std::unique_ptr<udmaio::DmaBufferAbstract> _udmabuf;
    std::unique_ptr<udmaio::DataHandlerSync> _dataHandler;

public:
    enum class LogLevel {
        fatal = boost::log::trivial::severity_level::fatal,
        info = boost::log::trivial::severity_level::info,
        debug = boost::log::trivial::severity_level::debug,
        trace = boost::log::trivial::severity_level::trace
    };

    LfsrIo(boost::log::trivial::severity_level log_lvl, std::shared_ptr<udmaio::UioConfigBase> cfg_ptr);

    void start(size_t pkt_size);
    void stop();

    py::array_t<uint16_t> read(uint32_t ms_timeout=0);
};
