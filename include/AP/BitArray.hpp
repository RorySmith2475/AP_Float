#ifndef BIT_ARRAY_HPP
#define BIT_ARRAY_HPP

#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cassert>
#include <string>
#include <math.h> 
#include <bitset>
#include <limits>
#include <iterator>

namespace AP
{
    
/**
 * Dynamic size integer class.
 */
class BitArray
{
public:
    using BLOCK = uint32_t; // Internal storage type.

    inline BitArray() = default;

    /**
     * Initialize the object with the value of v
     * @param v Object value
     */
    inline BitArray(uint64_t v);

    /**
     * Construct the object with the integer value encoded inside sv
     * 
     * @pre sv can only contain numeric characters (ie, 0 -> 9)
     * 
     * @param[in] sv String view pointing to the integer string
     */
    inline BitArray(const std::string_view sv);

    /**
     * Construct the object with the value of other
     * @param[in] other The other BitArray object to copy the value of
     */
    inline BitArray(const BitArray& other);

    /**
     * Plus equal operator, modifies the current object by adding the value
     * stored in other.
     * 
     * @param[in] other BitArray object to add the to *this
     * @return BitArray& Reference to *this
     */
    inline BitArray& operator+=(const BitArray& other);
    inline BitArray& operator+=(uint32_t other);

    /**
     * Minus equals operator, modifies the current object by subtracting the value
     * stored in other. If other is greater than *this, the result will be the absolute
     * value of *this - other
     * 
     * @param[in] other BitArray object to subtract from *this
     * @return BitArray& Reference to *this
     */
    inline BitArray& operator-=(const BitArray& other);
    inline BitArray& operator-=(uint32_t other);

    /**
     * Multiply equals operator, modifies the current object by multiplying it by the value
     * stored in other.
     * 
     * @param[in] other BitArray object to multiply *this by
     * @return BitArray& Reference to *this
     */
    inline BitArray& operator*=(const BitArray& other);
    inline BitArray& operator*=(uint32_t other);

    /**
     * Modify the object with the approximate result of this / denominator. Rather
     * than discarding all of the remainder, the object is bit shifted by as much 
     * as accuracy bits and the division is repeated until all of them have been set.
     * This results in essentially a fractional result. To discard these added bits
     * simple shift the object to the right by the return value. 
     * 
     * @param[in] denominator The value to divide *this by 
     * @return size_t The number of bits the object has been shifted by
     */
    inline size_t divide(const BitArray& denominator, uint32_t accuracy);
    inline size_t divide(uint32_t denominator, uint32_t accuracy);

    /**
     * Bit shift operator. Modify the current object by bit shifting
     * it to the right by other bits
     * 
     * @param[in] other The number of bits to shift by
     * @return BitArray& Reference to *this
     */
    inline BitArray& operator>>=(uint32_t other);

    /**
     * Bit shift operator. Modify the current object by bit shifting
     * it to the left by other bits
     * 
     * @param[in] other The number of bits to shift by
     * @return BitArray& Reference to *this
     */
    inline BitArray& operator<<=(uint32_t other);

    /**
     * Spaceship operator comparison.
     * 
     * @param[in] other The BitArray object to compare *this to
     * 
     * @return The ordering of *this <=> other, either less, greater, or equal
     */
    inline std::strong_ordering operator<=>(const BitArray& other) const;

    /**
     * Spaceship operator comparison.
     * 
     * @param[in] other The value to compare *this to
     * 
     * @return The ordering of *this <=> other, either less, greater, or equal
     */
    inline std::strong_ordering operator<=>(uint32_t other) const;

    /**
     * Determines if *this == other
     * 
     * @param[in] other The other BitArray object to compare *this to
     * 
     * @return true     If *this == other
     * @return false    If *this != other
     */
    inline bool operator==(const BitArray& other) const;

    /**
     * Determines if *this == other
     * 
     * @param[in] other The other Float object to compare *this to
     * 
     * @return true     If *this == other
     * @return false    If *this != other
     */
    inline bool operator==(uint32_t other) const;

    /**
     * Invert n bits. If n is larger than the current number of bits being stored than
     * the objects size in increased by in increments of sizeof(BLOCK) until n bits
     * can be stored.
     * 
     * @param[in] n The number of bits to be inverted.
     */
    inline void invert(uint32_t n);

    /**
     * Set the bit at index to either 1 or 0
     * 
     * @param[in] index The position of the bit
     * @param value Either 1 or 0
     */
    inline void setBit(size_t index, uint8_t value);

    /**
     * Get the value of the bit at index
     * 
     * @param[in] index The position of the bit
     * @return Either 1 or 0 
     */
    inline uint8_t getBit(size_t index) const;

    /**
     * Bit shift the object left until the largest set bit is at the end of a BLOCK.
     * 
     * @return size_t The number of bits the object was shifted by
     */
    inline size_t leftAlign();

    /**
     * Bit shift the object right until the smallest bit is in position 0 of the first BLOCK.
     * 
     * @return size_t The number of bits the object was shifted by
     */
    inline size_t rightAlign();

    /**
     * Return the BLOCK at index. Will throw if index >= size()
     * 
     * @param index Position of the block
     * @return BLOCK uint32_t value
     */
    inline BLOCK getBlock(size_t index) const;

    /**
     * Retrieves the value of the last BLOCK stored in the object
     * @return BLOCK The last value stored
     */
    inline BLOCK back() const
    {
        return mBits.back();
    }

    /**
     * Number of elements
     * @return size_t
     */
    inline size_t size() const
    {
        return mBits.size();
    }

    /**
     * Remove all data from the object, leaving it with the value of 0
     */
    inline void clear()
    {
        mBits.clear();
        mBits.push_back(0U);
    }

private:
    // Internal storage representation of an array of bits
    std::vector<BLOCK> mBits{0U};

    /**
     * Remove sequential trailing BLOCKS == 0U
     */
    inline void reduce();

    friend std::ostream& operator<<(std::ostream&, const BitArray&);
};

/**
 * Computes the rounded up log base 10 of b
 * 
 * @param b The BitArray to take the log10 of
 * @return uint64_t Rounded result
 */
inline uint64_t log10(const BitArray& b);

/**
 * Takes the rounded up log base 2 of b. Ie, returns the position
 * of the largest set bit in b
 * 
 * @param b The BitArray to take the log2 of
 * @return uint64_t Rounded result
 */
inline uint64_t log2(const BitArray& b);

inline BitArray operator+(const BitArray& lhs, const BitArray& rhs);
inline BitArray operator-(const BitArray& lhs, const BitArray& rhs);
inline BitArray operator*(const BitArray& lhs, const BitArray& rhs);
inline BitArray operator>>(const BitArray& lhs, uint32_t shift);
inline BitArray operator<<(const BitArray& lhs, uint32_t shift);

inline BitArray::BitArray(uint64_t v)
{
    mBits.at(0U) = (v & 0xFFFFFFFFU);
    v >>= 32UL;
    if(v)
    {
        mBits.push_back(v & 0xFFFFFFFFU);
    }
}

inline BitArray::BitArray(const BitArray& other)
    :   mBits(other.mBits)
{
    reduce();
}

inline BitArray::BitArray(const std::string_view s)
{
    uint32_t digit_pos = s.size() - 1U;

    const uint32_t split = (digit_pos > 8U) ? (digit_pos - 8U) : 0U;
    const std::string_view large_digits = s.substr(0U, split);
    const std::string_view small_digits = s.substr(split);

    if(!large_digits.empty())
    {
        BitArray tmp;
        for(auto c : large_digits)
        {
            if(c != '0')
            {
                tmp.mBits.at(0U) = (c - '0');

                uint32_t i;
                for(i = digit_pos; i >= 9U; i -= 9U)
                {
                    tmp *= 1000000000U;
                }
                tmp *= static_cast<uint32_t>(std::pow(10, i));

                *this += tmp;
                std::fill(tmp.mBits.begin(), tmp.mBits.end(), 0U);

            }
            --digit_pos;
        }
    }

    for(const char& c : small_digits)
    {
        if(c != '0')
        {
            *this += static_cast<uint32_t>(c - '0') * static_cast<uint32_t>(std::pow(10, digit_pos));
        }
        --digit_pos;
    }
}

inline BitArray& BitArray::operator+=(const BitArray& other)
{
    if(*this == other)
    {
        return (*this <<= 1U);
    }

    uint64_t overflow = 0U;
    uint32_t i;
    for(i = 0U; i < std::min(mBits.size(), other.mBits.size()); ++i)
    {
        overflow = static_cast<uint64_t>(mBits.at(i)) + static_cast<uint64_t>(other.mBits.at(i)) + (overflow >> 32U);
        mBits.at(i) = static_cast<BLOCK>(overflow & 0xFFFFFFFF);
    }
    for(; i < other.mBits.size(); ++i)
    {
        overflow = static_cast<uint64_t>(other.mBits.at(i)) + (overflow >> 32U);
        mBits.push_back(static_cast<BLOCK>(overflow & 0xFFFFFFFF));
    }

    overflow >>= 32U;
    for(; overflow != 0U; ++i)
    {
        if(i < mBits.size())
        {
            mBits.at(i) += static_cast<BLOCK>(overflow & 0xFFFFFFFF);
        }
        else
        {
            mBits.push_back(static_cast<BLOCK>(overflow & 0xFFFFFFFF));
        }
        overflow >>= 32U;
    }

    return *this;
}

inline BitArray& BitArray::operator+=(uint32_t other)
{
    uint64_t overflow = static_cast<uint64_t>(mBits.at(0U)) + static_cast<uint64_t>(other);
    mBits.at(0U) = static_cast<BLOCK>(overflow & 0xFFFFFFFF);

    overflow >>= 32U;
    for(std::size_t i = 1U; overflow != 0U; ++i)
    {
        if(i < mBits.size())
        {
            mBits.at(i) += static_cast<BLOCK>(overflow & 0xFFFFFFFF);
        }
        else
        {
            mBits.push_back(static_cast<BLOCK>(overflow & 0xFFFFFFFF));
        }
        overflow >>= 32U;
    }

    return *this;
}

inline BitArray& BitArray::operator-=(const BitArray& other)
{
    if(*this == other)
    {
        std::fill(mBits.begin(), mBits.end(), 0U);
    }
    else if(*this == 0U)
    {
        *this = other;
    }
    else if(other != 0U)
    {
        BitArray tmp;
        if(other > *this)
        {
            tmp = *this;
            *this = other;
        }
        else
        {
            tmp = other;
        }

        std::size_t bits = log2(*this);

        tmp.invert(bits);

        *this += tmp;
        *this += 1U;

        std::size_t new_bits = log2(*this);
        if(new_bits > bits)
        {
            setBit(new_bits - 1U, 0U);
        }
    }

    reduce();
    return *this;
}

inline BitArray& BitArray::operator-=(uint32_t other)
{
    return (*this -= BitArray(other));
}

inline BitArray& BitArray::operator*=(const BitArray& other)
{
    BitArray tmp = *this;
    std::fill(mBits.begin(), mBits.end(), 0U);

    uint32_t count = 0U;
    for(uint32_t i = 0U; i < static_cast<uint32_t>(log2(other)); ++i)
    {
        BLOCK block = other.getBlock(i / 32);
        if(block & (1U << (i % 32U)))
        {
            tmp <<= count;
            count = 1U;
            *this += tmp;
        }
        else
        {
            ++count;
        }
    }

    reduce();
    return *this;
}

inline BitArray& BitArray::operator*=(uint32_t v)
{
    BitArray tmp = *this;
    std::fill(mBits.begin(), mBits.end(), 0U);

    uint32_t count = 0U;
    for(uint32_t i = 0U; i <= static_cast<uint32_t>(std::log2(v)); ++i)
    {
        if((v >> i) & 0x01)
        {
            tmp <<= count;
            count = 1U;
            *this += tmp;
        }
        else
        {
            ++count;
        }
    }

    reduce();
    return *this;
}

[[nodiscard]]
inline size_t BitArray::divide(const BitArray& other, uint32_t accuracy)
{
    size_t shift = 0U;
    BitArray result;
    BitArray denominator = other;
    // std::fill(mBits.begin(), mBits.end(), 0U);
    // reduce();

    *this <<= accuracy;

    while((*this != 0U) && (shift <= accuracy))
    {
        while (*this >= denominator)
        {
            uint32_t i = 0;

            denominator <<= 1U;
            if(*this >= denominator)
            {
                i = log2(*this) - log2(denominator);
                denominator <<= i;

                if(*this >= denominator)
                {
                    denominator <<= 1U;
                    ++i;
                }
            }
            denominator >>= 1U;

            // set the corresponding bit in the result
            result.setBit(i, 1U);

            // subtract the multiple of the divisor to be left with the remainder
            *this -= denominator;
            denominator >>= i;
        }

        ++shift;
        *this <<= 1U;
        // result <<= 1U;
    }

    *this = result;
    return accuracy;
}

[[nodiscard]]
inline size_t BitArray::divide(uint32_t denominator, uint32_t accuracy)
{
    return divide(BitArray(denominator), accuracy);
}

inline BitArray& BitArray::operator>>=(uint32_t n)
{
    // shift by entire BLOCK
    const size_t blocks = static_cast<uint32_t>(n / 32.0F);
    if (blocks > 0U)
    {
        if (blocks >= mBits.size())
        {
            // std::fill(mBits.begin(), mBits.end(), 0U);
            // mBits.erase(mBits.begin(), mBits.begin() + (mBits.size() - 1U));
            // std::fill(mBits.begin(), mBits.end(), 0U);
            std::fill(mBits.begin(), mBits.end(), 0U);
        }
        else
        {
            mBits.erase(mBits.begin(), mBits.begin() + blocks);
        }
    }

    // shift by the rest
    const uint8_t remaining_shift = (n % 32U);
    if (remaining_shift)
    {
        uint32_t i;
        for (i = 0U; i < mBits.size() - 1U; ++i)
        {
            mBits.at(i) >>= remaining_shift;
            mBits.at(i) |= ((mBits.at(i + 1U) & ((1U << remaining_shift) - 1U)) << (32U - remaining_shift));
        }
        mBits.at(i) >>= remaining_shift;
    }

    reduce();
    return *this;
}

inline BitArray& BitArray::operator<<=(uint32_t n)
{
    // shift by entire BLOCK
    const uint32_t whole_blocks = static_cast<uint32_t>(n / 32.0F);
    if(whole_blocks)
    {
        mBits.insert(mBits.begin(), whole_blocks, 0U);
    }

    // shift by the rest
    const uint8_t remaining_shift = (n % 32U);
    uint64_t overflow = 0U;
    uint32_t i;
    for(i = 0U; i < mBits.size(); ++i)
    {
        overflow = (static_cast<uint64_t>(mBits.at(i)) << remaining_shift) | (overflow >> 32U);
        mBits.at(i) = static_cast<BLOCK>(overflow & 0xFFFFFFFF);
    }

    overflow >>= 32U;
    if(overflow)
    {
        if(mBits.size() > i)
        {
            // TODO look into this overwriting BLOCK
            mBits.at(i) = static_cast<BLOCK>(overflow & 0xFFFFFFFF);
        }
        else
        {
            mBits.push_back(static_cast<BLOCK>(overflow & 0xFFFFFFFF));
        }
    }

    reduce();
    return *this;
}

inline std::strong_ordering BitArray::operator<=>(const BitArray& rhs) const
{
    if (mBits.size() < rhs.mBits.size())
    {
        return std::strong_ordering::less;
    }
    else if (mBits.size() > rhs.mBits.size())
    {
        return std::strong_ordering::greater;
    }
    else // Same number of BLOCKs
    {
        for (int32_t i = mBits.size() - 1; i >= 0; --i)
        {
            if (mBits.at(i) < rhs.mBits.at(i))
            {
                return std::strong_ordering::less;
            }
            else if (mBits.at(i) > rhs.mBits.at(i))
            {
                return std::strong_ordering::greater;
            }
        }
    }

    return std::strong_ordering::equal;
}

inline std::strong_ordering BitArray::operator<=>(uint32_t rhs) const
{
    if (mBits.size() > 1U)
    {
        return std::strong_ordering::greater;
    }
    else if (mBits.size() == 0U) [[unlikely]]
    {
        // Handles edge case where mBits in uninitialized
        return std::strong_ordering::less;
    }
    else // Same number of BLOCKs
    {
        return mBits.at(0U) <=> rhs;
    }
}

inline bool BitArray::operator==(const BitArray& rhs) const
{
    return (*this <=> rhs) == std::strong_ordering::equal;
}

inline bool BitArray::operator==(uint32_t rhs) const
{
    return (*this <=> rhs) == std::strong_ordering::equal;
}

inline void BitArray::invert(uint32_t digits)
{
    const uint32_t blocks = static_cast<uint32_t>(digits / 32.0F);

    uint32_t i;
    for (i = 0U; (i < mBits.size()) && (i < blocks); ++i)
    {
        mBits.at(i) = ~mBits.at(i);
    }

    for (; i < blocks; ++i)
    {
        mBits.push_back(0xFFFFFFFF);
    }
    
    // Only invert bits in the number, not in the entire BLOCK
    const uint32_t remaining_digits = (digits % 32U);
    if (remaining_digits)
    {
        if (mBits.size() > i)
        {
            mBits.at(i) = ((~(mBits.at(i) << (32U - remaining_digits))) & 0xFFFFFFFF) >> (32U - remaining_digits);
        }
        else
        {
            mBits.push_back(0xFFFFFFFF >> (32U - remaining_digits));
        }
    }
}

inline void BitArray::setBit(size_t index, uint8_t value)
{
    const size_t blocks = static_cast<size_t>(index / 32.0);
    index -= (blocks * 32U);

    if (blocks >= mBits.size())
    {
        mBits.insert(mBits.begin(), (blocks - mBits.size() + 1U), 0U);
    }

    if (value) mBits.at(blocks) |= 1U << index;
    else mBits.at(blocks) &= ~(1U << index);
}

inline uint8_t BitArray::getBit(size_t index) const
{
    const size_t blocks = static_cast<size_t>(index / 32.0);
    index -= (blocks * 32U);

    return (getBlock(blocks) & (1 << index));
}

[[nodiscard]]
inline size_t BitArray::leftAlign()
{
    if(*this != 0U)
    {
        const size_t lShift = 32U - (log2(*this) - ((mBits.size() - 1U) * 32U));
        *this <<= lShift;
        return lShift;
    }

    return 0U;
}

[[nodiscard]]
inline size_t BitArray::rightAlign()
{
    if (*this == 0U)
    {
        return 0U;
    }

    uint32_t empty_blocks = 0U;    
    for (uint32_t i = 0U; (i < mBits.size()) && (mBits.at(i) == 0U); ++i)
    {
        ++empty_blocks;
    }
    *this >>= (empty_blocks * 32U);

    // https://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightMultLookup
    constexpr uint8_t const multiplyDeBruijnBitPosition[32] = 
    {
        0U, 1U, 28U, 2U, 29U, 14U, 24U, 3U, 30U, 22U, 20U, 15U, 25U, 17U, 4U, 8U, 
        31U, 27U, 13U, 23U, 21U, 19U, 16U, 7U, 26U, 12U, 18U, 6U, 11U, 5U, 10U, 9U
    };
    const uint8_t shift = multiplyDeBruijnBitPosition[((uint32_t)((mBits.at(0U) & -mBits.at(0U)) * 0x077CB531U)) >> 27];
    
    *this >>= shift;

    reduce();

    return shift + (empty_blocks * 32U);
}

[[nodiscard]]
inline BitArray::BLOCK BitArray::getBlock(size_t index) const
{
    return mBits.at(index);
}

inline void BitArray::reduce()
{
    std::size_t empty_blocks = 0U;
    for(int32_t i = mBits.size() - 1; i > 0; --i)
    {
        if(mBits.at(i) == 0U)
        {
            ++empty_blocks;
            // mBits.pop_back();
        }
        else
        {
            break;
        }
    }

    if(empty_blocks)
    {
        mBits.erase(mBits.begin() + mBits.size() - empty_blocks, mBits.begin() + mBits.size());
    }
}

inline std::ostream& operator<<(std::ostream& os, const BitArray& b)
{
    for(int32_t i = b.mBits.size() - 1; i >= 0; --i)
    {
        os << std::bitset<32U>(b.mBits.at(i)) << " ";
    }
    return os;
}

inline uint64_t log10(const BitArray& b)
{
    // Taken from https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10
    constexpr uint32_t const powersOf10[] = 
    {
        1U, 10U, 100U, 1000U, 10000U, 100000U,
        1000000U, 10000000U, 100000000U, 1000000000U
    };

    const uint64_t tmp = (log2(b) + 1U) * 1233U >> 12U;
    return tmp - static_cast<uint64_t>(b < powersOf10[tmp]);
}

inline uint64_t log2(const BitArray& b)
{
    uint64_t result = (b.size() - 1U) * 32U;
    uint32_t tmp = b.getBlock(b.size() - 1U);
    while(tmp != 0U)
    {
        ++result;
        tmp >>= 1U;
    }

    return result;
}

inline BitArray operator+(const BitArray& lhs, const BitArray& rhs)
{
    BitArray tmp = lhs;
    return (tmp += rhs);
}

inline BitArray operator-(const BitArray& lhs, const BitArray& rhs)
{
    BitArray tmp = lhs;
    return (tmp -= rhs);
}

inline BitArray operator*(const BitArray& lhs, const BitArray& rhs)
{
    BitArray tmp = lhs;
    return (tmp *= rhs);
}

inline BitArray operator<<(const BitArray& lhs, uint32_t n)
{
    BitArray tmp = lhs;
    return (tmp <<= n);
}

inline BitArray operator>>(const BitArray& lhs, uint32_t n)
{
    BitArray tmp = lhs;
    return (tmp >>= n);
}

}

#endif
