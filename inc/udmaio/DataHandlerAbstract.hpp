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

#include <boost/asio.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/log/trivial.hpp>
#include <boost/core/noncopyable.hpp>

#include "UDmaBuf.hpp"
#include "UioAxiDmaIf.hpp"
#include "UioMemSgdma.hpp"

namespace blt = boost::log::trivial;

namespace udmaio {

class DataHandlerAbstract : private boost::noncopyable {

    boost::log::sources::severity_logger<blt::severity_level> _slg;
    UioAxiDmaIf &_dma;
    UioMemSgdma &_desc;
    DmaBufferAbstract &_mem;

    boost::asio::io_service _svc;
    boost::asio::posix::stream_descriptor _sd;

    void _start_read();
    void _handle_input(const boost::system::error_code& ec);

  public:
    explicit DataHandlerAbstract(UioAxiDmaIf &dma, UioMemSgdma &desc, DmaBufferAbstract &mem);
    virtual ~DataHandlerAbstract();

    void stop();

    void operator()();

    virtual void process_data(std::vector<uint8_t> bytes) = 0;
};

} // namespace udmaio
