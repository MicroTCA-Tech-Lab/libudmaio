//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2018-2021 Deutsches Elektronen-Synchrotron DESY

#pragma once

#include <cstdint>

/// implements LFSR as described in "AXI Traffic Generator v3.0"
class AxiTrafficGenLfsr {
  public:
    AxiTrafficGenLfsr(uint16_t seed) : val{seed} {};

    /// set seed to a specific value
    void set(uint16_t seed) { val = seed; }

    /// advance LFSR by one iteration, return new value
    uint16_t advance() {
        uint16_t new_bit = 1 ^ (val) ^ (val >> 1) ^ (val >> 3) ^ (val >> 12);

        val = (new_bit << 15) | (val >> 1);
        return val;
    }

    /// get value without advancing the LFSR
    uint16_t get() { return val; }

  private:
    uint16_t val;
};
