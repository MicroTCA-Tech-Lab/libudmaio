#include <memory>

#include "udmaio/UioConfig.hpp"
#define BOOST_TEST_MODULE UioMemSgdma

#define BOOST_TEST_DYN_LINK

#include "udmaio/FpgaMemBufferOverAxi.hpp"
#include "udmaio/HwAccessor.hpp"
#include "udmaio/UioMemSgdma.hpp"
#include <boost/test/unit_test.hpp>

using namespace udmaio;

struct Fx {
    HwAccessorPtr hw_sgdma;
    UioMemSgdma sgdma;

    HwAccessorPtr hw_fpga_mem;
    FpgaMemBufferOverAxi fpga_mem;

    Fx()
        : hw_sgdma{std::make_shared<udmaio::HwAccessorMock>(16 * 1024)}
        , sgdma{UioDeviceLocation{hw_sgdma}}
        , hw_fpga_mem{std::make_shared<HwAccessorMock>(16 * 1024)}
        , fpga_mem{UioDeviceLocation{hw_fpga_mem}} {}
    ~Fx() {}

    static constexpr size_t num_bufs = 8, buf_size = 1024;
};

BOOST_FIXTURE_TEST_CASE(init_buffers, Fx) {
    sgdma.init_buffers(fpga_mem, num_bufs, buf_size);
    BOOST_CHECK_MESSAGE(sgdma.get_first_desc_addr() == hw_sgdma->get_phys_region().addr,
                        "Descriptor address mismatch");

    sgdma.print_descs();
}

BOOST_FIXTURE_TEST_CASE(foobar, Fx) {}
