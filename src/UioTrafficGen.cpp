//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2021 Deutsches Elektronen-Synchrotron DESY

#include <ios>

#include <boost/log/core/core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

#include "UioTrafficGen.hpp"

namespace blt = boost::log::trivial;

UioTrafficGen::UioTrafficGen(const std::string &uio_name, uintptr_t addr, size_t size)
    : UioIf{uio_name, addr, size, "UioTrafficGen"} {}

void UioTrafficGen::start() {
    StConfig st_config{
        .fields{.ranlen = 0, .randly = 0, .etkts = 0, .rsvd7_3 = 0, .tdest = 0, .pdly = 10}};
    _wr32(ADDR_ST_CONFIG, st_config.data);
    _wr32(ADDR_TR_LEN, 2 << 16 | 0x3FF);
    _wr32(ADDR_ST_CTRL, 1);
}

void UioTrafficGen::print_version() {
    StControl st_control;
    st_control.data = _rd32(ADDR_ST_CTRL);

    BOOST_LOG_SEV(_slg, blt::severity_level::info)
        << _log_name << ": version = 0x" << std::hex << st_control.fields.version << std::dec;
}
