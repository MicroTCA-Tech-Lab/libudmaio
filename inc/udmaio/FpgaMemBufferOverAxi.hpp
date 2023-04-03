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
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "DmaBufferAbstract.hpp"
#include "UioIf.hpp"

namespace udmaio {

/// DMA data buffer accessed over AXI/UIO, described w/ explicit address & size
class FpgaMemBufferOverAxi : public DmaBufferAbstract, public UioIf {
  public:
    FpgaMemBufferOverAxi(HwAccessorPtr hw) : UioIf("FpgaMemBufferOverAxi", std::move(hw)) {}

    uintptr_t get_phys_addr() const override { return _region.addr; }

    uintptr_t get_phys_size() const override { return _region.size; }

  protected:
    void copy_from_buf(uint8_t* dest, const UioRegion& buf_info) const override {
        BOOST_LOG_SEV(_lg, bls::trace)
            << "FpgaMemBufferOverAxi: append_from_buf: buf_info.addr = 0x" << std::hex
            << buf_info.addr << std::dec;
        BOOST_LOG_SEV(_lg, bls::trace)
            << "FpgaMemBufferOverAxi: append_from_buf: buf_info.size = " << buf_info.size;
        uintptr_t mmap_addr = buf_info.addr - _region.addr;
        std::memcpy(dest, static_cast<uint8_t*>(_mem) + mmap_addr, buf_info.size);
    }
};

}  // namespace udmaio
