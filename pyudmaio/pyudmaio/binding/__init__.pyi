from __future__ import annotations
import pyudmaio.binding
import typing
import numpy
_Shape = typing.Tuple[int, ...]

__all__ = [
    "ConfigBase",
    "ConfigUio",
    "ConfigXdma",
    "DEBUG",
    "DataHandler",
    "DmaBufferAbstract",
    "FATAL",
    "FpgaMemBufferOverXdma",
    "FrameFormat",
    "HwAccessor",
    "INFO",
    "LogLevel",
    "TRACE",
    "UDmaBuf",
    "UioAxiDmaIf",
    "UioDeviceLocation",
    "UioIf",
    "UioMemSgdma",
    "UioRegion",
    "set_logging_level"
]


class ConfigBase():
    def hw_acc(self, arg0: UioDeviceLocation) -> HwAccessor: ...
    pass
class ConfigUio(ConfigBase):
    def __init__(self) -> None: ...
    pass
class ConfigXdma(ConfigBase):
    def __init__(self, xdma_path: str, pcie_offs: int, x7_series_mode: bool = False) -> None: ...
    pass
class DataHandler():
    def __init__(self, arg0: UioAxiDmaIf, arg1: UioMemSgdma, arg2: DmaBufferAbstract) -> None: ...
    def read(self, ms_timeout: int) -> numpy.ndarray[numpy.uint8]: ...
    def read_nb(self) -> numpy.ndarray[numpy.uint8]: ...
    def start(self, nr_pkts: int, pkt_size: int, init_only: bool = False) -> None: ...
    def stop(self) -> None: ...
    pass
class DmaBufferAbstract():
    def get_phys_addr(self) -> int: ...
    def get_phys_size(self) -> int: ...
    pass
class FpgaMemBufferOverXdma(DmaBufferAbstract):
    def __init__(self, arg0: str, arg1: int) -> None: ...
    pass
class FrameFormat():
    class Dim():
        def __init__(self, arg0: int, arg1: int) -> None: ...
        @property
        def height(self) -> int:
            """
            :type: int
            """
        @height.setter
        def height(self, arg0: int) -> None:
            pass
        @property
        def width(self) -> int:
            """
            :type: int
            """
        @width.setter
        def width(self, arg0: int) -> None:
            pass
        pass
    class PixelFormat():
        """
        Members:

          unknown

          Mono8

          Mono10

          Mono12

          Mono14

          Mono16
        """
        def __eq__(self, other: object) -> bool: ...
        def __getstate__(self) -> int: ...
        def __hash__(self) -> int: ...
        def __index__(self) -> int: ...
        def __init__(self, value: int) -> None: ...
        def __int__(self) -> int: ...
        def __ne__(self, other: object) -> bool: ...
        def __repr__(self) -> str: ...
        def __setstate__(self, state: int) -> None: ...
        @property
        def name(self) -> str:
            """
            :type: str
            """
        @property
        def value(self) -> int:
            """
            :type: int
            """
        Mono10: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.Mono10: 17825795>
        Mono12: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.Mono12: 17825797>
        Mono14: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.Mono14: 17825829>
        Mono16: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.Mono16: 17825799>
        Mono8: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.Mono8: 17301505>
        __members__: dict # value = {'unknown': <PixelFormat.unknown: 0>, 'Mono8': <PixelFormat.Mono8: 17301505>, 'Mono10': <PixelFormat.Mono10: 17825795>, 'Mono12': <PixelFormat.Mono12: 17825797>, 'Mono14': <PixelFormat.Mono14: 17825829>, 'Mono16': <PixelFormat.Mono16: 17825799>}
        unknown: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.unknown: 0>
        pass
    def __init__(self) -> None: ...
    def get_bytes_per_pixel(self) -> int: ...
    def get_dim(self) -> FrameFormat.Dim: ...
    def get_frm_size(self) -> int: ...
    def get_hsize(self) -> int: ...
    def get_pixel_format(self) -> FrameFormat.PixelFormat: ...
    def get_pixel_format_str(self) -> str: ...
    def get_pixel_per_word(self) -> int: ...
    def get_word_width(self) -> int: ...
    def set_bpp(self, arg0: int) -> None: ...
    def set_dim(self, arg0: FrameFormat.Dim) -> None: ...
    @typing.overload
    def set_format(self, dim: FrameFormat.Dim, bytes_per_pixel: int, word_width: int = 4) -> None: ...
    @typing.overload
    def set_format(self, dim: FrameFormat.Dim, pix_fmt_str: str, word_width: int = 4) -> None: ...
    @typing.overload
    def set_format(self, dim: FrameFormat.Dim, pix_fmt: FrameFormat.PixelFormat, word_width: int = 4) -> None: ...
    def set_pix_fmt(self, arg0: FrameFormat.PixelFormat) -> None: ...
    def set_word_width(self, arg0: int) -> None: ...
    Mono10: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.Mono10: 17825795>
    Mono12: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.Mono12: 17825797>
    Mono14: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.Mono14: 17825829>
    Mono16: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.Mono16: 17825799>
    Mono8: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.Mono8: 17301505>
    unknown: pyudmaio.binding.FrameFormat.PixelFormat # value = <PixelFormat.unknown: 0>
    pass
class HwAccessor():
    pass
class LogLevel():
    """
    Members:

      FATAL

      INFO

      DEBUG

      TRACE
    """
    def __eq__(self, other: object) -> bool: ...
    def __getstate__(self) -> int: ...
    def __hash__(self) -> int: ...
    def __index__(self) -> int: ...
    def __init__(self, value: int) -> None: ...
    def __int__(self) -> int: ...
    def __ne__(self, other: object) -> bool: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, state: int) -> None: ...
    @property
    def name(self) -> str:
        """
        :type: str
        """
    @property
    def value(self) -> int:
        """
        :type: int
        """
    DEBUG: pyudmaio.binding.LogLevel # value = <LogLevel.DEBUG: 1>
    FATAL: pyudmaio.binding.LogLevel # value = <LogLevel.FATAL: 5>
    INFO: pyudmaio.binding.LogLevel # value = <LogLevel.INFO: 2>
    TRACE: pyudmaio.binding.LogLevel # value = <LogLevel.TRACE: 0>
    __members__: dict # value = {'FATAL': <LogLevel.FATAL: 5>, 'INFO': <LogLevel.INFO: 2>, 'DEBUG': <LogLevel.DEBUG: 1>, 'TRACE': <LogLevel.TRACE: 0>}
    pass
class UDmaBuf(DmaBufferAbstract):
    def __init__(self, buf_idx: int = 0) -> None: ...
    pass
class UioIf():
    def __init__(self, arg0: str, arg1: UioDeviceLocation) -> None: ...
    def _rd32(self, arg0: int) -> int: ...
    def _wr32(self, arg0: int, arg1: int) -> None: ...
    def arm_interrupt(self) -> None: ...
    def wait_for_interrupt(self) -> int: ...
    pass
class UioDeviceLocation():
    def __init__(self, uioname: str, xdmaregion: UioRegion, xdmaevtdev: str = '') -> None: ...
    def hw_acc(self) -> HwAccessor: ...
    @staticmethod
    def set_link_axi() -> None: ...
    @staticmethod
    def set_link_xdma(arg0: str, arg1: int, arg2: bool) -> None: ...
    @property
    def uio_name(self) -> str:
        """
        :type: str
        """
    @uio_name.setter
    def uio_name(self, arg0: str) -> None:
        pass
    @property
    def xdma_evt_dev(self) -> str:
        """
        :type: str
        """
    @xdma_evt_dev.setter
    def xdma_evt_dev(self, arg0: str) -> None:
        pass
    @property
    def xdma_region(self) -> UioRegion:
        """
        :type: UioRegion
        """
    @xdma_region.setter
    def xdma_region(self, arg0: UioRegion) -> None:
        pass
    pass
class UioAxiDmaIf(UioIf):
    def __init__(self, arg0: UioDeviceLocation) -> None: ...
    pass
class UioMemSgdma(UioIf):
    def __init__(self, arg0: UioDeviceLocation) -> None: ...
    def print_descs(self) -> None: ...
    pass
class UioRegion():
    def __init__(self, arg0: int, arg1: int) -> None: ...
    @property
    def addr(self) -> int:
        """
        :type: int
        """
    @addr.setter
    def addr(self, arg0: int) -> None:
        pass
    @property
    def size(self) -> int:
        """
        :type: int
        """
    @size.setter
    def size(self, arg0: int) -> None:
        pass
    pass
def set_logging_level(arg0: LogLevel) -> None:
    pass
DEBUG: pyudmaio.binding.LogLevel # value = <LogLevel.DEBUG: 1>
FATAL: pyudmaio.binding.LogLevel # value = <LogLevel.FATAL: 5>
INFO: pyudmaio.binding.LogLevel # value = <LogLevel.INFO: 2>
TRACE: pyudmaio.binding.LogLevel # value = <LogLevel.TRACE: 0>
