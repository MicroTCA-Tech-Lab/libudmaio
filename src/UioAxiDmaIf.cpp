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

#include <boost/log/core/core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

namespace blt = boost::log::trivial;

namespace udmaio {

const std::string_view UioAxiDmaIf::_log_name() const {
    return "UioAxiDmaIf";
}

void UioAxiDmaIf::start(uintptr_t start_desc) {
    BOOST_LOG_SEV(_slg, blt::severity_level::debug)
        << _log_name() << ": start, start_desc = " << std::hex << start_desc << std::dec;

    // 0.
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    s2mm_dmacr.wr({.reset = 1});

    // 1.
    s2mm_curdesc.wr({.current_descriptor_pointer = static_cast<uint32_t>(start_desc) >> 6});
    s2mm_curdesc_msb.wr(
        (sizeof(start_desc) > sizeof(uint32_t)) ? static_cast<uint32_t>(start_desc >> 32) : 0);

    // 2.
    auto ctrl_reg = s2mm_dmacr.rd();
    reg_cast_t<axi_dma::s2mm_dmacr_t> tmp{.data = ctrl_reg};
    BOOST_LOG_SEV(_slg, blt::severity_level::trace)
        << _log_name() << ": DMA ctrl = 0x" << std::hex << tmp.raw << std::dec;

    ctrl_reg.rs = 1;
    ctrl_reg.cyclic_bd_enable = 1;
    ctrl_reg.ioc_irq_en = 1;

    // 3.
    s2mm_dmacr.wr(ctrl_reg);

    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << _log_name() << ": DMA control write";

    // 4.
    s2mm_taildesc.wr({.tail_descriptor_pointer = 0x50 >> 6});  // for circular
    s2mm_taildesc_msb.wr(
        (sizeof(start_desc) > sizeof(uint32_t)) ? static_cast<uint32_t>(start_desc >> 32) : 0);

    tmp.data = s2mm_dmacr.rd();
    BOOST_LOG_SEV(_slg, blt::severity_level::trace)
        << _log_name() << ": DMA ctrl = 0x" << std::hex << tmp.raw << std::dec;
}

uint32_t UioAxiDmaIf::clear_interrupt() {
    uint32_t irq_count = wait_for_interrupt();

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    s2mm_dmasr.wr({.ioc_irq = 1});

    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << _log_name() << ": clear interrupt";
    return irq_count;
}

int UioAxiDmaIf::get_fd_int() const {
    return _fd_int;
}

bool UioAxiDmaIf::check_for_errors() {
    bool has_errors = false;

    auto sr = s2mm_dmasr.rd();
    if (sr.dma_int_err) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal) << _log_name() << ": DMA Internal Error";
    }

    if (sr.dma_slv_err) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal) << _log_name() << ": DMA Slave Error";
    }

    if (sr.dma_dec_err) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal) << _log_name() << ": DMA Decode Error";
    }

    if (sr.sg_int_err) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal)
            << _log_name() << ": Scatter Gather Internal Error";
    }

    if (sr.sg_slv_err) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal)
            << _log_name() << ": Scatter Gather Slave Error";
    }

    if (sr.sg_dec_err) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal)
            << _log_name() << ": Scatter Gather Decode Error";
    }

    return has_errors;
}

}  // namespace udmaio
