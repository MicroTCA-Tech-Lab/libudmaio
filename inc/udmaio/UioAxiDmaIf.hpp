//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#pragma once

#include <cstdint>
#include <tuple>

#include <sys/types.h>

#include "RegAccessor.hpp"
#include "UioIf.hpp"
#include "udmaio/rdl/AxiDma.hpp"
namespace udmaio {

using AxiDmaBlock = axi_dma::block<RegAccessorArray>;

/// Interface to AXI DMA Core
class UioAxiDmaIf : public UioIf, AxiDmaBlock {
  public:
    UioAxiDmaIf(UioDeviceLocation dev_loc) : UioIf{"AxiDma", dev_loc}, AxiDmaBlock{this} {}

    /// @brief Configure and start the AXI DMA controller
    /// @param start_desc Address of first SGDMA descriptor
    void start(uintptr_t start_desc);
    uintptr_t get_curr_desc();

    using UioIf::arm_interrupt;

    /// Wait for interrupt and acknowledge it
    std::tuple<uint32_t, axi_dma::s2mm_dmasr_t> clear_interrupt();

    /// @brief Check status register and log any errors
    /// @return true if any error occurred
    bool check_for_errors();
};

}  // namespace udmaio
