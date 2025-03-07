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
#include <iostream>
#include <vector>

#include <pthread.h>

namespace udmaio {

DataHandlerAbstract::DataHandlerAbstract(std::string name,
                                         UioAxiDmaIf& dma,
                                         UioMemSgdma& desc,
                                         DmaBufferAbstract& mem,
                                         bool receive_packets,
                                         bool rt_prio)
    : Logger(name)
    , _dma{dma}
    , _desc{desc}
    , _mem{mem}
    , _svc{}
    , _sd{_svc, _dma.get_fd_int()}
    , _rt_prio{rt_prio}
    , _receive_packets{receive_packets} {
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

    auto [irq_count, dma_stat] = _dma.clear_interrupt();
    BOOST_LOG_SEV(_lg, bls::trace) << "irq count = " << irq_count;
    if (dma_stat.err_irq && _dma.check_for_errors()) {
        BOOST_LOG_SEV(_lg, bls::fatal)
            << "DMA error, curr.desc 0x" << std::hex << _dma.get_curr_desc();
        _dma.dump_status();
        _desc.print_descs();
        throw std::runtime_error("DMA engine error raised");
    }

    if (dma_stat.ioc_irq) {
        bool spurious_event = true;
        // Receive data until there's no more incoming packets or full buffers
        // (esp. important in packet mode where get_next_packet() will only
        // return *one* packet, regardless of more incoming data in the pipeline)
        while (true) {
            auto full_bufs = _receive_packets ? _desc.get_next_packet() : _desc.get_full_buffers();
            if (full_bufs.empty()) {
                if (spurious_event) {
                    BOOST_LOG_SEV(_lg, bls::trace) << "spurious event, got no data";
                }
                break;
            } else {
                auto bytes = _desc.read_buffers(full_bufs);
                process_data(std::move(bytes));
                spurious_event = false;
            }
        }
    }

    _start_read();
}

void DataHandlerAbstract::operator()() {
    BOOST_LOG_SEV(_lg, bls::trace) << "started";

    if (_rt_prio) {
        // Set real-time scheduling
        sched_param sch_params = {.sched_priority = sched_get_priority_max(SCHED_FIFO)};
        if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch_params)) {
            BOOST_LOG_SEV(_lg, bls::error)
                << "Failed to set thread scheduling: Permission denied or invalid policy";
        }
    }

    _start_read();
    _svc.run();

    BOOST_LOG_SEV(_lg, bls::trace) << "finished";
}

}  // namespace udmaio
