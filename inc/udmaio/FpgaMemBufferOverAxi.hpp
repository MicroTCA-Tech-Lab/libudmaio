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

namespace blt = boost::log::trivial;

namespace udmaio {

class FpgaMemBufferOverAxi : public DmaBufferAbstract {
    UioRegion _phys;
    int _fd;
    void *_mem;
    mutable boost::log::sources::severity_logger_mt<blt::severity_level> _slg;

  public:
    explicit FpgaMemBufferOverAxi(uintptr_t phys_addr, uintptr_t size) : _phys{phys_addr, size} {
        BOOST_LOG_SEV(_slg, blt::severity_level::debug)
            << "FpgaMemBufferOverAxi: size      = " << _phys.size;
        BOOST_LOG_SEV(_slg, blt::severity_level::debug)
            << "FpgaMemBufferOverAxi: phys addr = " << std::hex << _phys.addr << std::dec;

        std::string mem_path{"/dev/mem"};
        _fd = open(mem_path.c_str(), O_RDWR | O_SYNC);
        if (_fd < 0) {
            throw std::runtime_error("could not open " + mem_path);
        }
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << "FpgaMemBufferOverAxi: fd =  " << _fd << ", size = " << _phys.size;
        _mem = mmap(NULL, _phys.size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, _phys.addr);
        BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "FpgaMemBufferOverAxi: mmap = " << _mem;
        if (_mem == MAP_FAILED) {
            throw std::runtime_error("mmap failed for " + mem_path);
        }
    }

    virtual ~FpgaMemBufferOverAxi() {
        munmap(_mem, _phys.size);
        close(_fd);
    }

    uintptr_t get_phys_addr() const override { return _phys.addr; }

    void copy_from_buf(const UioRegion &buf_info, std::vector<uint8_t> &out) const override {
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << "FpgaMemBufferOverAxi: copy_from_buf: buf_info.addr = 0x" << std::hex
            << buf_info.addr << std::dec;
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << "FpgaMemBufferOverAxi: copy_from_buf: buf_info.size = " << buf_info.size;
        size_t old_size = out.size();
        size_t new_size = old_size + buf_info.size;
        uintptr_t mmap_addr = buf_info.addr - _phys.addr;
        out.resize(new_size);
        std::memcpy(out.data() + old_size, static_cast<uint8_t *>(_mem) + mmap_addr, buf_info.size);
    }
};

} // namespace udmaio
