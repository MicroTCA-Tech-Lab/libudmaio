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
:DataHandlerSync(*dma_ptr, *desc_ptr, *mem_ptr)
,_dma_ptr(dma_ptr)
,_desc_ptr(desc_ptr)
,_mem_ptr(mem_ptr) {
}

void DataHandlerPython::start(size_t pkt_size) {
    _desc_ptr->init_buffers(*_mem_ptr, 32, pkt_size);

    uintptr_t first_desc = _desc_ptr->get_first_desc_addr();
    _dma_ptr->start(first_desc);

    this->operator()();
}

py::array_t<uint16_t> DataHandlerPython::numpy_read(uint32_t ms_timeout) {
    // Create vector on the heap, holding the data
    auto vec = new std::vector<uint8_t>(
        read(std::chrono::milliseconds{ms_timeout})
    );
    // Callback for Python garbage collector
    py::capsule gc_callback(vec, [](void *f) {
        auto ptr = reinterpret_cast<std::vector<uint8_t> *>(f);
        delete ptr;
    });
    // Return Numpy array, transferring ownership to Python
    return py::array_t<uint16_t>(
        {vec->size() / sizeof(uint16_t)}, // shape
        {sizeof(uint16_t)},               // stride
        reinterpret_cast<uint16_t*>(vec->data()), // data pointer
        gc_callback
    );
}

}
