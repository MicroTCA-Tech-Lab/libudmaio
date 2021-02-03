//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include <iostream>
#include <thread>

#include <boost/log/core/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/keywords/severity.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>

#include "UDmaBuf.hpp"
#include "UioAxiDmaIf.hpp"
#include "UioIf.hpp"
#include "UioIfFactory.hpp"
#include "UioMemSgdma.hpp"
#include "UioTrafficGen.hpp"

#include "DataHandlerPrint.hpp"

namespace blt = boost::log::trivial;
namespace bpo = boost::program_options;

int main(int argc, char *argv[]) {
    bpo::options_description desc("AXI DMA demo");
    bool debug, trace;
    uint16_t pkt_pause;
    uint16_t nr_pkts;
    uint32_t pkt_len;

    // clang-format off
    desc.add_options()
	("help,h", "this help")
	("debug", bpo::bool_switch(&debug), "enable verbose output (debug level)")
	("trace", bpo::bool_switch(&trace), "enable even more verbose output (trace level)")
	("pkt_pause", bpo::value<uint16_t>(&pkt_pause)->default_value(10), "pause between pkts - see AXI TG user's manual")
	("nr_pkts", bpo::value<uint16_t>(&nr_pkts)->default_value(1), "number of packets to generate - see AXI TG user's manual")
	("pkt_len", bpo::value<uint32_t>(&pkt_len)->default_value(1024), "packet length - see AXI TG user's manual")
    ;
    // clang-format on

    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
    bpo::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    if (trace) {
        boost::log::core::get()->set_filter(blt::severity >= blt::trace);
    } else if (debug) {
        boost::log::core::get()->set_filter(blt::severity >= blt::debug);
    } else {
        boost::log::core::get()->set_filter(blt::severity >= blt::info);
    }

    auto axi_dma = UioIfFactory::create<UioAxiDmaIf>("axi_dma_0");
    auto mem_sgdma = UioIfFactory::create<UioMemSgdma>("axi_bram_ctrl_0");
    auto traffic_gen = UioIfFactory::create<UioTrafficGen>("axi_traffic_gen_0");
    UDmaBuf udmabuf;
    uint64_t counter_ok = 0, counter_total = 0;
    DataHandlerPrint data_handler{*axi_dma, *mem_sgdma, udmabuf, counter_ok, counter_total};
    std::thread t1{data_handler};

    std::vector<uint64_t> dst_buf_addrs;
    for (int i = 0; i < 32; i++) {
        dst_buf_addrs.push_back(udmabuf.get_phys_addr() + i * mem_sgdma->BUF_LEN);
    };

    mem_sgdma->write_cyc_mode(dst_buf_addrs);

    uint64_t first_desc = mem_sgdma->get_first_desc_addr();
    axi_dma->start(first_desc);
    traffic_gen->start(nr_pkts, pkt_len, pkt_pause);

    // 100 = clk freq, 2 = "safety factor"
    unsigned int clk_cycles = (pkt_len + pkt_pause) * nr_pkts;
    std::this_thread::sleep_for(std::chrono::microseconds(clk_cycles / 100 * 2));

    data_handler.stop();
    t1.join();

    std::cout << "Counters: OK = " << counter_ok << ", total = " << counter_total << "\n";

    return !(counter_ok == counter_total);
}
