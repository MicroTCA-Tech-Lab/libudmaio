// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#pragma once

#include <memory>
#include <pybind11/numpy.h>

#include "udmaio/DataHandlerSync.hpp"

namespace py = pybind11;

namespace udmaio {

class DataHandlerPython : public DataHandlerSync {
    std::shared_ptr<UioAxiDmaIf> _dma_ptr;
    std::shared_ptr<UioMemSgdma> _desc_ptr;
    std::shared_ptr<DmaBufferAbstract> _mem_ptr;

public:
    DataHandlerPython(std::shared_ptr<UioAxiDmaIf>, std::shared_ptr<UioMemSgdma>, std::shared_ptr<DmaBufferAbstract>);

    void start(size_t pkt_size);
    py::array_t<uint16_t> numpy_read(uint32_t ms_timeout=0);

};

} // namespace udmaio
