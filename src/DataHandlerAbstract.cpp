//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include "udmaio/DataHandlerAbstract.hpp"

#include <functional>
#include <iomanip>
#include <iostream>
#include <vector>

namespace udmaio {

DataHandlerAbstract::DataHandlerAbstract(std::string name,
                                         UioAxiDmaIf& dma,
                                         UioMemSgdma& desc,
                                         DmaBufferAbstract& mem)
    : Logger(name), _dma{dma}, _desc{desc}, _mem{mem}, _svc{}, _sd{_svc, _dma.get_fd_int()} {
    BOOST_LOG_SEV(_lg, bls::trace) << "ctor";
};

DataHandlerAbstract::~DataHandlerAbstract() {
    BOOST_LOG_SEV(_lg, bls::trace) << "dtor";
}

void DataHandlerAbstract::stop() {
    BOOST_LOG_SEV(_lg, bls::trace) << "stop";
    _svc.stop();
}

void DataHandlerAbstract::_start_read() {
    _dma.arm_interrupt();

    _sd.async_read_some(
        boost::asio::null_buffers(),  // No actual reading - that's deferred to UioAxiDmaIf
        std::bind(&DataHandlerAbstract::_handle_input,
                  this,
                  std::placeholders::_1  // boost::asio::placeholders::error
                  ));
}

void DataHandlerAbstract::_handle_input(const boost::system::error_code& ec) {
    if (ec) {
        BOOST_LOG_SEV(_lg, bls::error) << "I/O error: " << ec.message();
        return;
    }

    // Check for error flags and log them
    _dma.check_for_errors();

    uint32_t irq_count = _dma.clear_interrupt();
    BOOST_LOG_SEV(_lg, bls::trace) << "irq count = " << irq_count;

    std::vector<UioRegion> full_bufs = _desc.get_full_buffers();
    std::vector<uint8_t> bytes;

    if (full_bufs.empty()) {
        BOOST_LOG_SEV(_lg, bls::trace) << "spurious event, got no data";
        goto done;
    }

    // Assuming all buffers are equally big, we need number of buffers times buffer size
    // Reserving enough space in advance to avoid re-allocation / copying
    bytes.reserve(full_bufs.size() * full_bufs[0].size);

    for (auto& buf : full_bufs) {
        _mem.append_from_buf(buf, bytes);
    }

    process_data(std::move(bytes));

done:
    _start_read();
}

void DataHandlerAbstract::operator()() {
    BOOST_LOG_SEV(_lg, bls::trace) << "started";

    _start_read();
    _svc.run();

    BOOST_LOG_SEV(_lg, bls::trace) << "finished";
}

}  // namespace udmaio
