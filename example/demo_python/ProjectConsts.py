# Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

from pyudmaio import UioDeviceLocation, UioRegion


class ZupExampleConsts(object):
    AXI_GPIO_STATUS = UioDeviceLocation(
        'axi_gpio_status', UioRegion(0x00801000, 4 * 1024)
    )
    AXI_DMA_0 = UioDeviceLocation(
        'hier_daq_arm_axi_dma_0', UioRegion(0x00910000, 4 * 1024), 'events0'
    )
    BRAM_CTRL_0 = UioDeviceLocation(
        'hier_daq_arm_axi_bram_ctrl_0', UioRegion(0x00920000, 8 * 1024)
    )
    AXI_TRAFFIC_GEN_0 = UioDeviceLocation(
        'hier_daq_arm_axi_traffic_gen_0', UioRegion(0x00890000, 64 * 1024)
    )

    FPGA_MEM_PHYS_ADDR = 0x400000000
    PCIE_AXI4L_OFFSET = 0x88000000
    LFSR_BYTES_PER_BEAT = 16


class Z7ioExampleConsts(object):
    AXI_GPIO_STATUS = UioDeviceLocation(
        'axi_gpio_status', UioRegion(0x00801000, 4 * 1024)
    )
    AXI_DMA_0 = UioDeviceLocation(
        'hier_daq_arm_axi_dma_0', UioRegion(0x00910000, 4 * 1024), 'events0'
    )
    BRAM_CTRL_0 = UioDeviceLocation(
        'hier_daq_arm_axi_bram_ctrl_0', UioRegion(0x00920000, 8 * 1024)
    )
    AXI_TRAFFIC_GEN_0 = UioDeviceLocation(
        'hier_daq_arm_axi_traffic_gen_0', UioRegion(0x00890000, 64 * 1024)
    )

    FPGA_MEM_PHYS_ADDR = 0x03f100000
    PCIE_AXI4L_OFFSET = 0x44000000
    LFSR_BYTES_PER_BEAT = 8
