//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include "udmaio/UioAxiDmaIf.hpp"

#include <ios>
#include <stdexcept>

namespace udmaio {

void UioAxiDmaIf::start(uintptr_t start_desc) {
    BOOST_LOG_SEV(_lg, bls::debug) << "start, start_desc = " << std::hex << start_desc << std::dec;

    // 0.
    s2mm_dmacr.wr({.reset = 1});

    // 1.
    s2mm_curdesc.wr({.current_descriptor_pointer = static_cast<uint32_t>(start_desc) >> 6});
    s2mm_curdesc_msb.wr(
        (sizeof(start_desc) > sizeof(uint32_t)) ? static_cast<uint32_t>(start_desc >> 32) : 0);

    // 2.
    auto ctrl_reg = s2mm_dmacr.rd();
    BOOST_LOG_SEV(_lg, bls::trace)
        << "DMA ctrl = 0x" << std::hex << reg_to_raw(ctrl_reg) << std::dec;

    ctrl_reg.rs = 1;
    ctrl_reg.cyclic_bd_enable = 0;
    ctrl_reg.ioc_irq_en = 1;
    ctrl_reg.err_irq_en = 1;

    // 3.
    s2mm_dmacr.wr(ctrl_reg);

    BOOST_LOG_SEV(_lg, bls::trace) << "DMA control write";

    // 4.
    s2mm_taildesc.wr({.tail_descriptor_pointer = 0x50 >> 6});  // for circular
    s2mm_taildesc_msb.wr(
        (sizeof(start_desc) > sizeof(uint32_t)) ? static_cast<uint32_t>(start_desc >> 32) : 0);

    BOOST_LOG_SEV(_lg, bls::trace)
        << "DMA ctrl = 0x" << std::hex << reg_to_raw(s2mm_dmacr.rd()) << std::dec;
}

std::tuple<uint32_t, axi_dma::s2mm_dmasr_t> UioAxiDmaIf::clear_interrupt() {
    uint32_t irq_count = wait_for_interrupt();

    auto stat = s2mm_dmasr.rd();
    if (stat.ioc_irq) {
        BOOST_LOG_SEV(_lg, bls::trace) << "I/O IRQ";
    }
    if (stat.err_irq) {
        BOOST_LOG_SEV(_lg, bls::warning) << "ERR IRQ";
    }
    s2mm_dmasr.wr({.ioc_irq = stat.ioc_irq, .err_irq = stat.err_irq});

    return std::make_tuple(irq_count, stat);
}

bool UioAxiDmaIf::check_for_errors() {
    bool has_errors = false;

    auto sr = s2mm_dmasr.rd();
    if (sr.halted) {
        BOOST_LOG_SEV(_lg, bls::warning) << "DMA Halted";
    }

    if (sr.idle) {
        BOOST_LOG_SEV(_lg, bls::warning) << "DMA Idle";
    }

    if (sr.dma_int_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "DMA Internal Error";
    }

    if (sr.dma_slv_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "DMA Slave Error";
    }

    if (sr.dma_dec_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "DMA Decode Error";
    }

    if (sr.sg_int_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "Scatter Gather Internal Error";
    }

    if (sr.sg_slv_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "Scatter Gather Slave Error";
    }

    if (sr.sg_dec_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "Scatter Gather Decode Error";
    }

    return has_errors;
}

}  // namespace udmaio
