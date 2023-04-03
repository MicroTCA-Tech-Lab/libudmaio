//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2023 Deutsches Elektronen-Synchrotron DESY

#include "udmaio/HwAccessor.hpp"

#include <regex>

namespace udmaio {

HwAccessor::HwAccessor(bool debug_enable) : Logger("HwAccessor"), _debug_enable(debug_enable) {}

HwAccessor::~HwAccessor() {}

uint64_t HwAccessor::_rd64(uint32_t offs) const {
    uint64_t result;
    uint32_t* const tmp = reinterpret_cast<uint32_t*>(&result);
    tmp[0] = _rd32(offs);
    tmp[1] = _rd32(offs + sizeof(uint32_t));
    return result;
}

// Default implementation uses 32 bit accesses
// Can be overridden by subclass if it supports native 64 bit access
void HwAccessor::_wr64(uint32_t offs, uint64_t data) {
    uint32_t* const tmp = reinterpret_cast<uint32_t*>(&data);
    _wr32(offs, tmp[0]);
    _wr32(offs + sizeof(uint32_t), tmp[1]);
}

void HwAccessor::_rd_mem32(uint32_t offs, void* __restrict__ mem, size_t size) const {
    uint32_t* __restrict__ ptr = reinterpret_cast<uint32_t*>(mem);
    for (uint32_t i = 0; i < size; i += sizeof(uint32_t)) {
        *ptr++ = _rd32(offs + i);
    }
}

void HwAccessor::_rd_mem64(uint32_t offs, void* __restrict__ mem, size_t size) const {
    uint64_t* __restrict__ ptr = reinterpret_cast<uint64_t*>(mem);
    for (uint32_t i = 0; i < size; i += sizeof(uint64_t)) {
        *ptr++ = _rd64(offs + i);
    }
}

void HwAccessor::_wr_mem32(uint32_t offs, const void* __restrict__ mem, size_t size) {
    // volatile needed to prevent the compiler from optimizing away the memory read
    const volatile uint32_t* ptr = reinterpret_cast<const volatile uint32_t*>(mem);
    for (uint32_t i = 0; i < size; i += sizeof(uint32_t)) {
        _wr32(offs + i, *ptr++);
    }
}

void HwAccessor::_wr_mem64(uint32_t offs, const void* __restrict__ mem, size_t size) {
    // volatile needed to prevent the compiler from optimizing away the memory read
    const volatile uint64_t* ptr = reinterpret_cast<const volatile uint64_t*>(mem);
    for (uint32_t i = 0; i < size; i += sizeof(uint64_t)) {
        _wr64(offs + i, *ptr++);
    }
}

void HwAccessor::arm_interrupt() {}

uint32_t HwAccessor::wait_for_interrupt() {
    return 0;
}

HwAccessorAxi::HwAccessorAxi(std::string dev_path,
                             UioRegion region,
                             uintptr_t mmap_offs,
                             bool debug_enable)
    : HwAccessorMmap<uint64_t>{dev_path, region, mmap_offs, debug_enable} {}

HwAccessorAxi::~HwAccessorAxi() {}

}  // namespace udmaio
