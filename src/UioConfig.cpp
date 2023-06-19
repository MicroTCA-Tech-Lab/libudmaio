//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY
// Copyright (c) 2022 Atom Computing, Inc.

#include "udmaio/UioConfig.hpp"

#include <filesystem>
#include <fstream>
#include <regex>

#include <unistd.h>

#include "udmaio/HwAccessor.hpp"

namespace udmaio {

std::istream& operator>>(std::istream& in, DmaMode& mode) {
    std::string token;
    in >> token;
    if (token == "xdma")
        mode = DmaMode::XDMA;
    else if (token == "uio")
        mode = DmaMode::UIO;
    else
        in.setstate(std::ios_base::failbit);
    return in;
}

std::unique_ptr<UioConfigBase> UioDeviceLocation::_link_cfg{};

void UioDeviceLocation::set_link_axi() {
    _link_cfg = std::make_unique<UioConfigUio>();
}

void UioDeviceLocation::set_link_xdma(std::string xdma_path,
                                      uintptr_t pcie_offs,
                                      bool x7_series_mode) {
    _link_cfg = std::make_unique<UioConfigXdma>(xdma_path, pcie_offs, x7_series_mode);
}

HwAccessorPtr UioDeviceLocation::hw_acc() const {
    if (_hw_acc_override) {
        // If a hardware accessor is set (e.g. for testing), return it in place of the regular one
        return _hw_acc_override;
    }
    if (!_link_cfg) {
        throw std::runtime_error("UioIf link type not set (use setLinkAxi() or setLinkXdma())");
    }
    // Create a hardware accessor for the actual UioConfig.
    return _link_cfg->hw_acc(*this);
}

/** @brief gets a number of UIO device based on the name
 *
 * i.e. searches for the uio instance where "/sys/class/uio/uioX/name"
 * matches `name`
 */
int UioConfigUio::_get_uio_number(std::string_view name) {
    std::string path = "/sys/class/uio/";
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::ifstream ifs{entry.path() / "name"};

        if (!ifs) {
            continue;
        }

        std::string cur_name;
        ifs >> cur_name;
        if (cur_name == name) {
            std::regex r{R"(uio(\d+))"};
            std::cmatch m;
            std::regex_match(entry.path().filename().c_str(), m, r);
            return std::stoi(m.str(1));
        }
    }
    return -1;
}

/** @brief gets a region for an uio map */
UioRegion UioConfigUio::_get_map_region(int uio_number, int map_index) {
    const std::string base_path{"/sys/class/uio/uio" + std::to_string(uio_number) + "/maps/map" +
                                std::to_string(map_index) + "/"};

    auto get_val = [](const std::string path) -> unsigned long long {
        std::ifstream ifs{path};
        if (!ifs) {
            throw std::runtime_error("could not find " + path);
        }
        std::string size_str;
        ifs >> size_str;
        return std::stoull(size_str, nullptr, 0);
    };

    return {
        static_cast<uintptr_t>(get_val(base_path + "addr")),
        static_cast<size_t>(get_val(base_path + "size")),
    };
}

/** @brief gets device info (mem region, mmap offset, ...) from a uio name
 *
 * In a case where there are more than one memory mappings, an individual memory
 * mapping can be selected by appending a colon (":") to the UIO name, followed
 * by the mapping index. The UIO names are (in most cases) derived from the IP
 * names in Vivado Block Diagram, and those names cannot include a colon.
 * Additionally, a colon cannot be used in the device tree identifiers. It is,
 * therefore, safe to reserve a colon as a special character.
 *
 * Example:
 *
 * Consider the following output of the `lsuio` command:
 *
 * ```
 * uio8: name=example_clocking, version=devicetree, events=0
 *     map[0]: addr=0xA0140000, size=65536
 * uio7: name=example_app_top, version=devicetree, events=0
 *     map[0]: addr=0xA0200000, size=1048576
 *     map[1]: addr=0xA0300000, size=65536
 * ```
 *
 * - The "example_clocking" UIO can be selected either with `example_clocking`
 *   or `example_clocking:0`.
 * - Similarly, the first memory mapping of "example_app_top" can be selected
 *   either with `example_app_top` or with `example_app_top:0`.
 * - Second memory mapping of "example_app_top" can only be selected with
 *   `example_app_top:1`
 *
 */
HwAccessorPtr UioConfigUio::hw_acc(const UioDeviceLocation& dev_loc) const {
    int map_index = 0;
    const std::string& dev_name = dev_loc.uio_name;
    std::string base_dev_name;  // a name without the colon

    size_t colon_pos = dev_name.find(":");
    if (colon_pos != std::string::npos) {
        base_dev_name = dev_name.substr(0, colon_pos);
        std::string_view idx_substr = dev_name.substr(colon_pos + 1, std::string::npos);
        map_index = std::stoi(std::string{idx_substr});
    } else {
        base_dev_name = dev_name;
    }

    int uio_number = _get_uio_number(base_dev_name);
    if (uio_number < 0) {
        throw std::runtime_error("could not find a UIO device " + dev_name);
    }
    return std::make_shared<HwAccessorAxi>(std::string{"/dev/uio"} + std::to_string(uio_number),
                                           _get_map_region(uio_number, map_index),
                                           static_cast<uintptr_t>(map_index * getpagesize()));
}

UioConfigXdma::UioConfigXdma(std::string xdma_path, uintptr_t pcie_offs, bool x7_series_mode)
    : _xdma_path(xdma_path), _pcie_offs(pcie_offs), _x7_series_mode(x7_series_mode) {}

HwAccessorPtr UioConfigXdma::hw_acc(const UioDeviceLocation& dev_loc) const {
    if (_x7_series_mode) {
        // Workaround for limited PCIe memory access to certain devices:
        // "For 7 series Gen2 IP, PCIe access from the Host system must be limited to 1DW (4 Bytes)
        // transaction only." (see Xilinx pg195, page 10) If using direct access to the mmap()ed area (or a
        // regular memcpy), the CPU will issue larger transfers and the system will crash
        return std::make_shared<HwAccessorXdma<uint32_t>>(
            _xdma_path,
            dev_loc.xdma_evt_dev,
            UioRegion{dev_loc.xdma_region.addr, dev_loc.xdma_region.size},
            _pcie_offs);
    } else {
        return std::make_shared<HwAccessorXdma<uint64_t>>(
            _xdma_path,
            dev_loc.xdma_evt_dev,
            UioRegion{dev_loc.xdma_region.addr, dev_loc.xdma_region.size},
            _pcie_offs);
    }
}

}  // namespace udmaio
