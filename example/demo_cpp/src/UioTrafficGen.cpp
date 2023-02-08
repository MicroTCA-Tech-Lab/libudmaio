//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include "UioTrafficGen.hpp"

#include <ios>

void UioTrafficGen::start(uint16_t nr_pkts, uint32_t pkt_size, uint16_t pkt_pause) {
    BOOST_LOG_SEV(_lg, bls::trace) << "start, nr pkts = " << nr_pkts << ", pkt size = " << pkt_size;

    stop();

    stConfig.wr({
        .ranlen = 0,
        .randly = 0,
        .etkts = 0,
        .rsvd7_3 = 0,
        .tdest = 0,
        .pdly = pkt_pause,
    });

    const auto num_beats_reg = pkt_size - 1;
    trLength.wr({
        .tlen = num_beats_reg & 0xffff,
        .tcnt = nr_pkts,
    });
    extTrLength.wr({
        .ext_tlen = num_beats_reg >> 16,
    });

    auto st_ctrl = stControl.rd();
    st_ctrl.done = 0;
    st_ctrl.stren = 1;
    stControl.wr(st_ctrl);
}

void UioTrafficGen::stop() {
    auto st_ctrl = stControl.rd();
    st_ctrl.stren = 0;
    if (st_ctrl.done) {
        BOOST_LOG_SEV(_lg, bls::trace) << "clearing done bit";
        // W1C – Write 1 to Clear
        st_ctrl.done = 1;
    }
    stControl.wr(st_ctrl);
}

void UioTrafficGen::print_version() const {
    BOOST_LOG_SEV(_lg, bls::info) << "version = 0x" << std::hex << stControl.rd().version
                                  << std::dec;
}
