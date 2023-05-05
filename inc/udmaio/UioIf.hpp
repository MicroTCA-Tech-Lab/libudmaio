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
#include <string>

#include <boost/core/noncopyable.hpp>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "udmaio/HwAccessor.hpp"
#include "udmaio/Logging.hpp"
#include "udmaio/UioConfig.hpp"

namespace udmaio {

/// Base class for UIO interfaces
class UioIf : private boost::noncopyable {
    template <typename C>
    friend class RegAccessorBase;

  public:
    UioIf(std::string name, HwAccessorPtr hw);

    virtual ~UioIf();

    /// @brief Get file descriptor of interrupt event file
    /// @return Event file descriptor
    int get_fd_int() const;

    void enable_debug(bool enable);

  protected:
    HwAccessorPtr _hw;
    boost_logger& _lg;

    template <typename C, typename = std::enable_if_t<(sizeof(C) == 4)>>
    static uint32_t reg_to_raw(C data) {
        union {
            uint32_t raw_val;
            C cooked_val;
        } u;
        u.cooked_val = data;
        return u.raw_val;
    }

    template <typename C, typename = std::enable_if_t<(sizeof(C) == 8)>>
    static uint64_t reg_to_raw(C data) {
        union {
            uint64_t raw_val;
            C cooked_val;
        } u;
        u.cooked_val = data;
        return u.raw_val;
    }

    inline uint32_t _rd32(uint32_t offs) const { return _hw->_rd32(offs); }
    inline uint64_t _rd64(uint32_t offs) const { return _hw->_rd64(offs); }
    inline void _wr32(uint32_t offs, uint32_t data) { _hw->_wr32(offs, data); }
    inline void _wr64(uint32_t offs, uint64_t data) { _hw->_wr64(offs, data); }

    template <typename C>
    C _rd_reg(uint32_t offs) const {
        return _hw->template _rd_reg<C>(offs);
    }

    template <typename C>
    void _wr_reg(uint32_t offs, const C& value) {
        _hw->template _wr_reg<C>(offs, value);
    }

    void arm_interrupt();
    uint32_t wait_for_interrupt();
};

}  // namespace udmaio
