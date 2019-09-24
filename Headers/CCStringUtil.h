#ifndef _CC_STRING_UTIL_H_
#define _CC_STRING_UTIL_H_

#include "CCMacros.h"

#include "CCBuffer.h"
#include "CCString.h"

#include <vector>

namespace CC
{
    // Used for specifying general string return.
    enum class ReturnType : size_t
    {
        CCBuffer = 0,
        CCString,
        CppString,

        _End,
        _Begin = 0
    };

    // Enum Class for Specifying Number-To-String Conversion types.
    enum class Base : size_t
    {
        Binary = 0,
        Octal,
        Decimal,
        Hexadecimal,

        _End,
        _Begin = 0
    };

    ///
    //
    //  Class   - StringUtil [STATIC]
    //
    //  Purpose - Wrapper for general string utilities.
    //
    ///
    class StringUtil
    {
        // Friend class for private-method testing.
        friend class StringUtilTests;

        /// Static Class - No Ctors/Dtors
        StringUtil() = delete;
        StringUtil(const StringUtil&) = delete;
        StringUtil(StringUtil&&) = delete;
        ~StringUtil() = delete;

        StringUtil& operator=(const StringUtil&) = delete;
        StringUtil& operator=(StringUtil&&) = delete;

    private:

        /// Private Helper Enums \\\

        // Used for indicating target operation type (e.g., copy, conversion, etc).
        enum class OperationType : size_t
        {
            Comparison = 0,
            Copy,
            UTFConversion,
            NumberConversion,

            End,
            Begin = 0
        };

        // Used for indicating if an early exit condition has been met and what value to return if so.
        enum class EarlyExitResult : size_t
        {
            NoExit = 0,
            True,
            False,
            EmptyString,
            ZeroedBuffer,

            End,
            Begin = 0
        };

    private:

        /// Constexpr Enum Validators \\\

        template <OperationType OT>
        [[nodiscard]] _Success_(return) static constexpr bool IsValidOperationType() noexcept
        {
            return OT == OperationType::Comparison
                || OT == OperationType::Copy
                || OT == OperationType::UTFConversion
                || OT == OperationType::NumberConversion;
        }

        template <EarlyExitResult EER>
        [[nodiscard]] _Success_(return) static constexpr bool IsValidEarlyExitResult() noexcept
        {
            return EER == EarlyExitResult::NoExit
                || EER == EarlyExitResult::True
                || EER == EarlyExitResult::False
                || EER == EarlyExitResult::EmptyString
                || EER == EarlyExitResult::ZeroedBuffer;
        }

        template <ReturnType RT>
        [[nodiscard]] _Success_(return) static constexpr bool IsValidReturnType() noexcept
        {
            return RT == ReturnType::CCBuffer
                || RT == ReturnType::CCString
                || RT == ReturnType::CppString;
        }

        /// Common Private Helpers \\\

        template <ReturnType RT, typename C>
        [[nodiscard]] static auto BuildEmptyString()
        {
            if constexpr (RT == ReturnType::CCBuffer)
            {
                return Buffer<C>();
            }
            else if constexpr (RT == ReturnType::CCString)
            {
                return String<C>();
            }
            else if constexpr (RT == ReturnType::CppString)
            {
                return std::basic_string<C>();
            }
        }

        template <ReturnType RT, typename C>
        [[nodiscard]] static auto BuildBuffer(_In_ const size_t& len)
        {
            if constexpr (RT == ReturnType::CCBuffer)
            {
                Buffer<C> ret(len + 1);
                if (!!ret)
                {
                    memset(ret.Get(), 0, sizeof(C) * (len + 1));
                }
                return ret;
            }
            else if constexpr (RT == ReturnType::CCString)
            {
                return String<C>(len);
            }
            else if constexpr (RT == ReturnType::CppString)
            {
                try
                {
                    return std::basic_string<C>(len, static_cast<C>('\0'));
                }
                catch (const std::exception&)
                {
                    return std::basic_string<C>();
                }
            }
            else
            {
                const std::string msg1 = __FUNCSIG__": Unknown ReturnType[";
                const std::string data1 = std::to_string(static_cast<std::underlying_type_t<ReturnType>>(RT));
                const std::string msg2 = "].";
                throw std::invalid_argument(msg1 + data1 + msg2);
            }
        }

        template <typename C>
        [[nodiscard]] static bool IsValidBuffer(_In_ Buffer<C>& dst) noexcept
        {
            return !!dst;
        }

        template <typename C>
        [[nodiscard]] static bool IsValidBuffer(_In_ String<C>& dst) noexcept
        {
            return !!dst;
        }

        template <typename C>
        [[nodiscard]] static bool IsValidBuffer(_In_ std::basic_string<C>& dst) noexcept
        {
            return !dst.empty();
        }


        /// Comparison Private Helpers \\\

        // Determines if an early-exit condition for string comparison has been met.
        // If an early-exit condition is met, returns value specifying early return result.
        template <typename C>
        [[nodiscard]] static EarlyExitResult CheckForComparisonEarlyExit(_In_ const C* lhs, _In_ const size_t& lhsLen, _In_ const C* rhs, _In_ const size_t& rhsLen) noexcept
        {
            if (lhsLen != rhsLen)
            {
                return EarlyExitResult::False;
            }

            if ((lhsLen == 0) && (rhsLen == 0))
            {
                return EarlyExitResult::True;
            }

            if (lhs == rhs)
            {
                return EarlyExitResult::True;
            }

            if (!lhs || !rhs)
            {
                return (!lhs && !rhs) ? EarlyExitResult::True : EarlyExitResult::False;
            }

            return EarlyExitResult::NoExit;
        }

        // Performs case-sensitive comparison of two specified strings.
        // Returns true if both strings are the same, false otherwise.
        template <class T>
        [[nodiscard]] _Success_(return) static bool CaseSensitiveCompare(_In_reads_(len) const T* lhs, _In_reads_(len) const T* rhs, _In_ const size_t& len) noexcept
        {
            if constexpr (std::is_same_v<T, utf8>)
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
        [[nodiscard]] _Success_(return) static bool CaseInsensitiveCompare(_In_reads_(len) const T* lhs, _In_reads_(len) const T* rhs, _In_ const size_t& len) noexcept
        {
            if constexpr (std::is_same_v<T, utf8>)
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
        [[nodiscard]] static EarlyExitResult CheckForCopyEarlyExit(_In_ const C* src, _In_ const size_t& len) noexcept
        {
            if (!src || len == 0)
            {
                if constexpr (RT == ReturnType::CCBuffer)
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
        [[nodiscard]] static EarlyExitResult CheckForUTFConversionEarlyExit(_In_ const CSrc* src, _In_ const size_t& len) noexcept
        {
            return CheckForCopyEarlyExit<RT>(src, len);
        }

        template <typename C>
        [[nodiscard]] _Ret_maybenull_z_ static C* GetRawDestinationPointer(_In_ Buffer<C>& dst) noexcept
        {
            return dst.Get();
        }

        template <typename C>
        [[nodiscard]] _Ret_z_ static C* GetRawDestinationPointer(_In_ String<C>& dst) noexcept
        {
            return dst.CStr();
        }

        template <typename C>
        [[nodiscard]] _Ret_z_ static C* GetRawDestinationPointer(_In_ std::basic_string<C>& dst) noexcept
        {
            return const_cast<C*>(dst.data());
        }

        template <typename CDst, typename CSrc>
        static void PerformConversion(_Out_writes_(len) CDst* dst, _In_reads_(len) const CSrc* src, _In_ const size_t& len) noexcept
        {
            for (size_t i = 0; i < len; i++)
            {
                dst[i] = static_cast<CDst>(src[i]);
            }
        }

        /// Number Conversion Private Helpers \\\

        [[nodiscard]] static const std::vector<SupportedCharacterTuple>& GetNumberCharsTuple()
        {
            static const std::vector<SupportedCharacterTuple> sNumberChars
            {
                CC_MAKE_CHAR_TUPLE('0'), CC_MAKE_CHAR_TUPLE('1'), CC_MAKE_CHAR_TUPLE('2'), CC_MAKE_CHAR_TUPLE('3'),
                CC_MAKE_CHAR_TUPLE('4'), CC_MAKE_CHAR_TUPLE('5'), CC_MAKE_CHAR_TUPLE('6'), CC_MAKE_CHAR_TUPLE('7'),
                CC_MAKE_CHAR_TUPLE('8'), CC_MAKE_CHAR_TUPLE('9'), CC_MAKE_CHAR_TUPLE('A'), CC_MAKE_CHAR_TUPLE('B'),
                CC_MAKE_CHAR_TUPLE('C'), CC_MAKE_CHAR_TUPLE('D'), CC_MAKE_CHAR_TUPLE('E'), CC_MAKE_CHAR_TUPLE('F')
            };

            return sNumberChars;
        }

        [[nodiscard]] static const std::vector<SupportedStringTuple>& GetBasePrefixesTuple()
        {
            static const std::vector<SupportedStringTuple> sBasePrefixes
            {
                CC_MAKE_STR_TUPLE("0b"),   // binary
                CC_MAKE_STR_TUPLE("0"),    // octal
                CC_MAKE_STR_TUPLE("0n"),   // decimal
                CC_MAKE_STR_TUPLE("0x"),   // hexadecimal
            };

            return sBasePrefixes;
        }

        // Returns true if Base template argument is one of the explicitly
        // defined Base enum class values.  Returns false otherwise.
        template <Base B>
        [[nodiscard]] _Success_(return) static constexpr bool IsValidBaseType() noexcept
        {
            if constexpr (B == Base::Binary || B == Base::Octal || B == Base::Decimal || B == Base::Hexadecimal)
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
        [[nodiscard]] static const std::basic_string<T>& GetBasePrefixString() noexcept
        {
            const std::vector<SupportedStringTuple>& bases = GetBasePrefixesTuple();

            if constexpr (B == Base::Binary)
            {
                return std::get<std::basic_string<T>>(bases[0]);
            }
            else if constexpr (B == Base::Octal)
            {
                return std::get<std::basic_string<T>>(bases[1]);
            }
            else if constexpr (B == Base::Decimal)
            {
                return std::get<std::basic_string<T>>(bases[2]);
            }
            else if constexpr (B == Base::Hexadecimal)
            {
                return std::get<std::basic_string<T>>(bases[3]);
            }
        }

        // Returns a predefined character representation of a number.
        // The number must be between 0 and 15, inclusive.
        template <class T, class N>
        [[nodiscard]] static const T& NumberToCharacter(_In_ const N& n) noexcept
        {
            return std::get<T>(GetNumberCharsTuple().at(static_cast<size_t>(n)));
        }

        // Returns the total number of digits of n, depending on Base representation.
        // For Binary, Octal, and Hexadecimal bases, this includes all leading zeroes.
        // Note: Number of leading zeroes is dependent on sizeof(N) and Base.
        template <Base B, class N>
        [[nodiscard]] static size_t GetTotalDigits(_In_ const N& n) noexcept
        {
            if constexpr (B == Base::Binary)
            {
                return sizeof(n) * 8;
            }
            else if constexpr (B == Base::Octal)
            {
                return sizeof(n) * 3;
            }
            else if constexpr (B == Base::Decimal)
            {
                N absVal = n;

                if constexpr (std::is_signed_v<N>)
                {
                    const bool bNeg = (std::is_signed_v<N> && (n < 0));
                    absVal = (bNeg && n == std::numeric_limits<N>::min()) ? std::numeric_limits<N>::max() : (bNeg ? -n : n);
                }

                if (absVal < 10)
                {
                    return 1;
                }
                else
                {
                    if (absVal != std::numeric_limits<N>::max())
                    {
                        absVal++;
                    }

                    return static_cast<size_t>(ceil(log10(absVal)));
                }
            }
            else if constexpr (B == Base::Hexadecimal)
            {
                return sizeof(n) * 2;
            }
        }

        // Returns pre-defined number that will be used to help get the next digit.
        // For Binary, Octal, and Hexadecimal, this will return the amount to shift by.
        // For Decimal, this will return the amount to divide by.
        template <Base B, class N>
        [[nodiscard]] static constexpr N GetAdjuster() noexcept
        {
            if constexpr (B == Base::Binary)
            {
                return 1;
            }
            else if constexpr (B == Base::Octal)
            {
                return 3;
            }
            else if constexpr (B == Base::Decimal)
            {
                return 10;
            }
            else if constexpr (B == Base::Hexadecimal)
            {
                return 4;
            }
        }

        // Returns next digit of the number, depending on Base.
        // For Binary, Octal, and Hexadecimal, this will apply an appropriate bit mask.
        // For Decimal, this will return the result of modulus-by-10.
        template <Base B, class N>
        [[nodiscard]] static N GetDigit(_In_ const N& n) noexcept
        {
            if constexpr (B == Base::Binary)
            {
                return n & 0b1;
            }
            else if constexpr (B == Base::Octal)
            {
                return n & 07;
            }
            else if constexpr (B == Base::Decimal)
            {
                if constexpr (std::is_signed_v<N>)
                {
                    return (n < 0) ? -(n % 10) : n % 10;
                }
                else
                {
                    return n % 10;
                }
            }
            else if constexpr (B == Base::Hexadecimal)
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
            if constexpr (B == Base::Binary || B == Base::Octal || B == Base::Hexadecimal)
            {
                n >>= GetAdjuster<B, N>();
            }
            else if constexpr (B == Base::Decimal)
            {
                n /= GetAdjuster<B, N>();
            }
        }

        // Returns a pre-defined base-dependent separator that is placed between groups of digits.
        template <Base B, class T>
        [[nodiscard]] static constexpr T GetSeparator() noexcept
        {
            if constexpr (B == Base::Binary || B == Base::Octal || B == Base::Hexadecimal)
            {
                return static_cast<T>('`');
            }
            else if constexpr (B == Base::Decimal)
            {
                return static_cast<T>(',');
            }
        }

        // Returns the base-dependent number of digits that occur between separators.
        template <Base B>
        [[nodiscard]] static constexpr size_t GetSeparatorInterval() noexcept
        {
            if constexpr (B == Base::Binary || B == Base::Hexadecimal)
            {
                return 4;
            }
            else if constexpr (B == Base::Octal || B == Base::Decimal)
            {
                return 3;
            }
        }

        // Returns the buffer length that is required to hold all digits.
        template <Base B, class T, class N>
        [[nodiscard]] static size_t GetRequiredLength(_In_ const N& n) noexcept
        {
            const size_t prefixSize = GetBasePrefixString<B, T>().size();
            const size_t digits = GetTotalDigits<B>(n);
            size_t separators = (digits - 1) / GetSeparatorInterval<B>();

            if constexpr (B == Base::Decimal)
            {
                return prefixSize + digits + separators + ((n < 0) ? 1 : 0);
            }
            else
            {
                return prefixSize + digits + separators;
            }
        }

        // Builds base-dependent string representation of number n.
        // The string representation will contain the base-dependent prefix and separators.
        template <Base B, class T, class N>
        static void BuildNumberString(_In_ const N& n, _Inout_updates_all_(l) T* p, _In_ const size_t& l)
        {
            if constexpr (B != Base::Decimal && std::is_signed_v<N>)
            {
                // Single-time re-entry - Build using unsigned type for non-decimal base cases (makes logic easier).
                BuildNumberString<B, T, std::make_unsigned_t<N>>(static_cast<std::make_unsigned_t<N>>(n), p, l);
                return;
            }
            else
            {
                const std::basic_string<T>& prefixStr = GetBasePrefixString<B, T>();
                N tmp = n;
                size_t idx = l - 1;
                size_t count = 0;

                // Copy base prefix string to destination string.
                memcpy_s(p, l * sizeof(T), prefixStr.c_str(), prefixStr.length() * sizeof(T));

                // Copy over the negative sign for negative decimal numbers.
                if constexpr (B == Base::Decimal)
                {
                    if (n < 0)
                    {
                        p[prefixStr.length()] = static_cast<T>('-');
                    }
                }

                // Convert and copy digits while tmp isn't zero.
                do
                {
                    if (count++ == GetSeparatorInterval<B>())
                    {
                        p[idx--] = GetSeparator<B, T>();
                        count = 1;
                    }

                    p[idx--] = NumberToCharacter<T, N>(GetDigit<B, N>(tmp));
                    AdjustNumber<B, N>(tmp);
                } while (tmp != 0);

                if constexpr (B != Base::Decimal)
                {
                    // Fill in the rest of the buffer with zeros as needed.
                    while (idx >= prefixStr.length())
                    {
                        if (count++ == GetSeparatorInterval<B>())
                        {
                            p[idx--] = GetSeparator<B, T>();
                            count = 1;
                        }

                        p[idx--] = static_cast<T>('0');
                    }
                }
            }
        }

    public:

        // Returns number of characters in a C-string, not including the null-terminator.
        // Note: nullptr is treated as an empty string (i.e., returns 0).
        template <class T>
        [[nodiscard]] static size_t GetLength(_In_opt_z_ const T* src) noexcept
        {
            static_assert(IsSupportedCharType<T>(), __FUNCTION__": Invalid character type.");

            if (!src)
            {
                return 0;
            }

            if constexpr (std::is_same_v<T, utf8>)
            {
                return strlen(src);
            }
            else
            {
                return wcslen(src);
            }
        }

        /// Comparison Public Methods \\\

        // Forwards approriate arguments to Compare(const T*, const size_t&, const T*, const size_t&, const bool&)
        template <class T>
        [[nodiscard]] _Success_(return) static bool Compare(_In_ const std::basic_string<T>& lhs, _In_ const std::basic_string<T>& rhs, _In_ const bool& bCaseSensitive = true) noexcept
        {
            return Compare(lhs.c_str(), lhs.length(), rhs.c_str(), rhs.length(), bCaseSensitive);
        }

        // Calculates string length and forwards approriate arguments to Compare(const T*, const size_t&, const T*, const size_t&, const bool&)
        template <class T>
        [[nodiscard]] _Success_(return) static bool Compare(_In_z_ const T* lhs, _In_z_ const T* rhs, _In_ const bool& bCaseSensitive = true) noexcept
        {
            return Compare(lhs, GetLength(lhs), rhs, GetLength(rhs), bCaseSensitive);
        }

        // Returns true if both strings are the same size and have the same contents.
        template <class T>
        [[nodiscard]] _Success_(return) static bool Compare(_In_ const T* lhs, _In_ const size_t& lhsLen, _In_ const T* rhs, _In_ const size_t& rhsLen, _In_ const bool& bCaseSensitive = true) noexcept
        {
            static_assert(IsSupportedCharType<T>(), __FUNCTION__": Invalid character type.");

            auto fpCmp = (bCaseSensitive) ? CaseSensitiveCompare<T> : CaseInsensitiveCompare<T>;
            switch (CheckForComparisonEarlyExit(lhs, lhsLen, rhs, rhsLen))
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
        [[nodiscard]] static auto Copy(_In_ const std::basic_string<C>& src)
        {
            return Copy<RT, C>(src.c_str(), src.length());
        }

        template <ReturnType RT, typename C>
        [[nodiscard]] static auto Copy(_In_z_ const C* src)
        {
            return Copy<RT, C>(src, GetLength(src));
        }

        template <ReturnType RT, typename C>
        [[nodiscard]] static auto Copy(_In_reads_(len) const C* src, _In_ const size_t& len)
        {
            static_assert(IsValidReturnType<RT>(), __FUNCTION__": Invalid ReturnType template argument");
            static_assert(IsSupportedCharType<C>(), __FUNCTION__": Invalid character type template argument");

            const EarlyExitResult eer = CheckForCopyEarlyExit<RT, C>(src, len);

            if (eer == EarlyExitResult::EmptyString)
            {
                return BuildEmptyString<RT, C>();
            }

            auto copy = BuildBuffer<RT, C>(len);

            if (eer == EarlyExitResult::NoExit && IsValidBuffer(copy))
            {
                memcpy(GetRawDestinationPointer<C>(copy), src, len * sizeof(C));
            }

            return copy;
        }


        /// UTFConversion Public Methods \\\

        template <ReturnType RT, typename CDst, typename CSrc>
        [[nodiscard]] static auto UTFConversion(_In_ const std::basic_string<CSrc>& src)
        {
            return UTFConversion<RT, CDst>(src.c_str(), src.length());
        }

        template <ReturnType RT, typename CDst, typename CSrc>
        [[nodiscard]] static auto UTFConversion(_In_z_ const CSrc* src)
        {
            return UTFConversion<RT, CDst>(src, GetLength(src));
        }

        template <ReturnType RT, typename CDst, typename CSrc>
        [[nodiscard]] static auto UTFConversion(_In_reads_(len) const CSrc* src, _In_ const size_t& len)
        {
            static_assert(IsValidReturnType<RT>(), __FUNCTION__": Invalid ReturnType template argument");
            static_assert(IsSupportedCharType<CDst>(), __FUNCTION__": Invalid destination character type.");
            static_assert(IsSupportedCharType<CSrc>(), __FUNCTION__": Invalid source character type.");

            if constexpr (std::is_same<CDst, CSrc>::value)
            {
                return Copy<RT, CDst>(src, len);
            }
            else
            {
                const EarlyExitResult eer = CheckForUTFConversionEarlyExit<RT, CSrc>(src, len);

                if (eer == EarlyExitResult::EmptyString)
                {
                    return BuildEmptyString<RT, CDst>();
                }

                auto conv = BuildBuffer<RT, CDst>(len);

                if (eer == EarlyExitResult::NoExit && IsValidBuffer(conv))
                {
                    PerformConversion(GetRawDestinationPointer<CDst>(conv), src, len);
                }

                return conv;
            }
        }

        /// NumberConversion Public Methods \\\

    #define _ENABLE_IF_NUMBER_CONVERT_SUPPORTED(T, N) \
    typename = typename std::enable_if<IsSupportedCharType<T>() && IsIntegerRepresentableType<N>()>::type

        template <ReturnType RT, Base B, class T, class N, _ENABLE_IF_NUMBER_CONVERT_SUPPORTED(T, N)>
        [[nodiscard]] static auto NumberConversion(_In_ const N& integral)
        {
            static_assert(IsSupportedCharType<T>(), __FUNCTION__": Invalid character type.");
            static_assert(IsValidBaseType<B>(), __FUNCTION__": Invalid Base Type");

            if constexpr (std::is_pointer_v<N>)
            {
                return NumberConversion<RT, B, T, uintptr_t>(reinterpret_cast<uintptr_t>(integral));
            }
            else
            {
                const size_t len = GetRequiredLength<B, T, N>(integral);
                auto buffer = BuildBuffer<RT, T>(len);
                if (IsValidBuffer(buffer))
                {
                    BuildNumberString<B, T, N>(integral, GetRawDestinationPointer(buffer), len);
                }

                return buffer;
            }
        }
    #undef _ENABLE_IF_NUMBER_CONVERT_SUPPORTED
    };
}

#endif // _CC_STRING_UTIL_H_