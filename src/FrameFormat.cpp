//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2023 Deutsches Elektronen-Synchrotron DESY

#include "udmaio/FrameFormat.hpp"

#include <cmath>

#include <boost/log/core/core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

namespace blt = boost::log::trivial;

namespace udmaio {

const std::unordered_map<FrameFormat::PixelFormat, std::string> FrameFormat::_pxfmt_enum_to_str_tab{
    {PixelFormat::Mono8, "Mono8"},
    {PixelFormat::Mono10, "Mono10"},
    {PixelFormat::Mono12, "Mono12"},
    {PixelFormat::Mono14, "Mono14"},
    {PixelFormat::Mono16, "Mono16"},
    {PixelFormat::BayerGR8, "BayerGR8"},
    {PixelFormat::BayerRG8, "BayerRG8"},
    {PixelFormat::BayerGB8, "BayerGB8"},
    {PixelFormat::BayerBG8, "BayerBG8"},
    {PixelFormat::BayerGR10, "BayerGR10"},
    {PixelFormat::BayerRG10, "BayerRG10"},
    {PixelFormat::BayerGB10, "BayerGB10"},
    {PixelFormat::BayerBG10, "BayerBG10"},
    {PixelFormat::BayerGR12, "BayerGR12"},
    {PixelFormat::BayerRG12, "BayerRG12"},
    {PixelFormat::BayerGB12, "BayerGB12"},
    {PixelFormat::BayerBG12, "BayerBG12"},
    {PixelFormat::BayerGR16, "BayerGR16"},
    {PixelFormat::BayerRG16, "BayerRG16"},
    {PixelFormat::BayerGB16, "BayerGB16"},
    {PixelFormat::BayerBG16, "BayerBG16"},
    {PixelFormat::RGB8, "RGB8"},
    {PixelFormat::BGR8, "BGR8"},
    {PixelFormat::RGBa8, "RGBa8"},
    {PixelFormat::BGRa8, "BGRa8"},
    {PixelFormat::RGB10V1Packed1, "RGB10V1Packed1"},
    {PixelFormat::RGB10p32, "RGB10p32"},
    {PixelFormat::RGB565p, "RGB565p"},
    {PixelFormat::BGR565p, "BGR565p"},
    {PixelFormat::YUV422_8_UYVY, "YUV422_8_UYVY"},
    {PixelFormat::YUV422_8, "YUV422_8"},
    {PixelFormat::YUV8_UYV, "YUV8_UYV"},
    {PixelFormat::YCbCr8_CbYCr, "YCbCr8_CbYCr"},
    {PixelFormat::YCbCr422_8, "YCbCr422_8"},
    {PixelFormat::YCbCr422_8_CbYCrY, "YCbCr422_8_CbYCrY"},
    {PixelFormat::YCbCr601_8_CbYCr, "YCbCr601_8_CbYCr"},
    {PixelFormat::YCbCr601_422_8, "YCbCr601_422_8"},
    {PixelFormat::YCbCr601_422_8_CbYCrY, "YCbCr601_422_8_CbYCrY"},
    {PixelFormat::YCbCr709_8_CbYCr, "YCbCr709_8_CbYCr"},
    {PixelFormat::YCbCr709_422_8, "YCbCr709_422_8"},
    {PixelFormat::YCbCr601_422_8_CbYCrY, "YCbCr601_422_8_CbYCrY"},
    {PixelFormat::YCbCr709_8_CbYCr, "YCbCr709_8_CbYCr"},
    {PixelFormat::YCbCr709_422_8, "YCbCr709_422_8"},
    {PixelFormat::YCbCr709_422_8_CbYCrY, "YCbCr709_422_8_CbYCrY"},
    {PixelFormat::RGB8_Planar, "RGB8_Planar"},
};

FrameFormat::PixelFormat FrameFormat::pix_fmt_from_str(std::string pix_fmt_str) {
    for (const auto& f : _pxfmt_enum_to_str_tab) {
        if (f.second == pix_fmt_str) {
            return f.first;
        }
    }
    return PixelFormat::unknown;
}

std::string FrameFormat::pix_fmt_to_str(PixelFormat pix_fmt) {
    auto it = _pxfmt_enum_to_str_tab.find(pix_fmt);
    if (it != _pxfmt_enum_to_str_tab.end()) {
        return it->second;
    }
    return "<unkown>";
}

std::ostream& operator<<(std::ostream& os, const FrameFormat::PixelFormat px_fmt) {
    os << FrameFormat::pix_fmt_to_str(px_fmt);
    return os;
}

FrameFormat::FrameFormat() : Logger("FrameFormat") {
    _dim = {0, 0};

    _pix_fmt = PixelFormat::unknown;
    _bpp = 1;

    _word_width = 4;
    _pix_per_word = 4;
    _hsize = 0;
}

void FrameFormat::set_format(dim_t dim, uint16_t bytes_per_pixel, uint8_t word_width) {
    update_frm_dim(dim);
    update_bpp(bytes_per_pixel);
    _word_width = word_width;
    update_hsize();
}

void FrameFormat::set_format(dim_t dim, std::string pix_fmt_str, uint8_t word_width) {
    set_format(dim, pix_fmt_from_str(pix_fmt_str), word_width);
}

void FrameFormat::set_format(dim_t dim, PixelFormat pixFmt, uint8_t word_width) {
    update_frm_dim(dim);
    update_px_fmt(pixFmt);
    _word_width = word_width;
    update_hsize();
}

void FrameFormat::set_dim(dim_t dim) {
    update_frm_dim(dim);
    update_hsize();
}

void FrameFormat::set_bpp(uint16_t bytes_per_pixel) {
    update_bpp(bytes_per_pixel);
    update_hsize();
}

void FrameFormat::set_pix_fmt(PixelFormat pixFmt) {
    update_px_fmt(pixFmt);
    update_hsize();
}

void FrameFormat::set_word_width(uint8_t word_width) {
    _word_width = word_width;
    update_hsize();
}

FrameFormat::dim_t FrameFormat::get_dim() const {
    return _dim;
}

FrameFormat::PixelFormat FrameFormat::get_pixel_format() const {
    return _pix_fmt;
}

std::string FrameFormat::get_pixel_format_str() const {
    return pix_fmt_to_str(_pix_fmt);
}

uint16_t FrameFormat::get_bytes_per_pixel() const {
    return _bpp;
}

uint8_t FrameFormat::get_word_width() const {
    return _word_width;
}

uint16_t FrameFormat::get_pixel_per_word() const {
    return _pix_per_word;
}

uint16_t FrameFormat::get_hsize() const {
    return _hsize;
}

size_t FrameFormat::get_frm_size() const {
    return (size_t)(_hsize * _dim.height);
}

void FrameFormat::update_frm_dim(dim_t dim) {
    _dim = dim;
}

void FrameFormat::update_bpp(uint16_t bytes_per_pixel) {
    _pix_fmt = PixelFormat::unknown;
    _bpp = bytes_per_pixel;
}

void FrameFormat::update_px_fmt(PixelFormat pixFmt) {
    if (pixFmt == PixelFormat::unknown) {
        BOOST_LOG_SEV(_lg, bls::error)
            << ": Tried to set pixel format to 'unknown_pxfmt', ignoring operation!";
    } else {
        _pix_fmt = pixFmt;
        _bpp = (0x00ff & (static_cast<uint32_t>(pixFmt) >> 16)) / 8;
    }
}

void FrameFormat::update_hsize() {
    _pix_per_word = _word_width / _bpp;
    _hsize = std::ceil((float)_dim.width / _pix_per_word) * _word_width;
}

std::ostream& operator<<(std::ostream& os, const FrameFormat& frm_fmt) {
    const auto dim = frm_fmt.get_dim();
    os << "width        = 0x" << std::hex << std::setfill('0') << std::setw(4) << dim.width
       << std::dec << "\n";
    os << "height       = 0x" << std::hex << std::setfill('0') << std::setw(4) << dim.height
       << std::dec << "\n";
    os << "px_fmt       = " << frm_fmt.get_pixel_format_str() << " (0x" << std::hex
       << std::setfill('0') << std::setw(8) << static_cast<uint32_t>(frm_fmt.get_pixel_format())
       << std::dec << ")"
       << "\n";
    os << "bpp          = 0x" << std::hex << std::setfill('0') << std::setw(4)
       << frm_fmt.get_bytes_per_pixel() << std::dec << "\n";
    os << "word_width   = 0x" << std::hex << std::setfill('0') << std::setw(2)
       << (int)frm_fmt.get_word_width() << std::dec << "\n";
    os << "pix_per_word = 0x" << std::hex << std::setfill('0') << std::setw(4)
       << frm_fmt.get_pixel_per_word() << std::dec << "\n";
    os << "hsize        = 0x" << std::hex << std::setfill('0') << std::setw(4)
       << frm_fmt.get_hsize() << std::dec << "\n";
    return os;
}

}  // namespace udmaio
