#include "udmaio/UioIf.hpp"

namespace udmaio {

UioIf::UioIf(std::string name, UioDeviceInfo dev)
    : Logger(name), _region{dev.region}, _skip_write_to_arm_int{!dev.evt_path.empty()} {
    // Can't call virtual fn from ctor, so can't use _log_name()
    BOOST_LOG_SEV(_lg, bls::debug) << "uio name = " << dev.dev_path;

    // open fd
    _fd = ::open(dev.dev_path.c_str(), O_RDWR);
    if (_fd < 0) {
        throw std::runtime_error("could not open " + dev.dev_path);
    }
    BOOST_LOG_SEV(_lg, bls::trace) << "fd =  " << _fd << ", size = " << _region.size;

    // create memory mapping
    _mem = mmap(NULL, _region.size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, dev.mmap_offs);
    BOOST_LOG_SEV(_lg, bls::trace) << "mmap = 0x" << _mem << std::dec;
    if (_mem == MAP_FAILED) {
        throw std::runtime_error("mmap failed for uio " + dev.dev_path);
    }

    // if there is an event filename, open event fd - otherwise use the same as for the mmap
    if (dev.evt_path.empty()) {
        _fd_int = _fd;
        BOOST_LOG_SEV(_lg, bls::trace) << "using fd for fd_int";
    } else {
        _fd_int = ::open(dev.evt_path.c_str(), O_RDWR);

        if (_fd_int < 0) {
            throw std::runtime_error("could not open " + dev.evt_path);
        }
        BOOST_LOG_SEV(_lg, bls::trace) << "fd_int =  " << _fd_int;
    }
}

UioIf::~UioIf() {
    munmap(_mem, _region.size);
    if (_fd_int != _fd) {
        ::close(_fd_int);
    }
    ::close(_fd);
}

void UioIf::arm_interrupt() {
    if (_skip_write_to_arm_int)
        return;

    uint32_t mask = 1;
    int rc = write(_fd_int, &mask, sizeof(mask));
    BOOST_LOG_SEV(_lg, bls::trace) << "arm interrupt enable, ret code = " << rc;
}

uint32_t UioIf::wait_for_interrupt() {
    uint32_t irq_count;
    BOOST_LOG_SEV(_lg, bls::trace) << "wait for interrupt ...";
    int rc = read(_fd_int, &irq_count, sizeof(irq_count));
    BOOST_LOG_SEV(_lg, bls::trace)
        << "interrupt received, rc = " << rc << ", irq count = " << irq_count;
    return irq_count;
}

uint32_t UioIf::_rd32(uint32_t offs) const {
    uint32_t tmp = *_reg_ptr(offs);
    BOOST_LOG_SEV(_lg, bls::trace)
        << "read at 0x" << std::hex << offs << " = 0x" << tmp << std::dec;
    return tmp;
}

void UioIf::_wr32(uint32_t offs, uint32_t data) {
    BOOST_LOG_SEV(_lg, bls::trace)
        << "write at 0x" << std::hex << offs << " = 0x" << data << std::dec;
    *_reg_ptr(offs) = data;
}

}  // namespace udmaio
