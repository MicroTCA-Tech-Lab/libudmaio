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

/// Interface to GPIO status port of the demo application
class UioGpioStatus : UioIf {
    struct REG_PACKED_ALIGNED GpioData {
        bool ddr4_init_calib_complete : 1;
        unsigned reserved : 31;
    };

    RegAccessor<GpioData, 0> gpio{this};

  public:
    UioGpioStatus(HwAccessorPtr hw) : UioIf("UioGpioStatus", std::move(hw)) {}

    bool is_ddr4_init_calib_complete() const { return gpio.rd().ddr4_init_calib_complete; }
};
