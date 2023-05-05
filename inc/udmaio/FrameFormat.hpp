//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2023 Deutsches Elektronen-Synchrotron DESY

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace udmaio {

class FrameFormat {
  public:
    /// Frame dimensions
    struct dim_t {
        uint16_t width;
        uint16_t height;
    };

    // clang-format off
    enum class PixelFormat : uint32_t {
      unknown                   = 0,
      // Mono1p                    = 0x01010037, //   1-bit monochrome, unsigned, 8 pixels packed in one byte.
      // Mono2p                    = 0x01020038, //   2-bit monochrome, unsigned, 4 pixels packed in one byte.
      // Mono4p                    = 0x01040039, //   4-bit monochrome, unsigned, 2 pixels packed in one byte.
      Mono8                     = 0x01080001, //   8-bit monochrome, unsigned, unpacked format.
      // Mono8s                    = 0x01080002, //   8-bit monochrome,   signed, unpacked format.
      Mono10                    = 0x01100003, //  10-bit monochrome, unsigned, unpacked format.
      // Mono10Packed1             = 0x010C0004, //  10-bit monochrome, unsigned, GigE Vision-specific packed format.
      Mono12                    = 0x01100005, //  12-bit monochrome, unsigned, unpacked format.
      // Mono12Packed1             = 0x010C0006, //  12-bit monochrome, unsigned, GigE Vision-specific packed format.
      Mono14                    = 0x01100025, //  14-bit monochrome, unsigned, unpacked format.
      Mono16                    = 0x01100007, //  16-bit monochrome, unsigned, unpacked format.
      BayerGR8                  = 0x01080008, //   8-bit GRBG (green-red-blue-green) Bayer pattern, unsigned, unpacked format.
      BayerRG8                  = 0x01080009, //   8-bit RGGB (red-green-green-blue) Bayer pattern, unsigned, unpacked format.
      BayerGB8                  = 0x0108000A, //   8-bit GBRG (green-blue-red-green) Bayer pattern, unsigned, unpacked format.
      BayerBG8                  = 0x0108000B, //   8-bit BGGR (blue-green-green-red) Bayer pattern, unsigned, unpacked format.
      BayerGR10                 = 0x0110000C, //  10-bit GRBG (green-red-blue-green) Bayer pattern, unsigned, unpacked format.
      BayerRG10                 = 0x0110000D, //  10-bit RGGB (red-green-green-blue) Bayer pattern, unsigned, unpacked format.
      BayerGB10                 = 0x0110000E, //  10-bit GBRG (green-blue-red-green) Bayer pattern, unsigned, unpacked format.
      BayerBG10                 = 0x0110000F, //  10-bit BGGR (blue-green-green-red) Bayer pattern, unsigned, unpacked format.
      BayerGR12                 = 0x01100010, //  12-bit GRBG (green-red-blue-green) Bayer pattern, unsigned, unpacked format.
      BayerRG12                 = 0x01100011, //  12-bit RGGB (red-green-green-blue) Bayer pattern, unsigned, unpacked format.
      BayerGB12                 = 0x01100012, //  12-bit GBRG (green-blue-red-green) Bayer pattern, unsigned, unpacked format.
      BayerBG12                 = 0x01100013, //  12-bit BGGR (blue-green-green-red) Bayer pattern, unsigned, unpacked format.
      // BayerGR10Packed1          = 0x010C0026, //  10-bit GRBG (green-red-blue-green) Bayer pattern, unsigned, GigE Vision-specific packed format.
      // BayerRG10Packed1          = 0x010C0027, //  10-bit RGGB (red-green-green-blue) Bayer pattern, unsigned, GigE Vision-specific packed format.
      // BayerGB10Packed1          = 0x010C0028, //  10-bit GBRG (green-blue-red-green) Bayer pattern, unsigned, GigE Vision-specific packed format.
      // BayerBG10Packed1          = 0x010C0029, //  10-bit BGGR (blue-green-green-red) Bayer pattern, unsigned, GigE Vision-specific packed format.
      // BayerGR12Packed1          = 0x010C002A, //  12-bit GRBG (green-red-blue-green) Bayer pattern, unsigned, GigE Vision-specific packed format.
      // BayerRG12Packed           = 0x010C002B, //  12-bit RGGB (red-green-green-blue) Bayer pattern, unsigned, GigE Vision-specific packed format.
      // BayerGB12Packed1          = 0x010C002C, //  12-bit GBRG (green-blue-red-green) Bayer pattern, unsigned, GigE Vision-specific packed format.
      // BayerBG12Packed1          = 0x010C002D, //  12-bit BGGR (blue-green-green-red) Bayer pattern, unsigned, GigE Vision-specific packed format.
      BayerGR16                 = 0x0110002E, //  16-bit GRBG (green-red-blue-green) Bayer pattern, unsigned, unpacked format.
      BayerRG16                 = 0x0110002F, //  16-bit RGGB (red-green-green-blue) Bayer pattern, unsigned, unpacked format.
      BayerGB16                 = 0x01100030, //  16-bit GBRG (green-blue-red-green) Bayer pattern, unsigned, unpacked format.
      BayerBG16                 = 0x01100031, //  16-bit BGGR (blue-green-green-red) Bayer pattern, unsigned, unpacked format.
      RGB8                      = 0x02180014, //   8-bit RGB (red-green-blue), unsigned, unpacked format.
      BGR8                      = 0x02180015, //   8-bit BGR (blue-green-red), unsigned, unpacked format.
      RGBa8                     = 0x02200016, //   8-bit RGBa (red-green-blue-alpha), unsigned, unpacked format.
      BGRa8                     = 0x02200017, //   8-bit BGRa (blue-green-red-alpha), unsigned, unpacked format.
      // RGB10                     = 0x02300018, //  10-bit RGB (red-green-blue), unsigned, unpacked format.
      // BGR10                     = 0x02300019, //  10-bit BGR (blue-green-red), unsigned, unpacked format.
      // RGB12                     = 0x0230001A, //  12-bit RGB (red-green-blue), unsigned, unpacked format.
      // BGR12                     = 0x0230001B, //  12-bit BGR (blue-green-red), unsigned, unpacked format.
      // RGB16                     = 0x02300033, //  16-bit RGB (red-green-blue), unsigned, unpacked format.
      RGB10V1Packed1            = 0x0220001C, //  10-bit RGB (red-green-blue), unsigned, GigE Vision-specific packed format.
      RGB10p32                  = 0x0220001D, //  10-bit RGB (red-green-blue), unsigned, packed format.
      // RGB12V1Packed1            = 0x02240034, //  12-bit RGB (red-green-blue), unsigned, GigE Vision-specific packed format.
      RGB565p                   = 0x02100035, //  RGB (red 5 bits - green 6 bits - blue 5 bits), unsigned, packed format.
      BGR565p                   = 0x02100036, //  BGR (blue 5 bits – green 6 bits – red 5 bits), unsigned, packed format.
      // YUV411_8_UYYVYY           = 0x020C001E, //   8-bit YUV 4:1:1, unsigned, unpacked format.
      YUV422_8_UYVY             = 0x0210001F, //   8-bit YUV 4:2:2, unsigned, unpacked format.
      YUV422_8                  = 0x02100032, //   8-bit YUV 4:2:2, unsigned, unpacked format.
      YUV8_UYV                  = 0x02180020, //   8-bit YUV 4:4:4, unsigned, unpacked format.
      YCbCr8_CbYCr              = 0x0218003A, //   8-bit YCbCr 4:4:4, unsigned, unpacked format.
      YCbCr422_8                = 0x0210003B, //   8-bit YCbCr 4:2:2, unsigned, unpacked format.
      YCbCr422_8_CbYCrY         = 0x02100043, //   8-bit YCbCr 4:2:2, unsigned, unpacked format.
      // YCbCr411_8_CbYYCrYY       = 0x020C003C, //   8-bit YCbCr 4:1:1, unsigned, unpacked format.
      YCbCr601_8_CbYCr          = 0x0218003D, //   8-bit YCbCr 4:4:4, unsigned, unpacked format. This pixel format uses the color space specified in ITU-R BT.601.
      YCbCr601_422_8            = 0x0210003E, //   8-bit YCbCr 4:2:2, unsigned, unpacked format. This pixel format uses the color space specified in ITU-R BT.601.
      YCbCr601_422_8_CbYCrY     = 0x02100044, //   8-bit YCbCr 4:2:2, unsigned, unpacked format. This pixel format uses the color space specified in ITU-R BT.601.
      // YCbCr601_411_8_CbYYCrYY   = 0x020C003F, //   8-bit YCbCr 4:1:1, unsigned, unpacked format. This pixel format uses the color space specified in ITU-R BT.601.
      YCbCr709_8_CbYCr          = 0x02180040, //   8-bit YCbCr 4:4:4, unsigned, unpacked format. This pixel format uses the color space specified in ITU-R BT.709.
      YCbCr709_422_8            = 0x02100041, //   8-bit YCbCr 4:2:2, unsigned, unpacked format. This pixel format uses the color space specified in ITU-R BT.709.
      YCbCr709_422_8_CbYCrY     = 0x02100045, //   8-bit YCbCr 4:2:2, unsigned, unpacked format. This pixel format uses the color space specified in ITU-R BT.709.
      // YCbCr709_411_8_CbYYCrYY   = 0x020C0042, //   8-bit YCbCr 4:1:1, unsigned, unpacked format. This pixel format uses the color space specified in ITU-R BT.709.
      RGB8_Planar               = 0x02180021, //   8-bit RGB (red-green-blue), unsigned, unpacked, planar format where all color planes are transmitted in a multi-part payload block (recommended) or each color plane is transmitted on a different stream channel.
      // RGB10_Planar              = 0x02300022, //  10-bit RGB (red-green-blue), unsigned, unpacked, planar format where all color planes are transmitted in a multi-part payload block (recommended) or each color plane is transmitted on a different stream channel.
      // RGB12_Planar              = 0x02300023, //  12-bit RGB (red-green-blue), unsigned, unpacked, planar format where all color planes are transmitted in a multi-part payload block (recommended) or each color plane is transmitted on a different stream channel.
      // RGB16_Planar              = 0x02300024, //  16-bit RGB (red-green-blue), unsigned, unpacked, planar format where all color planes are transmitted in a multi-part payload block (recommended) or each color plane is transmitted on a different stream channel.
    };
    // clang-format on

    /// @brief Gets pixel format enum from string
    /// @param pix_fmt_str String containing the name of the pixel format
    static PixelFormat pix_fmt_from_str(std::string pix_fmt_str);

    /// @brief Gets pixel format string from enum
    /// @param pix_fmt Enum containing the pixel format
    static std::string pix_fmt_to_str(PixelFormat pix_fmt);

    FrameFormat();

    /// @brief Set format of the frames used for the video stream
    /// @param dim  Width and height of the video frame (in pixels)
    /// @param bytes_per_pix Number of bytes per pixel, pixel format will be set to unknown
    /// @param word_width Number of bytes per data word used by the dma
    void set_format(dim_t dim, uint16_t bytes_per_pixel, uint8_t word_width);

    /// @brief Set format of the frames used for the video stream
    /// @param dim  Width and height of the video frame (in pixels)
    /// @param pixFmt Pixel format, will be used to set bytes per pixel value
    /// @param word_width Number of bytes per data word used by the dma
    void set_format(dim_t dim, std::string pix_fmt_str, uint8_t word_width);

    /// @brief Set format of the frames used for the video stream
    /// @param dim  Width and height of the video frame (in pixels)
    /// @param pixFmt Pixel format, will be used to set bytes per pixel value
    /// @param word_width Number of bytes per data word used by the dma
    void set_format(dim_t dim, PixelFormat pix_fmt, uint8_t word_width);

    /// @brief Set format of the frames used for the video stream, pixel format is untouched
    /// @param dim  Width and height of the video frame (in pixels)
    void set_dim(dim_t dim);

    /// @brief Set format of the frames used for the video stream, frame dimensions are not touched
    /// @param bytes_per_pix Number of bytes per pixel, pixel format will be set to unknown
    void set_bpp(uint16_t bytes_per_pix);

    /// @brief Set format of the frames used for the video stream, frame dimensions are not touched
    /// @param pixFmt Pixel format, will be used to set bytes per pixel value
    void set_pix_fmt(PixelFormat pix_fmt);

    /// @brief Set format of the frames used for the video stream, frame dimensions are not touched
    /// @param word_width Number of bytes per data word used by the dma
    void set_word_width(uint8_t word_width);

    dim_t get_dim() const;
    PixelFormat get_pixel_format() const;
    std::string get_pixel_format_str() const;
    uint16_t get_bytes_per_pixel() const;
    uint8_t get_word_width() const;
    uint16_t get_pixel_per_word() const;
    uint16_t get_hsize() const;
    size_t get_frm_size() const;

  private:
    static const std::unordered_map<PixelFormat, std::string> _pxfmt_enum_to_str_tab;

    void update_frm_dim(dim_t dim);
    void update_bpp(uint16_t bytes_per_pix);
    void update_px_fmt(PixelFormat pix_fmt);
    void update_hsize();

    // Frame dimensions
    dim_t _dim;
    /// Pixel format
    PixelFormat _pix_fmt;
    /// Bytes per pixel
    uint16_t _bpp;
    /// Number of bytes per data word used by the dma
    uint8_t _word_width;

    uint16_t _pix_per_word;
    /// Line length in bytes
    uint16_t _hsize;
};

std::ostream& operator<<(std::ostream& os, const FrameFormat::PixelFormat px_fmt);

std::ostream& operator<<(std::ostream& os, const FrameFormat& frm_fmt);

}  // namespace udmaio