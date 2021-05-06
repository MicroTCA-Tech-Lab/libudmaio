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
