#include "DmaMode.hpp"

std::istream &operator>>(std::istream &in, DmaMode &mode) {
    std::string token;
    in >> token;
    if (token == "xdma")
        mode = DmaMode::XDMA;
    else if (token == "uio")
        mode = DmaMode::UIO;
    else
        in.setstate(std::ios_base::failbit);
    return in;
}
