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

/** @brief gets a size for an uio map */
size_t UioConfigUio::_get_map_size(int uio_number, int map_index) {
    std::string path{"/sys/class/uio/uio" + std::to_string(uio_number) + "/maps/map" +
                     std::to_string(map_index) + "/size"};
    std::ifstream ifs{path};
    if (!ifs) {
        throw std::runtime_error("could not find a map info for UIO device " +
                                 std::to_string(uio_number));
    }
    std::string size_str;
    ifs >> size_str;
    return std::stoull(size_str, nullptr, 0);
}

/** @brief gets an address (physical) for an uio map */
std::uintptr_t UioConfigUio::_get_map_addr(int uio_number, int map_index) {
    std::string path{"/sys/class/uio/uio" + std::to_string(uio_number) + "/maps/map" +
                     std::to_string(map_index) + "/addr"};
    std::ifstream ifs{path};
    if (!ifs) {
        throw std::runtime_error("could not find a map info for UIO device " +
                                 std::to_string(uio_number));
    }
    std::string addr_str;
    ifs >> addr_str;
    return std::stoull(addr_str, nullptr, 0);
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
        .dev_path = std::string{"/dev/uio"} + std::to_string(uio_number),
        .evt_path = "",
        .region =
            {
                _get_map_addr(uio_number, map_index),
                _get_map_size(uio_number, map_index),
            },
        .mmap_offs = static_cast<uintptr_t>(map_index * getpagesize()),
    };
}

UioDeviceInfo UioConfigUio::operator()(UioDeviceLocation dev_loc) {
    return operator()(dev_loc.uio_name);
}

UioConfigXdma::UioConfigXdma(std::string xdma_path, uintptr_t pcie_offs)
    : _xdma_path(xdma_path), _pcie_offs(pcie_offs) {}

UioDeviceInfo UioConfigXdma::operator()(UioRegion dev_region, std::string evt_dev) {
    return {
        .dev_path = _xdma_path + "/user",
        .evt_path = !evt_dev.empty() ? _xdma_path + "/" + evt_dev : "",
        .region = {dev_region.addr | _pcie_offs, dev_region.size},
        .mmap_offs = dev_region.addr,
    };
};

UioDeviceInfo UioConfigXdma::operator()(UioDeviceLocation dev_loc) {
    return operator()(dev_loc.xdma_region, dev_loc.xdma_evt_dev);
}

}  // namespace udmaio
