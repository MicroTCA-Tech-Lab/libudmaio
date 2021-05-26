// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include "LfsrIo.h"

#include "udmaio/UDmaBuf.hpp"
#include "udmaio/FpgaMemBuffer.hpp"
#include "UioGpioStatus.hpp"
#include "ZupExampleProjectConsts.hpp"

using namespace udmaio;

LfsrIo::LfsrIo(boost::log::trivial::severity_level log_lvl, DmaMode mode, const std::string& dev_path) {
    boost::log::core::get()->set_filter(blt::severity >= log_lvl);

    if (mode == DmaMode::XDMA && dev_path.empty()) {
        throw std::runtime_error("Need dev_path in XDMA mode");
    }

    auto cfg_ptr = (mode == DmaMode::UIO)
        ? static_cast<std::unique_ptr<UioConfigBase>>(std::make_unique<UioConfigUio>())
        : static_cast<std::unique_ptr<UioConfigBase>>(std::make_unique<UioConfigXdma>(
            dev_path,
            zup_example_prj::pcie_axi4l_offset
        ));
    auto& cfg = *cfg_ptr;

    _checkDDR4Init(
        (mode == DmaMode::UIO)
        ? cfg("axi_gpio_status")
        : cfg(zup_example_prj::axi_gpio_status)
    );

    _axi_dma = std::make_unique<UioAxiDmaIf>(
        (mode == DmaMode::UIO)
        ? cfg("hier_daq_arm_axi_dma_0")
        : cfg(zup_example_prj::axi_dma_0, "events0")
    );

    _mem_sgdma = std::make_unique<UioMemSgdma>(
        (mode == DmaMode::UIO)
        ? cfg("hier_daq_arm_axi_bram_ctrl_0")
        : cfg(zup_example_prj::bram_ctrl_0)
    );

    _traffic_gen = std::make_unique<UioTrafficGen>(
        (mode == DmaMode::UIO)
        ? cfg("hier_daq_arm_axi_traffic_gen_0")
        : cfg(zup_example_prj::axi_traffic_gen_0)
    );

    _udmabuf =
        (mode == DmaMode::UIO)
        ? static_cast<std::unique_ptr<DmaBufferAbstract>>(std::make_unique<UDmaBuf>())
        : static_cast<std::unique_ptr<DmaBufferAbstract>>(std::make_unique<FpgaMemBuffer>(
            dev_path,
            zup_example_prj::fpga_mem_phys_addr
        ));
    
    _dataHandler = std::make_unique<DataHandlerSync>(
        *_axi_dma,
        *_mem_sgdma,
        *_udmabuf
    );

    std::vector<uintptr_t> dst_buf_addrs;
    for (int i = 0; i < 32; i++) {
        dst_buf_addrs.push_back(_udmabuf->get_phys_addr() + i * _mem_sgdma->BUF_LEN);
    };
    _mem_sgdma->write_cyc_mode(dst_buf_addrs);
}

void LfsrIo::start(uint32_t pkt_len, uint16_t nr_pkts, uint16_t pkt_pause) {
    uintptr_t first_desc = _mem_sgdma->get_first_desc_addr();
    _axi_dma->start(first_desc);
    _traffic_gen->start(nr_pkts, pkt_len, pkt_pause);

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

void LfsrIo::_checkDDR4Init(UioDeviceInfo dev)
{
    auto gpio_status = UioGpioStatus(dev);
    bool is_ddr4_init = gpio_status.is_ddr4_init_calib_complete();
    BOOST_LOG_TRIVIAL(debug) << "DDR4 init = " << is_ddr4_init;
    if (!is_ddr4_init) {
        throw std::runtime_error("DDR4 init calib is not complete");
    }
}
