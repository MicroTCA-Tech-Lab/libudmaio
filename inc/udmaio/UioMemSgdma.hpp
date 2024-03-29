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

#include "DmaBufferAbstract.hpp"
#include "RegAccessor.hpp"

namespace udmaio {

/// @brief Interface to AXI DMA scatter-gather buffers & descriptors
/// Uses a UioIf to access DMA descriptor memory
class UioMemSgdma : public UioIf {
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

    // set excessively high number of 1024 descriptors, b/c the actual number is not known at compile time
    RegAccessorArray<S2mmDesc, 0, 1024, DESC_ADDR_STEP> descriptors{this};

    size_t _nr_cyc_desc;
    size_t _next_readable_buf;

    void write_cyc_mode(const std::vector<UioRegion>& dst_bufs);

  public:
    UioMemSgdma(UioDeviceLocation dev_loc) : UioIf("UioMemSgdma", dev_loc) {}

    /// @brief Initialize SGDMA descriptors
    /// @param mem Memory receiving the SGDMA data
    /// @param num_buffers Number of descriptors / SGDMA blocks
    /// @param buf_size Size of each SGDMA block
    void init_buffers(DmaBufferAbstract& mem, size_t num_buffers, size_t buf_size);

    /// @brief Print SGDMA descriptor
    /// @param desc SGDMA descriptor
    void print_desc(const S2mmDesc& desc) const;

    /// @brief Print all SGDMA descriptors
    void print_descs() const;

    /// @brief Get address of first SGDMA descriptor (needed for the AXI DMA I/F)
    /// @return Address of first SGDMA descriptor
    uintptr_t get_first_desc_addr() const;

    /// @brief Get full SGDMA buffers
    /// @return Vector of UioRegion pointing into full buffers
    std::vector<UioRegion> get_full_buffers();
};

std::ostream& operator<<(std::ostream& os, const UioRegion& buf_info);

}  // namespace udmaio
