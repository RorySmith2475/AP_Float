#ifndef ARBITRARY_PRECISION_HPP
#define ARBITRARY_PRECISION_HPP

#include "BitArray.hpp"
#include <iostream>
#include <array>
#include <vector>
#include <cstddef>
#include <string>
#include <tuple>
#include <bitset>
#include <cstring>
#include <math.h> 
#include <stdlib.h>
#include <limits>
#include <optional>

namespace AP
{

/**************************************************
 ********************* CONFIG *********************
 **************************************************/
static constexpr size_t CONSTRUCTOR_MAX_ITERATIONS = 20U;
static constexpr size_t DIVISION_ACCURACY = 50U; // Number of decimal digits to include
static constexpr size_t SQRT_ACCURACY_INCREASE_RATIO = 4U; // By what multiple can the objects size increase during the sqrt algorithm
static constexpr double SQRT_ACCURACY = std::numeric_limits<double>::min();

/**
 * Floating point class. Allows for exact precision addition, subtraction, and multiplication.
 * Construction and division are potentially approximate with accuracy set by the constants
 * CONSTRUCTOR_MAX_ITERATIONS and DIVISION_ACCURACY.
 */
class Float
{
public:
    using sign_t = bool;
    static constexpr sign_t POSITIVE = false;
    static constexpr sign_t NEGATIVE = true;

    enum STATE : uint8_t
    {
        NORMAL = 0U,    // Constains a valid floating point numeric value
        INF,            // Infinity
        ERROR           // Parse error or calculation resulting in NAN
    };

    inline Float() = default;

    /**
     * Constructs the object with the unsigned integer value stored in b
     * @param b BitArray value object
     */
    inline Float(const BitArray& b);

    /**
     * Constructs object from a floating point value exactly
     * @param input The floating point value to copy 
     */
    inline Float(float input);

    /**
     * Constructs object from a floating point value exactly
     * @param input The floating point value to copy 
     */
    inline Float(double input);

    /**
     * Constructs the object from a signed template type value
     * 
     * @tparam T Any value that can be cast to a value accepted by a Float constructor
     */
    template <typename T>
    requires(std::is_integral_v<T> && std::is_signed_v<T>)
    inline Float(T input);

    /**
     * Constructs the object from an unsigned template type value
     * 
     * @tparam T Any value that can be cast to a value accepted by a Float constructor
     */
    template <typename T>
    requires(std::is_integral_v<T> && !std::is_signed_v<T>)
    inline Float(T input);

    /**
     * Constructs object from a string representing a floating point value. Will round
     * if the input can not be represented exactly. The accuracy of the rounded value is
     * set by the CONSTRUCTOR_MAX_ITERATIONS constant.
     * 
     * @param input A string view representing a floating point value
     */
    inline Float(std::string_view input);

    /**
     * Spaceship operator comparison.
     * 
     * @param[in] other The other Float object to compare *this to
     * 
     * @return The ordering of *this <=> other, either less, greater, equal, or unordered
     */
    constexpr std::partial_ordering operator<=>(const Float& other) const;

    /**
     * Determines if *this == other
     * 
     * @param[in] other The other Float object to compare *this to
     * 
     * @return true     If *this == other
     * @return false    If *this != other
     */
    constexpr bool operator==(const Float& other) const;

    /**
     * Spaceship operator comparison.
     * 
     * @tparam T Any primitive type that can be cast to a floating point type
     * @param[in] other The floating point value to compare *this to
     * 
     * @return The ordering of *this <=> other, either less, greater, equal, or unordered
     */
    template <typename T>
    requires(!std::is_same_v<T, Float>)
    constexpr std::partial_ordering operator<=>(const T& other) const;

    /**
     * Determines if *this == other
     * 
     * @tparam T Any primitive type that can be cast to a floating point type
     * @param[in] other The other value to compare *this to
     * 
     * @return true     If *this == other
     * @return false    If *this != other
     */
    template <typename T>
    requires(!std::is_same_v<T, Float>)
    constexpr bool operator==(const T& other) const;
    
    /**
     * Modifies the object with the exact sum of *this and other.
     * 
     * @warning Adding a very large value to a very small value will result in a
     *          object size significantly larger than either of the initial objects.
     * 
     * @param[in] other The other Float object to add to *this
     * 
     * @return The object by reference
     */
    template <typename T>
    inline Float& operator+=(const T& other);
    inline Float& operator+=(Float&& other);

    /**
     * Modifies the object with the exact subtractions of other from *this.
     * 
     * @warning Subtracting a very large value to a very small value can result in a
     *          object size significantly larger than either of the initial objects.
     * 
     * @param[in] other The other Float object to subtract from *this
     * 
     * @return The object by reference
     */
    template <typename T>
    inline Float& operator-=(const T& other);
    inline Float& operator-=(Float&& other);

    /**
     * Modifies the object with the exact value of *this * other
     * 
     * @param[in] other The other Float object to multiply *this by
     * 
     * @return The object by reference
     */
    template <typename T>
    requires(!std::is_same_v<T, Float>)
    inline Float& operator*=(const T& other);
    inline Float& operator*=(const Float& other);

    /**
     * Modifies the object with the value of *this / other. If the *this / other does
     * not result in a rational number, DIVISION_ACCURACY additional digits will be included in
     * the result
     * 
     * @param[in] other The other Float object
     * 
     * @return The object by reference 
     */
    template <typename T>
    inline Float& operator/=(const T& other);
    inline Float& operator/=(Float&& other);

    /**
     * Casts *this to the approximate float value, potentially loosing precision
     * @return float value 
     */
    inline operator float() const;

    /**
     * Casts *this to the approximate double value, potentially loosing precision
     * @return double value 
     */
    inline operator double() const;

    inline void clear()
    {
        mMantissa.clear();
        mShift = 0;
        mState = NORMAL;
    }

    /**
     * Returns the internal state of the object
     * @return STATE, either NORMAL, INF, or ERROR
     */
    inline STATE getState() const
    {
        return mState;
    }

    /**
     * Returns the sign of the object
     * @return POSITIVE or NEGATIVE
     */
    inline sign_t getSign() const
    {
        return mSign;
    }

private:
    // Holds the dynamic mantissa of the value, follows the ieee754
    // structure but with the leading digit included.
    BitArray mMantissa; 

    // Keeps track of how many digits in the mantissa are decimals.
    // A negative value indicates the mantissa does not have a fractional
    // portion and |shift| gives how many trailing zeros have been removed
    // to reduce memory usage.
    int32_t mShift = 0;

    sign_t mSign = POSITIVE;
    STATE mState = NORMAL;

    /**
     * Parses out the whole, decimal, and exponent from a scientific notation string encoded value.
     * 
     * @warning None of the returned values are validated, its up to the caller to
     *          check if the results is valid.
     * 
     * @param input A string view representing a floating point value
     * @return The whole number substring, decimal substring, exponent substring, and sign of the number
     */
    static constexpr std::tuple<std::string_view, std::string_view, std::string_view, sign_t>
    parseString(const std::string_view input);

    /**
     * Takes in the decimal and whole parts of a floating point value and shifts the
     * decimal place to remove the exponent.
     * 
     * @example 12.34e4 -> 123400
     * 
     * @pre whole_sv and decimal_sv point to strings representing integer values
     * 
     * @param[in] whole_sv      String view pointing to the whole portion of a floating point value
     * @param[in] decimal_sv    String view pointing to the decimal portion of a floating point value
     * @param[in] exp           Scientific exponent value (10^exp)
     * 
     * @return The newly constructed shifted strings
     */
    static inline std::pair<std::string, std::string>
    shiftStrings(std::string_view, std::string_view, int32_t exp);

    static constexpr int32_t stringToInt(std::string_view s);

    /**
     * Checks if a given string view contains only numbers. A singular
     * + or - at the beginning of the string is allowed.
     * 
     * @param sv string view to be checked
     * @return true if number, else false 
     */
    bool isNumber(std::string_view sv) const;

    /**
     * Converts the internal shift value to the base 2 exponent of the floating point numer
     * 
     * @return int32_t Base 2 exponent
     */
    int32_t shiftToExponent() const;

    friend std::ostream& operator<<(std::ostream&, const Float&);
    friend inline Float abs(Float);
    friend inline std::optional<Float> sqrt(const Float&);
    friend inline std::string to_string(const Float&, size_t);
};

/**
 * Absolute value
 * 
 * @param f Float object to take the absolute value of
 * @return Absolute value 
 */
inline Float abs(Float f);

/**
 * Computes the approximate square root of teh Float object f.
 * The accuracy of the approximation can be adjusted using the
 * SQRT_ACCURACY and SQRT_MAX_ITERATIONS constants.
 * 
 * @param f Float object
 * @return std::optional<Float> Newly constructed Float object equal to sqrt(f) or a null option 
 */
inline std::optional<Float> sqrt(const Float& f);

/**
 * Converts a Float object to its approximate base 10 representation.
 * 
 * @param f Float object to convert to a string
 * @return std::string Base 10 representation
 */
inline std::string to_string(const Float& f, size_t precision = 0U);

/**
 * Returns a newly constructed object with the exact value of lhs + rhs
 * 
 * @tparam T Any type that can be cast to Float, excluding Float objects
 * @param[in] lhs The object to the left of the + operator
 * @param[in] rhs The object to the right of the + operator
 * 
 * @return The newly constructed Float object
 */
template <typename T>
requires(!std::is_same_v<T, Float>)
inline Float operator+(const Float& lhs, const T& rhs);
inline Float operator+(const Float& lhs, const Float& rhs);

/**
 * Returns a newly constructed object with the exact value of lhs - rhs
 * 
 * @warning Subtracting a very large value to a very small value can result in a
 *          object size significantly larger than either of the initial objects.
 * 
 * @tparam T Any type that can be cast to Float, excluding Float objects
 * @param[in] lhs The object to the left of the - operator
 * @param[in] rhs The object to the right of the - operator
 * 
 * @return The newly constructed Float object
 */
template <typename T>
requires(!std::is_same_v<T, Float>)
inline Float operator-(const Float& lhs, const T& rhs);
inline Float operator-(const Float& lhs, const Float& rhs);

/**
 * Returns a newly constructed object with the exact value of lhs * rhs
 * 
 * @tparam T Any type that can be cast to Float, excluding Float objects
 * @param[in] lhs The object to the left of the * operator
 * @param[in] rhs The object to the right of the * operator
 * 
 * @return The newly constructed Float object
 */
template <typename T>
requires(!std::is_same_v<T, Float>)
inline Float operator*(const Float& lhs, const T& rhs);
inline Float operator*(const Float& lhs, const Float& rhs);

/**
 * Creates a new object with the value of lhs / rhs. If the lhs / rhs does
 * not result in a rational number, DIVISION_ACCURACY additional digits will be included in
 * the result
 * 
 * @tparam T Any type that can be cast to Float, excluding Float objects
 * @param[in] lhs The object to the left of the / operator
 * @param[in] rhs The object to the right of the / operator
 * 
 * @return The newly constructed Float object
 */
template <typename T>
requires(!std::is_same_v<T, Float>)
inline Float operator/(const Float& lhs, const T& rhs);
inline Float operator/(const Float& lhs, const Float& rhs);

inline Float::Float(const BitArray& b)
    :   mMantissa(b)
{
    mShift -= mMantissa.rightAlign();
}

inline Float::Float(float input)
{
    if(std::isnan(input))
    {
        mState = ERROR;
    }
    else if(std::isinf(input))
    {
        mState = INF;
        if(input < 0.0)
        {
            mSign = NEGATIVE;
        }
    }
    else
    {
        uint32_t input_bits;
        memcpy(&input_bits, &input, sizeof(input_bits));
        
        if(input != 0.0)
        {
            mMantissa = BitArray(0x800000U | (input_bits & 0x7FFFFFU));
            mShift = log2(mMantissa) - ((input_bits >> 23U) & 0xFF) - 1 + 127;
        }
        mSign = (input_bits & 0x80000000U) ? NEGATIVE : POSITIVE;

        mShift -= mMantissa.rightAlign();
    }
}

inline Float::Float(double input)
{
    if(std::isnan(input))
    {
        mState = ERROR;
    }
    else if(std::isinf(input))
    {
        mState = INF;
        if(input < 0.0)
        {
            mSign = NEGATIVE;
        }
    }
    else
    {
        uint64_t input_bits;
        memcpy(&input_bits, &input, sizeof(input_bits));
        
        if(input != 0.0)
        {
            mMantissa = BitArray(0x10000000000000UL | (input_bits & 0xFFFFFFFFFFFFFUL));
            mShift = log2(mMantissa) - ((input_bits >> 52U) & 0x7FFU) - 1 + 1023;
        }
        mSign = (input_bits & 0x8000000000000000UL) ? NEGATIVE : POSITIVE;

        mShift -= mMantissa.rightAlign();
    }
}

template <typename T>
requires(std::is_integral_v<T> && std::is_signed_v<T>)
inline Float::Float(T input)
    :   mMantissa(std::abs(input))
{
    if(input < 0)
    {
        mSign = NEGATIVE;
    }
    mShift -= mMantissa.rightAlign();
}

template <typename T>
requires(std::is_integral_v<T> && !std::is_signed_v<T>)
inline Float::Float(T input)
    :   mMantissa(input)
{
    mShift -= mMantissa.rightAlign();
}

inline Float::Float(std::string_view input)
{
    auto [whole_str, decimal_str, exp_str, sign] = parseString(input);
    mSign = sign;
    int32_t exp = 0;

    if(!isNumber(exp_str))
    {
        mState = ERROR;
        return;
    }
    exp = stringToInt(exp_str);

    if((!isNumber(whole_str)) || (!isNumber(decimal_str)))
    {
        mState = ERROR;
        return;
    }

    std::string new_whole_str, new_decimal_str;
    if(exp)
    {
        std::tie(new_whole_str, new_decimal_str) = shiftStrings(whole_str, decimal_str, exp);
        whole_str = new_whole_str;
        decimal_str = new_decimal_str;
    }

    if(!whole_str.empty())
    {
        mMantissa = BitArray(whole_str);
    }

    if(!decimal_str.empty())
    {
        BitArray decimals(decimal_str);
        const BitArray one("1" + std::string(decimal_str.size(), '0'));

        while((decimals != 0U) && (mShift < static_cast<int32_t>(CONSTRUCTOR_MAX_ITERATIONS * decimal_str.size())))
        {
            mMantissa <<= 1U;
            decimals <<= 1U;
            if(decimals < one)
            {
                mMantissa.setBit(0U, false);
            }
            else
            {
                mMantissa.setBit(0U, true);
                decimals -= one;
            }
            ++mShift;
        }
    }

    mShift -= mMantissa.rightAlign();
}

constexpr std::partial_ordering Float::operator<=>(const Float& other) const
{
    if((mState == ERROR) || (other.mState == ERROR))
    {
        return std::partial_ordering::unordered;
    }

    if((mSign == Float::NEGATIVE) && (other.mSign == Float::POSITIVE))
    {
        return std::partial_ordering::less;
    }
    else if((mSign == Float::POSITIVE) && (other.mSign == Float::NEGATIVE))
    {
        return std::partial_ordering::greater;
    }

    const int32_t this_exponent = shiftToExponent();
    const int32_t other_exponent = other.shiftToExponent();
    if(mSign == POSITIVE)
    {
        if(this_exponent < other_exponent)
        {
            return std::partial_ordering::less;
        }
        else if(this_exponent > other_exponent)
        {
            return std::partial_ordering::greater;
        }
    }
    else
    {
        if(this_exponent < other_exponent)
        {
            return std::partial_ordering::greater;
        }
        else if(this_exponent > other_exponent)
        {
            return std::partial_ordering::less;
        }
    }

    return (mMantissa <=> other.mMantissa);
}

constexpr bool Float::operator==(const Float& other) const
{
    if((mState == ERROR) || (other.mState == ERROR))
    {
        return false;
    }

    return
        (mState == other.mState) &&
        (mSign == other.mSign) &&
        (mShift == other.mShift) &&
        (mMantissa == other.mMantissa);
}

template <typename T>
requires(!std::is_same_v<T, Float>)
constexpr bool Float::operator==(const T& other) const
{
    return (*this == Float(other));
}

template <typename T>
requires(!std::is_same_v<T, Float>)
constexpr std::partial_ordering Float::operator<=>(const T& other) const
{
    return (*this <=> Float(other));
}

template <typename T>
inline Float& Float::operator+=(const T& other)
{
    return (*this += T(other));
}

inline Float& Float::operator+=(Float&& other)
{
    if((mState == ERROR) || (other.mState == ERROR)) [[unlikely]]
    {
        clear();
        mState = ERROR;
        return *this;
    }
    else if(mState == INF) [[unlikely]]
    {
        if((other.mState == INF) && (mSign != other.mSign))
        {
            clear();
            mState = ERROR;
        }
        return *this;
    }
    else if(other.mState == INF) [[unlikely]]
    {
        *this = other;
        return *this;
    }

    if(mShift > other.mShift)
    {
        other.mMantissa <<= (mShift - other.mShift);
        other.mShift = mShift;
    }
    else if(mShift < other.mShift)
    {
        mMantissa <<= (other.mShift - mShift);
        mShift = other.mShift;
    }

    if(mSign == other.mSign)
    {
        mMantissa += other.mMantissa;
    }
    else
    {
        if((mSign == Float::NEGATIVE) && (other.mSign == Float::POSITIVE))
        {
            if(other.mMantissa > mMantissa)
            {
                mSign = Float::POSITIVE;
            }
        }
        else // mSign == Float::POSITIVE && other.mSign == Float::NEGATIVE
        {
            if(other.mMantissa > mMantissa)
            {
                mSign = Float::NEGATIVE;
            }
        }

        mMantissa -= other.mMantissa;
    }

    mShift -= mMantissa.rightAlign();
    return *this;
}

template <typename T>
inline Float& Float::operator-=(const T& other)
{
    return (*this -= T(other));
}

inline Float& Float::operator-=(Float&& other)
{
    other.mSign = (other.mSign == POSITIVE) ? NEGATIVE : POSITIVE;
    return (*this += std::move(other));
}

template <typename T>
requires(!std::is_same_v<T, Float>)
inline Float& Float::operator*=(const T& other)
{
    return (*this *= Float(other));
}

inline Float& Float::operator*=(const Float& other)
{
    if((mState == ERROR) || (other.mState == ERROR)) [[unlikely]]
    {
        clear();
        mState = ERROR;
        return *this;
    }
    else if(((mState == INF) && ((other == 0.0) || (other == -0.0)))
        || (((*this == 0.0) || (*this == -0.0)) && (other.mState == INF))) [[unlikely]]
    {
        clear();
        mState = ERROR;
        return *this;
    }

    mSign ^= other.mSign;

    if((mState != INF) && (other.mState != INF))
    {
        mShift += other.mShift;
        mMantissa *= other.mMantissa;
        mShift -= mMantissa.rightAlign();
    }
    else [[unlikely]]
    {
        clear();
        mState = INF;
    }
    return *this;
}

template <typename T>
inline Float& Float::operator/=(const T& denominator)
{
    return (*this /= T(denominator));
}

inline Float& Float::operator/=(Float&& denominator)
{
    if((mState == ERROR) || (denominator.mState == ERROR)) [[unlikely]]
    {
        clear();
        mState = ERROR;
        return *this;
    }
    else if((mState == INF) && (denominator.mState == INF)) [[unlikely]]
    {
        clear();
        mState = ERROR;
        return *this;
    }
    else if(((denominator == 0.0) || (denominator == -0.0))
        && ((*this == 0.0) || (*this == -0.0))) [[unlikely]]
    {
        clear();
        mState = ERROR;
        return *this;
    }

    mSign ^= denominator.mSign;

    if((denominator == 0.0) || (denominator == -0.0)) [[unlikely]]
    {
        clear();
        mState = INF;
    }
    else if(denominator.mState == INF) [[unlikely]]
    {
        *this = 0.0;
    }
    else if(mState != INF)
    {
        mShift -= denominator.mShift;
        if(denominator.mMantissa != BitArray(1U))
        {
            mShift += mMantissa.divide(denominator.mMantissa, DIVISION_ACCURACY);
            mShift -= mMantissa.rightAlign();
        }
    }
    return *this;
}

inline Float::operator float() const
{
    if(mState == ERROR) [[unlikely]]
    {
        return std::numeric_limits<float>::signaling_NaN();
    }
    else if(mState == INF) [[unlikely]]
    {
        if(mSign == POSITIVE)
        {
            return std::numeric_limits<float>::infinity();
        }
        else
        {
            return -std::numeric_limits<float>::infinity();
        }
    }

    int32_t exponent = shiftToExponent();
    if(exponent > 127) [[unlikely]]
    {
        return std::numeric_limits<float>::infinity();
    }
    else if(exponent < -128) [[unlikely]]
    {
        return -std::numeric_limits<float>::infinity();
    }

    const uint32_t shift = 33U - std::log2(mMantissa.back());
    uint32_t result = static_cast<uint32_t>(mMantissa.back() << shift);
    if(mMantissa.size() > 1U)
    {
        result |= mMantissa.getBlock(mMantissa.size() - 2U) >> (32U - shift);
    }
    result >>= 9U;


    if(log2(mMantissa) != 0U)
    {
        result |= ((static_cast<int8_t>(exponent) + 127) << 23U);
    }
    result |= (mSign << 31U);

    float f;
    memcpy(&f, &result, sizeof(f));
    return f;
}

inline Float::operator double() const
{
    if(mState == ERROR) [[unlikely]]
    {
        return std::numeric_limits<double>::signaling_NaN();
    }
    else if(mState == INF) [[unlikely]]
    {
        if(mSign == POSITIVE)
        {
            return std::numeric_limits<double>::infinity();
        }
        else
        {
            return -std::numeric_limits<double>::infinity();
        }
    }

    int32_t exponent = shiftToExponent();
    if(exponent > 1023) [[unlikely]]
    {
        return std::numeric_limits<double>::infinity();
    }
    else if(exponent < -1024) [[unlikely]]
    {
        return 0.0;
    }

    const uint64_t shift = 33UL - std::log2(mMantissa.back());
    uint64_t result = static_cast<uint64_t>(mMantissa.back()) << (shift + 32UL);
    if(mMantissa.size() > 2U)
    {
        result |= static_cast<uint64_t>(mMantissa.getBlock(mMantissa.size() - 2UL)) << shift;
        result |= mMantissa.getBlock(mMantissa.size() - 3UL) >> (32UL - shift);
    }
    else if(mMantissa.size() > 1U)
    {
        result |= static_cast<uint64_t>(mMantissa.getBlock(mMantissa.size() - 2UL)) << shift;
    }
    result >>= 12UL;

    if(log2(mMantissa) != 0U)
    {
        result |= ((exponent + 1023L) << 52UL);
    }
    result |= (static_cast<uint64_t>(mSign) << 63UL);

    double f;
    memcpy(&f, &result, sizeof(f));
    return f;
}

constexpr std::tuple<std::string_view, std::string_view, std::string_view, Float::sign_t>
Float::parseString(const std::string_view input)
{
    sign_t sign = POSITIVE;
    if(input.size() > 0)
    {
        sign = (input.at(0U) == '-') ? NEGATIVE : POSITIVE;
    }

    std::string_view whole_str;
    std::string_view decimal_str;
    std::string_view exp_str;

    const size_t dot_pos = input.find(".");
    const size_t e_pos = input.find("e");

    if(dot_pos != std::string::npos)
    {
        whole_str = input.substr(sign, (dot_pos - sign));
        if(e_pos != std::string::npos)
        {
            decimal_str = input.substr(dot_pos + 1U, (e_pos - dot_pos) - 1U);
        }
        else
        {
            decimal_str = input.substr(dot_pos + 1U);
        }
    }
    else if(e_pos != std::string::npos)
    {
        whole_str = input.substr(sign, (e_pos - sign));
    }
    else
    {
        whole_str = input;
    }

    if(!decimal_str.empty())
    {
        const size_t lastDigit = decimal_str.find_last_not_of('0');
        if(lastDigit != std::string::npos)
        {
            decimal_str = decimal_str.substr(0U, lastDigit + 1U);
        }
        else if(decimal_str.at(0U) == '0')
        {
            // decimal_str only contains 0's
            decimal_str = decimal_str.substr(0U, 0U);
        }
    }

    if(e_pos != std::string::npos)
    {
        exp_str = input.substr(e_pos + 1U);
    }

    return {whole_str, decimal_str, exp_str, sign};
}

inline std::pair<std::string, std::string>
Float::shiftStrings(std::string_view whole_sv, std::string_view decimal_sv, int32_t exp)
{
    std::string whole, decimal;
    if(exp > 0)
    {
        const int32_t before_size = decimal_sv.size();
        if(before_size > exp)
        {
            whole = std::string(whole_sv);
            whole += std::string(decimal_sv.substr(0U, exp));
            decimal = std::string(decimal_sv.substr(exp));
        }
        else
        {
            whole = std::string(whole_sv);
            whole += std::string(decimal_sv);
            whole += std::string(exp - before_size, '0');
        }
        exp = 0;
    }
    else
    {
        const int32_t before_size = whole_sv.size();
        if(before_size > std::abs(exp))
        {
            decimal = std::string(whole_sv.substr(std::abs(exp)));
            decimal += std::string(decimal_sv);
            whole = std::string(decimal_sv.substr(0U, std::abs(exp)));
        }
        else
        {
            decimal = std::string(std::abs(exp) - before_size, '0');
            decimal += std::string(whole_sv);
            decimal += std::string(decimal_sv);
        }
        exp = 0;
    }

    return {whole, decimal};
}

constexpr int32_t Float::stringToInt(std::string_view s)
{
    int32_t result = 0;
    if(s.size())
    {
        sign_t sign = POSITIVE;
        if(s.at(0U) == '+')
        {
            s = s.substr(1U);
        }
        else if(s.at(0U) == '-')
        {
            s = s.substr(1U);
            sign = NEGATIVE;
        }

        uint32_t digit_pos = s.size() - 1U;

        for(auto c : s)
        {
            if(c != '0')
            {
                uint32_t tmp = static_cast<uint32_t>(c - '0');
                tmp *= static_cast<uint32_t>(std::pow(10, digit_pos));
                result += tmp;
            }
            --digit_pos;
        }

        if(sign == NEGATIVE)
        {
            result *= -1;
        }
    }

    return result;
}

bool Float::isNumber(std::string_view sv) const
{
    if(sv.size())
    {
        if((sv.at(0U) == '-') || (sv.at(0U) == '+'))
        {
            sv = sv.substr(1U);
        }
        return std::ranges::all_of(sv.begin(), sv.end(), [](char c) { return std::isdigit(c) != 0; });
    }
    return true;
}

int32_t Float::shiftToExponent() const
{
    return log2(mMantissa) - mShift - 1;
}

inline std::ostream& operator<<(std::ostream& os, const Float& f)
{
    os << to_string(f);
    return os;
}

inline Float abs(Float f)
{
    f.mSign = Float::POSITIVE;
    return f;
}

inline std::optional<Float> sqrt(const Float& f)
{
    if(f.getSign() == Float::NEGATIVE)
    {
        return std::nullopt;
    }

    Float curr = f;
    const size_t max_size = f.mMantissa.size() * SQRT_ACCURACY_INCREASE_RATIO;

    while(true)
    {
        const Float next = curr - (((curr * curr) - f) / (curr * 2.0));

        if(abs(abs(next) - abs(curr)) < SQRT_ACCURACY)
        {
            break;
        }
        else if(next.mMantissa.size() > max_size)
        {
            break;
        }

        curr = next;
    }

    return curr;
}

inline std::string to_string(const Float& f, size_t precision)
{
    std::string whole_str, decimal_str;
    int32_t exp = 0;
    const auto bits = log2(f.mMantissa);

    if(f.mShift <= 0)
    {
        whole_str = to_string(f.mMantissa << std::abs(f.mShift));
        exp = whole_str.size() - 1;
    }
    else if(static_cast<uint32_t>(f.mShift) < bits)
    {
        auto [whole, decimal] = f.mMantissa.split(static_cast<uint32_t>(f.mShift));
        const auto decimal_bits = log2(decimal);

        whole_str = to_string(whole);
        exp = whole_str.size() - 1;
        
        // I have no idea why this works
        for(auto i = 0U; i < static_cast<uint32_t>(f.mShift); ++i)
        {
            decimal *= 10U;
        }
        (void)decimal.rightAlign();

        if(static_cast<uint32_t>(f.mShift) > decimal_bits)
        {
            const size_t zeros = std::log2(static_cast<uint32_t>(f.mShift) - decimal_bits);
            decimal_str = std::string(zeros, '0');
        }

        decimal_str += to_string(decimal);
    }
    else
    {
        BitArray decimal = f.mMantissa;
        const auto decimal_bits = log2(decimal);

        for(auto i = 0U; i < static_cast<uint32_t>(f.mShift); ++i)
        {
            decimal *= 10U;
        }
        (void)decimal.rightAlign();

        const size_t zeros = std::log2(static_cast<uint32_t>(f.mShift) - decimal_bits);
        exp = -(zeros + 1U);
        decimal_str = to_string(decimal);
    }
    
    if(f.mSign == Float::NEGATIVE)
    {
        whole_str = "-" + whole_str;
    }

    if(precision)
    {
        decimal_str = decimal_str.substr(0U, precision);
    }

    if(exp)
    {
        decimal_str += "e" + std::to_string(exp);
    }

    if(!whole_str.empty())
    {
        whole_str.insert(1U, ".");
        if(decimal_str.empty())
        {
            decimal_str = "0";
        }
    }
    else
    {
        decimal_str.insert(1U, ".");
    }

    return (whole_str + decimal_str);
}

template <typename T>
requires(!std::is_same_v<T, Float>)
inline Float operator+(const Float& lhs, const T& rhs)
{
    return (lhs + Float(rhs));
}

inline Float operator+(const Float& lhs, const Float& rhs)
{
    Float result(lhs);
    return (result += rhs);
}

template <typename T>
requires(!std::is_same_v<T, Float>)
inline Float operator-(const Float& lhs, const T& rhs)
{
    return (lhs - Float(rhs));
}

inline Float operator-(const Float& lhs, const Float& rhs)
{
    Float result(lhs);
    return (result -= rhs);
}

template <typename T>
requires(!std::is_same_v<T, Float>)
inline Float operator*(const Float& lhs, const T& rhs)
{
    return (lhs * Float(rhs));
}

inline Float operator*(const Float& lhs, const Float& rhs)
{
    Float result(lhs);
    return (result *= rhs);
}

template <typename T>
requires(!std::is_same_v<T, Float>)
inline Float operator/(const Float& lhs, const T& rhs)
{
    return (lhs / Float(rhs));
}

inline Float operator/(const Float& lhs, const Float& rhs)
{
    Float result(lhs);
    return (result /= rhs);
}


}


#endif
