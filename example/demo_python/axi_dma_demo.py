#!/usr/bin/env python3

import sys
import os
import numpy as np
from itertools import zip_longest
import argparse

sys.path.append(os.getcwd())
from lfsr_demo import LfsrIo


def grouper(n, iterable):
    "grouper(3, 'abcdef') -> ('a','b','c'), ('d','e','f')"
    return zip_longest(*[iter(iterable)]*n)


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
    args = parser.parse_args()

    if args.xdma and not args.dev_path:
        print('Need device path in XDMA mode', file=sys.stderr)
        sys.exit(-1)

    print('Creating LfsrIo instance')
    l = LfsrIo(
        LfsrIo.xdma if args.xdma else LfsrIo.uio,
        args.dev_path or ""
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
