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

#include "udmaio/UioIf.hpp"

using namespace udmaio;

class UioGpioStatus : UioIf {
    static constexpr int ADDR_GPIO_DATA = 0;

    struct __attribute__((packed)) GpioData {
        bool ddr4_init_calib_complete : 1;
        unsigned reserved : 31;
    };

    virtual const std::string_view _log_name() const override {
        return "UioGpioStatus";
    };

  public:
    using UioIf::UioIf;

    bool is_ddr4_init_calib_complete() const {
        return _reg<GpioData>(ADDR_GPIO_DATA).ddr4_init_calib_complete;
    }
};
