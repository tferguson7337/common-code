#ifndef _STRING_UTIL_HPP_
#define _STRING_UTIL_HPP_

#include "Macros.h"

#include <memory>
#include <vector>

class StringUtil
{
    /// Static Class - No Ctors/Dtors
    StringUtil( ) = delete;
    StringUtil(const StringUtil&) = delete;
    StringUtil(StringUtil&&) = delete;
    ~StringUtil( ) = delete;

    StringUtil& operator=(const StringUtil&) = delete;
    StringUtil& operator=(StringUtil&&) = delete;

private:

    /// Private Helper Methods \\\

    template <class T>
    static void ValidateRawPointerArg(const T* p, const std::basic_string<utf8>& f)
    {
        if ( !p )
        {
            throw std::invalid_argument(f + ": Attempted string copy from nullptr.");
        }
    }

    static void ValidateLengthArg(const size_t len, const std::basic_string<utf8>& f)
    {
        if ( len == 0 )
        {
            throw std::invalid_argument(f + ": Attempted string copy using zero-length buffer.");
        }
    }

    template <class T>
    static bool IsTerminatorMatch(const T* str, const std::basic_string<T>& terminator)
    {
        size_t i = 0;

        if ( terminator.empty( ) )
        {
            return *str == static_cast<T>('\0');
        }

        for ( const auto& c : terminator )
        {
            if ( str[i++] != c )
            {
                return false;
            }
        }

        return true;
    }

public:

    template <class T>
    static size_t GetStringLength(const T* src, const std::basic_string<T>& terminator = std::basic_string<T>( ))
    {
        ValidateRawPointerArg(src, __FUNCTION__);

        // Get character count of source buffer.
        size_t len = 0;
        while ( !IsTerminatorMatch(src, terminator) )
        {
            len++;
            src++;
        }

        // Return character count + null.
        return len + 1;
    }

    // Specialization Utility Classes.
    class Copy;
    class UTFConversion;
    class NumberConversion;
};


///
//
//  Class   - StringUtil::Copy
//
//  Purpose - Copy source string to a new string.
//
///
class StringUtil::Copy
{
    /// Static Class - No Ctors/Dtors
    Copy( ) = delete;
    Copy(const Copy&) = delete;
    Copy(Copy&&) = delete;
    ~Copy( ) = delete;

    Copy& operator=(const Copy&) = delete;
    Copy& operator=(Copy&&) = delete;

public:

    template <class T>
    static std::basic_string<T> ToString(const std::basic_string<T>& src)
    {
        return ToString(src.c_str( ), src.length( ));
    }

    template <class T>
    static std::basic_string<T> ToString(const T* src)
    {
        return ToString(src, GetStringLength(src));
    }

    template <class T>
    static std::basic_string<T> ToString(const T* src, const size_t& len)
    {
        ValidateRawPointerArg(src, __FUNCTION__);
        ValidateLengthArg(len, __FUNCTION__);

        return std::basic_string<T>(src, len);
    }

    template <class T>
    static std::unique_ptr<T[ ]> ToCString(const std::basic_string<T>& src)
    {
        return ToCString(src.c_str( ), src.length( ));
    }

    template <class T>
    static std::unique_ptr<T[ ]> ToCString(const T* src)
    {
        return ToCString(src, GetStringLength<T>(src));
    }

    template <class T>
    static std::unique_ptr<T[ ]> ToCString(const T* src, const size_t& len)
    {
        std::unique_ptr<T[ ]> copy;

        ValidateRawPointerArg(src, __FUNCTION__);
        ValidateLengthArg(len, __FUNCTION__);

        copy = std::make_unique<T[ ]>(len + 1);
        memcpy(copy.get( ), src, sizeof(T)*(len + 1));

        return copy;
    }
};


///
//
//  Class   - StringUtil::UTFConversion
//
//  Purpose - Copy source string to a new string with differing character types.
//
///
class StringUtil::UTFConversion
{
    /// Static Class - No Ctors/Dtors
    UTFConversion( ) = delete;
    UTFConversion(const UTFConversion&) = delete;
    UTFConversion(UTFConversion&&) = delete;
    ~UTFConversion( ) = delete;

    UTFConversion& operator=(const UTFConversion&) = delete;
    UTFConversion& operator=(UTFConversion&&) = delete;

private:

    template <class T_Dst>
    static std::basic_string<T_Dst> BuildPreConversionString(const size_t& len)
    {
        return std::basic_string<T_Dst>(len, static_cast<T_Dst>('!'));
    }

    template <class T_Dst>
    static std::unique_ptr<T_Dst[ ]> BuildPreConversionCString(const size_t& len)
    {
        std::unique_ptr<T_Dst[ ]> ptr(std::make_unique<T_Dst[ ]>(len + 1));
        memset(ptr.get( ), static_cast<T_Dst>('\0'), sizeof(T_Dst)*(len + 1));
        return ptr;
    }

    template <class T_Dst, class T_Src>
    static void ToStringCommon(T_Dst* dst, const T_Src* src, const size_t& len)
    {
        for ( size_t i = 0; i < len; i++ )
        {
            dst[i] = static_cast<T_Dst>(src[i]);
        }
    }

public:

    template <class T_Dst, class T_Src>
    static std::basic_string<T_Dst> ToString(const std::basic_string<T_Src>& src)
    {
        return ToString<T_Dst, T_Src>(src.c_str( ), src.length( ));
    }

    template <class T_Dst, class T_Src>
    static std::basic_string<T_Dst> ToString(const T_Src* src)
    {
        return ToString<T_Dst, T_Src>(src, GetStringLength(src));
    }

    template <class T_Dst, class T_Src>
    static std::basic_string<T_Dst> ToString(const T_Src* src, const size_t& len)
    {
        ValidateRawPointerArg(src, __FUNCTION__);
        ValidateLengthArg(len, __FUNCTION__);

        if constexpr ( std::is_same<T_Dst, T_Src>::value )
        {
            return StringUtil::Copy::ToString<T_Dst>(src, len);
        }
        else
        {
            std::basic_string<T_Dst> str(BuildPreConversionString<T_Dst>(len));
            ToStringCommon<T_Dst, T_Src>(const_cast<T_Dst*>(str.data( )), src, len);
            return str;
        }
    }

    template <class T_Dst, class T_Src>
    static std::unique_ptr<T_Dst[ ]> ToCString(const std::basic_string<T_Src>& src)
    {
        return ToCString<T_Dst, T_Src>(src.c_str( ), src.length( ));
    }

    template <class T_Dst, class T_Src>
    static std::unique_ptr<T_Dst[ ]> ToCString(const T_Src* src)
    {
        return ToCString<T_Dst, T_Src>(src, GetStringLength(src));
    }

    template <class T_Dst, class T_Src>
    static std::unique_ptr<T_Dst[ ]> ToCString(const T_Src* src, const size_t& len)
    {
        ValidateRawPointerArg(src, __FUNCTION__);
        ValidateLengthArg(len, __FUNCTION__);

        if constexpr ( std::is_same<T_Dst, T_Src>::value )
        {
            return StringUtil::Copy::ToCString<T_Dst>(src, len);
        }
        else
        {
            std::unique_ptr<T_Dst[ ]> cstr(BuildPreConversionCString<T_Dst>(len));
            ToStringCommon<T_Dst, T_Src>(cstr.get( ), src, len);
            return cstr;
        }      
    }
};


///
//
//  Class   - StringUtil::NumberConversion
//
//  Purpose - Convert numeric values to string representations.
//
///
class StringUtil::NumberConversion
{
    /// Static Class - No Ctors/Dtors
    NumberConversion( ) = delete;
    NumberConversion(const NumberConversion&) = delete;
    NumberConversion(NumberConversion&&) = delete;
    ~NumberConversion( ) = delete;

    NumberConversion& operator=(const NumberConversion&) = delete;
    NumberConversion& operator=(NumberConversion&&) = delete;

public:
    // Enum Class for Specifying Number-To-String Conversion types.
    enum class Base : size_t
    {
        Binary = 2,
        Octal = 8,
        Decimal = 10,
        Hexidecimal = 16,
    };

    using BaseType = std::underlying_type<Base>::type;

private:

    template <class T>
    static const std::basic_string<T>& GetPrefixString(const Base& b)
    {
        static const std::vector<SupportedStringTuple> prefixes
        {
            MAKE_STR_TUPLE("0b"),  // binary
            MAKE_STR_TUPLE("0"),   // octal
            MAKE_STR_TUPLE(""),    // decimal
            MAKE_STR_TUPLE("0x"),  // hexadecimal
        };

        switch ( b )
        {
        case Base::Binary:
            return std::get<std::basic_string<T>>(prefixes[0]);

        case Base::Octal:
            return std::get<std::basic_string<T>>(prefixes[1]);

        case Base::Decimal:
            return std::get<std::basic_string<T>>(prefixes[2]);

        case Base::Hexidecimal:
            return std::get<std::basic_string<T>>(prefixes[3]);

        default:
            throw std::invalid_argument(
                __FUNCTION__" - Invalid Base[" +
                std::to_string(static_cast<BaseType>(b)) +
                "]."
            );
        }
    }

    template <class N>
    static size_t GetRequiredLength(const Base& b, const N& n)
    {
        N num = n;
        if constexpr ( std::is_signed<N>::value )
        {
            num = (n < 0) ? -n : n;
        }

        const double dNum = static_cast<double>(num);
        const double dBase = static_cast<double>(b);

        return static_cast<size_t>(std::ceil(std::log(dNum) / std::log(dBase)));
    }

    template <class T, class N>
    static T NumberToCharacter(const N& n)
    {
        // [0-9A-F]
        static const std::vector<SupportedCharacterTuple> vChars
        {
            MAKE_CHAR_TUPLE('0'), MAKE_CHAR_TUPLE('1'), MAKE_CHAR_TUPLE('2'), MAKE_CHAR_TUPLE('3'),
            MAKE_CHAR_TUPLE('4'), MAKE_CHAR_TUPLE('5'), MAKE_CHAR_TUPLE('6'), MAKE_CHAR_TUPLE('7'),
            MAKE_CHAR_TUPLE('8'), MAKE_CHAR_TUPLE('9'), MAKE_CHAR_TUPLE('A'), MAKE_CHAR_TUPLE('B'),
            MAKE_CHAR_TUPLE('C'), MAKE_CHAR_TUPLE('D'), MAKE_CHAR_TUPLE('E'), MAKE_CHAR_TUPLE('F')
        };

        const size_t idx = static_cast<size_t>(n);
        if ( idx >= vChars.size( ) )
        {
            throw std::out_of_range(std::string(__FUNCTION__ ": Given value[") + std::to_string(n) + "] is out of range[0-15].");
        }

        return std::get<T>(vChars.at(idx));
    }

    template <class T, class N>
    static std::basic_string<T> BuildPreConversionString(const Base& b, const N& n)
    {
        return std::basic_string<T>(GetRequiredLength(b, n) + GetPrefixString<T>(b).size( ), static_cast<T>('!'));
    }

    template <class T>
    static std::unique_ptr<T[ ]> BuildPreConversionCString(const size_t& l)
    {
        std::unique_ptr<T[ ]> ptr(std::make_unique<T[ ]>(l + 1));
        memset(ptr.get( ), static_cast<T>('\0'), sizeof(T)*(l + 1));
        return ptr;
    }

    template <class T, class N>
    static void ToStringCommon(const Base& b, N n, T* p, const size_t l)
    {
        const std::basic_string<T>& prefix = GetPrefixString<T>(b);
        const N base = static_cast<N>(b);
        size_t idx = 0;

        // Sanity check - ensure provided buffer is large enough to contain prefix and at-least one character.
        if ( prefix.size( ) >= l )
        {
            throw std::logic_error(__FUNCTION__ ": Buffer length too small to contain prefix and number conversion.");
        }

        // Add Base-depedent prefix string.
        for ( const auto& c : prefix )
        {
            p[idx++] = c;
        }

        // Perform conversion of number to string.
        idx = l - 1;
        do
        {
            p[idx--] = NumberToCharacter<T>(n % base);
            n /= base;
        } while ( n );
    }

public:

#define _ENABLE_IF_NUMBER_CONVERT_SUPPORTED(TC, TN) \
    typename = typename std::enable_if<IsSupportedCharType<TC>( ) && IsIntegerRepresentableType<TN>( )>::type

    // Return string representation of number argument
    template <class T, class N, _ENABLE_IF_NUMBER_CONVERT_SUPPORTED(T, N)>
    static std::basic_string<T> ToString(const Base& base, const N& number)
    {
        std::basic_string<T> str(BuildPreConversionString<T>(base, number));
        ToStringCommon(base, number, const_cast<T*>(str.data( )), str.size( ));
        return str;
    }

    template <class T, class N, _ENABLE_IF_NUMBER_CONVERT_SUPPORTED(T, N)>
    static std::unique_ptr<T[ ]> ToCString(const Base& base, const N& number)
    {
        const size_t len = GetRequiredLength(base, number) + GetPrefixString<T>(base).size( );
        std::unique_ptr<T[ ]> cstr = BuildPreConversionCString<T>(len);
        ToStringCommon(base, number, cstr.get( ), len);
        return cstr;
    }
};

#endif // _STRING_UTIL_HPP_