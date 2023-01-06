//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include "udmaio/UioConfig.hpp"

using udmaio::UioDeviceLocation;

// this information comes from the FPGA project
namespace target_hw_consts {
const UioDeviceLocation axi_gpio_status{"hier_interconnect_axi_gpio_status", {0x50000, 4 * 1024}};
const UioDeviceLocation axi_dma_0{"hier_daq_arm_axi_dma", {0x222000, 4 * 1024}, "events0"};
const UioDeviceLocation bram_ctrl_0{"hier_daq_arm_axi_bram_ctrl", {0x220000, 8 * 1024}};
const UioDeviceLocation axi_traffic_gen_0{"hier_daq_arm_axi_traffic_gen", {0x210000, 64 * 1024}};

constexpr uint16_t lfsr_bytes_per_beat = 16;

constexpr uintptr_t fpga_mem_phys_addr = 0x500000000UL;
constexpr uintptr_t pcie_axi4l_offset = 0xa0000000UL;
}  // namespace target_hw_consts
