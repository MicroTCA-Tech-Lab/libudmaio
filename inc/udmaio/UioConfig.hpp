#pragma once

#include <stdexcept>
#include <string>

namespace udmaio {

struct UioRegion {
    uintptr_t addr;
    size_t size;
};

struct UioDeviceInfo {
    std::string dev_path;
    std::string evt_path;
    UioRegion region;
    uintptr_t mmap_offs;
};

class UioConfigBase {
public:
    virtual UioDeviceInfo operator()([[maybe_unused]] std::string dev_name) {
        throw std::runtime_error("UioConfig: not implemented");
    };
    virtual UioDeviceInfo operator()([[maybe_unused]] UioRegion dev_region, [[maybe_unused]] std::string evt_dev = "") {
        throw std::runtime_error("UioConfig: not implemented");
    };
};

class UioConfigUio : public UioConfigBase {
    static int _get_uio_number(std::string_view name);
    static size_t _get_map_size(int uio_number, int map_index = 0);
    static std::uintptr_t _get_map_addr(int uio_number, int map_index = 0);

public:
    UioDeviceInfo operator()(std::string dev_name);
};

class UioConfigXdma : public UioConfigBase {
    std::string _xdma_path;
    uintptr_t _pcie_offs;

public:
    UioConfigXdma() = delete;
    UioConfigXdma(std::string xdma_path, uintptr_t pcie_offs);
    UioDeviceInfo operator()(UioRegion dev_region, std::string evt_dev = "");
};

} // namespace udmaio
