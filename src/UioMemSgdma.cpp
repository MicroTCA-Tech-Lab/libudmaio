//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include <bitset>

#include "udmaio/UioMemSgdma.hpp"

namespace blt = boost::log::trivial;

namespace udmaio {

const std::string_view UioMemSgdma::_log_name() const {
    return "UioMemSgdma";
}

// Workaround for limited PCIe memory access to certain devices:
// "For 7 series Gen2 IP, PCIe access from the Host system must be limited to 1DW (4 Bytes) transaction only." (see Xilinx pg195, page 10)
// If using direct access to the mmap()ed area (or a regular memcpy), the CPU will issue larger transfers and the system will crash
void UioMemSgdma::memcpy32_helper(void *__restrict__ dest, const void *__restrict__ src, size_t n) {
    assert((reinterpret_cast<uintptr_t>(dest) % sizeof(uint32_t) == 0) && "memcpy32_helper: Dest addr not aligned to 32 bit");
    assert((reinterpret_cast<uintptr_t>(src) % sizeof(uint32_t) == 0) && "memcpy32_helper: Source addr not aligned to 32 bit");
    assert((n % sizeof(uint32_t) == 0) && "memcpy32_helper: Copy size not aligned to 32 bit");
    n /= sizeof(uint32_t);
    uint32_t *__restrict__ dest32 = static_cast<uint32_t *>(dest);
    const uint32_t *__restrict__ src32 = static_cast<const uint32_t *>(src);
    while (n--) {
        *dest32++ = *src32++;
    }
}

UioMemSgdma::S2mmDesc* UioMemSgdma::desc_ptr(size_t i) const {
    assert(i < _nr_cyc_desc && "desc index out of range");
    char *addr = static_cast<char *>(_mem) + (i * DESC_ADDR_STEP);
    return reinterpret_cast<S2mmDesc *>(addr);
}

UioMemSgdma::S2mmDesc UioMemSgdma::read_desc(size_t i) const {
    S2mmDesc result;
    memcpy32_helper(&result, desc_ptr(i), sizeof(S2mmDesc));
    return result;
}

void UioMemSgdma::write_desc(size_t i, const S2mmDesc& src) {
    memcpy32_helper(desc_ptr(i), &src, sizeof(S2mmDesc));
}

void UioMemSgdma::write_cyc_mode(const std::vector<UioRegion> &dst_bufs) {
    _nr_cyc_desc = dst_bufs.size();
    _next_readable_buf = 0;
    size_t i = 0;
    for (auto dst_buf : dst_bufs) {
        BOOST_LOG_SEV(_slg, blt::severity_level::trace)
            << _log_name() << ": dest buf addr = 0x" << std::hex << dst_buf.addr << std::dec;

        uintptr_t nxtdesc = _region.addr +  ((i + 1) % _nr_cyc_desc) * DESC_ADDR_STEP;

#pragma GCC diagnostic push // We're OK that everything not listed is zero-initialized.
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
        S2mmDesc desc{
            .nxtdesc = nxtdesc,
            .buffer_addr = dst_buf.addr,
            .control = S2mmDescControl{
                .buffer_len = static_cast<uint32_t>(dst_buf.size),
                .rxeof = 0,
                .rxsof = 0,
                .rsvd = 0
            },
            .status = {0},
            .app = {0}
        };
        write_desc(i++, desc);
#pragma GCC diagnostic pop
    }
}

void UioMemSgdma::init_buffers(DmaBufferAbstract& mem, size_t num_buffers, size_t buf_size)
{
    // FIXME: enforce alignment constraints?
    std::vector<UioRegion> dst_bufs;
    for (size_t i = 0; i < num_buffers; i++) {
        dst_bufs.push_back({
            .addr = mem.get_phys_addr() + i * buf_size,
            .size = buf_size
        });
    };

    write_cyc_mode(dst_bufs);
}

void UioMemSgdma::print_desc(const S2mmDesc &desc) const {
#define BLI BOOST_LOG_SEV(_slg, blt::severity_level::info) << _log_name() << ": "
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

void UioMemSgdma::print_descs() const {
    for (size_t i = 0; i < _nr_cyc_desc; i++) {
        print_desc(read_desc(i));
    }
}

uintptr_t UioMemSgdma::get_first_desc_addr() const {
    return _region.addr;
}

std::ostream &operator<<(std::ostream &os, const UioRegion &buf_info) {
    os << "UioRegion{0x" << std::hex << buf_info.addr << ", 0x" << buf_info.size << std::dec
       << "}";
    return os;
}

std::vector<UioRegion> UioMemSgdma::get_full_buffers() {
    std::vector<UioRegion> bufs;

    for (size_t i = 0; i < _nr_cyc_desc; i++) {
        S2mmDesc desc = read_desc(_next_readable_buf);
        if (!desc.status.cmpit) {
            break;
        }
        desc.status.cmpit = 0;
        bufs.emplace_back(UioRegion{
                desc.buffer_addr,
                desc.status.buffer_len
            });
        BOOST_LOG_SEV(_slg, blt::severity_level::trace) <<
            "save buf #" << _next_readable_buf << " @ 0x" << std::hex << desc.buffer_addr;

        _next_readable_buf++;
        _next_readable_buf %= _nr_cyc_desc;
    }

    return bufs;
}

} // namespace udmaio
