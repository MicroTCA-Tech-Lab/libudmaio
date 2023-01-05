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

#include <boost/log/core/core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "DmaBufferAbstract.hpp"

namespace udmaio {

/// DMA data buffer accessed over XDMA using the xdma c2h0 stream channel
class FpgaMemBufferOverXdma : public DmaBufferAbstract {
    int _dma_fd;
    uintptr_t _phys_addr;

  public:
    /// @brief Constructs a DMA data buffer
    /// @param path Base path of XDMA instance in `/dev`
    /// @param phys_addr Physical address of DMA data buffer
    explicit FpgaMemBufferOverXdma(const std::string& path, uintptr_t phys_addr)
        : _phys_addr{phys_addr} {
        const std::string dev_path{path + "/c2h0"};
        _dma_fd = open(dev_path.c_str(), O_RDWR);
        if (_dma_fd < 0) {
            throw std::runtime_error("could not open " + dev_path);
        }
    }
    virtual ~FpgaMemBufferOverXdma() { close(_dma_fd); }

    uintptr_t get_phys_addr() const override { return _phys_addr; }

    uintptr_t get_phys_size() const override {
        // there is no way to get the size over the Xdma, return 0 and let the user handle it
        return 0;
    }

    void copy_from_buf(const UioRegion& buf_info, std::vector<uint8_t>& out) const override {
        size_t old_size = out.size();
        size_t new_size = old_size + buf_info.size;
        out.resize(new_size);
        lseek(_dma_fd, buf_info.addr, SEEK_SET);
        ssize_t rc = read(_dma_fd, out.data() + old_size, buf_info.size);
        if (rc < static_cast<ssize_t>(buf_info.size)) {
            // TODO: error handling
        }
    }
};

}  // namespace udmaio
