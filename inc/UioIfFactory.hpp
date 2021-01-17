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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>

#include "UioIf.hpp"

class UioIfFactory {
  public:
    template <typename T> static std::unique_ptr<T> create(const std::string &name) {
        static_assert(std::is_base_of<UioIf, T>::value, "T not derived from UioIf");
        int uio_number = _get_uio_number(name);
        if (uio_number < 0) {
            throw std::runtime_error("could not find a UIO device " + name);
        }
        uintptr_t addr = _get_map_addr(uio_number);
        int size = _get_map_size(uio_number);
        return std::make_unique<T>(std::string{"/dev/uio"} + std::to_string(uio_number), addr,
                                   size);
    }

  private:
    /** @brief gets a number of UIO device based on the name
     *
     * i.e. searches for the uio instance where "/sys/class/uio/uioX/name"
     * matches `name`
     */
    static int _get_uio_number(std::string_view name) {
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
    static int _get_map_size(int uio_number, int map_index = 0) {
        std::string path{"/sys/class/uio/uio" + std::to_string(uio_number) + "/maps/map" +
                         std::to_string(map_index) + "/size"};
        std::ifstream ifs{path};
        if (!ifs) {
            throw std::runtime_error("could not find a map info for UIO device " +
                                     std::to_string(uio_number));
        }
        std::string size_str;
        ifs >> size_str;
        int size = std::stoi(size_str, nullptr, 0);
        return size;
    }

    /** @brief gets an address (physical) for an uio map */
    static std::uintptr_t _get_map_addr(int uio_number, int map_index = 0) {
        std::string path{"/sys/class/uio/uio" + std::to_string(uio_number) + "/maps/map" +
                         std::to_string(map_index) + "/addr"};
        std::ifstream ifs{path};
        if (!ifs) {
            throw std::runtime_error("could not find a map info for UIO device " +
                                     std::to_string(uio_number));
        }
        std::string addr_str;
        ifs >> addr_str;
        std::uintptr_t addr = std::stoull(addr_str, nullptr, 0);
        return addr;
    }
};
