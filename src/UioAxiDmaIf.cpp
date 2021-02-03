//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include <ios>

#include <boost/log/core/core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

#include "UioAxiDmaIf.hpp"

namespace blt = boost::log::trivial;

UioAxiDmaIf::UioAxiDmaIf(const std::string &uio_name, uintptr_t addr, size_t size)
    : UioIf{uio_name, addr, size, "UioAxiDmaIf"} {}

void UioAxiDmaIf::start(uint64_t start_desc) {
    BOOST_LOG_SEV(_slg, blt::severity_level::debug)
        << _log_name << ": start, start_desc = " << std::hex << start_desc << std::dec;

    // 0.
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    S2mmDmaControlReg ctrl_reg_rst = {.fields = {.Reset = 1}};
    _wr32(ADDR_S2MM_DMACR, ctrl_reg_rst.data);

    // 1.
    _wr32(ADDR_S2MM_CURDESC, start_desc & ((1ULL << 32) - 1));
    _wr32(ADDR_S2MM_CURDESC_MSB, start_desc >> 32);

    // 2.
    S2mmDmaControlReg ctrl_reg = {.data = _rd32(ADDR_S2MM_DMACR)};
    BOOST_LOG_SEV(_slg, blt::severity_level::trace)
        << _log_name << ": DMA ctrl = 0x" << std::hex << ctrl_reg.data << std::dec;
    ctrl_reg.fields.RS = 1;
    ctrl_reg.fields.Cyc_bd_en = 1;
    ctrl_reg.fields.IOC_IrqEn = 1;

    // 3.
    _wr32(ADDR_S2MM_DMACR, ctrl_reg.data);

    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << _log_name << ": DMA control write";

    // 4.
    _wr32(ADDR_S2MM_TAILDESC, 0x50);                 // for circular
    _wr32(ADDR_S2MM_TAILDESC_MSB, start_desc >> 32); // for circular

    uint32_t tmp_ctrl_after = _rd32(ADDR_S2MM_DMACR);
    BOOST_LOG_SEV(_slg, blt::severity_level::trace)
        << _log_name << ": DMA ctrl = 0x" << std::hex << tmp_ctrl_after << std::dec;
}

void UioAxiDmaIf::arm_interrupt() {
    uint32_t mask = 1;
    int rc = write(_fd, &mask, sizeof(mask));
    BOOST_LOG_SEV(_slg, blt::severity_level::trace)
        << _log_name << ": arm interrupt enable, ret code = " << rc;
}

uint32_t UioAxiDmaIf::clear_interrupt() {
    uint32_t irq_count;
    int rc = read(_fd, &irq_count, sizeof(irq_count));

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    S2mmDmaStatusReg status = {.fields = {
                                   .IOC_Irq = 1,
                               }};
    _wr32(ADDR_S2MM_DMASR, status.data);

    BOOST_LOG_SEV(_slg, blt::severity_level::trace)
        << _log_name << ": clear interrupt , ret code = " << rc << ", irq count = " << irq_count;
    return irq_count;
}

int UioAxiDmaIf::get_fd() const { return _fd; }
