# Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

from pyudmaio import UioIf, UioReg


class GpioStatus(UioIf):
    def __init__(self, cfg):
        super().__init__('GpioStatus', cfg)
        self.GpioStatus = UioReg(self, 0x00000000, (
            ('Reserved', 'u31'),
            ('DDR4_INIT_CMPLT', 'u1'),
        ))

    def is_ddr4_init_calib_complete(self):
        self.GpioStatus.rd()
        return self.GpioStatus.DDR4_INIT_CMPLT != 0
