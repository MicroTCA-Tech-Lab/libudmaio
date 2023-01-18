//---------------------------------------------------------------------------//
//        ____  _____________  __    __  __ _           _____ ___   _        //
//       / __ \/ ____/ ___/\ \/ /   |  \/  (_)__ _ _ __|_   _/ __| /_\       //
//      / / / / __/  \__ \  \  /    | |\/| | / _| '_/ _ \| || (__ / _ \      //
//     / /_/ / /___ ___/ /  / /     |_|  |_|_\__|_| \___/|_| \___/_/ \_\     //
//    /_____/_____//____/  /_/      T  E  C  H  N  O  L  O  G  Y   L A B     //
//                                                                           //
//---------------------------------------------------------------------------//

// Copyright (c) 2023 Deutsches Elektronen-Synchrotron DESY

#include "udmaio/DmaBufferAbstract.hpp"

namespace udmaio {

void DmaBufferAbstract::append_from_buf(const UioRegion& buf_info,
                                        std::vector<uint8_t>& out) const {
    size_t old_size = out.size();
    size_t new_size = old_size + buf_info.size;
    out.resize(new_size);

    copy_from_buf(out.data() + old_size, buf_info);
}

}  // namespace udmaio
