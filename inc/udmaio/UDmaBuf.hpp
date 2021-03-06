//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#pragma once

#include <boost/log/core/core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

#include "DmaBufferAbstract.hpp"

namespace blt = boost::log::trivial;

namespace udmaio {

class UDmaBuf : public DmaBufferAbstract {
    int _fd;
    void *_mem;
    UioRegion _phys;
    boost::log::sources::severity_logger_mt<blt::severity_level> _slg;

    size_t _get_size(int buf_idx) const;
    uintptr_t _get_phys_addr(int buf_idx) const;

  public:
    explicit UDmaBuf(int buf_idx = 0);

    virtual ~UDmaBuf();

    uintptr_t get_phys_addr() const override;
    uintptr_t get_phys_size() const override;

    void copy_from_buf(const UioRegion &buf_info, std::vector<uint8_t> &out) const override;
};

} // namespace udmaio
