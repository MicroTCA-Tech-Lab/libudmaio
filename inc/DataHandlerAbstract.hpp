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

#include <vector>

#include <boost/log/trivial.hpp>

#include "UDmaBuf.hpp"
#include "UioAxiDmaIf.hpp"
#include "UioMemSgdma.hpp"

namespace blt = boost::log::trivial;

class DataHandlerAbstract {

    boost::log::sources::severity_logger<blt::severity_level> _slg;
    int _pipefd_read;
    int _pipefd_write;
    UioAxiDmaIf &_dma;
    UioMemSgdma &_desc;
    UDmaBuf &_mem;

  public:
    explicit DataHandlerAbstract(UioAxiDmaIf &dma, UioMemSgdma &desc, UDmaBuf &mem);

    void stop();

    void operator()();

    virtual void process_data(const std::vector<uint8_t> &bytes) = 0;
};
