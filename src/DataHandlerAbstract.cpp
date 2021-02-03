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
#include <thread>
#include <vector>

#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>

#include "DataHandlerAbstract.hpp"

DataHandlerAbstract::DataHandlerAbstract(UioAxiDmaIf &dma, UioMemSgdma &desc, UDmaBuf &mem)
    : _dma{dma}, _desc{desc}, _mem{mem} {

    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "DataHandler: ctor";

    int pipefd[2];
    int rc = pipe2(pipefd, O_DIRECT);
    if (rc < 0) {
        throw std::runtime_error("could not create a pipe");
    }

    _pipefd_read = pipefd[0];
    _pipefd_write = pipefd[1];
};

void DataHandlerAbstract::stop() {
    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "DataHandler: stop";
    char msg[1] = {0};
    write(_pipefd_write, msg, sizeof(msg));
}

void DataHandlerAbstract::operator()() {
    BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "DataHandler: new thread started";

    int dma_fd = _dma.get_fd();
    int nfds = std::max(_pipefd_read, dma_fd) + 1;

    fd_set rfds;
    FD_ZERO(&rfds);

    while (true) {
        _dma.arm_interrupt();
        FD_ZERO(&rfds);
        FD_SET(_pipefd_read, &rfds);
        FD_SET(dma_fd, &rfds);
        int select_ret = select(nfds, &rfds, NULL, NULL, NULL);

        BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "DataHandler: select =" << select_ret;

        if (select_ret < 0) {
            BOOST_LOG_SEV(_slg, blt::severity_level::fatal)
                << "DataHandler: select failed with code " << select_ret;
            return;
        }

        if (FD_ISSET(_pipefd_read, &rfds)) {
            BOOST_LOG_SEV(_slg, blt::severity_level::debug) << "DataHandler: stopping thread";
            return;
        }

        if (FD_ISSET(dma_fd, &rfds)) {
            uint32_t irq_count = _dma.clear_interrupt();
            BOOST_LOG_SEV(_slg, blt::severity_level::trace)
                << "DataHandler: irq count = " << irq_count;

            std::vector<UioMemSgdma::BufInfo> full_bufs = _desc.get_full_buffers();
            std::vector<uint8_t> bytes;

            for (auto &buf : full_bufs) {
                _mem.copy_from_buf(buf.buf_addr, buf.buf_len, bytes);
            }

            process_data(bytes);
        }
    }
}
