#pragma once

#include <string>
#include <vector>
#include <cstddef>

#include "udmaio/UioAxiDmaIf.hpp"
#include "udmaio/UioMemSgdma.hpp"
#include "udmaio/DmaBufferAbstract.hpp"
#include "udmaio/DataHandlerSync.hpp"

#include "DmaMode.hpp"
#include "UioTrafficGen.hpp"

class LfsrIo {
    enum class DebugLevel {
        none = 0,
        info = 1,
        debug = 2,
        trace = 3
    };

    static void _checkDDR4Init(const std::string& dev_path, DmaMode mode);

    std::unique_ptr<udmaio::UioAxiDmaIf> _axi_dma;
    std::unique_ptr<udmaio::UioMemSgdma> _mem_sgdma;
    std::unique_ptr<udmaio::DmaBufferAbstract> _udmabuf;
    std::unique_ptr<udmaio::DataHandlerSync> _dataHandler;
    std::unique_ptr<UioTrafficGen> _traffic_gen;

public:
    LfsrIo(const std::string& dev_path);

    void start(uint32_t pkt_len, uint16_t nr_pkts, uint16_t pkt_pause);
    void stop();

    std::vector<uint8_t> read(uint32_t ms_timeout=0);
};
