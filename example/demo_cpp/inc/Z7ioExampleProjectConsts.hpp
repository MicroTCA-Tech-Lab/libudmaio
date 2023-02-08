/***************************************************************************
 *      ____  _____________  __    __  __ _           _____ ___   _        *
 *     / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\  (R)  *
 *    / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      *
 *   / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     *
 *  /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     *
 *                                                                         *
 ***************************************************************************/

/*
 * Auto-generated by project-consts
 *   https://msktechvcs.desy.de/techlab/software/internal/project-consts
 *
 * This file was generated with:
 * project-consts.py damc_fmc1z7io_top.xsa -t 0x44000000 -a arm_m_axi -e axi_dma:events0 -o Z7ioExampleProjectConsts.hpp
 *
 */

#include "udmaio/UioConfig.hpp"

// clang-format off

namespace target_hw_consts {

const udmaio::UioDeviceLocation axi_bram_ctrl {
    "hier_daq_axi_bram_ctrl_0",
    { 0x0012'0000, 8 * 1024 },
};

const udmaio::UioDeviceLocation axi_dma {
    "hier_daq_axi_dma_0",
    { 0x0022'2000, 4 * 1024 },
};

const udmaio::UioDeviceLocation axi_gpio_0 {
    "hier_io_axi_gpio_0",
    { 0x0040'0000, 4 * 1024 },
};

const udmaio::UioDeviceLocation axi_gpio_status {
    "hier_interconnect_axi_gpio_status",
    { 0x0005'0000, 4 * 1024 },
};

const udmaio::UioDeviceLocation axi_traffic_gen {
    "hier_daq_axi_traffic_gen_0",
    { 0x0021'0000, 64 * 1024 },
};

const udmaio::UioDeviceLocation clk_monitor_0 {
    "clk_monitor_clk_monitor_0",
    { 0x0006'0000, 4 * 1024 },
};

const udmaio::UioDeviceLocation custom_irq_gen_apu {
    "hier_irq_gen_custom_irq_gen_apu",
    { 0x0030'0000, 4 * 1024 },
};

const udmaio::UioDeviceLocation custom_irq_gen_pcie {
    "hier_irq_gen_custom_irq_gen_pcie",
    { 0x0031'0000, 4 * 1024 },
};

const udmaio::UioDeviceLocation fmc1z7io_ident {
    "hier_interconnect_fmc1z7io_ident",
    { 0x0000'0000, 256 * 1024 },
};

constexpr uintptr_t pcie_offset = 0x4400'0000UL;

constexpr uint16_t lfsr_bytes_per_beat = 8;
constexpr uintptr_t fpga_mem_phys_addr = 0x0'3c00'0000UL;


} // namespace target_hw_consts