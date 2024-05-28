#include <cstdint>
#include <memory>

#include "udmaio/Logging.hpp"
#include "udmaio/UioConfig.hpp"
#define BOOST_TEST_MODULE UioMemSgdma

#define BOOST_TEST_DYN_LINK

#include "udmaio/FpgaMemBufferOverAxi.hpp"
#include "udmaio/HwAccessor.hpp"
#include "udmaio/UioMemSgdma.hpp"
#include <boost/test/unit_test.hpp>

namespace udmaio {

struct UioMemSgdmaTest : public Logger {
    HwAccessorPtr hw_sgdma;
    UioMemSgdma sgdma;

    HwAccessorPtr hw_fpga_mem;
    FpgaMemBufferOverAxi fpga_mem;

    UioMemSgdmaTest()
        : hw_sgdma{std::make_shared<udmaio::HwAccessorMock>(16 * 1024)}
        , sgdma{UioDeviceLocation{hw_sgdma}}
        , hw_fpga_mem{std::make_shared<HwAccessorMock>(16 * 1024)}
        , fpga_mem{UioDeviceLocation{hw_fpga_mem}}
        , descriptors{sgdma.descriptors}
        , Logger("UioMemSgdmaTest") {}
    ~UioMemSgdmaTest() {}

    RegAccessorArray<UioMemSgdma::S2mmDesc, 0, 1024, UioMemSgdma::DESC_ADDR_STEP>& descriptors;

    static constexpr size_t num_bufs = 8, buf_size = 1024;
};

BOOST_FIXTURE_TEST_CASE(init_buffers, UioMemSgdmaTest) {
    sgdma.init_buffers(fpga_mem, num_bufs, buf_size);
    BOOST_CHECK_MESSAGE(sgdma.get_first_desc_addr() == hw_sgdma->get_phys_region().addr,
                        "Descriptor address mismatch");
}

void fill_buffer(HwAccessorPtr& mem, uintptr_t addr, uint32_t start_count, size_t size) {
    assert(addr % sizeof(uint32_t) == 0);
    assert(size % sizeof(uint32_t) == 0);

    while (addr < size) {
        mem->_wr32(addr, addr + start_count);
        addr += sizeof(uint32_t);
    }
}

void check_buffer(std::vector<uint8_t> buf) {
    BOOST_CHECK_MESSAGE(buf.size() % sizeof(uint32_t) == 0, "Buffer size not 32bit aligned");
    for (size_t i = 0; i < buf.size(); i += sizeof(uint32_t)) {
        uint32_t val = *(reinterpret_cast<uint32_t*>(&buf[i]));
        BOOST_CHECK_MESSAGE(val == i,
                            "Buffer contents mismatch at " << i << ", val = " << std::hex << val);
    }
}

BOOST_FIXTURE_TEST_CASE(full_buffers, UioMemSgdmaTest) {
    sgdma.init_buffers(fpga_mem, num_bufs, buf_size);

    auto empty_bufs = sgdma.get_full_buffers();
    BOOST_CHECK_MESSAGE(empty_bufs.size() == 0, "Empty buffers result not empty");

    // Fill first buffer
    fill_buffer(hw_fpga_mem, 0x00000000, 0, buf_size);

    auto desc = descriptors[0].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = true;
    desc.status.rxeof = true;
    desc.status.num_stored_bytes = buf_size;
    descriptors[0].wr(desc);

    // sgdma.print_descs();

    auto bufs = sgdma.get_full_buffers();
    BOOST_CHECK_MESSAGE(bufs.size() == 1, "Expected one full buffer, received " << bufs.size());

    std::vector<uint8_t> data;
    fpga_mem.append_from_buf(bufs[0], data);
    BOOST_CHECK_MESSAGE(data.size() == buf_size, "Buffer size mismatch");
    check_buffer(data);
}

}  // namespace udmaio
