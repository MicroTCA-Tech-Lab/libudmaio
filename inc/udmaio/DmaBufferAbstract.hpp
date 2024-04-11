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

/// Base class for DMA data buffer
class DmaBufferAbstract : private boost::noncopyable {
  protected:
    virtual void copy_from_buf(uint8_t* dest, const UioRegion& buf_info) const = 0;

  public:
    /// @brief Get physical region
    /// @return Physical address and size of DMA data buffer
    virtual UioRegion get_phys_region() const = 0;

    /// @brief Append received DMA data to vector
    /// @param buf_info Memory region of DMA buffer
    /// @param out Vector to append received data to
    void append_from_buf(const UioRegion& buf_info, std::vector<uint8_t>& out) const;
};

}  // namespace udmaio
