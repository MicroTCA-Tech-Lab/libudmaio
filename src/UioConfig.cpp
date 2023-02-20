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

UioDeviceInfo::UioDeviceInfo(UioDeviceLocation dev_loc) {
    *this = dev_loc.dev_info();
}

UioDeviceInfo::UioDeviceInfo(std::string dev,
                             std::string evt,
                             UioRegion reg,
                             uintptr_t offs,
                             bool force_32bit)
    : dev_path{dev}, evt_path{evt}, region{reg}, mmap_offs{offs}, force_32bit{force_32bit} {}

std::unique_ptr<UioConfigBase> UioDeviceLocation::_link_cfg{};

void UioDeviceLocation::set_link_axi() {
    _link_cfg = std::make_unique<UioConfigUio>();
}

void UioDeviceLocation::set_link_xdma(std::string xdma_path,
                                      uintptr_t pcie_offs,
                                      bool x7_series_mode) {
    _link_cfg = std::make_unique<UioConfigXdma>(xdma_path, pcie_offs, x7_series_mode);
}

UioDeviceInfo UioDeviceLocation::dev_info() const {
    if (!_link_cfg) {
        throw std::runtime_error("UioIf link type not set (use setLinkAxi() or setLinkXdma())");
    }
    return _link_cfg->operator()(*this);
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
UioDeviceInfo UioConfigUio::operator()(std::string dev_name) {
    int map_index = 0;
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
    return {
        std::string{"/dev/uio"} + std::to_string(uio_number),
        "",
        _get_map_region(uio_number, map_index),
        static_cast<uintptr_t>(map_index * getpagesize()),
    };
}

UioDeviceInfo UioConfigUio::operator()(UioDeviceLocation dev_loc) {
    return operator()(dev_loc.uio_name);
}

UioConfigXdma::UioConfigXdma(std::string xdma_path, uintptr_t pcie_offs, bool x7_series_mode)
    : _xdma_path(xdma_path), _pcie_offs(pcie_offs), _x7_series_mode(x7_series_mode) {}

UioDeviceInfo UioConfigXdma::operator()(UioRegion dev_region, std::string evt_dev) {
    return {
        _xdma_path + "/user",
        !evt_dev.empty() ? _xdma_path + "/" + evt_dev : "",
        {dev_region.addr | _pcie_offs, dev_region.size},
        dev_region.addr,
        // Workaround for limited PCIe memory access to certain devices:
        // "For 7 series Gen2 IP, PCIe access from the Host system must be limited to 1DW (4 Bytes)
        // transaction only." (see Xilinx pg195, page 10) If using direct access to the mmap()ed area (or a
        // regular memcpy), the CPU will issue larger transfers and the system will crash
        _x7_series_mode,
    };
};

UioDeviceInfo UioConfigXdma::operator()(UioDeviceLocation dev_loc) {
    return operator()(dev_loc.xdma_region, dev_loc.xdma_evt_dev);
}

}  // namespace udmaio
