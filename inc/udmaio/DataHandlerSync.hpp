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

#include <optional>
#include <thread>

#include "DataHandlerAbstract.hpp"
#include "ConcurrentQueue.hpp"

namespace udmaio {

// Synchronous data handler with blocking read interface
class DataHandlerSync : public DataHandlerAbstract {
    ConcurrentQueue<std::vector<uint8_t>> _queue;
    std::optional<std::thread> _ioThread;

public:
    using DataHandlerAbstract::DataHandlerAbstract;
    virtual ~DataHandlerSync();

    void operator()();

    void process_data(std::vector<uint8_t> bytes) override;

    virtual void stop() override;

    std::vector<uint8_t> read();
    std::vector<uint8_t> read(std::chrono::milliseconds timeout);
};

} // namespace udmaio
