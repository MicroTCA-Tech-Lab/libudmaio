from lfsr_demo import UioIf
from UioReg import UioReg


class GpioStatus(UioIf):
    def __init__(self, cfg):
        super().__init__(cfg)
        self.GpioStatus = UioReg(self, 0x00000000, (
            ('Reserved', 'u31'),
            ('DDR4_INIT_CMPLT', 'u1'),
        ))

    def _log_name(self):
        return 'GpioStatus'

    def is_ddr4_init_calib_complete(self):
        self.GpioStatus.rd()
        return self.GpioStatus.DDR4_INIT_CMPLT != 0
