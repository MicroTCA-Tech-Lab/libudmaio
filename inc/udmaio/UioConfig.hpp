#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

namespace udmaio {

enum class DmaMode { XDMA, UIO };

std::istream &operator>>(std::istream &in, DmaMode &mode);

struct UioRegion {
    uintptr_t addr;
    size_t size;
};

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

class UioConfigUio : public UioConfigBase {
    static int _get_uio_number(std::string_view name);
    static size_t _get_map_size(int uio_number, int map_index = 0);
    static std::uintptr_t _get_map_addr(int uio_number, int map_index = 0);

public:
    UioDeviceInfo operator()(std::string dev_name) override;
    UioDeviceInfo operator()(UioDeviceLocation dev_loc) override;
    DmaMode mode() override { return DmaMode::UIO; };
};

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
