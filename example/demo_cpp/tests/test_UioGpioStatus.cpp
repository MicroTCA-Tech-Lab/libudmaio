#define BOOST_TEST_MODULE UioGpioStatus

#define BOOST_TEST_DYN_LINK

#include "UioGpioStatus.hpp"
#include "udmaio/HwAccessor.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(UioGpioStatus_readback) {
    udmaio::HwAccessorPtr mock_hw_inst = std::make_unique<udmaio::HwAccessorMock>(4096);

    // Save raw pointer for manipulation - this is safe as long as the UioIf instance is in scope
    auto mock_hw = mock_hw_inst.get();

    auto gpio_stat = UioGpioStatus(std::move(mock_hw_inst));

    mock_hw->_wr32(0, 0);
    BOOST_TEST(gpio_stat.is_ddr4_init_calib_complete() == false);

    mock_hw->_wr32(0, 1);
    BOOST_TEST(gpio_stat.is_ddr4_init_calib_complete() == true);
}
