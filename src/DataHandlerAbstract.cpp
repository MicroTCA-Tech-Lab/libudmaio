//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include <iomanip>
#include <iostream>
#include <functional>
#include <vector>

#include "udmaio/DataHandlerAbstract.hpp"

namespace udmaio {

DataHandlerAbstract::DataHandlerAbstract(UioAxiDmaIf &dma, UioMemSgdma &desc,
                                         DmaBufferAbstract &mem)
    : _dma{dma}, _desc{desc}, _mem{mem},
      _svc{}, _sd{_svc, _dma.get_fd_int()} {
    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "DataHandler: ctor";
};

DataHandlerAbstract::~DataHandlerAbstract() {
    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "DataHandler: dtor";
}

void DataHandlerAbstract::stop() {
    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "DataHandler: stop";
    _svc.stop();
}

void DataHandlerAbstract::_start_read() {
    _dma.arm_interrupt();

    _sd.async_read_some(
        boost::asio::null_buffers(), // No actual reading - that's deferred to UioAxiDmaIf
        std::bind(
            &DataHandlerAbstract::_handle_input,
            this,
            std::placeholders::_1 // boost::asio::placeholders::error
        )
    );
}

void DataHandlerAbstract::_handle_input(const boost::system::error_code& ec) {
    if (ec) {
        BOOST_LOG_SEV(_slg, blt::severity_level::error)
            << "DataHandler: I/O error: " << ec.message();
        return;
    }

    uint32_t irq_count = _dma.clear_interrupt();
    BOOST_LOG_SEV(_slg, blt::severity_level::trace)
        << "DataHandler: irq count = " << irq_count;

    std::vector<UioMemSgdma::BufInfo> full_bufs = _desc.get_full_buffers();
    std::vector<uint8_t> bytes;

    for (auto &buf : full_bufs) {
        _mem.copy_from_buf(buf, bytes);
    }

    if (!bytes.empty()) {
        process_data(bytes);
    } else {
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << "DataHandler: spurious event, got no data";
    }

    _start_read();
}

void DataHandlerAbstract::operator()() {
    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "DataHandler: started";

    _start_read();
    _svc.run();

    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "DataHandler: finished";
}

} // namespace udmaio
