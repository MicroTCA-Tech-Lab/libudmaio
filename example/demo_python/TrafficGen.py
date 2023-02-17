# Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

from pyudmaio import UioIf, UioReg


class TrafficGen(UioIf):
    def __init__(self, cfg):
        super().__init__('TrafficGen', cfg)
        self.StControl = UioReg(self, 0x30, (
            ('Version', 'u8'),
            ('Reserved', 'u22'),
            ('Done', 'u1'),
            ('STREN', 'u1')
        ))
        self.StConfig = UioReg(self, 0x34, (
            ('PDLY', 'u16'),
            ('TDEST', 'u8'),
            ('Reserved', 'u5'),
            ('ETKTS', 'u1'),
            ('RANDLY', 'u1'),
            ('RANLEN', 'u1'),
        ))
        self.TxLength = UioReg(self, 0x38, (
            ('TCNT', 'u16'),
            ('TLEN', 'u16'),
        ))
        self.ExTxLength = UioReg(self, 0x50, (
            ('Reserved', 'u24'),
            ('Ext_TLEN', 'u8'),
        ))

    def start(self, nr_pkts=1, pkt_size=1024, pkt_pause=60000):
        print(f'start, nr pkts = {nr_pkts}, pkt size = {pkt_size}')

        self.stop()

        self.StConfig.RANLEN = 0
        self.StConfig.RANDLY = 0
        self.StConfig.ETKTS = 0
        self.StConfig.TDEST = 0
        self.StConfig.PDLY = pkt_pause
        self.StConfig.wr()

        num_beats_reg = pkt_size - 1

        self.TxLength.TLEN = num_beats_reg & 0xffff
        self.TxLength.TCNT = nr_pkts
        self.TxLength.wr()

        self.ExTxLength.Ext_TLEN = num_beats_reg >> 16
        self.ExTxLength.wr()

        self.StControl.rd()
        self.StControl.Done = 0
        self.StControl.STREN = 1
        self.StControl.wr()

    def stop(self):
        self.StControl.rd()
        self.StControl.STREN = 0
        if self.StControl.Done:
            print('clearing done bit')
            # W1C – Write 1 to Clear
            self.StControl.Done = 1
        self.StControl.wr()

    def version(self):
        self.StControl.rd()
        return self.StControl.Version
