//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include <cstdint>

// this information comes from the FPGA project
namespace zup_example_prj {
  constexpr uint64_t axi_gpio_status_addr = 0x00801000;
  constexpr uint64_t axi_gpio_status_size = 4 * 1024;

  constexpr uint64_t axi_dma_0_addr = 0x00910000;
  constexpr uint64_t axi_dma_0_size = 4 * 1024;

  constexpr uint64_t bram_ctrl_0_addr = 0x00920000;
  constexpr uint64_t bram_ctrl_0_size = 8 * 1024;

  constexpr uint64_t axi_traffic_gen_0_addr = 0x00890000;
  constexpr uint64_t axi_traffic_gen_0_size = 64 * 1024;

  constexpr uint64_t fpga_mem_phys_addr = 0x400000000UL;

  constexpr uint16_t lfsr_bytes_per_beat = 16;
}
