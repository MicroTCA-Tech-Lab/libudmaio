#define BOOST_TEST_MODULE UioGpioStatus

#define BOOST_TEST_DYN_LINK

#include "UioGpioStatus.hpp"
#include "udmaio/HwAccessor.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(UioGpioStatus_readback) {
    udmaio::HwAccessorPtr mock_hw = std::make_shared<udmaio::HwAccessorMock>(4096);

    auto gpio_stat = UioGpioStatus(mock_hw);
    gpio_stat.enable_debug(true);

    mock_hw->_wr32(0, 0);
    BOOST_TEST(gpio_stat.is_ddr4_init_calib_complete() == false);

    mock_hw->_wr32(0, 1);
    BOOST_TEST(gpio_stat.is_ddr4_init_calib_complete() == true);
}
