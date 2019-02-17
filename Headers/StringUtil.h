#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include "Macros.h"

#include <sal.h>

#include <memory>
#include <vector>

///
//
//  Class   - StringUtil [STATIC]
//
//  Purpose - Wrapper for general string utilities.
//
///
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
    static void ValidateRawPointerArg(_In_opt_ const T* p, _In_ const std::basic_string<utf8>& f)
    {
        if ( !p )
        {
            throw std::invalid_argument(f + ": Attempted string operation using nullptr.");
        }
    }

    static void ValidateLengthArg(_In_ const size_t len, _In_ const std::basic_string<utf8>& f)
    {
        if ( len == 0 )
        {
            throw std::invalid_argument(f + ": Attempted string operation using zero-length buffer.");
        }
    }

    template <class T>
    static bool CaseSensitiveCompare(_In_ const T* lhs, _In_ const size_t& lhsLen, _In_ const T* rhs, _In_ const size_t& rhsLen)
    {
        if ( lhsLen != rhsLen )
        {
            return false;
        }

        if constexpr ( std::is_same_v<T, utf8> )
        {
            return strncmp(lhs, rhs, lhsLen) == 0;
        }
        else
        {
            return wcsncmp(lhs, rhs, lhsLen) == 0;
        }
    }

    template <class T>
    static bool CaseInsensitiveCompare(_In_ const T* lhs, _In_ const size_t& lhsLen, _In_ const T* rhs, _In_ const size_t& rhsLen)
    {
        if ( lhsLen != rhsLen )
        {
            return false;
        }

        if constexpr ( std::is_same_v<T, utf8> )
        {
            return _strnicmp(lhs, rhs, lhsLen) == 0;
        }
        else
        {
            return _wcsnicmp(lhs, rhs, lhsLen) == 0;
        }
    }

    template <class T>
    static bool CompareCommon(_In_ const T* lhs, _In_ const size_t& lhsLen, _In_ const T* rhs, _In_ const size_t& rhsLen, _In_ const bool& bCaseSensitive)
    {
        auto fpCmp = (bCaseSensitive) ? CaseSensitiveCompare<T> : CaseInsensitiveCompare<T>;
        return fpCmp(lhs, lhsLen, rhs, rhsLen);
    }

public:

    template <class T>
    static size_t GetLength(_In_z_ const T* src)
    {
        ValidateRawPointerArg(src, __FUNCTION__);

        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");
        
        if constexpr ( std::is_same_v<T, utf8> )
        {
            return strlen(src);
        }
        else
        {
            return wsclen(src);
        }
    }

    template <class T>
    static bool Compare(_In_ const T* lhs, _In_ const size_t& lhsLen, _In_ const T* rhs, _In_ const size_t& rhsLen, _In_ const bool& bCaseSensitive = true)
    { 
        ValidateRawPointerArg(lhs, __FUNCTION__);
        ValidateRawPointerArg(rhs, __FUNCTION__);
        ValidateLengthArg(lhsLen, __FUNCTION__);
        ValidateLengthArg(rhsLen, __FUNCTION__);

        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");

        return CompareCommon(lhs, lhsLen, rhs, rhsLen, bCaseSensitive);
    }

    template <class T>
    static bool Compare(_In_z_ const T* lhs, _In_z_ const T* rhs, const bool& bCaseSensitive = true)
    {
        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");
        return CompareCommon(lhs, GetLength(lhs), rhs, GetLength(rhs), bCaseSensitive);
    }

    template <class T>
    static bool Compare(_In_ const std::basic_string<T>& lhs, _In_ const std::basic_string<T>& rhs, _In_ const bool& bCaseSensitive = true)
    {
        ValidateLengthArg(lhs.length( ), __FUNCTION__);
        ValidateLengthArg(rhs.length( ), __FUNCTION__);

        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");

        return CompareCommon(lhs.c_str( ), lhs.size( ), rhs.c_str( ), rhs.size( ), bCaseSensitive);
    }

    // Specialization Utility Classes.
    class Copy;
    class UTFConversion;
    class NumberConversion;
};


///
//
//  Class   - StringUtil::Copy [STATIC]
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
    static std::basic_string<T> ToString(_In_ const std::basic_string<T>& src)
    {
        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");
        return ToString(src.c_str( ), src.length( ));
    }

    template <class T>
    static std::basic_string<T> ToString(_In_z_ const T* src)
    {
        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");
        return ToString(src, GetLength(src));
    }

    template <class T>
    static std::basic_string<T> ToString(_In_reads_(len) const T* src, _In_ const size_t& len)
    {
        ValidateRawPointerArg(src, __FUNCTION__);
        ValidateLengthArg(len, __FUNCTION__);

        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");

        return std::basic_string<T>(src, len);
    }

    template <class T>
    static std::unique_ptr<T[ ]> ToCString(_In_ const std::basic_string<T>& src)
    {
        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");
        return ToCString(src.c_str( ), src.length( ));
    }

    template <class T>
    static std::unique_ptr<T[ ]> ToCString(_In_z_ const T* src)
    {
        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");
        return ToCString(src, GetLength<T>(src));
    }

    template <class T>
    static std::unique_ptr<T[ ]> ToCString(_In_reads_(len) const T* src, _In_ const size_t& len)
    {
        std::unique_ptr<T[ ]> copy;

        ValidateRawPointerArg(src, __FUNCTION__);
        ValidateLengthArg(len, __FUNCTION__);

        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");

        copy = std::make_unique<T[ ]>(len + 1);
        memcpy(copy.get( ), src, sizeof(T)*(len + 1));

        return copy;
    }
};


///
//
//  Class   - StringUtil::UTFConversion [STATIC]
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
    static std::basic_string<T_Dst> BuildPreConversionString(_In_ const size_t& len)
    {
        return std::basic_string<T_Dst>(len, static_cast<T_Dst>('!'));
    }

    template <class T_Dst>
    static std::unique_ptr<T_Dst[ ]> BuildPreConversionCString(_In_ const size_t& len)
    {
        std::unique_ptr<T_Dst[ ]> ptr(std::make_unique<T_Dst[ ]>(len + 1));
        memset(ptr.get( ), static_cast<T_Dst>('\0'), sizeof(T_Dst)*(len + 1));
        return ptr;
    }

    template <class T_Dst, class T_Src>
    static void ToStringCommon(_Out_writes_(len) T_Dst* dst, _In_reads_(len) const T_Src* src, _In_ const size_t& len)
    {
        for ( size_t i = 0; i < len; i++ )
        {
            dst[i] = static_cast<T_Dst>(src[i]);
        }
    }

public:

    template <class T_Dst, class T_Src>
    static std::basic_string<T_Dst> ToString(_In_ const std::basic_string<T_Src>& src)
    {
        static_assert(IsSupportedCharType<T_Src>( ), __FUNCTION__": Invalid source character type.");
        static_assert(IsSupportedCharType<T_Dst>( ), __FUNCTION__": Invalid destination character type.");
        return ToString<T_Dst, T_Src>(src.c_str( ), src.length( ));
    }

    template <class T_Dst, class T_Src>
    static std::basic_string<T_Dst> ToString(_In_z_ const T_Src* src)
    {
        static_assert(IsSupportedCharType<T_Src>( ), __FUNCTION__": Invalid source character type.");
        static_assert(IsSupportedCharType<T_Dst>( ), __FUNCTION__": Invalid destination character type.");
        return ToString<T_Dst, T_Src>(src, GetLength(src));
    }

    template <class T_Dst, class T_Src>
    static std::basic_string<T_Dst> ToString(_In_ const T_Src* src, _In_ const size_t& len)
    {
        ValidateRawPointerArg(src, __FUNCTION__);
        ValidateLengthArg(len, __FUNCTION__);

        static_assert(IsSupportedCharType<T_Src>( ), __FUNCTION__": Invalid source character type.");
        static_assert(IsSupportedCharType<T_Dst>( ), __FUNCTION__": Invalid destination character type.");

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
    static std::unique_ptr<T_Dst[ ]> ToCString(_In_ const std::basic_string<T_Src>& src)
    {
        static_assert(IsSupportedCharType<T_Src>( ), __FUNCTION__": Invalid source character type.");
        static_assert(IsSupportedCharType<T_Dst>( ), __FUNCTION__": Invalid destination character type.");
        return ToCString<T_Dst, T_Src>(src.c_str( ), src.length( ));
    }

    template <class T_Dst, class T_Src>
    static std::unique_ptr<T_Dst[ ]> ToCString(_In_z_ const T_Src* src)
    {
        static_assert(IsSupportedCharType<T_Src>( ), __FUNCTION__": Invalid source character type.");
        static_assert(IsSupportedCharType<T_Dst>( ), __FUNCTION__": Invalid destination character type.");
        return ToCString<T_Dst, T_Src>(src, GetLength(src));
    }

    template <class T_Dst, class T_Src>
    static std::unique_ptr<T_Dst[ ]> ToCString(_In_reads_(len) const T_Src* src, _In_ const size_t& len)
    {
        ValidateRawPointerArg(src, __FUNCTION__);
        ValidateLengthArg(len, __FUNCTION__);

        static_assert(IsSupportedCharType<T_Src>( ), __FUNCTION__": Invalid source character type.");
        static_assert(IsSupportedCharType<T_Dst>( ), __FUNCTION__": Invalid destination character type.");

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
//  Class   - StringUtil::NumberConversion [STATIC]
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
        Hexadecimal = 16,
    };

    using BaseType = std::underlying_type<Base>::type;

private:

    static const std::vector<SupportedStringTuple> s_BasePrefixes;
    static const std::vector<SupportedCharacterTuple> s_NumberChars;

    template <Base B>
    static constexpr bool IsValidBaseType( )
    {
        if constexpr ( B == Base::Binary || B == Base::Octal || B == Base::Decimal || B == Base::Hexadecimal )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    template <Base B, class T>
    static constexpr const std::basic_string<T>& GetBasePrefixString( )
    {
        if constexpr ( B == Base::Binary )
        {
            return std::get<std::basic_string<T>>(s_BasePrefixes[0]);
        }
        else if constexpr ( B == Base::Octal )
        {
            return std::get<std::basic_string<T>>(s_BasePrefixes[1]);
        }
        else if constexpr ( B == Base::Decimal )
        {
            return std::get<std::basic_string<T>>(s_BasePrefixes[2]);
        }
        else if constexpr ( B == Base::Hexadecimal )
        {
            return std::get<std::basic_string<T>>(s_BasePrefixes[3]);
        }
    }

    template <class T, class N>
    static T NumberToCharacter(_In_range_(0, 15) const N& n)
    {
        return std::get<T>(s_NumberChars.at(static_cast<size_t>(n)));
    }

    template <Base B, class N>
    static const size_t GetTotalDigits(_In_ const N& n)
    {
        if constexpr ( B == Base::Binary )
        {
            return sizeof(n) * 8;
        }
        else if constexpr ( B == Base::Octal )
        {
            return sizeof(n) * 3;
        }
        else if constexpr ( B == Base::Decimal )
        {
            const bool bNeg = (std::is_signed_v<N> && (n < 0));
            N absVal = (bNeg && n == std::numeric_limits<N>::min( )) ? std::numeric_limits<N>::max( ) : static_cast<N>(abs(n));

            if ( absVal < 10 )
            {
                return 1;
            }
            else
            {
                if ( absVal != std::numeric_limits<N>::max( ) )
                {
                    absVal++;
                }

                return static_cast<size_t>(ceil(log10(absVal)));
            }
        }
        else if constexpr ( B == Base::Hexadecimal )
        {
            return sizeof(n) * 2;
        }
    }

    template <Base B, class N>
    static constexpr const N GetAdjuster( )
    {
        if constexpr ( B == Base::Binary )
        {
            return 1;
        }
        else if constexpr ( B == Base::Octal )
        {
            return 3;
        }
        else if constexpr ( B == Base::Decimal )
        {
            return 10;
        }
        else if constexpr ( B == Base::Hexadecimal )
        {
            return 4;
        }
    }

    template <Base B, class N>
    static const N GetDigit(_In_ const N& n)
    {
        if constexpr ( B == Base::Binary )
        {
            return n & 0b1;
        }
        else if constexpr ( B == Base::Octal )
        {
            return n & 07;
        }
        else if constexpr ( B == Base::Decimal )
        {
            return (n < 0) ? -(n % 10) : n % 10;
        }
        else if constexpr ( B == Base::Hexadecimal )
        {
            return n & 0xF;
        }
    }

    template <Base B, class N>
    static void AdjustNumber(_Inout_ N& n)
    {
        if constexpr ( B == Base::Binary || B == Base::Octal || B == Base::Hexadecimal )
        {
            n >>= GetAdjuster<B, N>( );
        }
        else if constexpr ( B == Base::Decimal )
        {
            n /= GetAdjuster<B, N>( );
        }
    }

    template <Base B, class T>
    static constexpr const T GetSeparator( )
    {
        if constexpr ( B == Base::Binary || B == Base::Octal || B == Base::Hexadecimal )
        {
            return static_cast<T>('`');
        }
        else if constexpr ( B == Base::Decimal )
        {
            return static_cast<T>(',');
        }
    }

    template <Base B>
    static constexpr const size_t GetSeparatorInterval( )
    {
        if constexpr ( B == Base::Binary || B == Base::Hexadecimal )
        {
            return 4;
        }
        else if constexpr ( B == Base::Octal || B == Base::Decimal )
        {
            return 3;
        }
    }

    template <Base B, class T, class N>
    static size_t GetRequiredLength(_In_ const N& n)
    {
        const size_t prefixSize = GetBasePrefixString<B, T>( ).size( );
        const size_t digits = GetTotalDigits<B>(n);
        size_t separators = (digits - 1) / GetSeparatorInterval<B>( );

        if constexpr ( B == Base::Decimal )
        {
            return prefixSize + digits + separators + ((n < 0) ? 1 : 0);
        }
        else
        {
            return prefixSize + digits + separators;
        }
    }

    template <class T, class N>
    static std::basic_string<T> BuildBuffer(_In_ const size_t& len)
    {
        return std::basic_string<T>(len, static_cast<T>('\0'));
    }

    template <class T, class N>
    static std::unique_ptr<T[ ]> BuildCBuffer(_In_ const size_t& len)
    {
        std::unique_ptr<T[ ]> ptr(std::make_unique<T[ ]>(len + 1));
        memset(ptr.get( ), static_cast<T>('\0'), sizeof(T)*(len + 1));
        return ptr;
    }

    template <Base B, class T, class N>
    static void BuildString(_In_ const N& n, _Inout_updates_(l) T* p, _In_ const size_t& l)
    {
        if constexpr ( B != Base::Decimal && std::is_signed_v<N> )
        {
            // Single-time re-entry - Build using unsigned type for non-decimal base cases (makes logic easier).
            BuildString<B, T, std::make_unsigned_t<N>>(static_cast<std::make_unsigned_t<N>>(n), p, l);
            return;
        }
        else
        {
            N tmp = n;
            size_t idx = l - 1;
            size_t count = 0;

            // Copy base prefix string to destination string.
            memcpy_s(p, l, GetBasePrefixString<B, T>( ).c_str( ), GetBasePrefixString<B, T>( ).size( ));

            // Copy over the negative sign for negative decimal numbers.
            if constexpr ( B == Base::Decimal )
            {
                if ( n < 0 )
                {
                    p[GetBasePrefixString<B, T>( ).size( )] = static_cast<T>('-');
                }
            }

            // Convert and copy digits while tmp isn't zero.
            do
            {
                if ( count != 0 && ((count % GetSeparatorInterval<B>( )) == 0) )
                {
                    p[idx--] = GetSeparator<B, T>( );
                }

                p[idx--] = NumberToCharacter<T, N>(GetDigit<B, N>(tmp));
                AdjustNumber<B, N>(tmp);
                count++;
            } while ( tmp != 0 );

            if constexpr ( B != Base::Decimal )
            {
                // Fill in the rest of the buffer with zeros as needed.
                while ( idx >= GetBasePrefixString<B, T>( ).size( ) )
                {
                    if ( count != 0 && ((count % GetSeparatorInterval<B>( )) == 0) )
                    {
                        p[idx--] = GetSeparator<B, T>( );
                    }

                    p[idx--] = static_cast<T>('0');
                    count++;
                }
            }
        }
    }

public:

#define _ENABLE_IF_NUMBER_CONVERT_SUPPORTED(T, N) \
    typename = typename std::enable_if<IsSupportedCharType<T>( ) && IsIntegerRepresentableType<N>( )>::type

    // Return string representation of number argument
    template <Base B, class T, class N, _ENABLE_IF_NUMBER_CONVERT_SUPPORTED(T, N)>
    static std::basic_string<T> ToString(_In_ const N& number)
    {
        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");
        static_assert(IsValidBaseType<B>( ), __FUNCTION__": Invalid Base Type");

        std::basic_string<T> str(BuildBuffer<T, N>(GetRequiredLength<B, T>(number)));
        BuildString<B, T>(number, const_cast<T*>(str.data( )), str.size( ));
        return str;
    }

    template <Base B, class T, class N, _ENABLE_IF_NUMBER_CONVERT_SUPPORTED(T, N)>
    static std::unique_ptr<T[ ]> ToCString(_In_ const N& number)
    {
        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");
        static_assert(IsValidBaseType<B>( ), __FUNCTION__": Invalid Base Type");

        const size_t len = GetRequiredLength<B, T>(number);
        std::unique_ptr<T[ ]> cstr = BuildCBuffer<T, N>(len);
        BuildString<B, T>(number, cstr.get( ), len);
        return cstr;
    }
};

#endif // _STRING_UTIL_H_