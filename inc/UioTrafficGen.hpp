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

#include "UioIf.hpp"

class UioTrafficGen : UioIf {
    static const int ADDR_ST_CTRL = 0x30;
    static const int ADDR_ST_CONFIG = 0x34;
    static const int ADDR_TR_LEN = 0x38;
    static const int ADDR_TR_COUNT = 0x3C;
    static const int ADDR_TR_TSTRB0 = 0x40;
    static const int ADDR_TR_TSTRB1 = 0x44;
    static const int ADDR_TR_TSTRB2 = 0x48;
    static const int ADDR_TR_TSTRB3 = 0x4C;
    static const int ADDR_EX_TR_LEN = 0x50;
    static const int ADDR_ST_ERR_STATUS = 0x70;
    static const int ADDR_ST_ERR_EN = 0x74;
    static const int ADDR_ST_ERR_INT_EN = 0x78;
    static const int ADDR_ST_ERR_CNT = 0x7C;

    union StControl {
        uint32_t data;
        struct __attribute__((packed)) {
            bool stren : 1;
            bool done : 1;
            uint32_t rsvd : 22;
            uint32_t version : 8;
        } fields;
    };

    union StConfig {
        uint32_t data;
        struct __attribute__((packed)) {
            bool ranlen : 1;
            bool randly : 1;
            bool etkts : 1;
            uint32_t rsvd7_3 : 5;
            uint32_t tdest : 8;
            uint32_t pdly : 16;
        } fields;
    };

  public:
    explicit UioTrafficGen(const std::string &uio_name, uintptr_t addr, size_t size);
    void start();
    void print_version();
};
