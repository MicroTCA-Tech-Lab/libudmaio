//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2023 Deutsches Elektronen-Synchrotron DESY

#include "udmaio/UioAxiVdmaIf.hpp"

#include <boost/log/core/core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

#include "udmaio/Logging.hpp"

namespace blt = boost::log::trivial;

namespace udmaio {

////////////////////////////////////////
// Helper functions
////////////////////////////////////////

void UioAxiVdmaIf::init_buffers(const std::vector<UioRegion>& frm_bufs) {
    for (size_t i = 0; i < frm_bufs.size(); i++) {
        BOOST_LOG_SEV(_lg, bls::trace)
            << "frame buf (" << i << ") addr = 0x" << std::hex << frm_bufs[i].addr << std::dec;

        if (_long_addrs) {
            s2mm_sa[i * 2].wr(frm_bufs[i].addr >> 32);
            s2mm_sa[i * 2 + 1].wr(frm_bufs[i].addr & 0xffffffff);
        } else {
            s2mm_sa[i].wr(frm_bufs[i].addr);
        }
    }
}

void UioAxiVdmaIf::set_frame_format(FrameFormat frm_format) {
    // Write stride (needed?)
    s2mm_frmdly_stride.wr({.stride = frm_format.get_hsize()});

    // Write horizontal size
    s2mm_hsize.wr({.horizontal_size = frm_format.get_hsize()});

    // Write vertical size
    s2mm_vsize.wr({.vertical_size = frm_format.get_dim().height});
}

////////////////////////////////////////
// Base config
////////////////////////////////////////

void UioAxiVdmaIf::config(const std::vector<UioRegion>& frm_bufs) {
    // 1. Write to VDMA control register, except run bit
    auto cr = s2mm_vdmacr.rd();
    cr.circular_park = 1;
    cr.repeat_en = 1;
    s2mm_vdmacr.wr(cr);

    // 2. Write to VDMA frame buffer start address register
    init_buffers(frm_bufs);
}

void UioAxiVdmaIf::config(const std::vector<UioRegion>& frm_bufs, FrameFormat frm_format) {
    // check frame buffer sizes
    for (auto frm_buf : frm_bufs) {
        if (frm_buf.size < frm_format.get_frm_size()) {
            BOOST_LOG_SEV(_lg, bls::error)
                << "frame buf size insufficient (got " << frm_buf.size << ", need "
                << frm_format.get_frm_size() << ")" << std::dec;
        }
    }

    config(frm_bufs);

    set_frame_format(frm_format);
}

void UioAxiVdmaIf::reset() {
    // Reset all errors in s2mm channel
    s2mm_vdmasr.wr({.vdma_int_err = 1,
                    .vdma_slv_err = 1,
                    .vdma_dec_err = 1,
                    .sof_early_err = 1,
                    .eol_early_err = 1,
                    .sof_late_err = 1,
                    .eol_late_err = 1});

    // Soft reset s2mm channel
    s2mm_vdmacr.wr({.reset = 1});
}

void UioAxiVdmaIf::print_regs() {
    BOOST_LOG_SEV(_lg, bls::debug)
        << "Park Pointer Register = 0x" << std::hex << reg_to_raw(park_ptr_reg.rd()) << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug) << "AXI VDMA Version Register = 0x" << std::hex
                                   << reg_to_raw(vdma_version.rd()) << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug) << "S2MM VDMA Control Register = 0x" << std::hex
                                   << reg_to_raw(s2mm_vdmacr.rd()) << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug)
        << "S2MM VDMA Status Register = 0x" << std::hex << reg_to_raw(s2mm_vdmasr.rd()) << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug) << "S2MM Error Interrupt Mask Register = 0x" << std::hex
                                   << reg_to_raw(s2mm_vdma_irq_mask.rd()) << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug)
        << "S2MM Register Index = 0x" << std::hex << reg_to_raw(s2mm_reg_index.rd()) << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug)
        << "S2MM Vertical Size = 0x" << std::hex << reg_to_raw(s2mm_vsize.rd()) << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug)
        << "S2MM Horizontal Size = 0x" << std::hex << reg_to_raw(s2mm_hsize.rd()) << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug) << "S2MM Frame Delay and Stride = 0x" << std::hex
                                   << reg_to_raw(s2mm_frmdly_stride.rd()) << std::dec;

    for (size_t i = 0; i < s2mm_sa.size(); i++)
        BOOST_LOG_SEV(_lg, bls::debug) << "S2MM Start Address Register " << i << " = 0x" << std::hex
                                       << reg_to_raw(s2mm_sa[i].rd()) << std::dec;
}

uint32_t UioAxiVdmaIf::get_cur_frmbuf() {
    return park_ptr_reg.rd().wr_frm_store;
}

void UioAxiVdmaIf::print_cur_frmbuf() {
    BOOST_LOG_SEV(_lg, bls::debug) << "Current frame buffer = " << get_cur_frmbuf();
}

////////////////////////////////////////
// Operational functions
////////////////////////////////////////

void UioAxiVdmaIf::start() {
    // Set run bit of s2mm channel to start operation
    auto cr = s2mm_vdmacr.rd();
    cr.rs = 1;
    s2mm_vdmacr.wr(cr);
}

void UioAxiVdmaIf::start(FrameFormat frm_format) {
    set_frame_format(frm_format);

    start();
}

void UioAxiVdmaIf::stop(bool wait_for_end = false) {
    // Reset run bit of s2mm channel to start operation
    auto cr = s2mm_vdmacr.rd();
    cr.rs = 0;
    s2mm_vdmacr.wr(cr);

    if (wait_for_end) {
        while (!s2mm_vdmasr.rd().halted)
            ;
    }
}

bool UioAxiVdmaIf::isrunning() {
    return !s2mm_vdmasr.rd().halted;
}

bool UioAxiVdmaIf::check_for_errors() {
    bool has_errors = false;

    auto sr = s2mm_vdmasr.rd();

    if (sr.vdma_int_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "VDMA Internal Error, reset required!";
    }

    if (sr.vdma_slv_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "VDMA Slave Error";
    }

    if (sr.vdma_dec_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "VDMA Decode Error";
    }

    if (sr.sof_early_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "Start of Frame Early Error";
    }

    if (sr.eol_early_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "End of Line Early Error";
    }

    if (sr.sof_late_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "Start of Frame Late Error";
    }

    if (sr.eol_late_err) {
        has_errors = true;
        BOOST_LOG_SEV(_lg, bls::fatal) << "End of Line Late Error";
    }

    return has_errors;
}

}  // namespace udmaio
