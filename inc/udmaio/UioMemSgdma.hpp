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

#include <cstddef>
#include <memory>

#include "DmaBufferAbstract.hpp"
#include "RegAccessor.hpp"

namespace udmaio {

/// @brief Interface to AXI DMA scatter-gather buffers & descriptors
/// Uses a UioIf to access DMA descriptor memory
class UioMemSgdma : public UioIf {
    friend struct UioMemSgdmaTest;

    static constexpr int DESC_ADDR_STEP = 0x40;

    struct __attribute__((packed)) S2mmDescControl {
        uint32_t buffer_len : 26;
        bool rxeof : 1;
        bool rxsof : 1;
        uint32_t rsvd : 4;
    };

    struct __attribute__((packed)) S2mmDescStatus {
        // This value indicates the amount of data received and stored in the buffer described by
        // this descriptor. This might or might not match the buffer length
        uint32_t num_stored_bytes : 26;
        // End of Frame. Flag indicating buffer holds the last part of packet.
        bool rxeof : 1;
        // Start of Frame. Flag indicating buffer holds first part of packet.
        bool rxsof : 1;
        // DMA Internal Error. Internal Error detected by primary AXI DataMover.
        bool dmainterr : 1;
        // DMA Slave Error. Slave Error detected by primary AXI DataMover.
        bool dmaslverr : 1;
        // DMA Decode Error. Decode Error detected by primary AXI DataMover.
        bool dmadecerr : 1;
        // Completed. This indicates to the software that the DMA Engine has
        // completed the transfer as described by the associated descriptor.
        bool cmplt : 1;
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
    // Make a separate accessor for the statuses only, so we don't have to read/write the whole descriptor when we only want the status
    RegAccessorArray<S2mmDescStatus, offsetof(S2mmDesc, status), 1024, DESC_ADDR_STEP>
        desc_statuses{this};

    size_t _nr_cyc_desc;
    size_t _next_readable_buf;

    // Cache buffer addrs/sizes in order to not have to pull it from BRAM all the time
    std::vector<uint64_t> _buf_addrs;
    size_t _buf_size;

    std::shared_ptr<DmaBufferAbstract> _mem;

    void write_cyc_mode(const std::vector<UioRegion>& dst_bufs);

  public:
    UioMemSgdma(UioDeviceLocation dev_loc) : UioIf("UioMemSgdma", dev_loc) {}

    /// @brief Initialize SGDMA descriptors
    /// @param mem Memory receiving the SGDMA data
    /// @param num_buffers Number of descriptors / SGDMA blocks
    /// @param buf_size Size of each SGDMA block
    void init_buffers(std::shared_ptr<DmaBufferAbstract> mem, size_t num_buffers, size_t buf_size);

    /// @brief Print SGDMA descriptor
    /// @param desc SGDMA descriptor
    void print_desc(const S2mmDesc& desc) const;

    /// @brief Print all SGDMA descriptors
    void print_descs() const;

    /// @brief Get address of first SGDMA descriptor (needed for the AXI DMA I/F)
    /// @return Address of first SGDMA descriptor
    uintptr_t get_first_desc_addr() const;

    /// @brief Get all full SGDMA buffers
    /// @return Vector of buffer indices of full buffers
    std::vector<size_t> get_full_buffers();

    /// @brief Get SGDMA buffers for next packet
    /// @return Vector of buffer indices for next packet
    /// Returns only complete packets
    std::vector<size_t> get_next_packet();

    /// @brief Read data from a set of buffers
    /// @param indices Vector of buffer indices to read
    std::vector<uint8_t> read_buffers(const std::vector<size_t> indices);
};

std::ostream& operator<<(std::ostream& os, const UioRegion& buf_info);

}  // namespace udmaio
