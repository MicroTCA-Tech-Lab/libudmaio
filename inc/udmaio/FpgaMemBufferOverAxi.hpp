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

#include <boost/log/core/core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

#include "DmaBufferAbstract.hpp"
#include "UioIf.hpp"

namespace blt = boost::log::trivial;

namespace udmaio {

// DMA data buffer accessed over AXI/UIO, described w/ explicit address & size
class FpgaMemBufferOverAxi : public DmaBufferAbstract, public UioIf {

    virtual const std::string_view _log_name() const override {
        return "FpgaMemBufferOverAxi";
    }

  public:
    using UioIf::UioIf;

    uintptr_t get_phys_addr() const override { return _region.addr; }

    uintptr_t get_phys_size() const override { return _region.size; }

    void copy_from_buf(const UioRegion &buf_info, std::vector<uint8_t> &out) const override {
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << "FpgaMemBufferOverAxi: copy_from_buf: buf_info.addr = 0x" << std::hex
            << buf_info.addr << std::dec;
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << "FpgaMemBufferOverAxi: copy_from_buf: buf_info.size = " << buf_info.size;
        size_t old_size = out.size();
        size_t new_size = old_size + buf_info.size;
        uintptr_t mmap_addr = buf_info.addr - _region.addr;
        out.resize(new_size);
        std::memcpy(out.data() + old_size, static_cast<uint8_t *>(_mem) + mmap_addr, buf_info.size);
    }
};

} // namespace udmaio
