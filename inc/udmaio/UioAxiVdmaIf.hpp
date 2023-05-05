//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2023 Deutsches Elektronen-Synchrotron DESY

#pragma once

#include <udmaio/RegAccessor.hpp>
#include <udmaio/UioIf.hpp>

#include "udmaio/FrameFormat.hpp"
#include "udmaio/rdl/AxiVdma.hpp"

namespace udmaio {

using AxiVdmaBlock = axi_vdma::block<RegAccessorArray>;

/// Interface to AXI VDMA Core
class UioAxiVdmaIf : public UioIf, AxiVdmaBlock {
    /// @brief true if VDMA core configured for 64-bit address space
    const bool _long_addrs;

  public:
    UioAxiVdmaIf(HwAccessorPtr dev_loc, bool long_addrs = false)
        : UioIf("UioAxiVdmaIf", dev_loc), AxiVdmaBlock(this), _long_addrs{long_addrs} {
        enable_debug(true);
    }

    ////////////////////////////////////////
    // Helper functions
    ////////////////////////////////////////

    /// @brief Configure the AXI VDMA controller
    /// @param frm_bufs Vector of memory regions used as frame buffers
    void init_buffers(const std::vector<UioRegion>& frm_bufs);

    /// @brief Configure the AXI VDMA controller
    /// @param frm_format Frame format to be set
    void set_frame_format(FrameFormat frm_format);

    ////////////////////////////////////////
    // Base config
    ////////////////////////////////////////

    /// @brief Configure the AXI VDMA controller
    /// @param frm_bufs Vector of memory regions used as frame buffers
    void config(const std::vector<UioRegion>& frm_bufs);

    /// @brief Configure the AXI VDMA controller
    /// @param frm_bufs Vector of memory regions used as frame buffers
    /// @param frm_format Frame format to be set
    void config(const std::vector<UioRegion>& frm_bufs, FrameFormat frm_format);

    /// @brief Reset the S2MM part of the AXI VDMA controller
    void reset();

    /// @brief Read S2MM registers of the AXI VDMA controller and print their content
    void print_regs();

    uint32_t get_cur_frmbuf();

    void print_cur_frmbuf();

    ////////////////////////////////////////
    // Operational functions
    ////////////////////////////////////////

    /// @brief Start the already configured AXI VDMA controller (again)
    void start();

    /// @brief Configure frame size and start the AXI VDMA controller
    /// @param frm_format Frame format to be set
    void start(FrameFormat frm_format);

    /// @brief Stops the AXI VDMA controller
    void stop(bool wait_for_end);

    /// @brief Check halted bit in status register
    /// @return true if operation is not stopped
    bool isrunning();

    /// @brief Check status register and log any errors
    /// @return true if any error occurred
    bool check_for_errors();
};

}  // namespace udmaio
