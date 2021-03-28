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

namespace udmaio {

class UioMemSgdma : UioIf {
    static constexpr int DESC_ADDR_STEP = 0x40;

    struct __attribute__((packed)) S2mmDescControl {
        uint32_t buffer_len : 26;
        bool rxeof : 1;
        bool rxsof : 1;
        uint32_t rsvd : 4;
    };

    struct __attribute__((packed)) S2mmDescStatus {
        uint32_t buffer_len : 26;
        bool rxeof : 1;
        bool rxsof : 1;
        bool dmainterr : 1;
        bool dmaslverr : 1;
        bool dmadecerr : 1;
        bool cmpit : 1;
    };

    struct __attribute__((packed, aligned(8))) S2mmDesc {
        uint64_t nxtdesc;
        uint64_t buffer_addr;
        uint32_t rsvd0x10;
        uint32_t rsvd0x14;
        S2mmDescControl control;
        S2mmDescStatus status;
        uint32_t app[5];
    };

    static_assert(sizeof(S2mmDescControl) == 4, "size of S2mmDescControl must be 4");
    static_assert(sizeof(S2mmDescStatus) == 4, "size of S2mmDescStatus must be 4");
    static_assert(sizeof(S2mmDesc) == 0x38, "size of S2mmDesc must be 0x34+4 for alignment");

    S2mmDesc &_desc(size_t i) const;

    size_t _nr_cyc_desc;

    virtual const std::string_view _log_name() const override;

  public:
    static constexpr int BUF_LEN = 2 * 1024 * 1024;

    using UioIf::UioIf;

    void write_cyc_mode(const std::vector<uintptr_t> &dst_buf_addrs);

    void print_desc(const S2mmDesc &desc);

    void print_descs();

    uintptr_t get_first_desc_addr() const;

    std::vector<UioRegion> get_full_buffers();
};

std::ostream &operator<<(std::ostream &os, const UioRegion &buf_info);

} // namespace udmaio
