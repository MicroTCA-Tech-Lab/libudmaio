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
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

namespace udmaio {

/// DMA access mode
enum class DmaMode {
    XDMA,  ///< PCIe XDMA driver
    UIO    ///< ARM userspace I/O
};

std::istream& operator>>(std::istream& in, DmaMode& mode);

/// General-purpose struct to define a memory area
struct UioRegion {
    uintptr_t addr;  ///< Start of region
    size_t size;     ///< Size of region
};

/// Data needed to construct an UioIf; contains information how to connect to a device
struct UioDeviceInfo {
    std::string dev_path;
    std::string evt_path;
    UioRegion region;
    uintptr_t mmap_offs;
    bool force_32bit;
};

class UioConfigBase;

/// Holds information where a device can be found over both UIO and XDMA
class UioDeviceLocation {
    static std::unique_ptr<UioConfigBase> _link_cfg;
    static bool _is_x7_series;

  public:
    UioDeviceLocation(std::string uioname, UioRegion xdmaregion, std::string xdmaevtdev = "")
        : uio_name(uioname), xdma_region(xdmaregion), xdma_evt_dev(xdmaevtdev){};
    /// Device name (from device tree) for access through UIO
    std::string uio_name;
    /// Memory-mapped region for access through XDMA
    UioRegion xdma_region;
    /// optional: Event file for access through XDMA
    std::string xdma_evt_dev;

    /// @brief Set UioIf's globally to use a AXI/UIO link
    static void setLinkAxi();

    /// @brief Set UioIf's globally to use a XDMA link
    /// @param xdma_path XDMA device instance directory in `/dev`
    /// @param pcie_offs XDMA core PCIe memory offset
    static void setLinkXdma(std::string xdma_path, uintptr_t pcie_offs);

    /// @brief Set the interface to Xilinx 7 series mode. PCIe connections to that device will be limited to 32 bits.
    static void setX7Series();

    operator UioDeviceInfo() const;
};

/// Base class for UioDeviceInfo configuration
class UioConfigBase {
  public:
    /// @brief Get device file path (if applicable)
    /// @return Device file path
    virtual std::string dev_path() { return ""; };

    /// @brief Mode of physical connection to the UioIf object
    /// @return DmaMode enum
    virtual DmaMode mode() = 0;

    /// @brief Make a UioDeviceInfo from a UioDeviceLocation
    /// @param dev_loc Configuration data for a UioIf for both XDMA and UIO
    /// @return UioDeviceInfo object to allow constructing the UioIf
    virtual UioDeviceInfo operator()(UioDeviceLocation dev_loc) = 0;
};

/// Creates UioDeviceInfo from UioDeviceLocation (UIO version)
class UioConfigUio : public UioConfigBase {
    static int _get_uio_number(std::string_view name);
    static size_t _get_map_size(int uio_number, int map_index);
    static std::uintptr_t _get_map_addr(int uio_number, int map_index);

  public:
    /// @brief Create UioDeviceInfo from device name
    /// @param dev_name device name as in `/sys/class/uio/uioX/name`
    /// @return UioDeviceInfo object to allow constructing the UioIf
    UioDeviceInfo operator()(std::string dev_name);
    UioDeviceInfo operator()(UioDeviceLocation dev_loc) override;
    DmaMode mode() override { return DmaMode::UIO; };
};

/// Creates UioDeviceInfo from UioDeviceLocation (XDMA version)
class UioConfigXdma : public UioConfigBase {
    std::string _xdma_path;
    uintptr_t _pcie_offs;

  public:
    UioConfigXdma() = delete;

    /// @brief Create UioConfigXdma
    /// @param xdma_path XDMA device path `/dev/...`
    /// @param pcie_offs PCIe offset in memory
    UioConfigXdma(std::string xdma_path, uintptr_t pcie_offs);

    /// @brief Create UioDeviceInfo from memory region
    /// @param dev_region Memory region for the UioIf
    /// @param evt_dev (optional) Event file for interrupts
    /// @return UioDeviceInfo object to allow constructing the UioIf
    UioDeviceInfo operator()(UioRegion dev_region, std::string evt_dev = "");
    UioDeviceInfo operator()(UioDeviceLocation dev_loc) override;
    std::string dev_path() override { return _xdma_path; };
    DmaMode mode() override { return DmaMode::XDMA; };
};

}  // namespace udmaio
