//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include <iostream>
#include <thread>

#include "DataHandlerPrint.hpp"

DataHandlerPrint::DataHandlerPrint(UioAxiDmaIf &dma, UioMemSgdma &desc, UDmaBuf &mem,
                                   uint64_t &counter_ok, uint64_t &counter_total)
    : DataHandlerAbstract{dma, desc, mem}, lfsr{std::nullopt}, _counter_ok{counter_ok},
      _counter_total{counter_total} {}

void DataHandlerPrint::process_data(const std::vector<uint8_t> &bytes) {
    BOOST_LOG_SEV(_slg, blt::severity_level::debug)
        << "DataHandlerPrint: process data, size = " << bytes.size();

    const uint16_t *vals = reinterpret_cast<const uint16_t *>(&bytes[0]);

    if (!lfsr) {
        uint16_t seed = vals[0];
        lfsr = AxiTrafficGenLfsr{seed};
    }

    for (unsigned int i = 0; i < bytes.size() / 2 / 8; i++) {
        uint16_t exp_val = lfsr->get();
        for (int j = 0; j < 8; j++) {
            uint16_t recv_val = vals[i * 8 + j];
            _counter_total++;
            if (exp_val != recv_val) {
                BOOST_LOG_SEV(_slg, blt::severity_level::fatal)
                    << "mismatch, at " << i * 8 + j << " recv = " << std::hex << recv_val
                    << ", exp = " << exp_val;
                return;
            } else {
                _counter_ok++;
            }
        }
        lfsr->advance();
    }
}
