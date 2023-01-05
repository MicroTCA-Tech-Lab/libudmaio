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

#include "UioIf.hpp"

namespace udmaio {

/// Interface to AXI DMA Core
class UioAxiDmaIf : public UioIf {
    static constexpr int ADDR_S2MM_DMACR = 0x30;
    static constexpr int ADDR_S2MM_DMASR = 0x34;
    static constexpr int ADDR_S2MM_CURDESC = 0x38;
    static constexpr int ADDR_S2MM_CURDESC_MSB = 0x3C;
    static constexpr int ADDR_S2MM_TAILDESC = 0x40;
    static constexpr int ADDR_S2MM_TAILDESC_MSB = 0x44;

    struct __attribute__((packed)) S2mmDmaControlReg {
        bool RS : 1;
        uint32_t rsvd1 : 1;
        bool Reset : 1;
        bool Keyhole : 1;
        bool Cyc_bd_en : 1;
        uint32_t rsvd11_5 : 7;
        bool IOC_IrqEn : 1;
        bool Dly_IrqEn : 1;
        bool Err_IrqEn : 1;
        uint32_t rsvd15 : 1;
        uint32_t IRQThreshold : 8;
        uint32_t IRQDelay : 8;
    };

    struct __attribute__((packed)) S2mmDmaStatusReg {
        bool Halted : 1;
        bool Idle : 1;
        uint32_t rsvd2 : 1;
        bool SGIncld : 1;
        bool DMAIntErr : 1;
        bool DMASlvErr : 1;
        bool DMADecErr : 1;
        uint32_t rsvd7 : 1;
        bool SGIntErr : 1;
        bool SGSlvErr : 1;
        bool SGDecErr : 1;
        uint32_t rsvd11 : 1;
        bool IOC_Irq : 1;
        bool Dly_Irq : 1;
        bool Err_Irq : 1;
        uint32_t rsvd15 : 1;
        uint32_t RQThresholdSts : 8;
        uint32_t IRQDelaySts : 8;
    };

    static_assert(sizeof(S2mmDmaControlReg) == 4);
    static_assert(sizeof(S2mmDmaStatusReg) == 4);

    virtual const std::string_view _log_name() const override;

  public:
    using UioIf::UioIf;

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
