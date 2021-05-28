//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include <csignal>
#include <iostream>
#include <stdexcept>
#include <future>

#include <boost/log/core/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/keywords/severity.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>

#include "udmaio/FpgaMemBuffer.hpp"
#include "udmaio/UDmaBuf.hpp"
#include "udmaio/UioAxiDmaIf.hpp"
#include "udmaio/UioIf.hpp"
#include "udmaio/UioMemSgdma.hpp"

#include "DataHandlerPrint.hpp"
#include "UioGpioStatus.hpp"
#include "UioTrafficGen.hpp"
#include "ZupExampleProjectConsts.hpp"

namespace blt = boost::log::trivial;
namespace bpo = boost::program_options;

using namespace udmaio;
using namespace std::chrono_literals;

volatile bool g_stop_loop = false;

void signal_handler([[maybe_unused]] int signal) { g_stop_loop = true; }

int main(int argc, char *argv[]) {
    bpo::options_description desc("AXI DMA demo");
    bool debug, trace;
    uint16_t pkt_pause;
    uint16_t nr_pkts;
    uint32_t pkt_len;
    DmaMode mode;
    std::string dev_path;

    // clang-format off
    desc.add_options()
    ("help,h", "this help")
    ("mode", bpo::value<DmaMode>(&mode)->multitoken()->required(), "select operation mode (xdma or uio) - see docs for details")
    ("debug", bpo::bool_switch(&debug), "enable verbose output (debug level)")
    ("trace", bpo::bool_switch(&trace), "enable even more verbose output (trace level)")
    ("pkt_pause", bpo::value<uint16_t>(&pkt_pause)->default_value(10), "pause between pkts - see AXI TG user's manual")
    ("nr_pkts", bpo::value<uint16_t>(&nr_pkts)->default_value(1), "number of packets to generate - see AXI TG user's manual")
    ("pkt_len", bpo::value<uint32_t>(&pkt_len)->default_value(1024), "packet length - see AXI TG user's manual")
    ("dev_path", bpo::value<std::string>(&dev_path), "Path to xdma device nodes (e.g. /dev/xdma/card0)")
    ;
    // clang-format on

    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    bpo::notify(vm);

    if (mode == DmaMode::XDMA && dev_path.empty()) {
        std::cerr << "XDMA mode needs path to device (--dev-path)" << std::endl;
        return 0;
    }

    if (trace) {
        boost::log::core::get()->set_filter(blt::severity >= blt::trace);
    } else if (debug) {
        boost::log::core::get()->set_filter(blt::severity >= blt::debug);
    } else {
        boost::log::core::get()->set_filter(blt::severity >= blt::info);
    }

    std::signal(SIGINT, signal_handler);

    auto cfg_ptr = (mode == DmaMode::UIO)
        ? static_cast<std::unique_ptr<UioConfigBase>>(std::make_unique<UioConfigUio>())
        : static_cast<std::unique_ptr<UioConfigBase>>(std::make_unique<UioConfigXdma>(
            dev_path,
            zup_example_prj::pcie_axi4l_offset
        ));
    auto& cfg = *cfg_ptr;
    
    auto gpio_status = std::make_unique<UioGpioStatus>(cfg(zup_example_prj::axi_gpio_status));

    bool is_ddr4_init = gpio_status->is_ddr4_init_calib_complete();
    BOOST_LOG_TRIVIAL(debug) << "DDR4 init = " << is_ddr4_init;
    if (!is_ddr4_init) {
        throw std::runtime_error("DDR4 init calib is not complete");
    }

    auto axi_dma = std::make_unique<UioAxiDmaIf>(cfg(zup_example_prj::axi_dma_0));
    auto mem_sgdma = std::make_unique<UioMemSgdma>(cfg(zup_example_prj::bram_ctrl_0));
    auto traffic_gen = std::make_unique<UioTrafficGen>(cfg(zup_example_prj::axi_traffic_gen_0));

    auto udmabuf = (mode == DmaMode::UIO)
        ? static_cast<std::unique_ptr<DmaBufferAbstract>>(std::make_unique<UDmaBuf>())
        : static_cast<std::unique_ptr<DmaBufferAbstract>>(std::make_unique<FpgaMemBuffer>(
            dev_path,
            zup_example_prj::fpga_mem_phys_addr
        ));

    const size_t pkt_size = pkt_len * zup_example_prj::lfsr_bytes_per_beat;

    DataHandlerPrint data_handler{
        *axi_dma,
        *mem_sgdma,
        *udmabuf,
        nr_pkts * pkt_size
    };
    auto fut = std::async(std::launch::async, std::ref(data_handler));

    mem_sgdma->init_buffers(*udmabuf, 32, pkt_size);

    uintptr_t first_desc = mem_sgdma->get_first_desc_addr();
    axi_dma->start(first_desc);
    traffic_gen->start(nr_pkts, pkt_len, pkt_pause);

    // Wait until data_handler has finished or user hit Ctrl-C
    while (fut.wait_for(10ms) != std::future_status::ready) {
        if (g_stop_loop) {
            data_handler.stop();
            fut.wait();
            break;
        }
    }

    traffic_gen->stop();

    auto [counter_ok, counter_total] = fut.get();
    std::cout << "Counters: OK = " << counter_ok << ", total = " << counter_total << "\n";

    return !(counter_ok == counter_total);
}
