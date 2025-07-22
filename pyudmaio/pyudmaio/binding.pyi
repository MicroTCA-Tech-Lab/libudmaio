from __future__ import annotations
import numpy
import numpy.typing
import typing
__all__ = ['ConfigBase', 'ConfigUio', 'ConfigXdma', 'DEBUG', 'DataHandler', 'DmaBufferAbstract', 'FATAL', 'FpgaMemBufferOverAxi', 'FpgaMemBufferOverXdma', 'FrameFormat', 'HwAccessor', 'INFO', 'LogLevel', 'TRACE', 'UDmaBuf', 'UioAxiDmaIf', 'UioDeviceLocation', 'UioIf', 'UioMemSgdma', 'UioRegion', 'set_logging_level']
class ConfigBase:
    """
    Base class for HwAccessor creation
    """
    def hw_acc(self, arg0: UioDeviceLocation) -> HwAccessor:
        ...
class ConfigUio(ConfigBase):
    """
    Creates HwAccessor from UioDeviceLocation (UIO version)
    """
    def __init__(self) -> None:
        ...
class ConfigXdma(ConfigBase):
    """
    Creates HwAccessor from UioDeviceLocation (XDMA version)
    """
    def __init__(self, xdma_path: str, pcie_offs: typing.SupportsInt, x7_series_mode: bool = False) -> None:
        ...
class DataHandler:
    """
    """
    def __init__(self, dma: UioAxiDmaIf, desc: UioMemSgdma, mem: DmaBufferAbstract, receive_packets: bool = True, queue_size: typing.SupportsInt = 64, rt_prio: bool = False) -> None:
        ...
    def read(self, ms_timeout: typing.SupportsInt) -> numpy.typing.NDArray[numpy.uint8]:
        ...
    def read_nb(self) -> numpy.typing.NDArray[numpy.uint8]:
        ...
    def start(self, nr_pkts: typing.SupportsInt, pkt_size: typing.SupportsInt, init_only: bool = False) -> None:
        ...
    def stop(self) -> None:
        ...
class DmaBufferAbstract:
    """
    Base class for DMA data buffer
    """
    def get_phys_region(self) -> UioRegion:
        """
        Get physical region
        
        Returns:
            Physical address and size of DMA data buffer
        """
class FpgaMemBufferOverAxi(DmaBufferAbstract, UioIf):
    """
    DMA data buffer accessed over AXI/UIO, described w/ explicit address &
    size
    """
    def __init__(self, arg0: UioDeviceLocation) -> None:
        ...
class FpgaMemBufferOverXdma(DmaBufferAbstract):
    """
    DMA data buffer accessed over XDMA using the xdma c2h0 stream channel
    """
    def __init__(self, arg0: str, arg1: typing.SupportsInt) -> None:
        """
        Constructs a DMA data buffer
        
        Parameter ``path``:
            Base path of XDMA instance in `/dev`
        
        Parameter ``phys_addr``:
            Physical address of DMA data buffer
        """
class FrameFormat:
    """
    """
    class Dim:
        """
        Frame dimensions
        """
        def __init__(self, arg0: typing.SupportsInt, arg1: typing.SupportsInt) -> None:
            ...
        @property
        def height(self) -> int:
            ...
        @height.setter
        def height(self, arg0: typing.SupportsInt) -> None:
            ...
        @property
        def width(self) -> int:
            ...
        @width.setter
        def width(self, arg0: typing.SupportsInt) -> None:
            ...
    class PixelFormat:
        """
        
        
        Members:
        
          unknown
        
          Mono8
        
          Mono10
        
          Mono12
        
          Mono14
        
          Mono16
        
          BayerGR8
        
          BayerRG8
        
          BayerGB8
        
          BayerBG8
        
          BayerGR10
        
          BayerRG10
        
          BayerGB10
        
          BayerBG10
        
          BayerGR12
        
          BayerRG12
        
          BayerGB12
        
          BayerBG12
        
          RGB8
        
          BGR8
        
          RGBa8
        
          BGRa8
        
          RGB10V1Packed1
        
          RGB10p32
        
          RGB565p
        
          BGR565p
        
          YUV422_8_UYVY
        
          YUV422_8
        
          YUV8_UYV
        
          YCbCr8_CbYCr
        
          YCbCr422_8
        
          YCbCr422_8_CbYCrY
        
          YCbCr601_8_CbYCr
        
          YCbCr601_422_8
        
          YCbCr601_422_8_CbYCrY
        
          YCbCr709_8_CbYCr
        
          YCbCr709_422_8
        
          YCbCr709_422_8_CbYCrY
        
          RGB8_Planar
        """
        BGR565p: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BGR565p: 34603062>
        BGR8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BGR8: 35127317>
        BGRa8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BGRa8: 35651607>
        BayerBG10: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerBG10: 17825807>
        BayerBG12: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerBG12: 17825811>
        BayerBG8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerBG8: 17301515>
        BayerGB10: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGB10: 17825806>
        BayerGB12: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGB12: 17825810>
        BayerGB8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGB8: 17301514>
        BayerGR10: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGR10: 17825804>
        BayerGR12: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGR12: 17825808>
        BayerGR8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGR8: 17301512>
        BayerRG10: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerRG10: 17825805>
        BayerRG12: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerRG12: 17825809>
        BayerRG8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerRG8: 17301513>
        Mono10: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.Mono10: 17825795>
        Mono12: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.Mono12: 17825797>
        Mono14: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.Mono14: 17825829>
        Mono16: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.Mono16: 17825799>
        Mono8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.Mono8: 17301505>
        RGB10V1Packed1: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGB10V1Packed1: 35651612>
        RGB10p32: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGB10p32: 35651613>
        RGB565p: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGB565p: 34603061>
        RGB8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGB8: 35127316>
        RGB8_Planar: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGB8_Planar: 35127329>
        RGBa8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGBa8: 35651606>
        YCbCr422_8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr422_8: 34603067>
        YCbCr422_8_CbYCrY: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr422_8_CbYCrY: 34603075>
        YCbCr601_422_8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr601_422_8: 34603070>
        YCbCr601_422_8_CbYCrY: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr601_422_8_CbYCrY: 34603076>
        YCbCr601_8_CbYCr: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr601_8_CbYCr: 35127357>
        YCbCr709_422_8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr709_422_8: 34603073>
        YCbCr709_422_8_CbYCrY: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr709_422_8_CbYCrY: 34603077>
        YCbCr709_8_CbYCr: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr709_8_CbYCr: 35127360>
        YCbCr8_CbYCr: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr8_CbYCr: 35127354>
        YUV422_8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YUV422_8: 34603058>
        YUV422_8_UYVY: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YUV422_8_UYVY: 34603039>
        YUV8_UYV: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YUV8_UYV: 35127328>
        __members__: typing.ClassVar[dict[str, FrameFormat.PixelFormat]]  # value = {'unknown': <PixelFormat.unknown: 0>, 'Mono8': <PixelFormat.Mono8: 17301505>, 'Mono10': <PixelFormat.Mono10: 17825795>, 'Mono12': <PixelFormat.Mono12: 17825797>, 'Mono14': <PixelFormat.Mono14: 17825829>, 'Mono16': <PixelFormat.Mono16: 17825799>, 'BayerGR8': <PixelFormat.BayerGR8: 17301512>, 'BayerRG8': <PixelFormat.BayerRG8: 17301513>, 'BayerGB8': <PixelFormat.BayerGB8: 17301514>, 'BayerBG8': <PixelFormat.BayerBG8: 17301515>, 'BayerGR10': <PixelFormat.BayerGR10: 17825804>, 'BayerRG10': <PixelFormat.BayerRG10: 17825805>, 'BayerGB10': <PixelFormat.BayerGB10: 17825806>, 'BayerBG10': <PixelFormat.BayerBG10: 17825807>, 'BayerGR12': <PixelFormat.BayerGR12: 17825808>, 'BayerRG12': <PixelFormat.BayerRG12: 17825809>, 'BayerGB12': <PixelFormat.BayerGB12: 17825810>, 'BayerBG12': <PixelFormat.BayerBG12: 17825811>, 'RGB8': <PixelFormat.RGB8: 35127316>, 'BGR8': <PixelFormat.BGR8: 35127317>, 'RGBa8': <PixelFormat.RGBa8: 35651606>, 'BGRa8': <PixelFormat.BGRa8: 35651607>, 'RGB10V1Packed1': <PixelFormat.RGB10V1Packed1: 35651612>, 'RGB10p32': <PixelFormat.RGB10p32: 35651613>, 'RGB565p': <PixelFormat.RGB565p: 34603061>, 'BGR565p': <PixelFormat.BGR565p: 34603062>, 'YUV422_8_UYVY': <PixelFormat.YUV422_8_UYVY: 34603039>, 'YUV422_8': <PixelFormat.YUV422_8: 34603058>, 'YUV8_UYV': <PixelFormat.YUV8_UYV: 35127328>, 'YCbCr8_CbYCr': <PixelFormat.YCbCr8_CbYCr: 35127354>, 'YCbCr422_8': <PixelFormat.YCbCr422_8: 34603067>, 'YCbCr422_8_CbYCrY': <PixelFormat.YCbCr422_8_CbYCrY: 34603075>, 'YCbCr601_8_CbYCr': <PixelFormat.YCbCr601_8_CbYCr: 35127357>, 'YCbCr601_422_8': <PixelFormat.YCbCr601_422_8: 34603070>, 'YCbCr601_422_8_CbYCrY': <PixelFormat.YCbCr601_422_8_CbYCrY: 34603076>, 'YCbCr709_8_CbYCr': <PixelFormat.YCbCr709_8_CbYCr: 35127360>, 'YCbCr709_422_8': <PixelFormat.YCbCr709_422_8: 34603073>, 'YCbCr709_422_8_CbYCrY': <PixelFormat.YCbCr709_422_8_CbYCrY: 34603077>, 'RGB8_Planar': <PixelFormat.RGB8_Planar: 35127329>}
        unknown: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.unknown: 0>
        def __eq__(self, other: typing.Any) -> bool:
            ...
        def __getstate__(self) -> int:
            ...
        def __hash__(self) -> int:
            ...
        def __index__(self) -> int:
            ...
        def __init__(self, value: typing.SupportsInt) -> None:
            ...
        def __int__(self) -> int:
            ...
        def __ne__(self, other: typing.Any) -> bool:
            ...
        def __repr__(self) -> str:
            ...
        def __setstate__(self, state: typing.SupportsInt) -> None:
            ...
        def __str__(self) -> str:
            ...
        @property
        def name(self) -> str:
            ...
        @property
        def value(self) -> int:
            ...
    BGR565p: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BGR565p: 34603062>
    BGR8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BGR8: 35127317>
    BGRa8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BGRa8: 35651607>
    BayerBG10: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerBG10: 17825807>
    BayerBG12: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerBG12: 17825811>
    BayerBG8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerBG8: 17301515>
    BayerGB10: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGB10: 17825806>
    BayerGB12: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGB12: 17825810>
    BayerGB8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGB8: 17301514>
    BayerGR10: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGR10: 17825804>
    BayerGR12: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGR12: 17825808>
    BayerGR8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerGR8: 17301512>
    BayerRG10: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerRG10: 17825805>
    BayerRG12: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerRG12: 17825809>
    BayerRG8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.BayerRG8: 17301513>
    Mono10: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.Mono10: 17825795>
    Mono12: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.Mono12: 17825797>
    Mono14: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.Mono14: 17825829>
    Mono16: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.Mono16: 17825799>
    Mono8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.Mono8: 17301505>
    RGB10V1Packed1: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGB10V1Packed1: 35651612>
    RGB10p32: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGB10p32: 35651613>
    RGB565p: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGB565p: 34603061>
    RGB8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGB8: 35127316>
    RGB8_Planar: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGB8_Planar: 35127329>
    RGBa8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.RGBa8: 35651606>
    YCbCr422_8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr422_8: 34603067>
    YCbCr422_8_CbYCrY: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr422_8_CbYCrY: 34603075>
    YCbCr601_422_8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr601_422_8: 34603070>
    YCbCr601_422_8_CbYCrY: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr601_422_8_CbYCrY: 34603076>
    YCbCr601_8_CbYCr: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr601_8_CbYCr: 35127357>
    YCbCr709_422_8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr709_422_8: 34603073>
    YCbCr709_422_8_CbYCrY: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr709_422_8_CbYCrY: 34603077>
    YCbCr709_8_CbYCr: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr709_8_CbYCr: 35127360>
    YCbCr8_CbYCr: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YCbCr8_CbYCr: 35127354>
    YUV422_8: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YUV422_8: 34603058>
    YUV422_8_UYVY: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YUV422_8_UYVY: 34603039>
    YUV8_UYV: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.YUV8_UYV: 35127328>
    unknown: typing.ClassVar[FrameFormat.PixelFormat]  # value = <PixelFormat.unknown: 0>
    def __init__(self) -> None:
        ...
    def get_bytes_per_pixel(self) -> int:
        ...
    def get_dim(self) -> FrameFormat.Dim:
        ...
    def get_frm_size(self) -> int:
        ...
    def get_hsize(self) -> int:
        ...
    def get_pixel_format(self) -> FrameFormat.PixelFormat:
        ...
    def get_pixel_format_str(self) -> str:
        ...
    def get_pixel_per_word(self) -> int:
        ...
    def get_word_width(self) -> int:
        ...
    def set_bpp(self, arg0: typing.SupportsInt) -> None:
        ...
    def set_dim(self, arg0: FrameFormat.Dim) -> None:
        ...
    @typing.overload
    def set_format(self, dim: FrameFormat.Dim, bytes_per_pixel: typing.SupportsInt, word_width: typing.SupportsInt = 4) -> None:
        ...
    @typing.overload
    def set_format(self, dim: FrameFormat.Dim, pix_fmt_str: str, word_width: typing.SupportsInt = 4) -> None:
        ...
    @typing.overload
    def set_format(self, dim: FrameFormat.Dim, pix_fmt: FrameFormat.PixelFormat, word_width: typing.SupportsInt = 4) -> None:
        ...
    def set_pix_fmt(self, arg0: FrameFormat.PixelFormat) -> None:
        ...
    def set_word_width(self, arg0: typing.SupportsInt) -> None:
        ...
class HwAccessor:
    """
    """
class LogLevel:
    """
    Members:
    
      FATAL
    
      INFO
    
      DEBUG
    
      TRACE
    """
    DEBUG: typing.ClassVar[LogLevel]  # value = <LogLevel.DEBUG: 1>
    FATAL: typing.ClassVar[LogLevel]  # value = <LogLevel.FATAL: 5>
    INFO: typing.ClassVar[LogLevel]  # value = <LogLevel.INFO: 2>
    TRACE: typing.ClassVar[LogLevel]  # value = <LogLevel.TRACE: 0>
    __members__: typing.ClassVar[dict[str, LogLevel]]  # value = {'FATAL': <LogLevel.FATAL: 5>, 'INFO': <LogLevel.INFO: 2>, 'DEBUG': <LogLevel.DEBUG: 1>, 'TRACE': <LogLevel.TRACE: 0>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __ne__(self, other: typing.Any) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    def __setstate__(self, state: typing.SupportsInt) -> None:
        ...
    def __str__(self) -> str:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def value(self) -> int:
        ...
class UDmaBuf(DmaBufferAbstract):
    """
    DMA data buffer accessed over AXI/UIO, implemented w/ udmabuf (see
    https://github.com/ikwzm/udmabuf)
    """
    def __init__(self, buf_idx: typing.SupportsInt = 0) -> None:
        """
        Constructs a UDmaBuf
        
        Parameter ``buf_idx``:
            Buffer index `/dev/udmabufN`
        """
class UioAxiDmaIf(UioIf):
    """
    Interface to AXI DMA Core
    """
    def __init__(self, arg0: UioDeviceLocation) -> None:
        ...
    def dump_status(self) -> None:
        """
        Dump all status register flags in the log
        """
class UioDeviceLocation:
    """
    Holds information where a device can be found over both UIO and XDMA
    """
    @staticmethod
    def set_link_axi() -> None:
        """
        Set UioIf's globally to use a AXI/UIO link
        """
    @staticmethod
    def set_link_xdma(arg0: str, arg1: typing.SupportsInt, arg2: bool) -> None:
        """
        Set UioIf's globally to use a XDMA link
        
        Parameter ``xdma_path``:
            XDMA device instance directory in `/dev`
        
        Parameter ``pcie_offs``:
            XDMA core PCIe memory offset
        
        Parameter ``x7_series_mode``:
            Set the interface to Xilinx 7 series mode. PCIe connections to
            that device will be limited to 32 bits.
        """
    def __init__(self, uioname: str, xdmaregion: UioRegion = ..., xdmaevtdev: str = '') -> None:
        ...
    def hw_acc(self) -> HwAccessor:
        ...
    @property
    def uio_name(self) -> str:
        """
        Device name (from device tree) for access through UIO
        """
    @uio_name.setter
    def uio_name(self, arg0: str) -> None:
        ...
    @property
    def xdma_evt_dev(self) -> str:
        """
        optional: Event file for access through XDMA
        """
    @xdma_evt_dev.setter
    def xdma_evt_dev(self, arg0: str) -> None:
        ...
    @property
    def xdma_region(self) -> UioRegion:
        """
        Memory-mapped region for access through XDMA
        """
    @xdma_region.setter
    def xdma_region(self, arg0: UioRegion) -> None:
        ...
class UioIf:
    """
    Base class for UIO interfaces
    """
    def __init__(self, arg0: str, arg1: UioDeviceLocation) -> None:
        ...
    def _rd32(self, arg0: typing.SupportsInt) -> int:
        ...
    def _wr32(self, arg0: typing.SupportsInt, arg1: typing.SupportsInt) -> None:
        ...
    def arm_interrupt(self) -> None:
        ...
    def read_bulk(self, arg0: typing.SupportsInt, arg1: typing.SupportsInt) -> numpy.typing.NDArray[numpy.uint8]:
        ...
    def wait_for_interrupt(self) -> int:
        ...
class UioMemSgdma(UioIf):
    """
    Interface to AXI DMA scatter-gather buffers & descriptors Uses a UioIf
    to access DMA descriptor memory
    """
    def __init__(self, arg0: UioDeviceLocation) -> None:
        ...
    def print_descs(self) -> None:
        """
        Print all SGDMA descriptors
        """
class UioRegion:
    """
    General-purpose struct to define a memory area
    """
    def __init__(self, arg0: typing.SupportsInt, arg1: typing.SupportsInt) -> None:
        ...
    @property
    def addr(self) -> int:
        ...
    @addr.setter
    def addr(self, arg0: typing.SupportsInt) -> None:
        ...
    @property
    def size(self) -> int:
        ...
    @size.setter
    def size(self, arg0: typing.SupportsInt) -> None:
        ...
def set_logging_level(arg0: LogLevel) -> None:
    ...
DEBUG: LogLevel  # value = <LogLevel.DEBUG: 1>
FATAL: LogLevel  # value = <LogLevel.FATAL: 5>
INFO: LogLevel  # value = <LogLevel.INFO: 2>
TRACE: LogLevel  # value = <LogLevel.TRACE: 0>
