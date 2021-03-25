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

#include <utility>

#include "udmaio/DataHandlerAbstract.hpp"

#include "AxiTrafficGenLfsr.hpp"

using namespace udmaio;

class DataHandlerPrint : public DataHandlerAbstract {

    std::optional<AxiTrafficGenLfsr> lfsr;

    boost::log::sources::severity_logger<blt::severity_level> _slg;

    void process_data(const std::vector<uint8_t> &bytes) override;

    uint64_t _counter_ok;
    uint64_t _counter_total;
    uint64_t _num_bytes_expected;
    uint64_t _num_bytes_rcvd;

  public:
    explicit DataHandlerPrint(UioAxiDmaIf &dma, UioMemSgdma &desc, DmaBufferAbstract &mem,
                              uint64_t num_bytes_expected);
    std::pair<uint64_t, uint64_t> operator()();
};
