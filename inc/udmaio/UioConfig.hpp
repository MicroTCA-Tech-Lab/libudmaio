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

class HwAccessor;
using HwAccessorPtr = std::shared_ptr<HwAccessor>;

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

class UioConfigBase;

/// Holds information where a device can be found over both UIO and XDMA
class UioDeviceLocation {
    friend class UioConfigXdma;

    static std::unique_ptr<UioConfigBase> _link_cfg;

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
    static void set_link_axi();

    /// @brief Set UioIf's globally to use a XDMA link
    /// @param xdma_path XDMA device instance directory in `/dev`
    /// @param pcie_offs XDMA core PCIe memory offset
    /// @param x7_series_mode Set the interface to Xilinx 7 series mode. PCIe connections to that device will be limited to 32 bits.
    static void set_link_xdma(std::string xdma_path, uintptr_t pcie_offs, bool x7_series_mode);

    HwAccessorPtr hw_acc() const;

    operator HwAccessorPtr() const;
};

/// Base class for HwAccessor creation
class UioConfigBase {
  public:
    /// @brief Mode of physical connection to the UioIf object
    /// @return DmaMode enum
    virtual DmaMode mode() = 0;

    virtual HwAccessorPtr hw_acc(const UioDeviceLocation& dev_loc) const = 0;
};

/// Creates HwAccessor from UioDeviceLocation (UIO version)
class UioConfigUio : public UioConfigBase {
    static int _get_uio_number(std::string_view name);
    static UioRegion _get_map_region(int uio_number, int map_index);

  public:
    DmaMode mode() override { return DmaMode::UIO; };

    HwAccessorPtr hw_acc(const UioDeviceLocation& dev_loc) const override;
};

/// Creates HwAccessor from UioDeviceLocation (XDMA version)
class UioConfigXdma : public UioConfigBase {
    std::string _xdma_path;
    uintptr_t _pcie_offs;
    bool _x7_series_mode;

  public:
    UioConfigXdma() = delete;

    /// @brief Create UioConfigXdma
    /// @param xdma_path XDMA device path `/dev/...`
    /// @param pcie_offs PCIe offset in memory
    UioConfigXdma(std::string xdma_path, uintptr_t pcie_offs, bool x7_series_mode = false);

    DmaMode mode() override { return DmaMode::XDMA; };

    HwAccessorPtr hw_acc(const UioDeviceLocation& dev_loc) const override;
};

}  // namespace udmaio
