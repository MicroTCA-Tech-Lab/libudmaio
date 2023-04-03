# libudmaio refactoring & unittest support

* Rename UioIf to HwAccessor
    * Create HwAccessor class hierarchy
    * HwAccessorXdma64
    * HwAccessorXdma32
    * HwAccessorAxi
    * HwAccessorMock

* Create HwAccessorFactory
    * Creates a HwAccessor instance when given a UioDeviceInfo
    * (just a static instance of HwAccessorBase?)

* Create a new UioIf as a base class for drivers, which uses a HwAccessor to do the actual accesses
    * UioIf receives same UioDeviceInfo etc as before, but uses a factory to populate the HwAccessor

* Clean up UioConfig (most of it should not be necessary anymore)

# variations of original UioIf depending on i/f

## XDMA

* dev_path  = _xdma_path + "/user"
* evt_path  = _xdma_path + "/" + evt_dev  or  ""
* region    = {dev_region.addr | _pcie_offs, dev_region.size},
* mmap_offs = dev_region.addr

## AXI

* dev_path  = std::string{"/dev/uio"} + std::to_string(uio_number),
* evt_path  = "",
* region    = _get_map_region(uio_number, map_index),
* mmap_offs = static_cast<uintptr_t>(map_index * getpagesize()),
