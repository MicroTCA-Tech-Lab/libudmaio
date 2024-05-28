//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include "udmaio/UioMemSgdma.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "udmaio/DmaBufferAbstract.hpp"

namespace udmaio {

void UioMemSgdma::write_cyc_mode(const std::vector<UioRegion>& dst_bufs) {
    _nr_cyc_desc = dst_bufs.size();
    _next_readable_buf = 0;
    size_t i = 0;

    for (auto dst_buf : dst_bufs) {
        BOOST_LOG_SEV(_lg, bls::trace)
            << "dest buf addr = 0x" << std::hex << dst_buf.addr << std::dec;

        uintptr_t nxtdesc = get_first_desc_addr() + ((i + 1) % _nr_cyc_desc) * DESC_ADDR_STEP;

        S2mmDesc desc{
            .nxtdesc = nxtdesc,
            .buffer_addr = dst_buf.addr,
            .control =
                S2mmDescControl{
                    .buffer_len = static_cast<uint32_t>(dst_buf.size),
                    .rxeof = 0,
                    .rxsof = 0,
                    .rsvd = 0,
                },
            .status = {0},
            .app = {0},
        };
        descriptors[i++].wr(desc);
    }
}

void UioMemSgdma::init_buffers(std::shared_ptr<DmaBufferAbstract> mem,
                               size_t num_buffers,
                               size_t buf_size) {
    _mem = mem;

    // FIXME: enforce alignment constraints?
    std::vector<UioRegion> dst_bufs;
    _buf_addrs.clear();
    for (size_t i = 0; i < num_buffers; i++) {
        uint64_t addr = mem->get_phys_region().addr + i * buf_size;
        dst_bufs.push_back({
            .addr = addr,
            .size = buf_size,
        });
        _buf_addrs.push_back(addr);
    };

    write_cyc_mode(dst_bufs);
    _buf_size = buf_size;
}

void UioMemSgdma::print_desc(const S2mmDesc& desc) const {
#define BLI BOOST_LOG_SEV(_lg, bls::info) << ""
    BLI << "S2mmDesc {";
    BLI << "  next desc   = 0x" << std::hex << desc.nxtdesc;
    BLI << "  buffer addr = 0x" << std::hex << desc.buffer_addr;
    BLI << "  control";
    BLI << "    buffer_len = " << std::dec << desc.control.buffer_len;
    BLI << "    sof        = " << std::dec << desc.control.rxsof;
    BLI << "    eof        = " << std::dec << desc.control.rxeof;
    BLI << "  status";
    BLI << "    buffer_len = " << std::dec << desc.status.num_stored_bytes;
    BLI << "    sof        = " << std::dec << desc.status.rxsof;
    BLI << "    eof        = " << std::dec << desc.status.rxeof;
    BLI << "    dmainterr  = " << std::dec << desc.status.dmainterr;
    BLI << "    dmaslverr  = " << std::dec << desc.status.dmaslverr;
    BLI << "    dmadecerr  = " << std::dec << desc.status.dmadecerr;
    BLI << "    cmplt      = " << std::dec << desc.status.cmplt;
    BLI << "}" << std::dec;
}

void UioMemSgdma::print_descs() const {
    for (size_t i = 0; i < _nr_cyc_desc; i++) {
        BLI << "Reading desc " << i << "/" << _nr_cyc_desc - 1 << " from offset 0x" << std::hex
            << i * DESC_ADDR_STEP << std::dec;
        print_desc(descriptors[i].rd());
    }
}

uintptr_t UioMemSgdma::get_first_desc_addr() const {
    return _hw->get_phys_region().addr;
}

std::ostream& operator<<(std::ostream& os, const UioRegion& buf_info) {
    os << "UioRegion{0x" << std::hex << buf_info.addr << ", 0x" << buf_info.size << std::dec << "}";
    return os;
}

std::vector<size_t> UioMemSgdma::get_full_buffers() {
    std::vector<size_t> result;

    for (size_t i = 0; i < _nr_cyc_desc; i++) {
        auto stat = desc_statuses[_next_readable_buf].rd();
        if (!stat.cmplt) {
            break;
        }

        if ((!stat.rxeof && (stat.num_stored_bytes != _buf_size)) || stat.num_stored_bytes == 0) {
            throw std::runtime_error("Descriptor #" + std::to_string(i) +
                                     " size mismatch (expected " + std::to_string(_buf_size) +
                                     ", received " + std::to_string(stat.num_stored_bytes) + ")");
        }

        result.push_back(_next_readable_buf);

        _next_readable_buf++;
        _next_readable_buf %= _nr_cyc_desc;
    }

    return result;
}

std::vector<uint8_t> UioMemSgdma::read_buffers(const std::vector<size_t> indices) {
    // Read statuses for all buffers
    std::vector<S2mmDescStatus> stats{indices.size()};
    size_t result_size = 0;
    for (size_t i = 0; i < indices.size(); i++) {
        stats[i] = desc_statuses[indices[i]].rd();
        result_size += stats[i].num_stored_bytes;
    }

    std::vector<uint8_t> result;
    // Reserve enough space in advance to avoid re-allocation / copying
    result.reserve(result_size);
    for (size_t i = 0; i < indices.size(); i++) {
        auto region = UioRegion{_buf_addrs[indices[i]], stats[i].num_stored_bytes};
        BOOST_LOG_SEV(_lg, bls::trace) << "save buf #" << indices[i] << " (" << region.size
                                       << " bytes) @ 0x" << std::hex << region.addr;
        _mem->append_from_buf(region, result);
        // Clear complete flag after having read the data
        stats[i].cmplt = 0;
        desc_statuses[indices[i]].wr(stats[i]);
    }
    return result;
}

}  // namespace udmaio
