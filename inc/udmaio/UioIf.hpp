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

#include "udmaio/Logging.hpp"
#include "udmaio/UioConfig.hpp"

namespace udmaio {

/// Base class for UIO interfaces
class UioIf : public Logger, private boost::noncopyable {
    template <typename C>
    friend class RegAccessorBase;

  public:
    /// @brief Construct the UioIf from a UioDeviceInfo
    /// @param name Channel name for logger
    /// @param dev UioDeviceInfo describing the connection information
    /// @param debug_enable Enable raw register access logging when loglevel is at bls::trace
    UioIf(std::string name, UioDeviceInfo dev, bool debug_enable = false);

    virtual ~UioIf();

  protected:
    int _fd, _fd_int;
    void* _mem;
    const UioRegion _region;
    const bool _skip_write_to_arm_int;
    const bool _debug_enable;
    const bool _force_32bit;

    template <typename C, typename = std::enable_if_t<(sizeof(C) == 4)>>
    static uint32_t reg_to_raw(C data) {
        return *reinterpret_cast<uint32_t*>(&data);
    }

    template <typename C, typename = std::enable_if_t<(sizeof(C) == 8)>>
    static uint64_t reg_to_raw(C data) {
        return *reinterpret_cast<uint64_t*>(&data);
    }

    template <typename access_width_t>
    inline volatile access_width_t* _reg_ptr(uint32_t offs) const {
        return reinterpret_cast<access_width_t*>(static_cast<uint8_t*>(_mem) +
                                                 static_cast<ptrdiff_t>(offs));
    }

    /**
     * @brief Read single data word from UIO interface
     * 
     * @tparam access_width_t Data width (uint32_t or uint64_t)
     * @param offs Offset into address space
     * @return access_width_t Data value
     */
    template <typename access_width_t>
    access_width_t _rd(uint32_t offs) const {
        const access_width_t tmp = *_reg_ptr<access_width_t>(offs);
        if (_debug_enable) {
            BOOST_LOG_SEV(_lg, bls::trace)
                << "0x" << std::hex << offs << " --> 0x" << std::setw(sizeof(access_width_t) * 2)
                << std::setfill('0') << tmp << std::dec;
        }
        return tmp;
    }

    /**
     * @brief Write single data word to UIO interface
     * 
     * @tparam access_width_t Data width (uint32_t or uint64_t)
     * @param offs Offset into address space
     * @param data Data value
     */
    template <typename access_width_t>
    void _wr(uint32_t offs, const access_width_t data) {
        if (_debug_enable) {
            BOOST_LOG_SEV(_lg, bls::trace)
                << "0x" << std::hex << offs << " <-- 0x" << std::setw(sizeof(access_width_t) * 2)
                << std::setfill('0') << data << std::dec;
        }
        *_reg_ptr<access_width_t>(offs) = data;
    }

    inline uint32_t _rd32(uint32_t offs) const { return _rd<uint32_t>(offs); }
    inline uint64_t _rd64(uint32_t offs) const { return _rd<uint64_t>(offs); }
    inline void _wr32(uint32_t offs, uint32_t data) { _wr<uint32_t>(offs, data); }
    inline void _wr64(uint32_t offs, uint64_t data) { _wr<uint64_t>(offs, data); }

    /**
     * @brief Read data blob from UIO interface
     * 
     * @tparam access_width_t Access width (uint32_t or uint64_t)
     * @param offs Offset into address space
     * @param mem Buffer to write to
     * @param size Buffer size
     */
    template <typename access_width_t>
    void _rd_mem(uint32_t offs, void* __restrict__ mem, size_t size) const {
        access_width_t* __restrict__ ptr = reinterpret_cast<access_width_t*>(mem);
        for (uint32_t i = 0; i < size; i += sizeof(access_width_t)) {
            *ptr++ = _rd<access_width_t>(offs + i);
        }
    }

    /**
     * @brief Write data blob to UIO interface
     * 
     * @tparam access_width_t Access width (uint32_t or uint64_t)
     * @param offs Offset into address space
     * @param mem Buffer to read from
     * @param size Buffer size
     */
    template <typename access_width_t>
    void _wr_mem(uint32_t offs, const void* __restrict__ mem, size_t size) {
        // volatile needed to prevent the compiler from optimizing away the memory read
        const volatile access_width_t* ptr = reinterpret_cast<const volatile access_width_t*>(mem);
        for (uint32_t i = 0; i < size; i += sizeof(access_width_t)) {
            _wr<access_width_t>(offs + i, *ptr++);
        }
    }

    /**
     * @brief Read register from UIO interface
     * 
     * @tparam C Register data type
     * @param offs Register offset into address space
     * @return C Value read from register
     */
    template <typename C>
    C _rd_reg(uint32_t offs) const {
        static_assert(sizeof(C) != 0);
        static_assert(sizeof(C) % sizeof(uint32_t) == 0);

        C result;
        if constexpr (sizeof(C) > sizeof(uint32_t) && sizeof(C) % sizeof(uint64_t) == 0) {
            if (!_force_32bit) {
                _rd_mem<uint64_t>(offs, reinterpret_cast<void*>(&result), sizeof(C));
            } else {
                _rd_mem<uint32_t>(offs, reinterpret_cast<void*>(&result), sizeof(C));
            }
        } else {
            _rd_mem<uint32_t>(offs, reinterpret_cast<void*>(&result), sizeof(C));
        }

        return result;
    }

    /**
     * @brief Write register to UIO interface
     * 
     * @tparam C Register data type
     * @param offs Register offset into address space
     * @param value Value to write
     */
    template <typename C>
    void _wr_reg(uint32_t offs, const C& value) {
        static_assert(sizeof(C) != 0);
        static_assert(sizeof(C) % sizeof(uint32_t) == 0);

        if constexpr (sizeof(C) > sizeof(uint32_t) && sizeof(C) % sizeof(uint64_t) == 0) {
            if (!_force_32bit) {
                _wr_mem<uint64_t>(offs, reinterpret_cast<const void*>(&value), sizeof(C));
            } else {
                _wr_mem<uint32_t>(offs, reinterpret_cast<const void*>(&value), sizeof(C));
            }
        } else {
            _wr_mem<uint32_t>(offs, reinterpret_cast<const void*>(&value), sizeof(C));
        }

        // Prevent compiler & CPU from re-ordering. Make sure the register is actually written here
        __sync_synchronize();
    }

    void arm_interrupt();
    uint32_t wait_for_interrupt();
};

}  // namespace udmaio
