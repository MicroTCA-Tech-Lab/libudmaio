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

#include <cstdint>

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

void UioMemSgdma::init_buffers(DmaBufferAbstract& mem, size_t num_buffers, size_t buf_size) {
    // FIXME: enforce alignment constraints?
    std::vector<UioRegion> dst_bufs;
    _buf_addrs.clear();
    for (size_t i = 0; i < num_buffers; i++) {
        uint64_t addr = mem.get_phys_region().addr + i * buf_size;
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

std::vector<UioRegion> UioMemSgdma::get_full_buffers() {
    std::vector<UioRegion> bufs;

    for (size_t i = 0; i < _nr_cyc_desc; i++) {
        auto stat = desc_statuses[_next_readable_buf].rd();
        if (!stat.cmplt) {
            break;
        }

        if ((!stat.rxeof && (stat.num_stored_bytes != _buf_size)) || stat.num_stored_bytes == 0) {
            BOOST_LOG_SEV(_lg, bls::error)
                << "Descriptor #" << i << " size mismatch (expected " << _buf_size << ", received "
                << stat.num_stored_bytes << "), skipping";

            print_desc(descriptors[_next_readable_buf].rd());
            continue;
        }

        stat.cmplt = 0;
        desc_statuses[_next_readable_buf].wr(stat);

        const uint64_t buf_addr = _buf_addrs[_next_readable_buf];
        bufs.emplace_back(UioRegion{static_cast<uintptr_t>(buf_addr), stat.num_stored_bytes});

        BOOST_LOG_SEV(_lg, bls::trace)
            << "save buf #" << _next_readable_buf << " (" << stat.num_stored_bytes << " bytes) @ 0x"
            << std::hex << buf_addr;

        _next_readable_buf++;
        _next_readable_buf %= _nr_cyc_desc;
    }

    return bufs;
}

}  // namespace udmaio
