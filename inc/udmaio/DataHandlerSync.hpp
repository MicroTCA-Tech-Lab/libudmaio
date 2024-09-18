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

#include <cstddef>
#include <iterator>
#include <optional>
#include <thread>

#include "ConcurrentQueue.hpp"
#include "DataHandlerAbstract.hpp"

namespace udmaio {

/// Synchronous data handler with blocking read interface
class DataHandlerSync : public DataHandlerAbstract {
    ConcurrentQueue<std::vector<uint8_t>> _queue;  ///< FIFO queue holding the received data
    std::optional<std::thread> _ioThread;          ///< I/O thread

  public:
    explicit DataHandlerSync(std::string name,
                             UioAxiDmaIf& dma,
                             UioMemSgdma& desc,
                             DmaBufferAbstract& mem,
                             bool receive_packets = true,
                             size_t queue_size = 64)
        : DataHandlerAbstract{name, dma, desc, mem, receive_packets}, _queue{queue_size} {}

    virtual ~DataHandlerSync();

    /// Run the data reception
    void operator()();

    void process_data(std::vector<uint8_t> bytes) override;

    virtual void stop() override;

    /// @brief Blocking read
    /// @return Received data
    std::vector<uint8_t> read();

    /// @brief Blocking read with timeout
    /// @param timeout Timeout in milliseconds
    /// @return Received data
    std::vector<uint8_t> read(std::chrono::milliseconds timeout);
};

}  // namespace udmaio
