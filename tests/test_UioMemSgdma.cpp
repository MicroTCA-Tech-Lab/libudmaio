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
    std::shared_ptr<FpgaMemBufferOverAxi> fpga_mem;

    UioMemSgdmaTest()
        : hw_sgdma{std::make_shared<udmaio::HwAccessorMock>(16 * 1024)}
        , sgdma{UioDeviceLocation{hw_sgdma}}
        , hw_fpga_mem{std::make_shared<HwAccessorMock>(16 * 1024)}
        , fpga_mem{std::make_shared<FpgaMemBufferOverAxi>(UioDeviceLocation{hw_fpga_mem})}
        , descriptors{sgdma.descriptors}
        , Logger("UioMemSgdmaTest") {
        BOOST_LOG_SEV(_lg, bls::debug) << "UioMemSgdmaTest ctor";
        sgdma.init_buffers(fpga_mem, num_bufs, buf_size);
    }
    ~UioMemSgdmaTest() {}

    RegAccessorArray<UioMemSgdma::S2mmDesc, 0, 1024, UioMemSgdma::DESC_ADDR_STEP>& descriptors;

    static constexpr size_t num_bufs = 8, buf_size = 1024;
};

BOOST_FIXTURE_TEST_CASE(desc_addr, UioMemSgdmaTest) {
    BOOST_CHECK_MESSAGE(sgdma.get_first_desc_addr() == hw_sgdma->get_phys_region().addr,
                        "Descriptor address mismatch");
}

void fill_buffer(HwAccessorPtr& mem, uintptr_t addr, uint32_t start_count, size_t size) {
    assert(addr % sizeof(uint32_t) == 0);
    assert(size % sizeof(uint32_t) == 0);

    std::cerr << "Filling memory from 0x" << std::hex << addr << " to 0x" << std::hex << addr + size
              << std::endl;

    uintptr_t end_addr = addr + size;
    uint32_t count = start_count;
    while (addr < end_addr) {
        mem->_wr32(addr, count++);
        addr += sizeof(uint32_t);
    }
}

void check_buffer(std::vector<uint8_t> buf, uint32_t start_count = 0) {
    uint32_t count = start_count;
    BOOST_CHECK_MESSAGE(buf.size() % sizeof(uint32_t) == 0, "Buffer size not 32bit aligned");
    for (size_t i = 0; i < buf.size(); i += sizeof(uint32_t)) {
        uint32_t val = *(reinterpret_cast<uint32_t*>(&buf[i]));
        BOOST_CHECK_MESSAGE(val == count,
                            "Buffer contents mismatch at " << i << ", val = " << val
                                                           << " (expected " << count << ")");
        count++;
    }
}

BOOST_FIXTURE_TEST_CASE(empty_buffers, UioMemSgdmaTest) {
    auto empty_bufs = sgdma.get_full_buffers();
    BOOST_CHECK_MESSAGE(empty_bufs.size() == 0, "Empty buffers result not empty");
}

BOOST_FIXTURE_TEST_CASE(buffer_read, UioMemSgdmaTest) {
    // Make a packet filling up the first buffer
    fill_buffer(hw_fpga_mem, buf_size * 0, 0, buf_size);

    auto desc = descriptors[0].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = true;
    desc.status.rxeof = true;
    desc.status.num_stored_bytes = buf_size;
    descriptors[0].wr(desc);

    auto bufs = sgdma.get_full_buffers();
    BOOST_CHECK_MESSAGE(bufs.size() == 1, "Expected one full buffer, received " << bufs.size());
    BOOST_CHECK_MESSAGE(bufs[0] == 0, "Expected first buffer to be full, received " << bufs[0]);

    std::vector<uint8_t> data = sgdma.read_buffers(bufs);
    BOOST_CHECK_MESSAGE(data.size() == buf_size, "Buffer size mismatch");
    check_buffer(data);

    bool cmplt_reset = !descriptors[0].rd().status.cmplt;
    BOOST_CHECK_MESSAGE(cmplt_reset, "Complete flag is not reset");

    // Make a packet spanning 1.5 buffers
    fill_buffer(hw_fpga_mem, buf_size * 1, 0, buf_size * 3 / 2);
    desc = descriptors[1].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = true;
    desc.status.rxeof = false;
    desc.status.num_stored_bytes = buf_size;
    descriptors[1].wr(desc);
    desc = descriptors[2].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = false;
    desc.status.rxeof = true;
    desc.status.num_stored_bytes = buf_size / 2;
    descriptors[2].wr(desc);

    bufs = sgdma.get_full_buffers();
    BOOST_CHECK_MESSAGE((bufs == std::vector<size_t>{1, 2}), "Buffer indices mismatch");

    data = sgdma.read_buffers(bufs);
    BOOST_CHECK_MESSAGE(data.size() == buf_size * 3 / 2, "Data size mismatch");
    check_buffer(data);

    cmplt_reset = !descriptors[1].rd().status.cmplt && !descriptors[2].rd().status.cmplt;
    BOOST_CHECK_MESSAGE(cmplt_reset, "Complete flags are not reset");

    // Make two packets spanning 4 buffers
    fill_buffer(hw_fpga_mem, buf_size * 3, 0, buf_size * 3 / 2);
    desc = descriptors[3].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = true;
    desc.status.rxeof = false;
    desc.status.num_stored_bytes = buf_size;
    descriptors[3].wr(desc);
    desc = descriptors[4].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = false;
    desc.status.rxeof = true;
    desc.status.num_stored_bytes = buf_size / 2;
    descriptors[4].wr(desc);

    fill_buffer(hw_fpga_mem, buf_size * 5, 384, buf_size * 2);
    desc = descriptors[5].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = true;
    desc.status.rxeof = false;
    desc.status.num_stored_bytes = buf_size;
    descriptors[5].wr(desc);
    desc = descriptors[6].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = false;
    desc.status.rxeof = true;
    desc.status.num_stored_bytes = buf_size;
    descriptors[6].wr(desc);

    bufs = sgdma.get_full_buffers();
    BOOST_CHECK_MESSAGE((bufs == std::vector<size_t>{3, 4, 5, 6}), "Buffer indices mismatch");

    data = sgdma.read_buffers(bufs);
    BOOST_CHECK_MESSAGE(data.size() == buf_size * 7 / 2, "Data size mismatch");
    check_buffer(data);
    cmplt_reset = !descriptors[3].rd().status.cmplt && !descriptors[4].rd().status.cmplt &&
                  !descriptors[5].rd().status.cmplt && !descriptors[6].rd().status.cmplt;
    BOOST_CHECK_MESSAGE(cmplt_reset, "Complete flags are not reset");
}

BOOST_FIXTURE_TEST_CASE(packets_read, UioMemSgdmaTest) {
    // Make a packet filling up the first buffer
    fill_buffer(hw_fpga_mem, buf_size * 0, 0, buf_size);

    auto desc = descriptors[0].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = true;
    desc.status.rxeof = true;
    desc.status.num_stored_bytes = buf_size;
    descriptors[0].wr(desc);

    auto bufs = sgdma.get_next_packet();
    BOOST_CHECK_MESSAGE(bufs.size() == 1, "Expected one full buffer, received " << bufs.size());
    BOOST_CHECK_MESSAGE(bufs[0] == 0, "Expected first buffer to be full, received " << bufs[0]);

    std::vector<uint8_t> data = sgdma.read_buffers(bufs);
    BOOST_CHECK_MESSAGE(data.size() == buf_size, "Buffer size mismatch");
    check_buffer(data);

    bool cmplt_reset = !descriptors[0].rd().status.cmplt;
    BOOST_CHECK_MESSAGE(cmplt_reset, "Complete flag is not reset");

    // Make a packet spanning 1.5 buffers
    fill_buffer(hw_fpga_mem, buf_size * 1, 0, buf_size * 3 / 2);
    desc = descriptors[1].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = true;
    desc.status.rxeof = false;
    desc.status.num_stored_bytes = buf_size;
    descriptors[1].wr(desc);
    desc = descriptors[2].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = false;
    desc.status.rxeof = true;
    desc.status.num_stored_bytes = buf_size / 2;
    descriptors[2].wr(desc);

    bufs = sgdma.get_next_packet();
    BOOST_CHECK_MESSAGE((bufs == std::vector<size_t>{1, 2}), "Buffer indices mismatch");

    data = sgdma.read_buffers(bufs);
    BOOST_CHECK_MESSAGE(data.size() == buf_size * 3 / 2, "Data size mismatch");
    check_buffer(data);

    cmplt_reset = !descriptors[1].rd().status.cmplt && !descriptors[2].rd().status.cmplt;
    BOOST_CHECK_MESSAGE(cmplt_reset, "Complete flags are not reset");

    // Make two packets spanning 4 buffers
    fill_buffer(hw_fpga_mem, buf_size * 3, 1234, buf_size * 3 / 2);
    desc = descriptors[3].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = true;
    desc.status.rxeof = false;
    desc.status.num_stored_bytes = buf_size;
    descriptors[3].wr(desc);

    // Try to read a packet; ensure it doesn't return anything yet w/o EOF
    bufs = sgdma.get_next_packet();
    BOOST_CHECK_MESSAGE(bufs.empty(), "Packet is not full yet");

    desc = descriptors[4].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = false;
    desc.status.rxeof = true;
    desc.status.num_stored_bytes = buf_size / 2;
    descriptors[4].wr(desc);

    fill_buffer(hw_fpga_mem, buf_size * 5, 5678, buf_size * 2);
    desc = descriptors[5].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = true;
    desc.status.rxeof = false;
    desc.status.num_stored_bytes = buf_size;
    descriptors[5].wr(desc);
    desc = descriptors[6].rd();
    desc.status.cmplt = true;
    desc.status.rxsof = false;
    desc.status.rxeof = true;
    desc.status.num_stored_bytes = buf_size;
    descriptors[6].wr(desc);

    bufs = sgdma.get_next_packet();
    BOOST_CHECK_MESSAGE((bufs == std::vector<size_t>{3, 4}), "Buffer indices mismatch");
    data = sgdma.read_buffers(bufs);
    BOOST_CHECK_MESSAGE(data.size() == buf_size * 3 / 2, "Data size mismatch");
    check_buffer(data, 1234);
    cmplt_reset = !descriptors[3].rd().status.cmplt && !descriptors[4].rd().status.cmplt;
    BOOST_CHECK_MESSAGE(cmplt_reset, "Complete flags are not reset");

    bufs = sgdma.get_next_packet();
    BOOST_CHECK_MESSAGE((bufs == std::vector<size_t>{5, 6}), "Buffer indices mismatch");
    data = sgdma.read_buffers(bufs);
    BOOST_CHECK_MESSAGE(data.size() == buf_size * 2, "Data size mismatch");
    check_buffer(data, 5678);
    cmplt_reset = !descriptors[3].rd().status.cmplt && !descriptors[4].rd().status.cmplt;
    BOOST_CHECK_MESSAGE(cmplt_reset, "Complete flags are not reset");
}
}  // namespace udmaio
