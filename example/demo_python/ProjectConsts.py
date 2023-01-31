# Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

from pyudmaio import UioDeviceLocation, UioRegion


class ZupExampleConsts(object):
    AXI_GPIO_STATUS = UioDeviceLocation(
        'hier_interconnect_axi_gpio_status', UioRegion(
            0x0005_0000, 4 * 1024
        )
    )
    AXI_DMA_0 = UioDeviceLocation(
        'hier_daq_arm_axi_dma', UioRegion(
            0x0022_2000, 4 * 1024
        ), 'events0'
    )
    BRAM_CTRL_0 = UioDeviceLocation(
        'hier_daq_arm_axi_bram_ctrl', UioRegion(
            0x0022_0000, 8 * 1024
        )
    )
    AXI_TRAFFIC_GEN_0 = UioDeviceLocation(
        'hier_daq_arm_axi_traffic_gen', UioRegion(
            0x0021_0000, 64 * 1024
        )
    )

    FPGA_MEM_PHYS_ADDR = 0x5_0000_0000
    PCIE_AXI4L_OFFSET = 0x0_a000_0000
    LFSR_BYTES_PER_BEAT = 16


class Z7ioExampleConsts(object):
    AXI_GPIO_STATUS = UioDeviceLocation(
        'hier_interconnect_axi_gpio_status', UioRegion(
            0x0005_0000, 4 * 1024
        )
    )
    AXI_DMA_0 = UioDeviceLocation(
        'hier_daq_axi_dma_0', UioRegion(
            0x0022_2000, 4 * 1024
        ), 'events0'
    )
    BRAM_CTRL_0 = UioDeviceLocation(
        'hier_daq_axi_bram_ctrl_0', UioRegion(
            0x0012_0000, 8 * 1024
        )
    )
    AXI_TRAFFIC_GEN_0 = UioDeviceLocation(
        'hier_daq_axi_traffic_gen_0', UioRegion(
            0x0021_0000, 64 * 1024
        )
    )

    FPGA_MEM_PHYS_ADDR = 0x0_3c00_0000
    PCIE_AXI4L_OFFSET = 0x0_4400_0000
    LFSR_BYTES_PER_BEAT = 8
