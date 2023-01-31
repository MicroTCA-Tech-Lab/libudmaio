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

#include "AxiDma.hpp"
#include "RegAccessor.hpp"
#include "UioIf.hpp"

namespace udmaio {

using AxiDmaBlock = axi_dma::block<RegAccessorArray>;

/// Interface to AXI DMA Core
class UioAxiDmaIf : public UioIf, AxiDmaBlock {
    virtual const std::string_view _log_name() const override;

  public:
    UioAxiDmaIf(UioDeviceInfo dev_loc) : UioIf(dev_loc), AxiDmaBlock(this) {}

    /// @brief Configure and start the AXI DMA controller
    /// @param start_desc Address of first SGDMA descriptor
    void start(uintptr_t start_desc);

    using UioIf::arm_interrupt;

    /// Wait for interrupt and acknowledge it
    uint32_t clear_interrupt();

    /// @brief Get file descriptor of interrupt event file
    /// @return Event file descriptor
    int get_fd_int() const;

    /// @brief Check status register and log any errors
    /// @return true if any error occurred
    bool check_for_errors();
};

}  // namespace udmaio
