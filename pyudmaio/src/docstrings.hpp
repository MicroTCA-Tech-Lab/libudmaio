/*
  This file contains docstrings for use in the Python bindings.
  Do not edit! They were automatically extracted by pybind11_mkdoc.
 */

#define __EXPAND(x)                                      x
#define __COUNT(_1, _2, _3, _4, _5, _6, _7, COUNT, ...)  COUNT
#define __VA_SIZE(...)                                   __EXPAND(__COUNT(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1))
#define __CAT1(a, b)                                     a ## b
#define __CAT2(a, b)                                     __CAT1(a, b)
#define __DOC1(n1)                                       __doc_##n1
#define __DOC2(n1, n2)                                   __doc_##n1##_##n2
#define __DOC3(n1, n2, n3)                               __doc_##n1##_##n2##_##n3
#define __DOC4(n1, n2, n3, n4)                           __doc_##n1##_##n2##_##n3##_##n4
#define __DOC5(n1, n2, n3, n4, n5)                       __doc_##n1##_##n2##_##n3##_##n4##_##n5
#define __DOC6(n1, n2, n3, n4, n5, n6)                   __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6
#define __DOC7(n1, n2, n3, n4, n5, n6, n7)               __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6##_##n7
#define DOC(...)                                         __EXPAND(__EXPAND(__CAT2(__DOC, __VA_SIZE(__VA_ARGS__)))(__VA_ARGS__))

#if defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


static const char *__doc_udmaio_DataHandlerPython = R"doc()doc";

static const char *__doc_udmaio_DataHandlerPython_DataHandlerPython = R"doc()doc";

static const char *__doc_udmaio_DataHandlerPython_desc_ptr = R"doc()doc";

static const char *__doc_udmaio_DataHandlerPython_dma_ptr = R"doc()doc";

static const char *__doc_udmaio_DataHandlerPython_mem_ptr = R"doc()doc";

static const char *__doc_udmaio_DataHandlerPython_numpy_read = R"doc()doc";

static const char *__doc_udmaio_DataHandlerPython_numpy_read_nb = R"doc()doc";

static const char *__doc_udmaio_DataHandlerPython_start = R"doc()doc";

static const char *__doc_udmaio_DmaBufferAbstract = R"doc(Base class for DMA data buffer)doc";

static const char *__doc_udmaio_DmaBufferAbstract_append_from_buf =
R"doc(Append received DMA data to vector

Parameter ``buf_info``:
    Memory region of DMA buffer

Parameter ``out``:
    Vector to append received data to)doc";

static const char *__doc_udmaio_DmaBufferAbstract_copy_from_buf = R"doc()doc";

static const char *__doc_udmaio_DmaBufferAbstract_get_phys_region =
R"doc(Get physical region

Returns:
    Physical address and size of DMA data buffer)doc";

static const char *__doc_udmaio_DmaMode = R"doc(DMA access mode)doc";

static const char *__doc_udmaio_DmaMode_UIO = R"doc(< ARM userspace I/O)doc";

static const char *__doc_udmaio_DmaMode_XDMA = R"doc(< PCIe XDMA driver)doc";

static const char *__doc_udmaio_FpgaMemBufferOverAxi =
R"doc(DMA data buffer accessed over AXI/UIO, described w/ explicit address &
size)doc";

static const char *__doc_udmaio_FpgaMemBufferOverAxi_FpgaMemBufferOverAxi = R"doc()doc";

static const char *__doc_udmaio_FpgaMemBufferOverAxi_copy_from_buf = R"doc()doc";

static const char *__doc_udmaio_FpgaMemBufferOverAxi_get_phys_region = R"doc()doc";

static const char *__doc_udmaio_FpgaMemBufferOverXdma = R"doc(DMA data buffer accessed over XDMA using the xdma c2h0 stream channel)doc";

static const char *__doc_udmaio_FpgaMemBufferOverXdma_FpgaMemBufferOverXdma =
R"doc(Constructs a DMA data buffer

Parameter ``path``:
    Base path of XDMA instance in `/dev`

Parameter ``phys_addr``:
    Physical address of DMA data buffer)doc";

static const char *__doc_udmaio_FpgaMemBufferOverXdma_copy_from_buf = R"doc()doc";

static const char *__doc_udmaio_FpgaMemBufferOverXdma_dma_fd = R"doc()doc";

static const char *__doc_udmaio_FpgaMemBufferOverXdma_get_phys_region = R"doc()doc";

static const char *__doc_udmaio_FpgaMemBufferOverXdma_phys_region = R"doc()doc";

static const char *__doc_udmaio_FrameFormat = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_FrameFormat = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BGR565p = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BGR8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BGRa8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerBG10 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerBG12 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerBG16 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerBG8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerGB10 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerGB12 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerGB16 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerGB8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerGR10 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerGR12 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerGR16 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerGR8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerRG10 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerRG12 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerRG16 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_BayerRG8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_Mono10 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_Mono12 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_Mono14 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_Mono16 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_Mono8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_RGB10V1Packed1 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_RGB10p32 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_RGB565p = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_RGB8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_RGB8_Planar = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_RGBa8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YCbCr422_8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YCbCr422_8_CbYCrY = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YCbCr601_422_8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YCbCr601_422_8_CbYCrY = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YCbCr601_8_CbYCr = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YCbCr709_422_8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YCbCr709_422_8_CbYCrY = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YCbCr709_8_CbYCr = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YCbCr8_CbYCr = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YUV422_8 = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YUV422_8_UYVY = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_YUV8_UYV = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_PixelFormat_unknown = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_bpp = R"doc(Bytes per pixel)doc";

static const char *__doc_udmaio_FrameFormat_dim = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_dim_t = R"doc(Frame dimensions)doc";

static const char *__doc_udmaio_FrameFormat_dim_t_dim_t = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_dim_t_height = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_dim_t_width = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_get_bytes_per_pixel = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_get_dim = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_get_frm_size = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_get_hsize = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_get_pixel_format = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_get_pixel_format_str = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_get_pixel_per_word = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_get_word_width = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_hsize = R"doc(Line length in bytes)doc";

static const char *__doc_udmaio_FrameFormat_pix_fmt = R"doc(Pixel format)doc";

static const char *__doc_udmaio_FrameFormat_pix_fmt_from_str =
R"doc(Gets pixel format enum from string

Parameter ``pix_fmt_str``:
    String containing the name of the pixel format)doc";

static const char *__doc_udmaio_FrameFormat_pix_fmt_to_str =
R"doc(Gets pixel format string from enum

Parameter ``pix_fmt``:
    Enum containing the pixel format)doc";

static const char *__doc_udmaio_FrameFormat_pix_per_word = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_set_bpp =
R"doc(Set format of the frames used for the video stream, frame dimensions
are not touched

Parameter ``bytes_per_pix``:
    Number of bytes per pixel, pixel format will be set to unknown)doc";

static const char *__doc_udmaio_FrameFormat_set_dim =
R"doc(Set format of the frames used for the video stream, pixel format is
untouched

Parameter ``dim``:
    Width and height of the video frame (in pixels))doc";

static const char *__doc_udmaio_FrameFormat_set_format =
R"doc(Set format of the frames used for the video stream

Parameter ``dim``:
    Width and height of the video frame (in pixels)

Parameter ``bytes_per_pix``:
    Number of bytes per pixel, pixel format will be set to unknown

Parameter ``word_width``:
    Number of bytes per data word used by the dma)doc";

static const char *__doc_udmaio_FrameFormat_set_format_2 =
R"doc(Set format of the frames used for the video stream

Parameter ``dim``:
    Width and height of the video frame (in pixels)

Parameter ``pixFmt``:
    Pixel format, will be used to set bytes per pixel value

Parameter ``word_width``:
    Number of bytes per data word used by the dma)doc";

static const char *__doc_udmaio_FrameFormat_set_format_3 =
R"doc(Set format of the frames used for the video stream

Parameter ``dim``:
    Width and height of the video frame (in pixels)

Parameter ``pixFmt``:
    Pixel format, will be used to set bytes per pixel value

Parameter ``word_width``:
    Number of bytes per data word used by the dma)doc";

static const char *__doc_udmaio_FrameFormat_set_pix_fmt =
R"doc(Set format of the frames used for the video stream, frame dimensions
are not touched

Parameter ``pixFmt``:
    Pixel format, will be used to set bytes per pixel value)doc";

static const char *__doc_udmaio_FrameFormat_set_word_width =
R"doc(Set format of the frames used for the video stream, frame dimensions
are not touched

Parameter ``word_width``:
    Number of bytes per data word used by the dma)doc";

static const char *__doc_udmaio_FrameFormat_update_bpp = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_update_frm_dim = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_update_hsize = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_update_px_fmt = R"doc()doc";

static const char *__doc_udmaio_FrameFormat_word_width = R"doc(Number of bytes per data word used by the dma)doc";

static const char *__doc_udmaio_HwAccessor = R"doc()doc";

static const char *__doc_udmaio_UDmaBuf =
R"doc(DMA data buffer accessed over AXI/UIO, implemented w/ udmabuf (see
https://github.com/ikwzm/udmabuf))doc";

static const char *__doc_udmaio_UDmaBuf_UDmaBuf =
R"doc(Constructs a UDmaBuf

Parameter ``buf_idx``:
    Buffer index `/dev/udmabufN`)doc";

static const char *__doc_udmaio_UDmaBuf_copy_from_buf = R"doc()doc";

static const char *__doc_udmaio_UDmaBuf_fd = R"doc()doc";

static const char *__doc_udmaio_UDmaBuf_get_phys_addr = R"doc()doc";

static const char *__doc_udmaio_UDmaBuf_get_phys_region = R"doc()doc";

static const char *__doc_udmaio_UDmaBuf_get_size = R"doc()doc";

static const char *__doc_udmaio_UDmaBuf_mem = R"doc()doc";

static const char *__doc_udmaio_UDmaBuf_phys = R"doc()doc";

static const char *__doc_udmaio_UioAxiDmaIf = R"doc(Interface to AXI DMA Core)doc";

static const char *__doc_udmaio_UioAxiDmaIf_UioAxiDmaIf = R"doc()doc";

static const char *__doc_udmaio_UioAxiDmaIf_check_for_errors =
R"doc(Check status register and log any errors

Returns:
    true if any error occurred)doc";

static const char *__doc_udmaio_UioAxiDmaIf_clear_interrupt = R"doc(Wait for interrupt and acknowledge it)doc";

static const char *__doc_udmaio_UioAxiDmaIf_dump_status = R"doc(Dump all status register flags in the log)doc";

static const char *__doc_udmaio_UioAxiDmaIf_get_curr_desc = R"doc()doc";

static const char *__doc_udmaio_UioAxiDmaIf_start =
R"doc(Configure and start the AXI DMA controller

Parameter ``start_desc``:
    Address of first SGDMA descriptor)doc";

static const char *__doc_udmaio_UioConfigBase = R"doc(Base class for HwAccessor creation)doc";

static const char *__doc_udmaio_UioConfigBase_2 = R"doc(Base class for HwAccessor creation)doc";

static const char *__doc_udmaio_UioConfigBase_hw_acc = R"doc()doc";

static const char *__doc_udmaio_UioConfigBase_mode =
R"doc(Mode of physical connection to the UioIf object

Returns:
    DmaMode enum)doc";

static const char *__doc_udmaio_UioConfigUio = R"doc(Creates HwAccessor from UioDeviceLocation (UIO version))doc";

static const char *__doc_udmaio_UioConfigUio_get_map_region = R"doc()doc";

static const char *__doc_udmaio_UioConfigUio_get_uio_number = R"doc()doc";

static const char *__doc_udmaio_UioConfigUio_hw_acc = R"doc()doc";

static const char *__doc_udmaio_UioConfigUio_mode = R"doc()doc";

static const char *__doc_udmaio_UioConfigXdma = R"doc(Creates HwAccessor from UioDeviceLocation (XDMA version))doc";

static const char *__doc_udmaio_UioConfigXdma_UioConfigXdma = R"doc()doc";

static const char *__doc_udmaio_UioConfigXdma_UioConfigXdma_2 =
R"doc(Create UioConfigXdma

Parameter ``xdma_path``:
    XDMA device path `/dev/...`

Parameter ``pcie_offs``:
    PCIe offset in memory)doc";

static const char *__doc_udmaio_UioConfigXdma_hw_acc = R"doc()doc";

static const char *__doc_udmaio_UioConfigXdma_mode = R"doc()doc";

static const char *__doc_udmaio_UioConfigXdma_pcie_offs = R"doc()doc";

static const char *__doc_udmaio_UioConfigXdma_x7_series_mode = R"doc()doc";

static const char *__doc_udmaio_UioConfigXdma_xdma_path = R"doc()doc";

static const char *__doc_udmaio_UioDeviceLocation = R"doc(Holds information where a device can be found over both UIO and XDMA)doc";

static const char *__doc_udmaio_UioDeviceLocation_UioDeviceLocation = R"doc()doc";

static const char *__doc_udmaio_UioDeviceLocation_UioDeviceLocation_2 = R"doc()doc";

static const char *__doc_udmaio_UioDeviceLocation_hw_acc = R"doc()doc";

static const char *__doc_udmaio_UioDeviceLocation_hw_acc_override = R"doc(Override the regular hardware accessor (for testing purposes))doc";

static const char *__doc_udmaio_UioDeviceLocation_set_link_axi = R"doc(Set UioIf's globally to use a AXI/UIO link)doc";

static const char *__doc_udmaio_UioDeviceLocation_set_link_xdma =
R"doc(Set UioIf's globally to use a XDMA link

Parameter ``xdma_path``:
    XDMA device instance directory in `/dev`

Parameter ``pcie_offs``:
    XDMA core PCIe memory offset

Parameter ``x7_series_mode``:
    Set the interface to Xilinx 7 series mode. PCIe connections to
    that device will be limited to 32 bits.)doc";

static const char *__doc_udmaio_UioDeviceLocation_uio_name = R"doc(Device name (from device tree) for access through UIO)doc";

static const char *__doc_udmaio_UioDeviceLocation_xdma_evt_dev = R"doc(optional: Event file for access through XDMA)doc";

static const char *__doc_udmaio_UioDeviceLocation_xdma_region = R"doc(Memory-mapped region for access through XDMA)doc";

static const char *__doc_udmaio_UioIf = R"doc(Base class for UIO interfaces)doc";

static const char *__doc_udmaio_UioIf_UioIf = R"doc()doc";

static const char *__doc_udmaio_UioIf_arm_interrupt = R"doc()doc";

static const char *__doc_udmaio_UioIf_enable_debug = R"doc()doc";

static const char *__doc_udmaio_UioIf_get_fd_int =
R"doc(Get file descriptor of interrupt event file

Returns:
    Event file descriptor)doc";

static const char *__doc_udmaio_UioIf_hw = R"doc()doc";

static const char *__doc_udmaio_UioIf_lg = R"doc()doc";

static const char *__doc_udmaio_UioIf_rd32 = R"doc()doc";

static const char *__doc_udmaio_UioIf_rd64 = R"doc()doc";

static const char *__doc_udmaio_UioIf_rd_reg = R"doc()doc";

static const char *__doc_udmaio_UioIf_reg_to_raw = R"doc()doc";

static const char *__doc_udmaio_UioIf_reg_to_raw_2 = R"doc()doc";

static const char *__doc_udmaio_UioIf_wait_for_interrupt = R"doc()doc";

static const char *__doc_udmaio_UioIf_wr32 = R"doc()doc";

static const char *__doc_udmaio_UioIf_wr64 = R"doc()doc";

static const char *__doc_udmaio_UioIf_wr_reg = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma =
R"doc(Interface to AXI DMA scatter-gather buffers & descriptors Uses a UioIf
to access DMA descriptor memory)doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDesc = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescControl = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescControl_buffer_len = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescControl_rsvd = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescControl_rxeof = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescControl_rxsof = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescStatus = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescStatus_cmplt = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescStatus_dmadecerr = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescStatus_dmainterr = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescStatus_dmaslverr = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescStatus_num_stored_bytes = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescStatus_rxeof = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDescStatus_rxsof = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDesc_app = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDesc_buffer_addr = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDesc_control = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDesc_nxtdesc = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDesc_rsvd0x10 = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDesc_rsvd0x14 = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_S2mmDesc_status = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_UioMemSgdma = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_buf_addrs = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_buf_size = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_desc_statuses = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_descriptors = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_get_first_desc_addr =
R"doc(Get address of first SGDMA descriptor (needed for the AXI DMA I/F)

Returns:
    Address of first SGDMA descriptor)doc";

static const char *__doc_udmaio_UioMemSgdma_get_full_buffers =
R"doc(Get all full SGDMA buffers

Returns:
    Vector of buffer indices of full buffers)doc";

static const char *__doc_udmaio_UioMemSgdma_get_next_packet =
R"doc(Get SGDMA buffers for next packet

Returns:
    Vector of buffer indices for next packet Returns only complete
    packets)doc";

static const char *__doc_udmaio_UioMemSgdma_init_buffers =
R"doc(Initialize SGDMA descriptors

Parameter ``mem``:
    Memory receiving the SGDMA data

Parameter ``num_buffers``:
    Number of descriptors / SGDMA blocks

Parameter ``buf_size``:
    Size of each SGDMA block)doc";

static const char *__doc_udmaio_UioMemSgdma_mem = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_next_readable_buf = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_nr_cyc_desc = R"doc()doc";

static const char *__doc_udmaio_UioMemSgdma_print_desc =
R"doc(Print SGDMA descriptor

Parameter ``desc``:
    SGDMA descriptor)doc";

static const char *__doc_udmaio_UioMemSgdma_print_descs = R"doc(Print all SGDMA descriptors)doc";

static const char *__doc_udmaio_UioMemSgdma_read_buffers =
R"doc(Read data from a set of buffers

Parameter ``indices``:
    Vector of buffer indices to read)doc";

static const char *__doc_udmaio_UioMemSgdma_write_cyc_mode = R"doc()doc";

static const char *__doc_udmaio_UioRegion = R"doc(General-purpose struct to define a memory area)doc";

static const char *__doc_udmaio_UioRegion_addr = R"doc(< Start of region)doc";

static const char *__doc_udmaio_UioRegion_size = R"doc(< Size of region)doc";

static const char *__doc_udmaio_operator_lshift = R"doc()doc";

static const char *__doc_udmaio_operator_lshift_2 = R"doc()doc";

static const char *__doc_udmaio_operator_lshift_3 = R"doc()doc";

static const char *__doc_udmaio_operator_rshift = R"doc()doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

