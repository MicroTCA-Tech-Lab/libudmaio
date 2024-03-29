#define BOOST_TEST_MODULE UioTrafficGen

#define BOOST_TEST_DYN_LINK

#include "UioTrafficGen.hpp"
#include "udmaio/HwAccessor.hpp"
#include <boost/test/unit_test.hpp>

struct Fx {
    udmaio::HwAccessorPtr hw;
    udmaio::UioDeviceLocation dev_loc;

    UioTrafficGen traffic_gen;

    Fx() : hw{std::make_shared<udmaio::HwAccessorMock>(4096)}, dev_loc{hw}, traffic_gen{dev_loc} {
        traffic_gen.enable_debug(true);
    }
    ~Fx() {}
};

BOOST_FIXTURE_TEST_CASE(UioTrafficGen_start, Fx) {
    traffic_gen.start(42, 128, 10000);

    auto config_reg = hw->_rd32(0x34);
    BOOST_CHECK_MESSAGE(config_reg == (10000 << 16), "pkt_pause match");

    auto tr_len = hw->_rd32(0x38);
    BOOST_CHECK_MESSAGE(tr_len == ((42 << 16) | 127), "tr_len match");

    auto ext_tlen = hw->_rd32(0x50);
    BOOST_CHECK_MESSAGE(ext_tlen == 0, "ext_tlen match");

    auto st_ctrl = hw->_rd32(0x30);
    BOOST_CHECK_MESSAGE((st_ctrl & 0b11) == 0b01, "st_ctrl match");
}

BOOST_FIXTURE_TEST_CASE(UioTrafficGen_stop, Fx) {
    // set done bit
    hw->_wr32(0x30, hw->_rd32(0x30) | 0b10);

    traffic_gen.stop();

    auto st_ctrl = hw->_rd32(0x30);
    BOOST_CHECK_MESSAGE((st_ctrl & 0b11) == 0b10, "st_ctrl match");
}
