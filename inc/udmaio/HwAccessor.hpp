//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2023 Deutsches Elektronen-Synchrotron DESY

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

/// Base class for hardware access
class HwAccessor : public Logger, private boost::noncopyable {
    template <typename C>
    friend class RegAccessorBase;
    friend class UioIf;

  protected:
    bool _debug_enable;

  public:
    HwAccessor();
    virtual ~HwAccessor();

    // Enable raw register access log
    void enable_debug(bool enable);

    virtual uint32_t _rd32(uint32_t offs) const = 0;
    virtual void _wr32(uint32_t offs, uint32_t data) = 0;

    // Default implementation uses 32 bit accesses
    // Can be overridden by subclass if it supports native 64 bit access
    virtual uint64_t _rd64(uint32_t offs) const;

    // Default implementation uses 32 bit accesses
    // Can be overridden by subclass if it supports native 64 bit access
    virtual void _wr64(uint32_t offs, uint64_t data);

    void _rd_mem32(uint32_t offs, void* __restrict__ mem, size_t size) const;
    void _rd_mem64(uint32_t offs, void* __restrict__ mem, size_t size) const;

    void _wr_mem32(uint32_t offs, const void* __restrict__ mem, size_t size);
    void _wr_mem64(uint32_t offs, const void* __restrict__ mem, size_t size);

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
            _rd_mem64(offs, reinterpret_cast<void*>(&result), sizeof(C));
        } else {
            _rd_mem32(offs, reinterpret_cast<void*>(&result), sizeof(C));
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
            _wr_mem64(offs, reinterpret_cast<const void*>(&value), sizeof(C));
        } else {
            _wr_mem32(offs, reinterpret_cast<const void*>(&value), sizeof(C));
        }

        // Prevent compiler & CPU from re-ordering. Make sure the register is actually written here
        __sync_synchronize();
    }

    virtual void arm_interrupt();
    virtual uint32_t wait_for_interrupt();
    virtual int get_fd_int() const { return -1; }
    virtual uintptr_t get_phys_addr() const { return 0; }
};

// Base class for mmap'ed hardware access
template <typename max_access_width_t>
class HwAccessorMmap : public HwAccessor {
  public:
    HwAccessorMmap(std::string dev_path, UioRegion region, uintptr_t mmap_offs)
        : HwAccessor(), _region{region} {
        BOOST_LOG_SEV(HwAccessor::_lg, bls::debug) << "dev name = " << dev_path;

        _fd = ::open(dev_path.c_str(), O_RDWR);
        if (_fd < 0) {
            throw std::runtime_error("could not open " + dev_path);
        }
        BOOST_LOG_SEV(HwAccessor::_lg, bls::trace) << "fd = " << _fd << ", size = " << _region.size;

        // create memory mapping
        _mem = mmap(NULL, _region.size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, mmap_offs);
        BOOST_LOG_SEV(HwAccessor::_lg, bls::trace)
            << "mmap = 0x" << std::hex << mmap_offs << " -> 0x" << _mem << std::dec;

        if (_mem == MAP_FAILED) {
            throw std::runtime_error("mmap failed for uio " + dev_path);
        }
    }

    virtual ~HwAccessorMmap() {
        munmap(_mem, _region.size);
        ::close(_fd);
    }

  protected:
    int _fd;
    void* _mem;
    const UioRegion _region;

    template <typename access_width_t>
    inline volatile access_width_t* _mem_ptr(uint32_t offs) const {
        return reinterpret_cast<access_width_t*>(static_cast<uint8_t*>(_mem) +
                                                 static_cast<ptrdiff_t>(offs));
    }

    uint32_t _rd32(uint32_t offs) const final override {
        const uint32_t tmp = *_mem_ptr<uint32_t>(offs);
        if (_debug_enable) {
            BOOST_LOG_SEV(_lg, bls::trace)
                << "0x" << std::hex << offs << " --> 0x" << std::setw(sizeof(uint32_t) * 2)
                << std::setfill('0') << tmp << std::dec;
        }
        return tmp;
    }

    void _wr32(uint32_t offs, const uint32_t data) final override {
        if (_debug_enable) {
            BOOST_LOG_SEV(_lg, bls::trace)
                << "0x" << std::hex << offs << " <-- 0x" << std::setw(sizeof(uint32_t) * 2)
                << std::setfill('0') << data << std::dec;
        }
        *_mem_ptr<uint32_t>(offs) = data;
    }

    uint64_t _rd64(uint32_t offs) const final override {
        if constexpr (sizeof(max_access_width_t) == sizeof(uint64_t)) {
            // We support native 64-bit access
            const uint64_t tmp = *_mem_ptr<uint64_t>(offs);
            if (_debug_enable) {
                BOOST_LOG_SEV(_lg, bls::trace)
                    << "0x" << std::hex << offs << " --> 0x" << std::setw(sizeof(uint64_t) * 2)
                    << std::setfill('0') << tmp << std::dec;
            }
            return tmp;
        } else {
            // Use default (32-bit) implementation
            return HwAccessor::_rd64(offs);
        }
    }

    void _wr64(uint32_t offs, const uint64_t data) final override {
        if constexpr (sizeof(max_access_width_t) == sizeof(uint64_t)) {
            // We support native 64-bit access
            if (_debug_enable) {
                BOOST_LOG_SEV(_lg, bls::trace)
                    << "0x" << std::hex << offs << " <-- 0x" << std::setw(sizeof(uint64_t) * 2)
                    << std::setfill('0') << data << std::dec;
            }
            *_mem_ptr<uint64_t>(offs) = data;
        } else {
            // Use default (32-bit) implementation
            HwAccessor::_wr64(offs, data);
        }
    }

    uint32_t wait_for_interrupt() final override {
        const int fd_int = get_fd_int();
        uint32_t irq_count;
        BOOST_LOG_SEV(_lg, bls::trace) << "wait for interrupt ...";
        if (fd_int < 0) {
            BOOST_LOG_SEV(_lg, bls::fatal) << "fd_int not set";
        }
        int rc = read(fd_int, &irq_count, sizeof(irq_count));
        BOOST_LOG_SEV(_lg, bls::trace)
            << "interrupt received, rc = " << rc << ", irq count = " << irq_count;
        return irq_count;
    }

  public:
    uintptr_t get_phys_addr() const final override { return _region.addr; }
};

// Hardware accessor for XDMA. Can support either 32 or 64 bit access
template <typename max_access_width_t>
class HwAccessorXdma : public HwAccessorMmap<max_access_width_t> {
    int _fd_int;

  public:
    HwAccessorXdma(std::string xdma_path,
                   std::string evt_dev,
                   UioRegion region,
                   uintptr_t pcie_offs)
        : HwAccessorMmap<max_access_width_t>(xdma_path + "/user",
                                             {region.addr | pcie_offs, region.size},
                                             region.addr) {
        if (!evt_dev.empty()) {
            const auto evt_path = xdma_path + "/" + evt_dev;
            _fd_int = ::open(evt_path.c_str(), O_RDWR);

            if (_fd_int < 0) {
                throw std::runtime_error("could not open " + evt_path);
            }
            BOOST_LOG_SEV(HwAccessor::_lg, bls::trace) << "fd_int =  " << _fd_int;
        } else {
            _fd_int = -1;
        }
    }

    virtual ~HwAccessorXdma() {
        if (_fd_int > 0) {
            ::close(_fd_int);
        }
    }

    int get_fd_int() const final override { return _fd_int; }
};  // namespace udmaio

// Hardware accessor for AXI. Always supports 64 bit access
class HwAccessorAxi : public HwAccessorMmap<uint64_t> {
  public:
    HwAccessorAxi(std::string dev_path, UioRegion region, uintptr_t mmap_offs);
    virtual ~HwAccessorAxi();

    int get_fd_int() const final override { return _fd; }

    void arm_interrupt() final override {
        uint32_t mask = 1;
        int rc = write(_fd, &mask, sizeof(mask));
        BOOST_LOG_SEV(HwAccessor::_lg, bls::trace) << "arm interrupt enable, ret code = " << rc;
    }
};

// Base class for mock hardware (for unit tests)
class HwAccessorMock : public HwAccessor {
    mutable std::vector<uint8_t> _mem;

  public:
    HwAccessorMock(size_t mem_size) : HwAccessor{}, _mem(mem_size, 0) {}

    virtual ~HwAccessorMock() {}

  protected:
    template <typename access_width_t>
    inline volatile access_width_t* _mem_ptr(uint32_t offs) const {
        if (offs + sizeof(access_width_t) > _mem.size()) {
            throw std::runtime_error("Buffer overflow (" + std::to_string(sizeof(access_width_t)) +
                                     " bytes at " + std::to_string(offs) + ")");
        }
        return reinterpret_cast<access_width_t*>(&_mem[offs]);
    }

    uint32_t _rd32(uint32_t offs) const final override {
        const uint32_t tmp = *_mem_ptr<uint32_t>(offs);
        if (_debug_enable) {
            BOOST_LOG_SEV(_lg, bls::trace)
                << "0x" << std::hex << offs << " --> 0x" << std::setw(sizeof(uint32_t) * 2)
                << std::setfill('0') << tmp << std::dec;
        }
        return tmp;
    }

    void _wr32(uint32_t offs, const uint32_t data) final override {
        if (_debug_enable) {
            BOOST_LOG_SEV(_lg, bls::trace)
                << "0x" << std::hex << offs << " <-- 0x" << std::setw(sizeof(uint32_t) * 2)
                << std::setfill('0') << data << std::dec;
        }
        *_mem_ptr<uint32_t>(offs) = data;
    }
};

}  // namespace udmaio
