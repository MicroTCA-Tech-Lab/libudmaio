#!/usr/bin/env python3

# Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

import bitstruct


class UioReg(object):
    def __init__(self, intf, offs, fields=None):
        self.dict = {}
        self.intf = intf
        self.offs = offs
        if fields is not None:
            bitfield = ''.join(list(zip(*fields))[1])
            if bitstruct.calcsize(bitfield) != 32:
                raise RuntimeError('UioReg: Bitfield must be 32 bits in size')
            names = list(zip(*fields))[0]
            self.bs = bitstruct.compile(bitfield, names=names)
            self.val = 0
        else:
            self.bs = None

    def rd(self):
        val = self.intf._rd32(self.offs)
        if self.bs is not None:
            self.val = val
        return val

    def wr(self, val = None):
        if val is not None:
            self.intf._wr32(self.offs, val)
        else:
            self.intf._wr32(self.offs, self.val)

    def __getattr__(self, name):
        if name != 'dict' and name in self.dict:
            return self.dict[name]
        elif name == 'val':
            return int.from_bytes(self.raw_val, 'big')
        else:
            return object.__getattribute__(self, name)

    def __setattr__(self, name, value):
        if name != 'dict' and name in self.dict:
            self.dict[name] = value
            self.raw_val = self.bs.pack(self.dict)
        elif name == 'val':
            self.raw_val = value.to_bytes(4, 'big')
            self.dict = self.bs.unpack(self.raw_val)
        elif name in ['raw_val', 'dict', 'intf', 'offs', 'bs']:
            super().__setattr__(name, value)
        else:
            raise AttributeError(f'{name}: unknown attribute')


class UioRegTest:
    def __init__(self):
        self.test_reg = UioReg(self, 0x1000, (
            ('MSBh', 'u4'),
            ('MSBl', 'u4'),
            ('B2_7', 'u7'),
            ('B2_0', 'u1'),
            ('B1', 's8'),
            ('LSB', 'u8'),
        ))
    
    def _rd32(self, offs):
        print(f'Reading at {offs:08x}')
        return 0
    
    def _wr32(self, offs, val):
        print(f'Writing {val:08x} to {offs:08x}')

    def test(self):
        self.test_reg.rd()
        print(self.test_reg.raw_val)
        print(self.test_reg.val)
        print(self.test_reg.dict)
        self.test_reg.MSBh = 0xf
        self.test_reg.MSBl = 0x5
        self.test_reg.B2_7 = 0
        self.test_reg.B2_0 = 1
        self.test_reg.B1 = -2
        self.test_reg.LSB = 0xaa
        print(self.test_reg.raw_val)
        print(self.test_reg.val)
        print(self.test_reg.dict)
        self.test_reg.wr()

if __name__ == '__main__':
    regtest = UioRegTest()
    regtest.test()
