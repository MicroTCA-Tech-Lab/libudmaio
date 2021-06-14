//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include "udmaio/DataHandlerSync.hpp"

namespace udmaio {

DataHandlerSync::~DataHandlerSync() {
    if (_ioThread) {
        _ioThread->join();
    }
}

void DataHandlerSync::operator()() {
    _ioThread = std::thread{&DataHandlerAbstract::operator(), this};
}

void DataHandlerSync::process_data(std::vector<uint8_t> bytes) {
    _queue.push(std::move(bytes));
}

void DataHandlerSync::stop() {
    _queue.abort();
    DataHandlerAbstract::stop();
}

std::vector<uint8_t> DataHandlerSync::read() {
    return _queue.pop();
}

std::vector<uint8_t> DataHandlerSync::read(std::chrono::milliseconds timeout) {
    return _queue.pop(timeout);
}

}
