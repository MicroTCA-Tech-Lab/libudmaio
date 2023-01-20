#pragma once

#include <cstdint>

#include "udmaio/UioIf.hpp"

namespace udmaio {

/**
 * @brief Accessor for register autogenerated via HECTARE from RDL
 * 
 * @tparam C Register content type
 * @param offset Register offset into UioIf address space
 */
template <typename C, uint32_t offset>
class RegAccessor {
    static_assert((sizeof(C) % sizeof(uint32_t)) == 0, "Register size must be aligned to 32 bits");
    static_assert((offset % sizeof(uint32_t)) == 0, "Register offset must be aligned to 32 bits");

    UioIf* _if;

  public:
    using constructor_arg_type = UioIf*;

    RegAccessor(constructor_arg_type interface) : _if{interface} {}

    /**
     * @brief Read register
     * 
     * @return C Register content
     */
    C rd() const {
        C result;
        uint32_t* ptr = reinterpret_cast<uint32_t*>(&result);
        for (uint32_t i = 0; i < sizeof(C); i += sizeof(uint32_t)) {
            *ptr++ = _if->_rd32(offset + i);
        }
        return result;
    }

    /**
     * @brief Write register
     * 
     * @param value Content to write to register
     */
    void wr(const C& value) {
        const uint32_t* ptr = reinterpret_cast<const uint32_t*>(&value);
        for (uint32_t i = 0; i < sizeof(C); i += sizeof(uint32_t)) {
            _if->_wr32(offset + i, *ptr++);
        }
    }
};

/**
 * @brief Convert fixed C string to a std::string
 * 
 * @tparam T Register field holding the string (must be of type char[N])
 * @param chararray Register field holding the string (must be of type char[N])
 * @return std::string Converted C++ string, limited to max. N chars
 */
template <typename T>
std::string field_to_str(const T& chararray) {
    return {chararray, strnlen(chararray, sizeof(chararray))};
}

}  // namespace udmaio