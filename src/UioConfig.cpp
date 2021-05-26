#include "udmaio/UioConfig.hpp"

#include <fstream>
#include <filesystem>
#include <regex>

namespace udmaio {

/** @brief gets a number of UIO device based on the name
 *
 * i.e. searches for the uio instance where "/sys/class/uio/uioX/name"
 * matches `name`
 */
int UioConfigUio::_get_uio_number(std::string_view name) {
    std::string path = "/sys/class/uio/";
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
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

UioDeviceInfo UioConfigUio::operator()(std::string dev_name) {
    int uio_number = _get_uio_number(dev_name);
    if (uio_number < 0) {
        throw std::runtime_error("could not find a UIO device " + dev_name);
    }
    return {
        .dev_path = std::string{"/dev/uio"} + std::to_string(uio_number),
        .evt_path = "",
        .region = {
            _get_map_addr(uio_number),
            _get_map_size(uio_number)
        },
        .mmap_offs = 0
    };
}

UioConfigXdma::UioConfigXdma(std::string xdma_path, uintptr_t pcie_offs)
:_xdma_path(xdma_path)
,_pcie_offs(pcie_offs) {
}

UioDeviceInfo UioConfigXdma::operator()(UioRegion dev_region, std::string evt_dev) {
    return {
        .dev_path = _xdma_path + "/user",
        .evt_path = !evt_dev.empty() ? _xdma_path + "/" + evt_dev : "",
        .region = {
            dev_region.addr | _pcie_offs,
            dev_region.size
        },
        .mmap_offs = dev_region.addr
    };
};

}
