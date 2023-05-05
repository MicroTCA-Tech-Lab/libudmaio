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

FrameFormat::FrameFormat() : Logger("FrameFormat") {
    pixel = 0;
    lines = 0;

    pxFmt = PixelFormat::unknown;
    bpp = 1;  // bytes per pixel

    data_width = 4;
    pix_per_word = 4;
    hsize = 0;  // line length in bytes
}

void FrameFormat::setFormat(uint16_t width,
                            uint16_t height,
                            uint16_t bytes_per_pixel,
                            uint8_t word_width) {
    update_frm_dim(width, height);
    update_bpp(bytes_per_pixel);
    data_width = word_width;
    update_hsize();
}

void FrameFormat::setFormat(uint16_t width,
                            uint16_t height,
                            std::string pixFmt,
                            uint8_t word_width) {
    update_frm_dim(width, height);
    update_px_fmt(toPixFormat(pixFmt));
    data_width = word_width;
    update_hsize();
}

void FrameFormat::setFormat(uint16_t width,
                            uint16_t height,
                            PixelFormat pixFmt,
                            uint8_t word_width) {
    update_frm_dim(width, height);
    update_px_fmt(pixFmt);
    data_width = word_width;
    update_hsize();
}

void FrameFormat::setFormat(uint16_t width, uint16_t height) {
    update_frm_dim(width, height);
    update_hsize();
}

void FrameFormat::setFormat(uint16_t bytes_per_pixel) {
    update_bpp(bytes_per_pixel);
    update_hsize();
}

void FrameFormat::setFormat(PixelFormat pixFmt) {
    update_px_fmt(pixFmt);
    update_hsize();
}

void FrameFormat::setFormat(uint8_t word_width) {
    data_width = word_width;
    update_hsize();
}

FrameFormat::PixelFormat FrameFormat::toPixFormat(std::string pixFrmt) {
    for (const auto& f : _pxfmt_enum_to_str_tab) {
        if (f.second == pixFrmt) {
            return f.first;
        }
    }
    return PixelFormat::unknown;
}

std::string FrameFormat::toString() {
    auto it = _pxfmt_enum_to_str_tab.find(pxFmt);
    if (it != _pxfmt_enum_to_str_tab.end()) {
        return it->second;
    }
    return "<unkown>";
}

uint16_t FrameFormat::get_pixel() {
    return pixel;
}

uint16_t FrameFormat::get_lines() {
    return lines;
}

FrameFormat::PixelFormat FrameFormat::get_pixel_format() {
    return pxFmt;
}

uint16_t FrameFormat::get_bytes_per_pixel() {
    return bpp;
}

uint8_t FrameFormat::get_word_width() {
    return data_width;
}

uint16_t FrameFormat::get_pixel_per_word() {
    return pix_per_word;
}

uint16_t FrameFormat::get_hsize() {
    return hsize;
}

size_t FrameFormat::get_frm_size() {
    return (size_t)(hsize * lines);
    // return (size_t)(pixel*lines*bpp);
}

void FrameFormat::dump_frame_format() {
    BOOST_LOG_SEV(_lg, bls::debug) << ": pixel        = 0x" << std::hex << std::setfill('0')
                                   << std::setw(4) << pixel << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug) << ": lines        = 0x" << std::hex << std::setfill('0')
                                   << std::setw(4) << lines << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug)
        << ": pxFmt        = " << toString() << " (0x" << std::hex << std::setfill('0')
        << std::setw(8) << static_cast<uint32_t>(pxFmt) << std::dec << ")";
    BOOST_LOG_SEV(_lg, bls::debug) << ": bpp          = 0x" << std::hex << std::setfill('0')
                                   << std::setw(4) << bpp << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug) << ": data_width   = 0x" << std::hex << std::setfill('0')
                                   << std::setw(2) << (int)data_width << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug) << ": pix_per_word = 0x" << std::hex << std::setfill('0')
                                   << std::setw(4) << pix_per_word << std::dec;
    BOOST_LOG_SEV(_lg, bls::debug) << ": hsize        = 0x" << std::hex << std::setfill('0')
                                   << std::setw(4) << hsize << std::dec;
}

void FrameFormat::update_frm_dim(uint16_t width, uint16_t height) {
    pixel = width;
    lines = height;
}

void FrameFormat::update_bpp(uint16_t bytes_per_pixel) {
    pxFmt = PixelFormat::unknown;
    bpp = bytes_per_pixel;  // bytes per pixel
}

void FrameFormat::update_px_fmt(PixelFormat pixFmt) {
    if (pixFmt == PixelFormat::unknown) {
        BOOST_LOG_SEV(_lg, bls::error)
            << ": Tried to set pixel format to 'unknown_pxfmt', ignoring operation!";
    } else {
        pxFmt = pixFmt;
        bpp = (0x00ff & ((uint32_t)pixFmt >> 16)) / 8;  // bytes per pixel
    }
}

void FrameFormat::update_hsize() {
    pix_per_word = data_width / bpp;
    hsize = std::ceil((float)pixel / pix_per_word) * data_width;
}
}  // namespace udmaio
