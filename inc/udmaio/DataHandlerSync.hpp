// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#pragma once

#include <optional>
#include <thread>

#include "DataHandlerAbstract.hpp"
#include "ConcurrentQueue.hpp"

namespace udmaio {

class DataHandlerSync : public DataHandlerAbstract {
    ConcurrentQueue<std::vector<uint8_t>> _queue;
    std::optional<std::thread> _ioThread;

public:
    using DataHandlerAbstract::DataHandlerAbstract;
    ~DataHandlerSync();

    void operator()();

    void process_data(std::vector<uint8_t> bytes) override;

    void stop() override;

    std::vector<uint8_t> read();
    std::vector<uint8_t> read(std::chrono::milliseconds timeout);
};

} // namespace udmaio
