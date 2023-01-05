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
    _wr_reg<S2mmDmaControlReg>(ADDR_S2MM_DMACR, {.Reset = 1});

    // 1.
    _wr32(ADDR_S2MM_CURDESC, start_desc & ((1ULL << 32) - 1));
    _wr32(ADDR_S2MM_CURDESC_MSB, start_desc >> 32);

    // 2.
    BOOST_LOG_SEV(_slg, blt::severity_level::trace)
        << _log_name() << ": DMA ctrl = 0x" << std::hex << _rd32(ADDR_S2MM_DMACR) << std::dec;

    auto ctrl_reg = _rd_reg<S2mmDmaControlReg>(ADDR_S2MM_DMACR);
    ctrl_reg.RS = 1;
    ctrl_reg.Cyc_bd_en = 1;
    ctrl_reg.IOC_IrqEn = 1;

    // 3.
    _wr_reg<S2mmDmaControlReg>(ADDR_S2MM_DMACR, ctrl_reg);

    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << _log_name() << ": DMA control write";

    // 4.
    _wr32(ADDR_S2MM_TAILDESC, 0x50);                  // for circular
    _wr32(ADDR_S2MM_TAILDESC_MSB, start_desc >> 32);  // for circular

    uint32_t tmp_ctrl_after = _rd32(ADDR_S2MM_DMACR);
    BOOST_LOG_SEV(_slg, blt::severity_level::trace)
        << _log_name() << ": DMA ctrl = 0x" << std::hex << tmp_ctrl_after << std::dec;
}

uint32_t UioAxiDmaIf::clear_interrupt() {
    uint32_t irq_count = wait_for_interrupt();

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    _wr_reg<S2mmDmaStatusReg>(ADDR_S2MM_DMASR, {.IOC_Irq = 1});

    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << _log_name() << ": clear interrupt";
    return irq_count;
}

int UioAxiDmaIf::get_fd_int() const {
    return _fd_int;
}

bool UioAxiDmaIf::check_for_errors() {
    bool has_errors = false;

    auto sr = _rd_reg<S2mmDmaStatusReg>(ADDR_S2MM_DMASR);
    if (sr.DMAIntErr) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal) << _log_name() << ": DMA Internal Error";
    }

    if (sr.DMASlvErr) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal) << _log_name() << ": DMA Slave Error";
    }

    if (sr.DMADecErr) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal) << _log_name() << ": DMA Decode Error";
    }

    if (sr.SGIntErr) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal)
            << _log_name() << ": Scatter Gather Internal Error";
    }

    if (sr.SGSlvErr) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal)
            << _log_name() << ": Scatter Gather Slave Error";
    }

    if (sr.SGDecErr) {
        has_errors = true;
        BOOST_LOG_SEV(_slg, blt::severity_level::fatal)
            << _log_name() << ": Scatter Gather Decode Error";
    }

    return has_errors;
}

}  // namespace udmaio
