//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include "DataHandlerPython.hpp"

namespace udmaio {

DataHandlerPython::DataHandlerPython(std::shared_ptr<UioAxiDmaIf> dma_ptr,
                                     std::shared_ptr<UioMemSgdma> desc_ptr,
                                     std::shared_ptr<DmaBufferAbstract> mem_ptr)
    : DataHandlerSync("DataHandlerPython", *dma_ptr, *desc_ptr, *mem_ptr)
    , _dma_ptr(dma_ptr)
    , _desc_ptr(desc_ptr)
    , _mem_ptr(mem_ptr) {}

void DataHandlerPython::start(int nr_pkts, size_t pkt_size, bool init_only) {
    _desc_ptr->init_buffers(*_mem_ptr, nr_pkts, pkt_size);

    uintptr_t first_desc = _desc_ptr->get_first_desc_addr();
    _dma_ptr->start(first_desc);

    if (!init_only) {
        this->operator()();
    }
}

py::array_t<uint8_t> DataHandlerPython::numpy_read(uint32_t ms_timeout) {
    if (_dma_ptr->check_for_errors()) {
        throw std::runtime_error("DMA has experienced an error");
    }

    BOOST_LOG_SEV(_lg, bls::trace) << "Starting blocking read...";
    // Create vector on the heap, holding the data
    auto vec = new std::vector<uint8_t>(read(std::chrono::milliseconds{ms_timeout}));
    BOOST_LOG_SEV(_lg, bls::trace) << "read() returned with " << vec->size() << " bytes";

    // Callback for Python garbage collector
    py::capsule gc_callback(vec, [](void* f) {
        auto ptr = reinterpret_cast<std::vector<uint8_t>*>(f);
        delete ptr;
    });

    // Return Numpy array, transferring ownership to Python
    return py::array_t<uint8_t>({vec->size() / sizeof(uint8_t)},          // shape
                                {sizeof(uint8_t)},                        // stride
                                reinterpret_cast<uint8_t*>(vec->data()),  // data pointer
                                gc_callback);
}

py::array_t<uint8_t> DataHandlerPython::numpy_read_nb() {
    if (_dma_ptr->check_for_errors()) {
        throw std::runtime_error("DMA has experienced an error");
    }

    std::vector<UioRegion> full_bufs = _desc_ptr->get_full_buffers();
    auto vec = new std::vector<uint8_t>();

    for (auto& buf : full_bufs) {
        _mem_ptr->append_from_buf(buf, *vec);
    }
    // Callback for Python garbage collector
    py::capsule gc_callback(vec, [](void* f) {
        auto ptr = reinterpret_cast<std::vector<uint8_t>*>(f);
        delete ptr;
    });
    // Return Numpy array, transferring ownership to Python
    return py::array_t<uint8_t>({vec->size() / sizeof(uint8_t)},          // shape
                                {sizeof(uint8_t)},                        // stride
                                reinterpret_cast<uint8_t*>(vec->data()),  // data pointer
                                gc_callback);
}

}  // namespace udmaio
