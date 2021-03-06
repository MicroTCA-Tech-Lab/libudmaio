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

#include <iostream>
#include <stdexcept>
#include <string>

namespace udmaio {

/// DMA access mode
enum class DmaMode {
    XDMA,   // PCIe XDMA driver
    UIO     // ARM userspace I/O
};

std::istream &operator>>(std::istream &in, DmaMode &mode);

/// General-purpose struct to define a memory area
struct UioRegion {
    uintptr_t addr;
    size_t size;
};

/// Holds information where a device can be found over both UIO and XDMA
struct UioDeviceLocation {
    UioDeviceLocation(std::string uioname, UioRegion xdmaregion, std::string xdmaevtdev = "")
    : uio_name(uioname), xdma_region(xdmaregion), xdma_evt_dev(xdmaevtdev) {};
    // Device name (from device tree) for access through UIO
    std::string uio_name;
    // Memory-mapped region for access through XDMA
    UioRegion xdma_region;
    // optional: event file for access through XDMA
    std::string xdma_evt_dev;
};

/// Data needed to construct an UioIf; contains information how to connect to a device
struct UioDeviceInfo {
    std::string dev_path;
    std::string evt_path;
    UioRegion region;
    uintptr_t mmap_offs;
};

class UioConfigBase {
public:
    virtual std::string dev_path() { return ""; };
    virtual DmaMode mode() = 0;
    virtual UioDeviceInfo operator()([[maybe_unused]] std::string dev_name) {
        throw std::runtime_error("UioConfig: not implemented");
    };
    virtual UioDeviceInfo operator()([[maybe_unused]] UioRegion dev_region, [[maybe_unused]] std::string evt_dev = "") {
        throw std::runtime_error("UioConfig: not implemented");
    };
    virtual UioDeviceInfo operator()(UioDeviceLocation dev_loc) = 0;
};

// Creates UioDeviceInfo from UioDeviceLocation (UIO version)
class UioConfigUio : public UioConfigBase {
    static int _get_uio_number(std::string_view name);
    static size_t _get_map_size(int uio_number, int map_index = 0);
    static std::uintptr_t _get_map_addr(int uio_number, int map_index = 0);

public:
    UioDeviceInfo operator()(std::string dev_name) override;
    UioDeviceInfo operator()(UioDeviceLocation dev_loc) override;
    DmaMode mode() override { return DmaMode::UIO; };
};

// Creates UioDeviceInfo from UioDeviceLocation (XDMA version)
class UioConfigXdma : public UioConfigBase {
    std::string _xdma_path;
    uintptr_t _pcie_offs;

public:
    UioConfigXdma() = delete;
    UioConfigXdma(std::string xdma_path, uintptr_t pcie_offs);
    UioDeviceInfo operator()(UioRegion dev_region, std::string evt_dev = "") override;
    UioDeviceInfo operator()(UioDeviceLocation dev_loc) override;
    std::string dev_path() override { return _xdma_path; };
    DmaMode mode() override { return DmaMode::XDMA; };
};

} // namespace udmaio
