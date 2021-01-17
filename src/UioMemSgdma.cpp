//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include "UioMemSgdma.hpp"

namespace blt = boost::log::trivial;

UioMemSgdma::UioMemSgdma(const std::string &uio_name, uintptr_t addr, size_t size)
    : UioIf{uio_name, addr, size, "UioMemSgdma"} {}

void UioMemSgdma::_write_desc(uintptr_t mem_offs, const S2mmDesc *desc) {
    memcpy(static_cast<char *>(_mem) + mem_offs, desc, sizeof(S2mmDesc));
}

void UioMemSgdma::_read_desc(uintptr_t mem_offs, S2mmDesc *desc) {
    memcpy(desc, static_cast<char *>(_mem) + mem_offs, sizeof(S2mmDesc));
}

void UioMemSgdma::write_cyc_mode(const std::vector<uint64_t> &dst_buf_addrs) {
    uint64_t offs = 0;
    for (auto dst_buf_addr : dst_buf_addrs) {
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << _log_name << ": dest buf addr = 0x" << std::hex << dst_buf_addr << std::dec;

        bool is_last = dst_buf_addr == dst_buf_addrs.back();
        uint64_t nxtdesc = _int_addr + (is_last ? 0 : offs + DESC_ADDR_STEP);

        S2mmDesc desc{.nxtdesc = nxtdesc,
                      .buffer_addr = dst_buf_addr,
                      .control =
                          S2mmDescControl{.buffer_len = BUF_LEN, .rxeof = 0, .rxsof = 0, .rsvd = 0},
                      .status = {0},
                      .app = {0}};

        _write_desc(offs, &desc);

        offs += DESC_ADDR_STEP;
    }
}

void UioMemSgdma::print_desc(const S2mmDesc &desc) {
#define BLI BOOST_LOG_SEV(_slg, blt::severity_level::info) << _log_name << ": "
    BLI << "S2mmDesc {";
    BLI << "  next desc   = 0x" << std::hex << desc.nxtdesc;
    BLI << "  buffer addr = 0x" << std::hex << desc.buffer_addr;
    BLI << "  control";
    BLI << "    buffer_len = " << std::dec << desc.control.buffer_len;
    BLI << "    sof        = " << std::dec << desc.control.rxsof;
    BLI << "    eof        = " << std::dec << desc.control.rxeof;
    BLI << "  status";
    BLI << "    buffer_len = " << std::dec << desc.status.buffer_len;
    BLI << "    sof        = " << std::dec << desc.status.rxsof;
    BLI << "    eof        = " << std::dec << desc.status.rxeof;
    BLI << "    dmainterr  = " << std::dec << desc.status.dmainterr;
    BLI << "    dmaslverr  = " << std::dec << desc.status.dmaslverr;
    BLI << "    dmadecerr  = " << std::dec << desc.status.dmadecerr;
    BLI << "    cmpit      = " << std::dec << desc.status.cmpit;
    BLI << "}" << std::dec;
}

void UioMemSgdma::print_descs(unsigned int nr_desc) {
    for (unsigned int i = 0; i < nr_desc; i++) {
        S2mmDesc desc;
        _read_desc(i * DESC_ADDR_STEP, &desc);
        print_desc(desc);
    }
}

uint64_t UioMemSgdma::get_first_desc_addr() { return _int_addr; }
