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
#include <string_view>

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
    template <typename C>
    friend class RegAccessorBase;

  public:
    /// @brief Construct the UioIf from a UioDeviceInfo
    /// @param dev UioDeviceInfo describing the connection information
    UioIf(UioDeviceInfo dev);

    virtual ~UioIf();

  protected:
    int _fd, _fd_int;
    void* _mem;
    UioRegion _region;
    mutable boost::log::sources::severity_logger_mt<blt::severity_level> _slg;
    bool _skip_write_to_arm_int;

    volatile uint32_t* _reg_ptr32(uint32_t offs) const {
        return static_cast<volatile uint32_t*>(_mem) + offs / 4;
    }
    volatile uint64_t* _reg_ptr64(uint32_t offs) const {
        return static_cast<volatile uint64_t*>(_mem) + offs / 8;
    }

    template <typename T>
    uint32_t reg_to_raw(T data) {
        static_assert(sizeof(T) == 4, "register must be 32 bit");
        return *reinterpret_cast<uint32_t*>(&data);
    }

    uint32_t _rd32(uint32_t offs) const;
    uint64_t _rd64(uint32_t offs) const;
    void _wr32(uint32_t offs, uint32_t data);
    void _wr64(uint32_t offs, uint64_t data);

    void arm_interrupt();
    uint32_t wait_for_interrupt();

    virtual const std::string_view _log_name() const = 0;
};

}  // namespace udmaio
