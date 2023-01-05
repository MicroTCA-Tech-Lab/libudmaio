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

#include <cstdint>
#include <vector>

#include <boost/core/noncopyable.hpp>

#include "udmaio/UioConfig.hpp"

namespace udmaio {

// Base class for DMA data buffer
class DmaBufferAbstract : private boost::noncopyable {
  public:
    virtual uintptr_t get_phys_addr() const = 0;
    virtual uintptr_t get_phys_size() const = 0;
    virtual void copy_from_buf(const UioRegion &buf_info, std::vector<uint8_t> &out) const = 0;
};

} // namespace udmaio
