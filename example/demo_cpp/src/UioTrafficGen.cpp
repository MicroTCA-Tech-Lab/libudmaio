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

#include <boost/log/core/core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

namespace blt = boost::log::trivial;

const std::string_view UioTrafficGen::_log_name() const {
    return "UioTrafficGen";
}

void UioTrafficGen::start(uint16_t nr_pkts, uint32_t pkt_size, uint16_t pkt_pause) {
    BOOST_LOG_SEV(_slg, blt::severity_level::trace)
        << _log_name() << ": start, nr pkts = " << nr_pkts << ", pkt size = " << pkt_size;

    stop();

    _wr_reg<StConfig>(
        ADDR_ST_CONFIG,
        {.ranlen = 0, .randly = 0, .etkts = 0, .rsvd7_3 = 0, .tdest = 0, .pdly = pkt_pause});

    const auto num_beats_reg = pkt_size - 1;
    _wr32(ADDR_TR_LEN, nr_pkts << 16 | (num_beats_reg & 0xffff));
    _wr32(ADDR_EX_TR_LEN, num_beats_reg >> 16);

    auto st_ctrl = _rd_reg<StControl>(ADDR_ST_CTRL);
    st_ctrl.done = 0;
    st_ctrl.stren = 1;
    _wr_reg<StControl>(ADDR_ST_CTRL, st_ctrl);
}

void UioTrafficGen::stop() {
    auto st_ctrl = _rd_reg<StControl>(ADDR_ST_CTRL);
    st_ctrl.stren = 0;
    if (st_ctrl.done) {
        BOOST_LOG_SEV(_slg, blt::severity_level::trace) << _log_name() << ": clearing done bit";
        // W1C – Write 1 to Clear
        st_ctrl.done = 1;
    }
    _wr_reg<StControl>(ADDR_ST_CTRL, st_ctrl);
}

void UioTrafficGen::print_version() const {
    BOOST_LOG_SEV(_slg, blt::severity_level::info)
        << _log_name() << ": version = 0x" << std::hex << _rd_reg<StControl>(ADDR_ST_CTRL).version
        << std::dec;
}
