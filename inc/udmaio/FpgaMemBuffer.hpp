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

namespace udmaio {

class FpgaMemBuffer : public DmaBufferAbstract {
    int _dma_fd;
    uint64_t _phys_addr;

  public:
    explicit FpgaMemBuffer(uint64_t phys_addr) : _phys_addr{phys_addr} {
        _dma_fd = open("/dev/xdma/card0/c2h0", O_RDWR);
        if (_dma_fd < 0) {
            throw std::runtime_error("could not open /dev/xdma/card0/c2h0");
        }
    }
    virtual ~FpgaMemBuffer() {
        close(_dma_fd);
    }

    uint64_t get_phys_addr() const override {
        return _phys_addr;
    }

    void copy_from_buf(const UioMemSgdma::BufInfo &buf_info, std::vector<uint8_t> &out) const override {
        size_t old_size = out.size();
        size_t new_size = old_size + buf_info.len;
        out.resize(new_size);
        lseek(_dma_fd, buf_info.addr, SEEK_SET);
        ssize_t rc = read(_dma_fd, out.data() + old_size, buf_info.len);
        if (rc < static_cast<ssize_t>(buf_info.len)) {
            // TODO: error handling
        }
    }
};

} // namespace udmaio
