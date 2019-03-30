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

    /// Protected Static Helper Methods \\\

    // Used for indicating target operation type (e.g., copy, conversion, etc).
    enum class OperationType : size_t
    {
        Comparison,
        Copy,
        UTFConversion,
        NumberConversion
    };

    // Used for indicating if an early exit condition has been met and what value to return if so.
    enum class EarlyExitResult : size_t
    {
        NoExit,
        True,
        False,
        EmptyString,
        ZeroedBuffer
    };

public:

    // Used for specifying general string return.
    enum class ReturnType : size_t
    {
        SmartCString,
        StringObj
    };

private:

    /// Constexpr Enum Validators \\\

    template <OperationType OT>
    static constexpr bool IsValidOperationType( ) noexcept
    {
        return OT == OperationType::Comparison
            || OT == OperationType::Copy
            || OT == OperationType::UTFConversion
            || OT == OperationType::NumberConversion;
    }

    template <EarlyExitResult EER>
    static constexpr bool IsValidEarlyExitResult( ) noexcept
    {
        return EER == EarlyExitResult::NoExit
            || EER == EarlyExitResult::True
            || EER == EarlyExitResult::False
            || EER == EarlyExitResult::EmptyString
            || EER == EarlyExitResult::ZeroedBuffer;
    }

    template <ReturnType RT>
    static constexpr bool IsValidReturnType( ) noexcept
    {
        return RT == ReturnType::SmartCString
            || RT == ReturnType::StringObj;
    }

    /// Common Private Helpers \\\

    template <ReturnType RT, typename C>
    static auto BuildString(_In_ const size_t& len)
    {
        if constexpr ( RT == ReturnType::SmartCString )
        {
            std::unique_ptr<C[ ]> ret(std::make_unique<C[ ]>(len + 1));
            memset(ret.get( ), static_cast<C>('\0'), (len + 1) * sizeof(C));
            return ret;
        }
        else // RT == ReturnType::StringObj
        {
            return std::basic_string<C>(len, static_cast<C>('\0'));
        }
    }

    /// Comparison Private Helpers \\\

    // Determines if an early-exit condition for string comparison has been met.
    // If an early-exit condition is met, returns value specifying early return result.
    template <typename C>
    static EarlyExitResult CheckForComparisonEarlyExit(_In_ const C* lhs, _In_ const size_t& lhsLen, _In_ const C* rhs, _In_ const size_t& rhsLen) noexcept
    {
        if ( lhsLen != rhsLen )
        {
            return EarlyExitResult::False;
        }
        else if ( (lhsLen == 0) && (rhsLen == 0) )
        {
            return EarlyExitResult::True;
        }
        else if ( !lhs || !rhs )
        {
            return (!lhs && !rhs) ? EarlyExitResult::True : EarlyExitResult::False;
        }

        return EarlyExitResult::NoExit;
    }

    // Performs case-sensitive comparison of two specified strings.
    // Returns true if both strings are the same, false otherwise.
    template <class T>
    static bool CaseSensitiveCompare(_In_ const T* lhs, _In_ const T* rhs, _In_ const size_t& len) noexcept
    {
        if constexpr ( std::is_same_v<T, utf8> )
        {
            return strncmp(lhs, rhs, len) == 0;
        }
        else
        {
            return wcsncmp(lhs, rhs, len) == 0;
        }
    }

    // Performs case-insensitive comparison of two specified strings.
    // Returns true if both strings are the same, false otherwise.
    template <class T>
    static bool CaseInsensitiveCompare(_In_ const T* lhs, _In_ const T* rhs, _In_ const size_t& len) noexcept
    {
        if constexpr ( std::is_same_v<T, utf8> )
        {
            return _strnicmp(lhs, rhs, len) == 0;
        }
        else
        {
            return _wcsnicmp(lhs, rhs, len) == 0;
        }
    }


    /// Copy/UTFConversion Private Helpers \\\

    template <ReturnType RT, typename C>
    static EarlyExitResult CheckForCopyEarlyExit(_In_ const C* src, _In_ const size_t& len) noexcept
    {
        if ( !src || len == 0 )
        {
            if constexpr ( RT == ReturnType::SmartCString )
            {
                return EarlyExitResult::ZeroedBuffer;
            }
            else
            {
                return EarlyExitResult::EmptyString;
            }
        }

        return EarlyExitResult::NoExit;
    }

    template <ReturnType RT, typename CSrc>
    static EarlyExitResult CheckForUTFConversionEarlyExit(_In_ const CSrc* src, _In_ const size_t& len) noexcept
    {
        return CheckForCopyEarlyExit<RT>(src, len);
    }

    template <typename C>
    static C* GetRawDestinationPointer(std::unique_ptr<C[]>& dst) noexcept
    {
        return dst.get( );
    }

    template <typename C>
    static C* GetRawDestinationPointer(std::basic_string<C>& dst) noexcept
    {
        return const_cast<C*>(dst.data( ));
    }

    template <typename CDst, typename CSrc>
    static void PerformConversion(_Out_writes_(len) CDst* dst, _In_reads_(len) const CSrc* src, _In_ const size_t& len) noexcept
    {
        for ( size_t i = 0; i < len; i++ )
        {
            dst[i] = static_cast<CDst>(src[i]);
        }
    }

public:

    // Returns number of characters in a C-string, not including the null-terminator.
    // Note: nullptr is treated as an empty string (i.e., returns 0).
    template <class T>
    static size_t GetLength(_In_z_ const T* src) noexcept
    {
        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");

        if ( !src )
        {
            return 0;
        }
        
        if constexpr ( std::is_same_v<T, utf8> )
        {
            return strlen(src);
        }
        else
        {
            return wsclen(src);
        }
    }

    /// Comparison Public Methods \\\

    // Forwards approriate arguments to Compare(const T*, const size_t&, const T*, const size_t&, const bool&)
    template <class T>
    static bool Compare(_In_ const std::basic_string<T>& lhs, _In_ const std::basic_string<T>& rhs, _In_ const bool& bCaseSensitive = true) noexcept
    {
        return Compare(lhs.c_str( ), lhs.length( ), rhs.c_str( ), rhs.length( ), bCaseSensitive);
    }

    // Calculates string length and forwards approriate arguments to Compare(const T*, const size_t&, const T*, const size_t&, const bool&)
    template <class T>
    static bool Compare(_In_z_ const T* lhs, _In_z_ const T* rhs, const bool& bCaseSensitive = true) noexcept
    {
        return Compare(lhs, GetLength(lhs), rhs, GetLength(rhs), bCaseSensitive);
    }

    // Returns true if both strings are the same size and have the same contents.
    template <class T>
    static bool Compare(_In_ const T* lhs, _In_ const size_t& lhsLen, _In_ const T* rhs, _In_ const size_t& rhsLen, _In_ const bool& bCaseSensitive = true) noexcept
    {
        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");

        auto fpCmp = (bCaseSensitive) ? CaseSensitiveCompare<T> : CaseInsensitiveCompare<T>;
        switch ( CheckForComparisonEarlyExit(lhs, lhsLen, rhs, rhsLen) )
        {
        case EarlyExitResult::NoExit:
            return fpCmp(lhs, rhs, lhsLen);

        case EarlyExitResult::True:
            return true;

        case EarlyExitResult::False:
        default:
            return false;
        }
    }

    /// Copy Public Methods \\\

    template <ReturnType RT, typename C>
    static auto Copy(_In_ const std::basic_string<C>& src)
    {
        return Copy<RT, C>(src.c_str( ), src.length( ));
    }

    template <ReturnType RT, typename C>
    static auto Copy(_In_z_ const C* src)
    {
        return Copy<RT, C>(src, GetLength(src));
    }

    template <ReturnType RT, typename C>
    static auto Copy(_In_ const C* src, const size_t& len)
    {
        static_assert(IsValidReturnType<RT>( ), __FUNCTION__": Invalid ReturnType template argument");
        static_assert(IsSupportedCharType<C>( ), __FUNCTION__": Invalid character type template argument");

        auto copy = BuildString<RT, C>(len);

        if ( CheckForCopyEarlyExit<RT, C>(src, len) == EarlyExitResult::NoExit )
        {
            memcpy(GetRawDestinationPointer<C>(copy), src, len * sizeof(C));
        }

        return copy;
    }


    /// UTFConversion Public Methods \\\

    template <ReturnType RT, typename CDst, typename CSrc>
    static auto UTFConversion(_In_ const std::basic_string<CSrc>& src)
    {
        return UTFConversion<RT, CDst>(src.c_str( ), src.length( ));
    }

    template <ReturnType RT, typename CDst, typename CSrc>
    static auto UTFConversion(_In_z_ const CSrc* src)
    {
        return UTFConversion<RT, CDst>(src, GetLength(src));
    }

    template <ReturnType RT, typename CDst, typename CSrc>
    static auto UTFConversion(_In_ const CSrc* src, const size_t& len)
    {
        static_assert(IsValidReturnType<RT>( ), __FUNCTION__": Invalid ReturnType template argument");
        static_assert(IsSupportedCharType<CDst>( ), __FUNCTION__": Invalid destination character type.");
        static_assert(IsSupportedCharType<CSrc>( ), __FUNCTION__": Invalid source character type.");

        if constexpr ( std::is_same<CDst, CSrc>::value )
        {
            return Copy<RT, CDst>(src, len);
        }
        else
        {
            auto conv = BuildString<RT, CDst>(len);

            if ( CheckForUTFConversionEarlyExit<RT, CSrc>(src, len) == EarlyExitResult::NoExit )
            {
                PerformConversion(GetRawDestinationPointer(conv), src, len);
            }

            return conv;
        }
    }

    // Specialization Utility Classes.
    class NumberConversion;
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

    static const std::vector<SupportedCharacterTuple>& GetNumberCharsTuple( )
    {
        static const std::vector<SupportedCharacterTuple> sNumberChars
        {
            MAKE_CHAR_TUPLE('0'), MAKE_CHAR_TUPLE('1'), MAKE_CHAR_TUPLE('2'), MAKE_CHAR_TUPLE('3'),
            MAKE_CHAR_TUPLE('4'), MAKE_CHAR_TUPLE('5'), MAKE_CHAR_TUPLE('6'), MAKE_CHAR_TUPLE('7'),
            MAKE_CHAR_TUPLE('8'), MAKE_CHAR_TUPLE('9'), MAKE_CHAR_TUPLE('A'), MAKE_CHAR_TUPLE('B'),
            MAKE_CHAR_TUPLE('C'), MAKE_CHAR_TUPLE('D'), MAKE_CHAR_TUPLE('E'), MAKE_CHAR_TUPLE('F')
        };

        return sNumberChars;
    }

    static const std::vector<SupportedStringTuple>& GetBasePrefixesTuple( )
    {
        static const std::vector<SupportedStringTuple> sBasePrefixes
        {
            MAKE_STR_TUPLE("0b"),   // binary
            MAKE_STR_TUPLE("0"),    // octal
            MAKE_STR_TUPLE("0n"),   // decimal
            MAKE_STR_TUPLE("0x"),   // hexadecimal
        };

        return sBasePrefixes;
    }

    // Returns true if Base template argument is one of the explicitly
    // defined Base enum class values.  Returns false otherwise.
    template <Base B>
    static constexpr bool IsValidBaseType( ) noexcept
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

    // Returns a predefined prefix string for a particular Base value.
    template <Base B, class T>
    static constexpr const std::basic_string<T>& GetBasePrefixString( ) noexcept
    {
        const std::vector<SupportedStringTuple>& bases = GetBasePrefixesTuple( )

        if constexpr ( B == Base::Binary )
        {
            return std::get<std::basic_string<T>>(bases[0]);
        }
        else if constexpr ( B == Base::Octal )
        {
            return std::get<std::basic_string<T>>(bases[1]);
        }
        else if constexpr ( B == Base::Decimal )
        {
            return std::get<std::basic_string<T>>(bases[2]);
        }
        else if constexpr ( B == Base::Hexadecimal )
        {
            return std::get<std::basic_string<T>>(bases[3]);
        }
    }

    // Returns a predefined character representation of a number.
    // The number must be between 0 and 15, inclusive.
    template <class T, class N>
    static const T& NumberToCharacter(_In_range_(0, 15) const N& n) noexcept
    {
        return std::get<T>(GetNumberCharsTuple( ).at(static_cast<size_t>(n)));
    }

    // Returns the total number of digits of n, depending on Base representation.
    // For Binary, Octal, and Hexadecimal bases, this includes all leading zeroes.
    // Note: Number of leading zeroes is dependent on sizeof(N) and Base.
    template <Base B, class N>
    static const size_t GetTotalDigits(_In_ const N& n) noexcept
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

    // Returns pre-defined number that will be used to help get the next digit.
    // For Binary, Octal, and Hexadecimal, this will return the amount to shift by.
    // For Decimal, this will return the amount to divide by.
    template <Base B, class N>
    static constexpr const N GetAdjuster( ) noexcept
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

    // Returns next digit of the number, depending on Base.
    // For Binary, Octal, and Hexadecimal, this will apply an appropriate bit mask.
    // For Decimal, this will return the result of modulus-by-10.
    template <Base B, class N>
    static const N GetDigit(_In_ const N& n) noexcept
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

    // Retrieves the base-dependent adjuster value and performs the base-dependent operation to n.
    // For Binary, Octal, and Hexadecimal bases, this performs a shift operation on n.
    // For Decimal base, this performs division.
    template <Base B, class N>
    static void AdjustNumber(_Inout_ N& n) noexcept
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

    // Returns a pre-defined base-dependent separator that is placed between groups of digits.
    template <Base B, class T>
    static constexpr const T GetSeparator( ) noexcept
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

    // Returns the base-dependent number of digits that occur between separators.
    template <Base B>
    static constexpr const size_t GetSeparatorInterval( ) noexcept
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

    // Returns the buffer length that is required to hold all digits.
    template <Base B, class T, class N>
    static size_t GetRequiredLength(_In_ const N& n) noexcept
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

    // Returns C++ string of length len.
    template <class T, class N>
    static std::basic_string<T> BuildBuffer(_In_ const size_t& len)
    {
        return std::basic_string<T>(len, static_cast<T>('\0'));
    }

    // Returns C-string of length len, wrapped in a smart pointer.
    template <class T, class N>
    static std::unique_ptr<T[ ]> BuildCBuffer(_In_ const size_t& len)
    {
        std::unique_ptr<T[ ]> ptr(std::make_unique<T[ ]>(len + 1));
        memset(ptr.get( ), static_cast<T>('\0'), sizeof(T)*(len + 1));
        return ptr;
    }

    // Builds base-dependent string representation of number n.
    // The string representation will contain the base-dependent prefix and separators.
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

    // Return C++ string representation of number argument.
    template <Base B, class T, class N, _ENABLE_IF_NUMBER_CONVERT_SUPPORTED(T, N)>
    static std::basic_string<T> ToString(_In_ const N& number)
    {
        static_assert(IsSupportedCharType<T>( ), __FUNCTION__": Invalid character type.");
        static_assert(IsValidBaseType<B>( ), __FUNCTION__": Invalid Base Type");

        std::basic_string<T> str(BuildBuffer<T, N>(GetRequiredLength<B, T>(number)));
        BuildString<B, T>(number, const_cast<T*>(str.data( )), str.size( ));
        return str;
    }

    // Return C-string representation of number argument, contained in a smart pointer.
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