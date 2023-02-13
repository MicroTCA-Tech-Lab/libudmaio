#pragma once

#include <cstdint>

#include "udmaio/UioIf.hpp"

#ifndef REG_PACKED_ALIGNED
#define REG_PACKED_ALIGNED __attribute__((packed, aligned(4)))
#endif

namespace udmaio {

/**
 * @brief Base class implementing register access. Only used by subclasses, not used directly
 * @tparam C Register content type
 */
template <typename C>
class RegAccessorBase {
    static_assert((sizeof(C) % sizeof(uint32_t)) == 0,
                  "Register size must be a aligned to 32 bits");

  protected:
    UioIf* _if;

    /**
     * @brief Read register
     * @param offs Offset into address space
     * @return C Register content
     */
    C _rd(uint32_t offs) const { return _if->template _rd_reg<C>(offs); }

    /**
     * @brief Write register
     * @param offs Offset into address space
     * @param value Content to write to register
     */
    void _wr(uint32_t offs, const C& value) { _if->template _wr_reg<C>(offs, value); }

  public:
    using constructor_arg_type = UioIf*;

    RegAccessorBase() = delete;

    /**
     * @brief Construct a RegAccessorBase
     * @param interface Hardware interface
     */
    RegAccessorBase(constructor_arg_type interface) : _if{interface} {}
};

/**
 * @brief Accessor for register array element. Returned by RegAccessorArray<C>::operator[]
 * @tparam C Register content type
 */
template <typename C>
class RegAccessorArrayElement : public RegAccessorBase<C> {
    static_assert((sizeof(C) % sizeof(uint32_t)) == 0, "Register size must be aligned to 32 bits");

    const uint32_t _reg_offs;

  public:
    /**
     * @brief Construct a new register array element accessor
     * @param c Hardware interface
     * @param reg_offs Offset of array element into address space
     */
    RegAccessorArrayElement(typename RegAccessorBase<C>::constructor_arg_type c, uint32_t reg_offs)
        : RegAccessorBase<C>{c}, _reg_offs{reg_offs} {}

    /**
     * @brief Read register array element
     * @return T Register content
     */
    C rd() const { return RegAccessorBase<C>::_rd(_reg_offs); }

    /**
     * @brief Write register
     * @param value Content to write to register
     */
    void wr(const C& value) { RegAccessorBase<C>::_wr(_reg_offs, value); }
};

/**
 * @brief Accessor for register array
 * @tparam C Content type of a register array element
 * @tparam offset Offset of first register array element into address space
 * @tparam arr_size Number of array elements
 * @tparam arr_stride Address distance from one element to the next
 */
template <typename C, uint32_t offset, uint32_t arr_size, uint32_t arr_stride>
class RegAccessorArray : public RegAccessorBase<C> {
    static_assert((sizeof(C) % sizeof(uint32_t)) == 0, "Register size must be aligned to 32 bits");
    static_assert((offset % sizeof(uint32_t)) == 0,
                  "Register array offset must be aligned to 32 bits");
    static_assert(arr_size > 0, "Register array size must be non-zero");
    static_assert((arr_stride % sizeof(uint32_t)) == 0,
                  "Register array stride must be aligned to 32 bits");

  public:
    using RegAccessorBase<C>::RegAccessorBase;

    /**
     * @brief Return element accessor
     * @param idx Index into register array
     * @return RegAccessorArrayElement<C> Accessor for requested array element
     */
    RegAccessorArrayElement<C> operator[](uint32_t idx) {
        assert(idx < arr_size);
        const uint32_t reg_offs = offset + idx * arr_stride;
        return RegAccessorArrayElement<C>{RegAccessorBase<C>::_if, reg_offs};
    }

    const RegAccessorArrayElement<C> operator[](uint32_t idx) const {
        assert(idx < arr_size);
        const uint32_t reg_offs = offset + idx * arr_stride;
        return RegAccessorArrayElement<C>{RegAccessorBase<C>::_if, reg_offs};
    }

    /**
     * @brief Get array size
     * @return constexpr uint32_t Number of elements in this array
     */
    constexpr uint32_t size() { return arr_size; }
};

/**
 * @brief Accessor for single register
 * @tparam C Register type
 * @param offset Register offset into address space
 */
template <typename C, uint32_t offset>
class RegAccessorArray<C, offset, 0, 0> : public RegAccessorBase<C> {
    static_assert((sizeof(C) % sizeof(uint32_t)) == 0, "Register size must be aligned to 32 bits");
    static_assert((offset % sizeof(uint32_t)) == 0, "Register offset must be aligned to 32 bits");

  public:
    using RegAccessorBase<C>::RegAccessorBase;

    /**
     * @brief Read register
     * @return T Register content
     */
    C rd() const { return RegAccessorBase<C>::_rd(offset); }

    /**
     * @brief Write register
     * @param value Content to write to register
     */
    void wr(const C& value) { RegAccessorBase<C>::_wr(offset, value); }
};

/**
 * @brief Convenience wrapper for single (i.e. non-array) register
 * @tparam C Register content type
 * @tparam offset Register offset into address space
 */
template <typename C, uint32_t offset>
using RegAccessor = RegAccessorArray<C, offset, 0, 0>;

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
