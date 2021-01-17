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

#include "UDmaBuf.hpp"
#include "UioAxiDmaIf.hpp"
#include "UioIf.hpp"
#include "UioIfFactory.hpp"
#include "UioMemSgdma.hpp"
#include "UioTrafficGen.hpp"

namespace blt = boost::log::trivial;

int main() {
    boost::log::core::get()->set_filter(blt::severity >= blt::trace);

    auto axi_dma = UioIfFactory::create<UioAxiDmaIf>("axi_dma_0");
    auto mem_sgdma = UioIfFactory::create<UioMemSgdma>("axi_bram_ctrl_0");
    auto traffic_gen = UioIfFactory::create<UioTrafficGen>("axi_traffic_gen_0");
    UDmaBuf udmabuf;

    std::vector<uint64_t> dst_buf_addrs;
    for (int i = 0; i < 4; i++) {
        dst_buf_addrs.push_back(udmabuf.get_phys_addr() + i * mem_sgdma->BUF_LEN);
    };

    mem_sgdma->write_cyc_mode(dst_buf_addrs);
    mem_sgdma->print_descs(dst_buf_addrs.size());

    traffic_gen->print_version();
    traffic_gen->start();

    uint64_t first_desc = mem_sgdma->get_first_desc_addr();
    axi_dma->start(first_desc);

    for (int i = 0; i < 3; i++) {
        std::cout << "===============================================\n";
        mem_sgdma->print_descs(dst_buf_addrs.size());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
