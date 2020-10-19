#pragma once

// CC
#include "CCMacros.h"
#include "CCPointer.h"
#include "CCPointerHelper.h"

// STL
#include <algorithm>
#include <cstring>
#include <cwctype>
#include <memory>
#include <stdexcept>


namespace CC
{
    template <typename T, CC_ENABLE_IF_SUPPORTED_CHARACTER_TYPE(T)>
    class [[nodiscard]] String
    {
        // Testing class.
        friend class StringTests;

        // Type aliases.
        using PH = PointerHelper<T>;

    private:

        // Private Static Data Members //

        static constexpr size_t ms_SSOCap = 32;


        // Private Data Members //

        T m_SSOArr[ms_SSOCap];
        T* m_pStr;
        size_t m_Len;
        size_t m_Cap;


        // Private Throwing Validators //

        // Throws std::out_of_range if using idx would access memory beyond the length of the string.
        inline void ValidateIndexT(_In_z_ const char* const f, _In_ const size_t& idx) const
        {
            if (idx >= m_Len)
            {
                static const std::string msg1 = ": Index[";
                static const std::string msg2 = "] exceeds length of string[";
                static const std::string msg3 = "].";
                const std::string idxStr = std::to_string(idx);
                const std::string endStr = std::to_string(m_Len);

                throw std::out_of_range(f + msg1 + idxStr + msg2 + endStr + msg3);
            }
        }


        // Private Static Validators //

        // Returns false if p is nullptr, true otherwise.
        [[nodiscard]] inline static bool ValidatePointer(_In_opt_ const T* const p) noexcept
        {
            return !!p;
        }

        // Returns false if len is ms_InvalidLen, true otherwise.
        [[nodiscard]] inline static bool ValidateLength(_In_ const size_t& len) noexcept
        {
            return (len != ms_InvalidLen);
        }

        // Returns false if p is nullptr or len is ms_InvalidLen, true otherwise.
        [[nodiscard]] inline static bool ValidatePointerAndLength(_In_opt_ const T* const p, _In_ const size_t& len) noexcept
        {
            return ValidatePointer(p) && ValidateLength(len);
        }


        // Static Private Helper Methods //

        // Returns true if tc is ms_NullTerminator, false otherwise.
        [[nodiscard]] inline static bool IsNullTerminator(_In_ const T& tc) noexcept
        {
            return (tc == ms_NullTerminator) ? true : false;
        }

        // Returns length of C-string argument.
        // Note: Returned length excludes null-terminator character.
        // Note: If pCStr is nullptr, this returns ms_InvalidLen.
        [[nodiscard]] _Success_(return != ms_InvalidLen) inline static size_t GetStringLength(_In_opt_z_ const T* pCStr) noexcept
        {
            size_t idx = 0;
            if (!pCStr)
            {
                return ms_InvalidLen;
            }

            while (pCStr[idx] != ms_NullTerminator)
            {
                idx++;
            }

            return idx;
        }

        // Returns true if strings are equivalent, false otherwise.
        [[nodiscard]] inline static bool CompareStrings(
            _In_ const String<T> & lhs,
            _In_ const String<T> & rhs,
            _In_ const bool bCaseInsensitive = false) noexcept
        {
            return CompareStrings(lhs.m_pStr, lhs.m_Len, rhs.m_pStr, rhs.m_Len, bCaseInsensitive);
        }

        // Returns true if strings are equivalent, false otherwise.
        [[nodiscard]] inline static bool CompareStrings(
            _In_ const String<T> & lhs,
            _In_reads_(rhsLen) const T * pRhs,
            _In_ const size_t rhsLen,
            _In_ const bool bCaseInsensitive = false) noexcept
        {
            return CompareStrings(lhs.m_pStr, lhs.m_Len, pRhs, rhsLen, bCaseInsensitive);
        }

        // Returns true if strings are equivalent, false otherwise.
        [[nodiscard]] inline static bool CompareStrings(
            _In_reads_(lhsLen) const T * pLhs,
            _In_ const size_t lhsLen,
            _In_ const String<T> & rhs,
            _In_ const bool bCaseInsensitive = false) noexcept
        {
            return CompareStrings(pLhs, lhsLen, rhs.m_pStr, rhs.m_Len, bCaseInsensitive);
        }

        // Returns true if strings are equivalent, false otherwise.
        [[nodiscard]] static bool CompareStrings(
            _In_reads_(lhsLen) const T * pLhs, _In_ const size_t lhsLen,
            _In_reads_(rhsLen) const T * pRhs, _In_ const size_t rhsLen,
            _In_ const bool bCaseInsensitive = false) noexcept
        {
            if (pLhs == pRhs)
            {
                // Same pointers mean same contents - match.
                return true;
            }

            if ((lhsLen == 0) && (rhsLen == 0))
            {
                // Zero-length comparison is treated as a match.
                return true;
            }

            if (!pLhs || !pRhs)
            {
                // One pointer is nullptr - no match.
                return false;
            }


            if (bCaseInsensitive)
            {
                const size_t compLen = std::min(lhsLen, rhsLen);
                for (size_t i = 0; i < compLen; ++i)
                {
                    if constexpr (std::is_same_v<T, char>)
                    {
                        if (std::tolower(static_cast<int>(pLhs[i])) != std::tolower(static_cast<int>(pRhs[i])))
                        {
                            return false;
                        }
                    }
                    else // if constexpr (std::is_same_v<T, wchar_t>)
                    {
                        if (std::towlower(static_cast<std::wint_t>(pLhs[i])) != std::towlower(static_cast<std::wint_t>(pRhs[i])))
                        {
                            return false;
                        }
                    }
                }

                return true;
            }
            else
            {
                const size_t compSize = sizeof(T) * std::min(lhsLen, rhsLen);
                return memcmp(pLhs, pRhs, compSize) == 0;
            }
        }


        // Private Helper Methods //

        // Allocates new string array, assigns it to m_pStr, and sets m_Cap.  Also copies old contents if specified.
        // Returns true if allocation succeeds, false otherwise.
        // Note: Does not modify data members if allocation fails.
        template <bool bCopyFromOld>
        [[nodiscard]] _Success_(return) bool AllocateNewStringArray(_In_ const size_t& len) noexcept
        {
            const size_t newCap = std::max((m_Cap + len + 16), (m_Cap + (m_Cap >> 1)));
            T* p = PH::AllocateLen(newCap);
            if (!p)
            {
                return false;
            }

            if constexpr (bCopyFromOld)
            {
                memcpy(p, m_pStr, sizeof(T) * m_Len);
            }

            DeleteStringPointerIfNotSSO();
            m_pStr = p;
            m_Cap = newCap;

            return true;
        }

        // Returns true if copying len tc's is successful, false otherwise.
        [[nodiscard]] _Success_(return) bool LengthCharacterCtorHelper(_In_ const size_t& len, _In_ const T& tc) noexcept
        {
            if (len == 0)
            {
                return true;
            }

            if (len >= ms_SSOCap)
            {
                if (!AllocateNewStringArray<false>(len))
                {
                    return false;
                }
            }

            if constexpr (std::is_same_v<T, char>)
            {
                memset(m_pStr, tc, len);
            }
            else if constexpr (std::is_same_v<T, wchar_t>)
            {
                wmemset(m_pStr, tc, len);
            }

            m_Len = len;
            m_pStr[m_Len] = ms_NullTerminator;

            return true;
        }

        // Returns true if the SSO array is in use, false if the dynamic array is in use.
        [[nodiscard]] inline bool IsUsingSSOArray() const noexcept
        {
            return m_pStr == m_SSOArr;
        }

        // Places null-terminator at beginning of SSO array.
        inline void ResetSSOArray() noexcept
        {
            m_SSOArr[0] = ms_NullTerminator;
        }

        // Deletes m_pStr.
        // Caution: Does not check if m_pStr is SSO array prior to delete.
        inline void DeleteStringPointer() noexcept
        {
            delete[] m_pStr;
        }

        // Deletes m_pStr if m_pStr is not pointing to the SSO array.
        inline void DeleteStringPointerIfNotSSO() noexcept
        {
            if (!IsUsingSSOArray())
            {
                DeleteStringPointer();
            }
        }

        // Deletes m_pStr and points it to SSO array.
        inline void ResetStringPointer() noexcept
        {
            DeleteStringPointer();
            m_pStr = m_SSOArr;
        }

        // Deletes m_pStr and points it to SSO array if m_pStr is not already pointing to SSO array.
        inline void ResetStringPointerIfNotSSO() noexcept
        {
            if (!IsUsingSSOArray())
            {
                ResetStringPointer();
                m_Cap = ms_SSOCap;
            }
        }


        // Private Helper Methods - Copy Helpers //

        // Copy raw pointer contents to string.
        // Returns true if copy succeeds, false otherwise.
        // Note: May allocate a new array or go back to using SSO array, depending on specified length.
        [[nodiscard]] _Success_(return) bool CopyFromRawPointer(_In_reads_opt_(len) const T* pSrc, _In_ const size_t& len) noexcept
        {
            if (!pSrc || (len == 0))
            {
                // Treat nullptr/zero-length string as signal to clear the string.
                Clear();
                return true;
            }

            if (pSrc == m_pStr)
            {
                // Avoid self-copy.
                return true;
            }

            if (len < ms_SSOCap)
            {
                // New copy can fit in SSO array.
                ResetStringPointerIfNotSSO();
            }
            else if (len >= m_Cap)
            {
                // New copy needs more space.
                if (!AllocateNewStringArray<false>(len))
                {
                    return false;
                }
            }

            m_Len = len;
            memcpy(m_pStr, pSrc, sizeof(T) * m_Len);
            m_pStr[m_Len] = ms_NullTerminator;

            return true;
        }

        // Copies string object to this string.
        // Returns true if copy succeeds, false otherwise.
        // Note: May allocate a new array or go back to using SSO array, depending on specified length.
        [[nodiscard]] _Success_(return) inline bool CopyFromStringObj(_In_ const String<T>& src) noexcept
        {
            // Avoid self-copy.
            return (this == &src) || CopyFromRawPointer(src.CStr(), src.Length());
        }


        // Moves string object to this string, if possible.
        // Returns true if move succeeds, false otherwise.
        // Note: If move cannot be done (e.g., source is using SSO array) then this will copy instead.
        [[nodiscard]] _Success_(return) bool MoveFromStringObj(_Inout_ String<T>& src) noexcept
        {
            if (this == &src)
            {
                // Avoid self-copy/move
                return true;
            }

            if (src.IsUsingSSOArray())
            {
                if (!CopyFromRawPointer(src.CStr(), src.Length()))
                {
                    return false;
                }
            }
            else
            {
                // Steal pointer.
                DeleteStringPointerIfNotSSO();
                m_pStr = src.CStr();
                m_Len = src.Length();
                m_Cap = src.Capacity();

                // this now owns the pointer - make sure src clearing doesn't delete it.
                src.m_pStr = src.m_SSOArr;
                src.m_Cap = ms_SSOCap;
            }

            src.Clear();

            return true;
        }


        // Private Helper Methods - Append Helpers //

        // Appends raw pointer contents to end of this string.
        // Returns true if append operation is successful, false otherwise.
        // Note: Will lead to new array allocation if not enough space is available.
        [[nodiscard]] _Success_(return) bool AppendUsingRawPointer(_In_reads_opt_(len) const T* pSrc, _In_ const size_t& len) noexcept
        {
            if (!pSrc || (len == 0))
            {
                // Nothing to append - exit early.
                return true;
            }

            if ((m_Len + len) >= m_Cap)
            {
                // See if we need to adjust pSrc after new allocation.
                bool bSelfAppend = (pSrc == m_pStr);

                // Need more room for append.
                if (!AllocateNewStringArray<true>(len))
                {
                    return false;
                }

                if (bSelfAppend)
                {
                    // Use new string pointer.
                    pSrc = m_pStr;
                }
            }

            memcpy(m_pStr + m_Len, pSrc, sizeof(T) * len);
            m_Len += len;
            m_pStr[m_Len] = ms_NullTerminator;

            return true;
        }

        // Append string object contents to end of this string.
        // Returns true if append operation is successful, false otherwise.
        // Note: Will lead to new array allocation if not enough space is available.
        [[nodiscard]] _Success_(return) inline bool AppendUsingStringObj(_In_ const String<T>& src) noexcept
        {
            return AppendUsingRawPointer(src.CStr(), src.Length());
        }

    public:

        // Public Static Data Members //

        static constexpr T ms_NullTerminator = static_cast<T>('\0');
        static constexpr size_t ms_InvalidPos = std::numeric_limits<size_t>::max();
        static constexpr size_t ms_InvalidLen = std::numeric_limits<size_t>::max();


        // Constructors //

        // Default constructor - string will be empty.
        String() noexcept :
            m_pStr(m_SSOArr),
            m_Len(0),
            m_Cap(ms_SSOCap)
        {
            ResetSSOArray();
        }

        // Length and character constructor
        // Note: If len exceeds size of SSO array, dynamic array will be used.
        String(_In_ const size_t& len, _In_ const T& tc = String<T>::ms_NullTerminator) noexcept :
            String()
        {
            if (!LengthCharacterCtorHelper(len, tc))
            {
                Clear();
            }
        }

        // C-string constructor
        // Note: If copy fails for any reason, this string will be cleared out.
        explicit String(_In_opt_z_ const T* const pCStr) noexcept :
            String()
        {
            if (!CopyFromRawPointer(pCStr, GetStringLength(pCStr)))
            {
                Clear();
            }
        }

        // Pointer-and-length constructor
        // Note: If copy fails for any reason, this string will be cleared out.
        String(_In_reads_opt_(len) const T* const pStr, _In_ const size_t& len) noexcept :
            String()
        {
            if (!CopyFromRawPointer(pStr, len))
            {
                Clear();
            }
        }

        // Copy constructor
        // Note: If copy fails for any reason, this string will be cleared out.
        String(_In_ const String<T>& src) noexcept :
            String()
        {
            if (!CopyFromStringObj(src))
            {
                Clear();
            }
        }

        // Move constructor
        // Note: If move fails for any reason, this string will be cleared out.
        String(_Inout_ String<T>&& src) noexcept :
            String()
        {
            if (!MoveFromStringObj(src))
            {
                Clear();
            }
        }


        // Destructor //

        ~String() noexcept
        {
            DeleteStringPointerIfNotSSO();
        }


        // Assignment Overloads //

        // Assigns C-string contents to this string.
        // Note: Clears string if assignment fails.
        String<T>& operator=(_In_opt_z_ const T* pCStr)
        {
            if (!CopyFromRawPointer(pCStr, GetStringLength(pCStr)))
            {
                Clear();
            }

            return *this;
        }

        // Assigns source string to this string via copy.
        String<T>& operator=(_In_ const String<T>& src) noexcept
        {
            // CopyFromStringObj performs self-copy check.
            if (!CopyFromStringObj(src))
            {
                Clear();
            }

            return *this;
        }

        // Assigns source string to this string via move.
        // Note: Source string will be cleared.
        String<T>& operator=(_Inout_ String<T>&& src) noexcept
        {
            // MoveFromStringObj performs self-copy/move check.
            if (!MoveFromStringObj(src))
            {
                Clear();
            }

            return *this;
        }


        // Operator Overloads //

        // Returns true if this string is not empty (length != 0).
        [[nodiscard]] explicit operator bool() const noexcept
        {
            return !IsEmpty();
        }

        // Appends C-string contents to this string.
        // Note: Clears string if append fails.
        String<T>& operator+=(_In_opt_z_ const T* pCStr)
        {
            if (!AppendUsingRawPointer(pCStr, GetStringLength(pCStr)))
            {
                Clear();
            }

            return *this;
        }

        // Appends source string contents to this string.
        // Note: Clears string if append fails.
        String<T>& operator+=(_In_ const String<T>& src)
        {
            if (!AppendUsingStringObj(src))
            {
                Clear();
            }

            return *this;
        }

        // Returns reference to character at specified position (mutable).
        // Note: Throws std::out_of_range if using idx would access memory beyond the length of the string.
        [[nodiscard]] T& operator[](_In_ const size_t& idx)
        {
            ValidateIndexT(__PRETTY_FUNCTION__, idx);
            return m_pStr[idx];
        }

        // Returns reference to character at specified position (immutable).
        // Note: Throws std::out_of_range if using idx would access memory beyond the length of the string.
        [[nodiscard]] const T& operator[](_In_ const size_t& idx) const
        {
            ValidateIndexT(__PRETTY_FUNCTION__, idx);
            return m_pStr[idx];
        }

        // Performs case-sensitive comparison between C-string and this string - returns true if contents match, false otherwise.
        [[nodiscard]] bool operator==(_In_opt_z_ const T* pCStr) const noexcept
        {
            const size_t len = GetStringLength(pCStr);
            if (!pCStr)
            {
                // Consider empty string and nullptr input to be equivalent.
                return IsEmpty();
            }

            if (len != Length())
            {
                // Equiv-op is strict - length mismatch means no match.
                return false;
            }

            return CompareStrings(*this, pCStr, len);
        }

        // Performs case-sensitive comparison between source string and this string - returns true if contents match, false otherwise.
        [[nodiscard]] bool operator==(_In_ const String<T>& other) const noexcept
        {
            return (m_Len == other.m_Len) && CompareStrings(*this, other);
        }


        // Getters //

        // Returns C-string pointer (mutable).
        [[nodiscard]] _Ret_z_ inline T* CStr() noexcept
        {
            return m_pStr;
        }

        // Returns C-string pointer (immutable).
        [[nodiscard]] _Ret_z_ inline const T* CStr() const noexcept
        {
            return m_pStr;
        }

        // Returns length of string.
        // Note: Does not include null-terminator.
        [[nodiscard]] inline const size_t& Length() const noexcept
        {
            return m_Len;
        }

        // Returns current capacity of string.
        // Note: Fixed to ms_SSOCap if dynamic array is not in use.
        [[nodiscard]] inline const size_t& Capacity() const noexcept
        {
            return m_Cap;
        }

        // Returns reference to first character of string (mutable).
        // Note: Throws std::out_of_range if string is empty.
        [[nodiscard]] inline T& Front()
        {
            ValidateIndexT(__PRETTY_FUNCTION__, 0);
            return m_pStr[0];
        }

        // Returns reference to first character of string (immutable).
        // Note: Throws std::out_of_range if string is empty.
        [[nodiscard]] inline const T& Front() const
        {
            ValidateIndexT(__PRETTY_FUNCTION__, 0);
            return m_pStr[0];
        }

        // Returns reference to last character of string (mutable).
        // Note: Throws std::out_of_range if string is empty.
        [[nodiscard]] inline T& Back()
        {
            ValidateIndexT(__PRETTY_FUNCTION__, IsEmpty() ? ms_InvalidPos : m_Len - 1);
            return m_pStr[m_Len - 1];
        }

        // Returns reference to last character of string (immutable).
        // Note: Throws std::out_of_range if string is empty.
        [[nodiscard]] inline const T& Back() const
        {
            ValidateIndexT(__PRETTY_FUNCTION__, IsEmpty() ? ms_InvalidPos : m_Len - 1);
            return m_pStr[m_Len - 1];
        }


        // Public Methods //


        // Assign //

        // Assigns a single character tc to this string.
        [[nodiscard]] _Success_(return) inline bool Assign(_In_ const T& tc) noexcept
        {
            return Assign(&tc, 1);
        }

        // Assigns C-string contents to this string (including null-terminator).
        [[nodiscard]] _Success_(return) inline bool Assign(_In_z_ const T* const pCStr) noexcept
        {
            return Assign(pCStr, GetStringLength(pCStr));
        }

        // Assigns pStr contents to this string.
        // Note: This will append a null-terminator after assigning pStr contents.
        [[nodiscard]] _Success_(return) inline bool Assign(_In_reads_(len) const T* const pStr, _In_ const size_t& len) noexcept
        {
            return CopyFromRawPointer(pStr, len);
        }

        // Assigns source string contents to this string.
        [[nodiscard]] _Success_(return) inline bool Assign(_In_ const String<T>& src) noexcept
        {
            return CopyFromStringObj(src);
        }

        // Assigns source string contents to this string, up to len characters.
        // Note: If len > src.Length(), this will correct to appending the entire str object.
        [[nodiscard]] _Success_(return) inline bool Assign(_In_ const String<T>& src, _In_ const size_t& len) noexcept
        {
            return CopyFromRawPointer(src.m_pStr, std::min<size_t>(src.m_Len, len));
        }

        // Assigns source string contents to this string, clears out the source string.
        [[nodiscard]] _Success_(return) inline bool Assign(_Inout_ String<T>&& src) noexcept
        {
            return MoveFromStringObj(src);
        }


        // Append //

        // Appends a single character tc to this string.
        [[nodiscard]] _Success_(return) inline bool Append(_In_ const T& tc) noexcept
        {
            return Append(&tc, 1);
        }

        // Appends C-string contents to this string (including null-terminator).
        [[nodiscard]] _Success_(return) inline bool Append(_In_z_ const T* const pCStr) noexcept
        {
            return Append(pCStr, GetStringLength(pCStr));
        }

        // Appends pStr contents to this string.
        // Note: This will append a null-terminator after assigning pStr contents.
        [[nodiscard]] _Success_(return) inline bool Append(_In_reads_(len) const T* const pCStr, _In_ const size_t& len) noexcept
        {
            return AppendUsingRawPointer(pCStr, len);
        }

        // Appends source string contents to this string.
        [[nodiscard]] _Success_(return) inline bool Append(_In_ const String<T>& src) noexcept
        {
            return AppendUsingStringObj(src);
        }

        // Appends source string contents to this string up to len characters.
        // Note: If len > src.Length(), this will correct to appending the entire str object.
        [[nodiscard]] _Success_(return) inline bool Append(_In_ const String<T>& src, _In_ const size_t& len) noexcept
        {
            return AppendUsingRawPointer(src.m_pStr, std::min<size_t>(len, src.m_Len));
        }


        // Compare //

        // Compares C-string contents to this string, using specified case-sensitivity option.
        [[nodiscard]] inline bool Compare(_In_z_ const T * const pCStr, _In_ const bool bCaseInsensitive = false) const noexcept
        {
            const size_t len = GetStringLength(pCStr);
            return (len != ms_InvalidLen) && (m_Len == len) && CompareStrings(*this, pCStr, len, bCaseInsensitive);
        }

        // Compares contents of pStr to this string, using specified case-sensitivity option.
        [[nodiscard]] inline bool Compare(_In_reads_(len) const T * const pStr, _In_ const size_t len, _In_ const bool bCaseInsensitive = false) const noexcept
        {
            return (m_Len >= len) && CompareStrings(*this, pStr, len, bCaseInsensitive);
        }

        // Compares contents of source string to this string, using specified case-sensitivity option.
        [[nodiscard]] inline bool Compare(_In_ const String<T> & rhs, _In_ const bool bCaseInsensitive = false) const noexcept
        {
            return (m_Len == rhs.m_Len) && CompareStrings(*this, rhs, bCaseInsensitive);
        }

        // Compares contents of source string to this string, up to len characters and using specified case-sensitivity option.
        [[nodiscard]] inline bool Compare(_In_ const String<T> & rhs, _In_ const size_t len, _In_ const bool bCaseInsensitive = false) const noexcept
        {
            const size_t rhsLen = std::min(rhs.m_Len, len);
            return (m_Len >= rhsLen) && CompareStrings(*this, rhs.m_pStr, rhsLen, bCaseInsensitive);
        }


        // Misc //

        // Resets the string to default state (empty).
        inline void Clear() noexcept
        {
            ResetStringPointerIfNotSSO();
            ResetSSOArray();
            m_Len = 0;
        }

        // Returns true if string is currently empty (length is 0), false otherwise.
        [[nodiscard]] inline bool IsEmpty() const noexcept
        {
            return m_Len == 0;
        }
    };

    // Aliases for supported string types.
    using StringUTF8 = String<char>;
    using StringUTF16 = String<wchar_t>;
}