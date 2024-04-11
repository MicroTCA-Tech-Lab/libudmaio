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

#include "DmaBufferAbstract.hpp"
#include "Logging.hpp"
#include "udmaio/UioConfig.hpp"

namespace udmaio {

/// @brief DMA data buffer accessed over AXI/UIO, implemented w/ udmabuf
/// (see https://github.com/ikwzm/udmabuf)
class UDmaBuf : public DmaBufferAbstract, public Logger {
    int _fd;
    void* _mem;
    const UioRegion _phys;

    static size_t _get_size(int buf_idx);
    static uintptr_t _get_phys_addr(int buf_idx);
    void copy_from_buf(uint8_t* dest, const UioRegion& buf_info) const override;

  public:
    /// @brief Constructs a UDmaBuf
    /// @param buf_idx Buffer index `/dev/udmabufN`
    explicit UDmaBuf(int buf_idx = 0);

    virtual ~UDmaBuf();

    UioRegion get_phys_region() const override;
};

}  // namespace udmaio
