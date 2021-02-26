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

#include <iostream>
#include <string>

enum DmaMode { XDMA, ARM };

std::istream &operator>>(std::istream &in, DmaMode &mode) {
    std::string token;
    in >> token;
    if (token == "xdma")
        mode = DmaMode::XDMA;
    else if (token == "arm")
        mode = DmaMode::ARM;
    else
        in.setstate(std::ios_base::failbit);
    return in;
}
