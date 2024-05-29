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

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>

#include "Logging.hpp"
#include "UDmaBuf.hpp"
#include "UioAxiDmaIf.hpp"
#include "UioMemSgdma.hpp"
#include <boost/asio/posix/stream_descriptor.hpp>

namespace udmaio {

/// Base class to implement a DMA data reception handler
class DataHandlerAbstract : public Logger, private boost::noncopyable {
    UioAxiDmaIf& _dma;        ///< AXI DMA interface
    UioMemSgdma& _desc;       ///< SGDMA data interface
    DmaBufferAbstract& _mem;  ///< SGDMA buffer holding the data received from AXI DMA

    boost::asio::io_service _svc;
    boost::asio::posix::stream_descriptor _sd;

    void _start_read();
    void _handle_input(const boost::system::error_code& ec);

  protected:
    bool _receive_packets;  ///< Enable segmentation of stream into SOF/EOF delimited frames

  public:
    /// @brief Construct a Data Handler
    /// @param dma Interface to the AXI DMA core
    /// @param desc Interface to the SGDMA descriptors
    /// @param mem Interface to the memory holding the SGDMA data buffers
    /// @param receive_packets Receive packets/frames delimited by SOF/EOF. If not set, receive stream as-is without regard for packets
    explicit DataHandlerAbstract(std::string name,
                                 UioAxiDmaIf& dma,
                                 UioMemSgdma& desc,
                                 DmaBufferAbstract& mem,
                                 bool receive_packets = true);
    virtual ~DataHandlerAbstract();

    /// @brief Stop the data reception
    virtual void stop();

    /// @brief Run the data reception
    void operator()();

    /// @brief Process the received data
    /// @param bytes Data block
    virtual void process_data(std::vector<uint8_t> bytes) = 0;
};

}  // namespace udmaio
