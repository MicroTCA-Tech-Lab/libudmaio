#include "udmaio/UioIf.hpp"

namespace udmaio {

UioIf::UioIf(std::string name, std::unique_ptr<HwAccessor> hw) : _hw{std::move(hw)}, _lg{_hw->_lg} {
    _lg.channel(name);
}

UioIf::~UioIf() {}

void UioIf::arm_interrupt() {
    _hw->arm_interrupt();
}

uint32_t UioIf::wait_for_interrupt() {
    return _hw->wait_for_interrupt();
}

int UioIf::get_fd_int() const {
    return _hw->get_fd_int();
}

void UioIf::enable_debug(bool enable) {
    _hw->enable_debug(enable);
}
}  // namespace udmaio
