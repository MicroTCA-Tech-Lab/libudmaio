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

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <boost/log/core/core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

#include "DmaBufferAbstract.hpp"

namespace dmamgmt {

class FpgaDdr4Buffer : public DmaBufferAbstract {
    int _dma_fd;

  public:
    explicit FpgaDdr4Buffer() {
        _dma_fd = open("/dev/xdma/card0/c2h0", O_RDWR);
        if (_dma_fd < 0) {
            throw std::runtime_error("could not open /dev/xdma/card0/c2h0");
        }
    }

    uint64_t get_phys_addr() {
        // FPGA DDR4 is at this address
        return 0x400000000UL;
    }

    void copy_from_buf(uint64_t buf_addr, uint32_t len, std::vector<uint8_t> &out) {
        size_t old_size = out.size();
        size_t new_size = old_size + len;
        out.resize(new_size);
        lseek(_dma_fd, buf_addr, SEEK_SET);
        int rc = read(_dma_fd, out.data() + old_size, len);
    }
};

} // namespace dmamgmt
