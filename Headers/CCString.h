#pragma once

// CC
#include "CCDynamicBuffer.h"
#include "CCMacros.h"
#include "CCPointerHelpers.h"

// STL
#include <stdexcept>


namespace CC
{
    template <typename T, CC_ENABLE_IF_SUPPORTED_CHARACTER_TYPE(T)>
    class [[nodiscard]] String
    {
        // Testing class.
        friend class StringTests;

        // Type aliases.
        using PH = PointerHelpers;

    private:

        /// Private Static Data Members \\\

        static constexpr T ms_NullTerminator = static_cast<T>('\0');
        static constexpr size_t ms_InvalidPos = std::numeric_limits<size_t>::max( );
        static constexpr size_t ms_InvalidLen = std::numeric_limits<size_t>::max( );
        static constexpr size_t ms_SSOCap = 32;

        /// Private Data Members \\\

        DynamicBuffer<T> m_DynBuf;
        T m_SSOArr[ms_SSOCap];
        size_t m_SSOPos;

        /// Private Throwing Validators \\\

        // Throws std::out_of_range if using idx would access memory beyond the length of the string.
        void ValidateIndexT(_In_z_ const char* const f, _In_ const size_t& idx) const
        {
            const size_t& len = Length( );
            if ( idx >= len )
            {
                static const std::string msg1 = ": Index[";
                static const std::string msg2 = "] exceeds length of string[";
                static const std::string msg3 = "].";
                const std::string idxStr = std::to_string(idx);
                const std::string endStr = std::to_string(len);

                throw std::out_of_range(f + msg1 + idxStr + msg2 + endStr + msg3);
            }
        }

        /// Private Static Validators \\\

        // Returns false if p is nullptr, true otherwise.
        [[nodiscard]] static bool ValidatePointer(_In_opt_ const T* const p) noexcept
        {
            return !!p;
        }

        // Returns false if len is ms_InvalidLen, true otherwise.
        [[nodiscard]] static bool ValidateLength(_In_ const size_t& len) noexcept
        {
            return (len != ms_InvalidLen);
        }

        // Returns false if p is nullptr or len is ms_InvalidLen, true otherwise.
        [[nodiscard]] static bool ValidatePointerAndLength(_In_opt_ const T* const p, _In_ const size_t& len) noexcept
        {
            return ValidatePointer(p) && ValidateLength(len);
        }

        /// Static Private Helper Methods - Copy Helpers \\\

        // Copies pStr contents to beginning of SSO array.
        // Note: This method assumes that the caller has already determined that the copy will fit in the SSO array.
        [[nodiscard]] _Success_(return) static bool CopyRawPointerToSSOArray(_Out_ String<T>& dst, _In_reads_(len) const T* const pStr, _In_ const size_t& len) noexcept
        {
            dst.Clear( );
            memcpy(dst.m_SSOArr, pStr, sizeof(T) * len);
            dst.m_SSOPos = len;
            dst.m_SSOArr[dst.m_SSOPos] = ms_NullTerminator;
            return true;
        }

        // Resets dst's SSO array, creates a dynamic buffer for dst and copies pStr contents into the buffer.
        [[nodiscard]] _Success_(return) static bool CopyRawPointerToDynamicBuffer(_Out_ String<T>& dst, _In_reads_(len) const T* const pStr, _In_ const size_t& len) noexcept
        {
            dst.ResetSSOArray( );
            dst.m_DynBuf = DynamicBuffer<T>(len + 1);
            return !!dst.m_DynBuf
                && dst.m_DynBuf.Write(pStr, len)
                && dst.m_DynBuf.Write(ms_NullTerminator)
                && dst.DecrementDynBufWritePos( );
        }

        /// Static Private Helper Methods - Append Helpers \\\

        // Appends pStr contents to end of current SSO array position.
        // Note: This method assumes that the caller has already determined that the append will fit in the SSO array.
        [[nodiscard]] _Success_(return) static bool AppendRawPointerToSSOArray(_Inout_ String<T>& dst, _In_reads_(len) const T* const pStr, _In_ const size_t& len) noexcept
        {
            memcpy(dst.m_SSOArr + dst.m_SSOPos, pStr, sizeof(T) * len);
            dst.m_SSOPos += len;
            dst.m_SSOArr[dst.m_SSOPos] = ms_NullTerminator;
            return true;
        }

        // Appends pStr contents to end of the dynamic buffer.
        [[nodiscard]] _Success_(return) static bool AppendRawPointerToDynamicBuffer(_Inout_ String<T>& dst, _In_reads_(len) const T* const pStr, _In_ const size_t& len) noexcept
        {
            return dst.m_DynBuf.Write(pStr, len)
                && dst.m_DynBuf.Write(ms_NullTerminator)
                && dst.DecrementDynBufWritePos( );
        }

        // Copies both SSO array and pStr contents to dynamic buffer.
        // Note: This will overwrite any previous dynamic buffer contents.
        [[nodiscard]] _Success_(return) static bool CopySSOArrayAndAppendRawPointerToDynamicBuffer(_Inout_ String<T>& dst, _In_reads_(len) const T* const pStr, _In_ const size_t& len) noexcept
        {
            dst.m_DynBuf = DynamicBuffer<T>(dst.m_SSOPos + len + 1);
            return !!dst.m_DynBuf
                && dst.CopySSOArrayToDynamicBuffer( )
                && AppendRawPointerToDynamicBuffer(dst, pStr, len);
        }

        /// Static Private Helper Methods \\\

        // Returns true if tc is ms_NullTerminator, false otherwise.
        [[nodiscard]] static bool IsNullTerminator(_In_ const T& tc) noexcept
        {
            return (tc == ms_NullTerminator) ? true : false;
        }

        // Returns length of C-string argument, which does not include the null-terminator.
        // Note: If pCStr is nullptr, this returns ms_InvalidLen.
        [[nodiscard]] _Success_(return != ms_InvalidPos) static size_t GetStringLength(_In_opt_z_ const T* pCStr) noexcept
        {
            size_t idx = 0;
            if ( !pCStr )
            {
                return ms_InvalidLen;
            }

            while ( pCStr[idx] != ms_NullTerminator )
            {
                idx++;
            }

            return idx;
        }

        // Copies src string contents to dst.
        [[nodiscard]] _Success_(return) static bool CopyFromStringObj(_Out_ String<T>& dst, _In_ const String<T>& src) noexcept
        {
            bool bRet = true;

            dst.Clear( );

            if ( src.IsUsingSSOArray( ) )
            {
                dst.m_DynBuf.Free( );
                memcpy(dst.m_SSOArr, src.m_SSOArr, sizeof(T) * (src.m_SSOPos + 1));
                dst.m_SSOPos = src.m_SSOPos;
            }
            else
            {
                dst.m_DynBuf = src.m_DynBuf;
                bRet = !!dst.m_DynBuf;
            }

            return bRet;
        }

        // Moves src string contents to dst.
        // Note: This clears out src.
        [[nodiscard]] _Success_(return) static bool MoveFromStringObj(_Out_ String<T>& dst, _Inout_ String<T>&& src) noexcept
        {
            dst.Clear( );

            if ( src.IsUsingSSOArray( ) )
            {
                dst.m_DynBuf.Free( );
                memcpy(dst.m_SSOArr, src.m_SSOArr, sizeof(T) * (src.m_SSOPos + 1));
                dst.m_SSOPos = src.m_SSOPos;
            }
            else
            {
                dst.m_DynBuf = std::move(src.m_DynBuf);
            }

            src.Clear( );

            return true;
        }

        // Copies contents of pStr to dst.
        [[nodiscard]] _Success_(return) static bool CopyFromRawPointer(_Inout_ String<T>& dst, _In_reads_opt_(len) const T* const pStr, _In_ const size_t& len) noexcept
        {
            if ( !pStr || (len == 0))
            {
                dst.Clear( );
                return true;
            }

            return (len < ms_SSOCap) ? CopyRawPointerToSSOArray(dst, pStr, len) : CopyRawPointerToDynamicBuffer(dst, pStr, len);
        }

        // Appends pStr contents to dst.
        // Note: If append would write past the end of the SSO array, dst allocates a dynamic buffer, copies SSO array to new buffer, and then appends pStr contents.
        [[nodiscard]] _Success_(return) static bool AppendUsingRawPointer(_Inout_ String<T>& dst, _In_reads_opt_(len) const T* const pStr, _In_ const size_t& len) noexcept
        {
            bool bRet = true;
            if ( !pStr || (len == 0))
            {
                return true;
            }

            if ( dst.IsUsingSSOArray( ) )
            {
                if ( (dst.m_SSOPos + len) < ms_SSOCap )
                {
                    bRet = AppendRawPointerToSSOArray(dst, pStr, len);
                }
                else
                {
                    bRet = CopySSOArrayAndAppendRawPointerToDynamicBuffer(dst, pStr, len);
                }
            }
            else
            {
                bRet = AppendRawPointerToDynamicBuffer(dst, pStr, len);
            }

            return bRet;
        }

        // Appends src contents to end of dst.
        // Returns true if the append operation is successful, false otherwise.
        [[nodiscard]] _Success_(return) static bool AppendUsingStringObj(_Inout_ String<T>& dst, _In_ const String<T>& src) noexcept
        {
            return AppendUsingRawPointer(dst, src.CStr( ), src.Length( ));
        }

        // Compares the two String objects, returns true
        template <bool bCI = false>
        [[nodiscard]] static bool CompareStrings(_In_ const String<T>& lhs, _In_ const String<T>& rhs) noexcept
        {
            return CompareStrings<bCI>(lhs.CStr( ), lhs.Length( ), rhs.CStr( ), rhs.Length( ));
        }

        template <bool bCI = false>
        [[nodiscard]] static bool CompareStrings(_In_ const String<T>& lhs, _In_reads_(rhsLen) const T* const pRhs, _In_ const size_t& rhsLen) noexcept
        {
            return CompareStrings<bCI>(lhs.CStr( ), lhs.Length( ), pRhs, rhsLen);
        }

        template <bool bCI = false>
        [[nodiscard]] static bool CompareStrings(_In_reads_(lhsLen) const T* const pLhs, _In_ const size_t& lhsLen, _In_ const String<T>& rhs) noexcept
        {
            return CompareStrings<bCI>(pLhs, lhsLen, rhs.CStr( ), rhs.Length( ));
        }

        template <bool bCI = false>
        [[nodiscard]] static bool CompareStrings(_In_reads_(lhsLen) const T* const pLhs, _In_ const size_t& lhsLen, _In_reads_(rhsLen) const T* const pRhs, _In_ const size_t& rhsLen) noexcept
        {
            size_t compLen = std::min(lhsLen, rhsLen);

            if ( pLhs == pRhs )
            {
                return true;
            }

            if ( !pLhs || !pRhs )
            {
                return false;
            }

            if constexpr ( !bCI )
            {
                return memcmp(pLhs, pRhs, sizeof(T) * compLen) == 0;
            }
            else
            {
                if constexpr ( std::is_same_v<T, utf8> )
                {
                    return _strnicmp(pLhs, pRhs, compLen) == 0;
                }
                else if constexpr ( std::is_same_v<T, utf16> )
                {
                    return _wcsnicmp(pLhs, pRhs, compLen) == 0;
                }
            }
        }

        /// Private Helper Methods \\\

        // Returns true if copying len tc's is successful, false otherwise.
        [[nodiscard]] _Success_(return) bool LengthCharacterCtorHelper(_In_ const size_t& len, _In_ const T& tc) noexcept
        {
            if ( len == 0 )
            {
                return true;
            }

            if ( len < ms_SSOCap )
            {
                if constexpr ( std::is_same_v<T, utf8> )
                {
                    memset(m_SSOArr, tc, len);
                }
                else if constexpr ( std::is_same_v<T, utf16> )
                {
                    wmemset(m_SSOArr, tc, len);
                }

                m_SSOPos = len;
                m_SSOArr[m_SSOPos] = ms_NullTerminator;

                return true;
            }
            else
            {
                m_DynBuf = DynamicBuffer<T>(len + 1);
                if ( !m_DynBuf )
                {
                    return false;
                }

                for ( size_t i = 0; i < len; i++ )
                {
                    if ( !m_DynBuf.Write(tc) )
                    {
                        return false;
                    }
                }

                return m_DynBuf.Write(ms_NullTerminator) && DecrementDynBufWritePos( );
            }
        }

        // Copies SSO array contents to dynamic buffer
        // Note: Intended to be used as a supporting function for append operations when SSO array no longer has room.
        // Note: Does not copy null-terminator.
        // Note: Assumes caller has already primed the dynamic buffer for the copy (buffer is not written to and pre-allocated).
        [[nodiscard]] _Success_(return) bool CopySSOArrayToDynamicBuffer( ) noexcept
        {
            if ( m_SSOPos == 0 )
            {
                return true;
            }

            if ( !m_DynBuf.Write(m_SSOArr, m_SSOPos) )
            {
                return false;
            }

            return true;
        }

        // Returns true if the SSO array is in use, false if the dynamic buffer is in use.
        [[nodiscard]] bool IsUsingSSOArray( ) const noexcept
        {
            return !m_DynBuf;
        }

        // Returns true if decrementing the write position is successful, false otherwise.
        [[nodiscard]] _Success_(return) bool DecrementDynBufWritePos( ) noexcept
        {
            return m_DynBuf.SetWritePosition(m_DynBuf.WritePosition( ) - 1);
        }

        void ResetSSOArray( ) noexcept
        {
            m_SSOArr[0] = ms_NullTerminator;
            m_SSOPos = 0;
        }

    public:

        /// Constructors \\\

        // Default constructor - string will be empty.
        String( ) noexcept
        {
            ResetSSOArray( );
        }

        // Length and character constructor
        // Note: If len exceeds size of SSO array, dynamic buffer will be used.
        String(_In_ const size_t& len, _In_ const T& tc) noexcept :
            String( )
        {
            if ( !LengthCharacterCtorHelper(len, tc) )
            {
                Clear( );
            }
        }

        // C-string constructor
        // Note: If copy fails for any reason, this string will be cleared out. 
        explicit String(_In_opt_z_ const T* const pCStr) noexcept :
            String( )
        {
            if ( !CopyFromRawPointer(*this, pCStr, GetStringLength(pCStr)) )
            {
                Clear( );
            }
        }

        // Pointer-and-length constructor
        // Note: If copy fails for any reason, this string will be cleared out.
        String(_In_reads_opt_(len) const T* const pStr, _In_ const size_t& len) noexcept :
            String( )
        {
            if ( !CopyFromRawPointer(*this, pStr, len) )
            {
                Clear( );
            }
        }

        // Copy constructor
        // Note: If copy fails for any reason, this string will be cleared out.
        String(_In_ const String<T>& src) noexcept :
            String( )
        {
            if ( !CopyFromStringObj(*this, src) )
            {
                Clear( );
            }
        }

        // Move constructor
        // Note: If move fails for any reason, this string will be cleared out.
        String(_Inout_ String<T>&& src) noexcept :
            String( )
        {
            if ( !MoveFromStringObj(*this, std::move(src)) )
            {
                Clear( );
            }
        }

        /// Destructor \\\

        ~String( ) noexcept = default;

        /// Assignment Overloads \\\
                
        // Assigns C-string contents to this string.
        // Note: Clears string if assignment fails.
        String<T>& operator=(_In_opt_z_ const T* pCStr)
        {
            if ( (pCStr != CStr( )) && (!pCStr || !CopyFromRawPointer(*this, pCStr, GetStringLength(pCStr))) )
            {
                Clear( );
            }

            return *this;
        }

        // Assigns source string to this string via copy.
        String<T>& operator=(_In_ const String<T>& src) noexcept
        {
            if ( this != &src )
            {
                if ( !CopyFromStringObj(*this, src) )
                {
                    Clear( );
                }
            }

            return *this;
        }

        // Assigns source string to this string via move.
        // Note: Source string will be cleared.
        String<T>& operator=(_Inout_ String<T>&& src) noexcept
        {
            if ( this != &src )
            {
                if ( !MoveFromStringObj(*this, std::move(src)) )
                {
                    Clear( );
                }
            }

            return *this;
        }

        /// Operator Overloads \\\

        // Returns true if this string is not empty (length != 0).
        [[nodiscard]] explicit operator bool( ) const noexcept
        {
            return !IsEmpty( );
        }

        // Appends C-string contents to this string.
        // Note: Clears string if append fails.
        String<T>& operator+=(_In_z_ const T* pCStr)
        {
            if ( !AppendUsingRawPointer(*this, pCStr, GetStringLength(pCStr)) )
            {
                Clear( );
            }

            return *this;
        }

        // Appends source string contents to this string.
        // Note: Clears string if append fails.
        String<T>& operator+=(_In_ const String<T>& src)
        {
            if ( !AppendUsingStringObj(*this, src) )
            {
                Clear( );
            }

            return *this;
        }

        // Returns reference to character at specified position (mutable).
        // Note: Throws std::out_of_range if using idx would access memory beyond the length of the string.
        [[nodiscard]] T& operator[](_In_ const size_t& idx)
        {
            ValidateIndexT(__FUNCSIG__, idx);
            return CStr( )[idx];
        }

        // Returns reference to character at specified position (immutable).
        // Note: Throws std::out_of_range if using idx would access memory beyond the length of the string.
        [[nodiscard]] const T& operator[](_In_ const size_t& idx) const
        {
            ValidateIndexT(__FUNCSIG__, idx);
            return CStr( )[idx];
        }

        // Performs case-sensitive comparison between C-string and this string - returns true if contents match, false otherwise.
        [[nodiscard]] bool operator==(_In_opt_z_ const T* pCStr) const noexcept
        {
            size_t len = GetStringLength(pCStr);
            if ( !pCStr || (len != Length( )) )
            {
                return false;
            }

            return CompareStrings(*this, pCStr, len);
        }

        // Performs case-sensitive comparison between source string and this string - returns true if contents match, false otherwise.
        [[nodiscard]] bool operator==(_In_ const String<T>& other) const noexcept
        {
            return (Length() == other.Length()) && CompareStrings(*this, other);
        }

        /// Getters \\\

        // Returns C-string pointer (mutable).
        [[nodiscard]] _Ret_z_ T* CStr( ) noexcept
        {
            return IsUsingSSOArray( ) ? m_SSOArr : m_DynBuf.Get( );
        }

        // Returns C-string pointer (immutable).
        [[nodiscard]] _Ret_z_ const T* CStr( ) const noexcept
        {
            return IsUsingSSOArray( ) ? m_SSOArr : m_DynBuf.Get( );
        }

        // Returns length of string.
        // Note: Does not include null-terminator.
        [[nodiscard]] const size_t& Length( ) const noexcept
        {
            return IsUsingSSOArray( ) ? m_SSOPos : m_DynBuf.WritePosition( );
        }

        // Returns current capacity of string.
        // Note: Fixed to ms_SSOCap if dynamic buffer is not in use.
        [[nodiscard]] const size_t& Capacity( ) const noexcept
        {
            return IsUsingSSOArray( ) ? ms_SSOCap : m_DynBuf.Length( );
        }

        // Returns reference to first character of string (mutable).
        // Note: Throws std::out_of_range if string is empty.
        [[nodiscard]] T& Front( )
        {
            ValidateIndexT(__FUNCSIG__, 0);
            return CStr( )[0];
        }

        // Returns reference to first character of string (immutable).
        // Note: Throws std::out_of_range if string is empty.
        [[nodiscard]] const T& Front( ) const
        {
            ValidateIndexT(__FUNCSIG__, 0);
            return CStr( )[0];
        }

        // Returns reference to last character of string (mutable).
        // Note: Throws std::out_of_range if string is empty.
        [[nodiscard]] T& Back( )
        {
            ValidateIndexT(__FUNCSIG__, IsEmpty( ) ? ms_InvalidPos : Length( ) - 1);
            return CStr( )[Length( ) - 1];
        }

        // Returns reference to last character of string (immutable).
        // Note: Throws std::out_of_range if string is empty.
        [[nodiscard]] const T& Back( ) const
        {
            ValidateIndexT(__FUNCSIG__, IsEmpty( ) ? ms_InvalidPos : Length( ) - 1);
            return CStr( )[Length( ) - 1];
        }

        /// Public Methods \\\


        /// Assign \\\

        // Assigns a single character tc to this string.
        [[nodiscard]] _Success_(return) bool Assign(_In_ const T& tc) noexcept
        {
            return Assign(&tc, 1);
        }

        // Assigns C-string contents to this string (including null-terminator).
        [[nodiscard]] _Success_(return) bool Assign(_In_z_ const T* const pCStr) noexcept
        {
            return Assign(pCStr, GetStringLength(pCStr));
        }

        // Assigns pStr contents to this string.
        // Note: This will append a null-terminator after assigning pStr contents.
        [[nodiscard]] _Success_(return) bool Assign(_In_reads_(len) const T* const pStr, _In_ const size_t& len) noexcept
        {
            return CopyFromRawPointer(*this, pStr, len);
        }

        // Assigns source string contents to this string.
        [[nodiscard]] _Success_(return) bool Assign(_In_ const String<T>& src) noexcept
        {
            return CopyFromStringObj(*this, src);
        }

		// Assigns source string contents to this string, up to len characters.
		// Note: If len > src.Length(), this will correct to appending the entire str object.
		[[nodiscard]] _Success_(return) bool Assign(_In_ const String<T>& src, _In_ const size_t& len) noexcept
		{
			return CopyFromRawPointer(*this, src.CStr(), std::min<size_t>(src.Length(), len));
		}

        // Assigns source string contents to this string, clears out the source string.
        [[nodiscard]] _Success_(return) bool Assign(_Inout_ String<T>&& src) noexcept
        {
            return MoveFromStringObj(*this, std::move(src));
        }


        /// Append \\\

        // Appends a single character tc to this string.
        [[nodiscard]] _Success_(return) bool Append(_In_ const T& tc) noexcept
        {
            return Append(&tc, 1);
        }

        // Appends C-string contents to this string (including null-terminator).
        [[nodiscard]] _Success_(return) bool Append(_In_z_ const T* const pCStr) noexcept
        {
            return Append(pCStr, GetStringLength(pCStr));
        }

        // Appends pStr contents to this string.
        // Note: This will append a null-terminator after assigning pStr contents.
        [[nodiscard]] _Success_(return) bool Append(_In_reads_(len) const T* const pCStr, _In_ const size_t& len) noexcept
        {
            return AppendUsingRawPointer(*this, pCStr, len);
        }

        // Appends source string contents to this string.
        [[nodiscard]] _Success_(return) bool Append(_In_ const String<T>& src) noexcept
        {
            return AppendUsingStringObj(*this, src);
        }

		// Appends source string contents to this string up to len characters.
		// Note: If len > src.Length(), this will correct to appending the entire str object.
		[[nodiscard]] _Success_(return) bool Append(_In_ const String<T>& src, _In_ const size_t& len) noexcept
		{
			return AppendUsingRawPointer(*this, src.CStr(), std::min<size_t>(len, src.Length()));
		}


        /// Compare \\\

        // Compares C-string contents to this string, using specified case-sensitivity option.
        template <bool bCaseInsensitive = false>
        [[nodiscard]] bool Compare(_In_z_ const T* const pCStr) const noexcept
        {
            const size_t len = GetStringLength(pCStr);
            return (len != ms_InvalidLen) && (Length() == len) && CompareStrings<bCaseInsensitive>(*this, pCStr, len);
        }

        // Compares contents of pStr to this string, using specified case-sensitivity option.
        template <bool bCaseInsensitive = false>
        [[nodiscard]] bool Compare(_In_reads_(len) const T* const pStr, _In_ const size_t& len) const noexcept
        {
            return (Length() >= len) && CompareStrings<bCaseInsensitive>(*this, pStr, len);
        }

        // Compares contents of source string to this string, using specified case-sensitivity option.
        template <bool bCaseInsensitive = false>
        [[nodiscard]] bool Compare(_In_ const String<T>& rhs) const noexcept
        {
            return (Length() == rhs.Length()) && CompareStrings<bCaseInsensitive>(*this, rhs);
        }

		// Compares contents of source string to this string, up to len characters and using specified case-sensitivity option.
		template <bool bCaseInsensitive = false>
		[[nodiscard]] bool Compare(_In_ const String<T>& rhs, _In_ const size_t& len) const noexcept
		{
			const size_t& rhsLen = std::min(rhs.Length(), len);
			return (Length() >= rhsLen) && CompareStrings<bCaseInsensitive>(*this, rhs.CStr(), rhsLen);
		}


        /// Misc \\\

        // Resets the string to default state (empty).
        void Clear( ) noexcept
        {
            m_DynBuf.Free( );
            ResetSSOArray( );
        }

        // Returns true if string is currently empty (length is 0), false otherwise.
        [[nodiscard]] bool IsEmpty( ) noexcept
        {
            return Length( ) == 0;
        }
    };

	// Aliases for supported string types.
	using StringUTF8 = String<utf8>;
	using StringUTF16 = String<utf16>;
}