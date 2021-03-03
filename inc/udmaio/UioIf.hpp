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

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/log/core/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/keywords/severity.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

namespace blt = boost::log::trivial;

namespace udmaio {

class UioIf {
  public:
    explicit UioIf(const std::string &uio_name, uintptr_t addr, size_t size, uintptr_t offs = 0,
                   const std::string &event_filename = "", bool skip_write_to_arm_int = false)
        : _int_addr{addr}, _int_size{size}, _slg{}, _skip_write_to_arm_int{skip_write_to_arm_int} {

        BOOST_LOG_SEV(_slg, blt::severity_level::debug) << _log_name << ": uio name = " << uio_name;

        // open fd
        _fd = ::open(uio_name.c_str(), O_RDWR);
        if (_fd < 0) {
            throw std::runtime_error("could not open " + uio_name);
        }
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << _log_name << ": fd =  " << _fd << ", size = " << size;

        // create memory mapping
        _mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, offs);
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << _log_name << ": mmap = 0x" << _mem << std::dec;
        if (_mem == MAP_FAILED) {
            throw std::runtime_error("mmap failed for uio " + uio_name);
        }

        // if there is an event filename, open event fd - otherwise use the same as for the mmap
        if (event_filename.empty()) {
            _fd_int = _fd;
            BOOST_LOG_SEV(_slg, blt::severity_level::trace) << _log_name << ": using fd for fd_int";
        } else {
            _fd_int = ::open(event_filename.c_str(), O_RDWR);

            if (_fd_int < 0) {
                throw std::runtime_error("could not open " + event_filename);
            }
            BOOST_LOG_SEV(_slg, blt::severity_level::trace)
                << _log_name << ": fd_int =  " << _fd_int;
        }
    }

  protected:
    ~UioIf() {
        munmap(_mem, _int_size);
        ::close(_fd);
    }

    int _fd, _fd_int;
    void *_mem;
    uintptr_t _int_addr;
    size_t _int_size;
    boost::log::sources::severity_logger<blt::severity_level> _slg;
    static constexpr std::string_view _log_name{""};
    bool _skip_write_to_arm_int;

    uint32_t _rd32(uint32_t offs) {
        uint32_t tmp = *(static_cast<uint32_t *>(_mem) + offs / 4);
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << _log_name << ": read at 0x" << std::hex << offs << " = 0x" << tmp << std::dec;
        return tmp;
    }

    void _wr32(uint32_t offs, uint32_t data) {
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << _log_name << ": write at 0x" << std::hex << offs << " = 0x" << data << std::dec;
        *(static_cast<uint32_t *>(_mem) + offs / 4) = data;
    }
};

} // namespace udmaio
