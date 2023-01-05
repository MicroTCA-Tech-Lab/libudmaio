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
#include <boost/core/noncopyable.hpp>
#include <boost/log/trivial.hpp>

#include "UDmaBuf.hpp"
#include "UioAxiDmaIf.hpp"
#include "UioMemSgdma.hpp"

namespace blt = boost::log::trivial;

namespace udmaio {

/// Base class to implement a DMA data reception handler
class DataHandlerAbstract : private boost::noncopyable {

    boost::log::sources::severity_logger_mt<blt::severity_level> _slg;

    UioAxiDmaIf &_dma;       ///< AXI DMA interface
    UioMemSgdma &_desc;      ///< SGDMA data interface
    DmaBufferAbstract &_mem; ///< SGDMA buffer holding the data received from AXI DMA

    boost::asio::io_service _svc;
    boost::asio::posix::stream_descriptor _sd;

    void _start_read();
    void _handle_input(const boost::system::error_code &ec);

  public:
    /// @brief Construct a Data Handler
    /// @param dma Interface to the AXI DMA core
    /// @param desc Interface to the SGDMA descriptors
    /// @param mem Interface to the memory holding the SGDMA data buffers
    explicit DataHandlerAbstract(UioAxiDmaIf &dma, UioMemSgdma &desc, DmaBufferAbstract &mem);
    virtual ~DataHandlerAbstract();

    /// @brief Stop the data reception
    virtual void stop();

    /// @brief Run the data reception
    void operator()();

    /// @brief Process the received data
    /// @param bytes Data block
    virtual void process_data(std::vector<uint8_t> bytes) = 0;
};

} // namespace udmaio
