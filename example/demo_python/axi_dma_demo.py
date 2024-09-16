#!/usr/bin/env python3

# Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

import sys
import os
import numpy as np
import argparse

from pyudmaio import UioDeviceLocation, ConfigUio, ConfigXdma, FpgaMemBufferOverXdma, FpgaMemBufferOverAxi
from pyudmaio import UDmaBuf, UioAxiDmaIf, UioMemSgdma, DataHandler
from pyudmaio import LogLevel, set_logging_level
from GpioStatus import GpioStatus
from TrafficGen import TrafficGen

import ProjectConsts

# Implements LFSR as described in "AXI Traffic Generator v3.0"


class Lfsr(object):
    def __init__(self, seed):
        self.state = seed

    def advance(self):
        old_val = self.state
        new_bit = 1 ^ (self.state) ^ (self.state >> 1) ^ (
            self.state >> 3) ^ (self.state >> 12)
        self.state = (new_bit << 15) | (self.state >> 1)
        self.state &= 0xffff
        return old_val


# Checks array against expected LFSR values
class LfsrChecker(object):

    # blk_len: Each array is built up by blocks of N identical values (determined by bus width)
    # e.g. 8 for ZUP, 4 for Z7IO
    def __init__(self, blk_len):
        self.lfsr = None
        self.blk_len = blk_len

    def check(self, arr):
        if self.lfsr is None:
            self.lfsr = Lfsr(arr[0])

        # Create vector of expected values
        vfy = np.asarray([self.lfsr.advance() for n in range(
            len(arr) // self.blk_len)], dtype=np.uint16)

        # Create matrix where rows are blocks of identical values
        arr = arr.reshape(-1, self.blk_len)

        for n, v in enumerate(vfy):
            if not np.all(np.equal(arr[n], v)):
                print(f'mismatch at row {n}: rcv {arr[n]}, exp {v}')
                return False
        return True


def main():
    parser = argparse.ArgumentParser(
        description='AXI DMA demo for LFSR traffic generator'
    )
    parser.add_argument('-H', '--hardware',
                        type=str,
                        default='zup',
                        help='Hardware (zup or z7io)'
                        )
    parser.add_argument('-l', '--pkt_len',
                        type=int,
                        default=1024,
                        help='Packet length, default 1024'
                        )
    parser.add_argument('-n', '--nr_pkts',
                        type=int,
                        default=1,
                        help='Number of packets, default 1'
                        )
    parser.add_argument('-p', '--pkt_pause',
                        type=int,
                        default=10,
                        help='Pause between packets, default 10'
                        )
    parser.add_argument('-d', '--dev_path',
                        type=str,
                        help='Path to xdma device nodes'
                        )
    dma_mode = parser.add_mutually_exclusive_group(required=True)
    dma_mode.add_argument('-x', '--xdma',
                          action='store_true',
                          help='Use XDMA mode'
                          )
    dma_mode.add_argument('-u', '--uio',
                          action='store_true',
                          help='Use UIO mode'
                          )
    log_lvl = parser.add_mutually_exclusive_group(required=False)
    log_lvl.add_argument('--debug',
                         action='store_true',
                         help='Enable verbose output (debug level)'
                         )
    log_lvl.add_argument('--trace',
                         action='store_true',
                         help='Enable even more verbose output (trace level)'
                         )
    args = parser.parse_args()

    if args.xdma and not args.dev_path:
        print('Need device path in XDMA mode', file=sys.stderr)
        sys.exit(-1)

    if args.trace:
        print("Set log level to TRACE")
        set_logging_level(LogLevel.TRACE)
    elif args.debug:
        print("Set log level to DEBUG")
        set_logging_level(LogLevel.DEBUG)

    consts = {
        'zup': ProjectConsts.ZupExampleConsts,
        'z7io': ProjectConsts.Z7ioExampleConsts
    }[args.hardware.lower()]

    if args.xdma:
        UioDeviceLocation.set_link_xdma(
            args.dev_path, consts.PCIE_AXI4L_OFFSET, args.hardware.lower() == 'z7io')
    else:
        UioDeviceLocation.set_link_axi()

    g = GpioStatus(consts.AXI_GPIO_STATUS)
    if not g.is_ddr4_init_calib_complete():
        raise RuntimeError('DDR4 init calib is not complete')

    print('Creating DMA handler')

    axi_dma = UioAxiDmaIf(consts.AXI_DMA_0)
    mem_sgdma = UioMemSgdma(consts.BRAM_CTRL_0)
    if args.xdma:
        udmabuf = FpgaMemBufferOverXdma(
            args.dev_path, consts.FPGA_MEM_PHYS_ADDR)
    else:
        udmabuf = UDmaBuf()
        # when using predefined UIO region such as PL-DDR
        # udmabuf = FpgaMemBufferOverAxi(UioDeviceLocation('plddr-axi-test'))

    data_handler = DataHandler(axi_dma, mem_sgdma, udmabuf)
    traffic_gen = TrafficGen(consts.AXI_TRAFFIC_GEN_0)

    print('Starting DMA')
    NR_BUFFERS = 32
    data_handler.start(NR_BUFFERS, args.pkt_len * consts.LFSR_BYTES_PER_BEAT)

    print('Starting TrafficGen')
    traffic_gen.start(args.nr_pkts, args.pkt_len, args.pkt_pause)

    checker = LfsrChecker(consts.LFSR_BYTES_PER_BEAT // 2)  # 2 bytes per word
    words_total = 0

    while True:
        data_bytes = data_handler.read(10)
        if not data_bytes.size:
            break
        result = np.frombuffer(data_bytes.tobytes(), dtype=np.uint16)
        if not checker.check(result):
            break
        words_total += result.size

    print(f'{words_total} words OK')
    traffic_gen.stop()
    data_handler.stop()
    axi_dma.dump_status()


if __name__ == '__main__':
    main()
