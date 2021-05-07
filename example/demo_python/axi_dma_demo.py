#!/usr/bin/env python3

import sys
import os

sys.path.append(os.getcwd())
from lfsr_demo import LfsrIo

print("Creating LfsrIo instance")
l = LfsrIo("/dev/xdma/slot4")

print("Starting LfsrIo")
l.start(1024, 1, 60000)

print("Reading LfsrIo packet")
result = l.read()

print(result)
