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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include <boost/core/noncopyable.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/keywords/severity.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "udmaio/UioConfig.hpp"
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace blt = boost::log::trivial;

namespace udmaio {

/// Base class for UIO interfaces
class UioIf : private boost::noncopyable {
  public:
    /// @brief Construct the UioIf
    /// @param dev UioDeviceInfo describing the connection information
    explicit UioIf(UioDeviceInfo dev)
        : _region{dev.region}, _slg{}, _skip_write_to_arm_int{!dev.evt_path.empty()} {
        // Can't call virtual fn from ctor, so can't use _log_name()
        BOOST_LOG_SEV(_slg, blt::severity_level::debug) << "UioIf: uio name = " << dev.dev_path;

        // open fd
        _fd = ::open(dev.dev_path.c_str(), O_RDWR);
        if (_fd < 0) {
            throw std::runtime_error("could not open " + dev.dev_path);
        }
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << "UioIf: fd =  " << _fd << ", size = " << _region.size;

        // create memory mapping
        _mem = mmap(NULL, _region.size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, dev.mmap_offs);
        BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "UioIf: mmap = 0x" << _mem << std::dec;
        if (_mem == MAP_FAILED) {
            throw std::runtime_error("mmap failed for uio " + dev.dev_path);
        }

        // if there is an event filename, open event fd - otherwise use the same as for the mmap
        if (dev.evt_path.empty()) {
            _fd_int = _fd;
            BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "UioIf: using fd for fd_int";
        } else {
            _fd_int = ::open(dev.evt_path.c_str(), O_RDWR);

            if (_fd_int < 0) {
                throw std::runtime_error("could not open " + dev.evt_path);
            }
            BOOST_LOG_SEV(_slg, blt::severity_level::trace) << "UioIf: fd_int =  " << _fd_int;
        }
    }

    virtual ~UioIf() {
        munmap(_mem, _region.size);
        if (_fd_int != _fd) {
            ::close(_fd_int);
        }
        ::close(_fd);
    }

  protected:
    int _fd, _fd_int;
    void* _mem;
    UioRegion _region;
    mutable boost::log::sources::severity_logger_mt<blt::severity_level> _slg;
    bool _skip_write_to_arm_int;

    volatile uint32_t* _reg_ptr(uint32_t offs) const {
        return static_cast<volatile uint32_t*>(_mem) + offs / 4;
    }

    template <typename T>
    struct reg_cast_t {
        union {
            uint32_t raw;
            T data;
        };
    };

    template <typename T>
    T _rd_reg(uint32_t offs) const {
        static_assert(sizeof(T) == 4, "register access must be 32 bit");
        reg_cast_t<T> tmp = {.raw = _rd32(offs)};
        return tmp.data;
    }

    template <typename T>
    void _wr_reg(uint32_t offs, T data) {
        static_assert(sizeof(T) == 4, "register access must be 32 bit");
        reg_cast_t<T> tmp = {.data = data};
        _wr32(offs, tmp.raw);
    }

    uint32_t _rd32(uint32_t offs) const {
        uint32_t tmp = *_reg_ptr(offs);
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << _log_name() << ": read at 0x" << std::hex << offs << " = 0x" << tmp << std::dec;
        return tmp;
    }

    void _wr32(uint32_t offs, uint32_t data) {
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << _log_name() << ": write at 0x" << std::hex << offs << " = 0x" << data << std::dec;
        *_reg_ptr(offs) = data;
    }

    void arm_interrupt() {
        if (_skip_write_to_arm_int)
            return;

        uint32_t mask = 1;
        int rc = write(_fd_int, &mask, sizeof(mask));
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << _log_name() << ": arm interrupt enable, ret code = " << rc;
    }

    uint32_t wait_for_interrupt() {
        uint32_t irq_count;
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << _log_name() << ": wait for interrupt ...";
        int rc = read(_fd_int, &irq_count, sizeof(irq_count));
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << _log_name() << ": interrupt received, rc = " << rc << ", irq count = " << irq_count;
        return irq_count;
    }

    virtual const std::string_view _log_name() const = 0;
};

}  // namespace udmaio
