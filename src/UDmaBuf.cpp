//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include "udmaio/UDmaBuf.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace udmaio {

UDmaBuf::UDmaBuf(int buf_idx)
    : Logger("UDmaBuf"), _phys{_get_phys_addr(buf_idx), _get_size(buf_idx)} {
    BOOST_LOG_SEV(_lg, bls::debug) << "size      = " << _phys.size;
    BOOST_LOG_SEV(_lg, bls::debug) << "phys addr = " << std::hex << _phys.addr << std::dec;

    std::string dev_path{"/dev/udmabuf" + std::to_string(buf_idx)};
    _fd = ::open(dev_path.c_str(), O_RDWR | O_SYNC);
    if (_fd < 0) {
        throw std::runtime_error("could not open " + dev_path);
    }
    BOOST_LOG_SEV(_lg, bls::trace) << "fd =  " << _fd << ", size = " << _phys.size;
    _mem = mmap(NULL, _phys.size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
    BOOST_LOG_SEV(_lg, bls::trace) << "mmap = " << _mem;
    if (_mem == MAP_FAILED) {
        throw std::runtime_error("mmap failed for " + dev_path);
    }
}

size_t UDmaBuf::_get_size(int buf_idx) {
    std::string path{"/sys/class/u-dma-buf/udmabuf" + std::to_string(buf_idx) + "/size"};
    std::ifstream ifs{path};
    if (!ifs) {
        throw std::runtime_error("could not find size for udmabuf " + std::to_string(buf_idx));
    }
    std::string size_str;
    ifs >> size_str;
    return std::stoull(size_str, nullptr, 0);
}

uintptr_t UDmaBuf::_get_phys_addr(int buf_idx) {
    std::string path{"/sys/class/u-dma-buf/udmabuf" + std::to_string(buf_idx) + "/phys_addr"};
    std::ifstream ifs{path};
    if (!ifs) {
        throw std::runtime_error("could not find phys_addr for udmabuf " + std::to_string(buf_idx));
    }
    std::string phys_addr_str;
    ifs >> phys_addr_str;
    return std::stoull(phys_addr_str, nullptr, 0);
}

UDmaBuf::~UDmaBuf() {
    munmap(_mem, _phys.size);
    ::close(_fd);
}

uintptr_t UDmaBuf::get_phys_addr() const {
    return _phys.addr;
}

uintptr_t UDmaBuf::get_phys_size() const {
    return _phys.size;
}

void UDmaBuf::copy_from_buf(uint8_t* dest, const UioRegion& buf_info) const {
    uintptr_t mmap_addr = buf_info.addr - _phys.addr;
    std::memcpy(dest, static_cast<uint8_t*>(_mem) + mmap_addr, buf_info.size);
}

}  // namespace udmaio
