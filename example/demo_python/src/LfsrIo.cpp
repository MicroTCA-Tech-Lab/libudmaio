// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include "LfsrIo.h"

#include "udmaio/UDmaBuf.hpp"
#include "udmaio/FpgaMemBuffer.hpp"
#include "UioGpioStatus.hpp"
#include "ZupExampleProjectConsts.hpp"

using namespace udmaio;

LfsrIo::LfsrIo(boost::log::trivial::severity_level log_lvl, std::shared_ptr<udmaio::UioConfigBase> cfg_ptr) {
    boost::log::core::get()->set_filter(blt::severity >= log_lvl);

    udmaio::UioConfigBase& cfg = *cfg_ptr;

    _axi_dma = std::make_unique<UioAxiDmaIf>(cfg(zup_example_prj::axi_dma_0));
    _mem_sgdma = std::make_unique<UioMemSgdma>(cfg(zup_example_prj::bram_ctrl_0));
    _udmabuf =
        (cfg.mode() == DmaMode::UIO)
        ? static_cast<std::unique_ptr<DmaBufferAbstract>>(std::make_unique<UDmaBuf>())
        : static_cast<std::unique_ptr<DmaBufferAbstract>>(std::make_unique<FpgaMemBuffer>(
            cfg.dev_path(),
            zup_example_prj::fpga_mem_phys_addr
        ));
    
    _dataHandler = std::make_unique<DataHandlerSync>(
        *_axi_dma,
        *_mem_sgdma,
        *_udmabuf
    );
}

void LfsrIo::start(size_t pkt_size) {
    _mem_sgdma->init_buffers(*_udmabuf, 32, pkt_size);

    uintptr_t first_desc = _mem_sgdma->get_first_desc_addr();
    _axi_dma->start(first_desc);

    (*_dataHandler)();
}

void LfsrIo::stop() {
    _dataHandler->stop();
}

py::array_t<uint16_t> LfsrIo::read(uint32_t ms_timeout) {
    // Create vector on the heap, holding the data
    auto vec = new std::vector<uint8_t>(
        _dataHandler->read(std::chrono::milliseconds{ms_timeout})
    );
    // Callback for Python garbage collector
    py::capsule gc_callback(vec, [](void *f) {
        auto ptr = reinterpret_cast<std::vector<uint8_t> *>(f);
        delete ptr;
    });
    // Return Numpy array, transferring ownership to Python
    return py::array_t<uint16_t>(
        {vec->size() / sizeof(uint16_t)}, // shape
        {sizeof(uint16_t)},               // stride
        reinterpret_cast<uint16_t*>(vec->data()), // data pointer
        gc_callback
    );
}
