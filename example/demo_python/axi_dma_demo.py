#!/usr/bin/env python3

import sys
import os
import numpy as np
import argparse

sys.path.append(os.getcwd())
from lfsr_demo import LfsrIo, ConfigUio, ConfigXdma, UioDeviceLocation, UioRegion, UioIf

AXI_GPIO_STATUS = UioDeviceLocation(
    'axi_gpio_status', UioRegion(0x00801000, 4 * 1024)
)

PCIE_AXI4L_OFFSET = 0x88000000

class GpioStatus(UioIf):
    ADDR_GPIO_DATA = 0

    def _log_name(self):
        return 'PythonGpioStatus'
    
    def test(self):
        print('Testing GpioStatus...')
        a = self._rd32(GpioStatus.ADDR_GPIO_DATA)
        print(f'Read {a} from GPIO')

# Implements LFSR as described in "AXI Traffic Generator v3.0"
class Lfsr(object):
    def __init__(self, seed):
        self.state = seed

    def advance(self):
        old_val = self.state
        new_bit = 1 ^ (self.state) ^ (self.state >> 1) ^ (self.state >> 3) ^ (self.state >> 12)
        self.state = (new_bit << 15) | (self.state >> 1)
        self.state &= 0xffff
        return old_val


# Checks array against expected LFSR values
class LfsrChecker(object):

    # Each array is built up by blocks of N identical values (determined by bus size)
    IDENTICAL_BLK_LEN = 8

    def __init__(self):
        self.lfsr = None
    
    def check(self, arr):
        if self.lfsr is None:
            self.lfsr = Lfsr(arr[0])
        
        # Create vector of expected values
        vfy = np.asarray([self.lfsr.advance() for n in range(len(arr) // self.IDENTICAL_BLK_LEN)], dtype=np.uint16)

        # Create matrix where rows are blocks of identical values
        arr = arr.reshape(-1, self.IDENTICAL_BLK_LEN)

        for n, v in enumerate(vfy):
            if not np.all(np.equal(arr[n], v)):
                print(f'mismatch at row {n}: rcv {arr[n]}, exp {v}')
                return False
        return True


def main():
    parser = argparse.ArgumentParser(
        description='AXI DMA demo for LFSR traffic generator'
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
    
    if args.xdma:
        cfg = ConfigXdma(args.dev_path, PCIE_AXI4L_OFFSET)
    else:
        cfg = ConfigUio()

    g = GpioStatus(cfg(AXI_GPIO_STATUS))
    g.test()

    print('Creating LfsrIo instance')
    l = LfsrIo(
        LfsrIo.trace if args.trace else LfsrIo.debug if args.debug else LfsrIo.info,
        cfg
    )

    print('Starting LfsrIo')
    l.start(args.pkt_len, args.nr_pkts, args.pkt_pause)

    checker = LfsrChecker()
    words_total = 0

    while True:
        result = l.read(10)
        if not result.size:
            break
        if not checker.check(result):
            break
        words_total += result.size

    print(f'{words_total} words OK')

    l.stop()


if __name__ == '__main__':
    main()
