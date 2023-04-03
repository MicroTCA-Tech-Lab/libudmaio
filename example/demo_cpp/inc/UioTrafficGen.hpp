//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#pragma once

#include "udmaio/RegAccessor.hpp"

using namespace udmaio;

/// Interface to LFSR core of the demo application
class UioTrafficGen : UioIf {
    struct REG_PACKED_ALIGNED StControl {
        bool stren : 1;
        bool done : 1;
        unsigned rsvd : 22;
        unsigned version : 8;
    };

    struct REG_PACKED_ALIGNED StConfig {
        bool ranlen : 1;
        bool randly : 1;
        bool etkts : 1;
        unsigned rsvd7_3 : 5;
        unsigned tdest : 8;
        unsigned pdly : 16;
    };

    struct REG_PACKED_ALIGNED TrLength {
        unsigned tlen : 16;
        unsigned tcnt : 16;
    };

    struct REG_PACKED_ALIGNED ExtTrLength {
        unsigned ext_tlen : 8;
        unsigned rsvd : 24;
    };

    RegAccessor<StControl, 0x30> stControl{this};
    RegAccessor<StConfig, 0x34> stConfig{this};
    RegAccessor<TrLength, 0x38> trLength{this};
    RegAccessor<ExtTrLength, 0x50> extTrLength{this};

  public:
    UioTrafficGen(HwAccessorPtr hw) : UioIf("UioTrafficGen", std::move(hw)) {}

    void start(uint16_t nr_pkts, uint32_t pkt_size, uint16_t pkt_pause);
    void stop();

    void print_version() const;
};
