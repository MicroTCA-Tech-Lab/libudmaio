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

DataHandlerPrint::DataHandlerPrint(UioAxiDmaIf &dma, UioMemSgdma &desc, DmaBufferAbstract &mem,
                                   uint64_t num_bytes_expected)
    : DataHandlerAbstract{dma, desc, mem}, lfsr{std::nullopt},
      _counter_ok{0}, _counter_total{0},
      _num_bytes_expected{num_bytes_expected}, _num_bytes_rcvd{0} {}

void DataHandlerPrint::process_data(const std::vector<uint8_t> &bytes) {
    BOOST_LOG_SEV(_slg, blt::severity_level::debug)
        << "DataHandlerPrint: process data, size = " << bytes.size();
    _num_bytes_rcvd += bytes.size();

    if (bytes.size() == 0) {
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << "DataHandlerPrint: nothing to do, exiting";
        return;
    }

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
                goto finish;
            } else {
                _counter_ok++;
            }
        }
        lfsr->advance();
    }

finish:
    if (_num_bytes_expected != 0 && _num_bytes_rcvd >= _num_bytes_expected) {
        // We're done.
        if (_num_bytes_rcvd == _num_bytes_expected) {
            BOOST_LOG_SEV(_slg, blt::severity_level::debug) << "DataHandlerPrint: Received all packets";
        } else {
            BOOST_LOG_SEV(_slg, blt::severity_level::error) << "DataHandlerPrint: Received more packets than expected";
        }
        stop();
    }
}

std::pair<uint64_t, uint64_t> DataHandlerPrint::operator()() {
    DataHandlerAbstract::operator()();
    return std::make_pair(_counter_ok, _counter_total);
}
