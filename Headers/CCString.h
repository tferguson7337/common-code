#pragma once

// CC
#include "CCDynamicBuffer.h"
#include "CCMacros.h"

// STL
#include <stdexcept>


namespace CC
{
    template <typename T, CC_ENABLE_IF_SUPPORTED_CHARACTER_TYPE(T)>
    class [[nodiscard]] String
    {
        // Testing class.
        friend class StringTests;

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

        void ValidateIndexT(_In_z_ const T* const f, _In_ const size_t& idx) const
        {
            const size_t& len = Length( );
            if ( idx >= len )
            {
                static const std::string msg1 = ": Index[";
                static const std::string msg2 = "] exceeds length of buffer[";
                static const std::string msg3 = "].";
                const std::string idxStr = std::to_string(idx);
                const std::string endStr = std::to_string(len);

                throw std::out_of_range(f + msg1 + idxStr + msg2 + endStr + msg3);
            }
        }

        /// Private Static Throwing Validators \\\

        static void ValidatePointerT(_In_z_ const T* const f, _In_opt_ const T* p)
        {
            if ( !p )
            {
                static const std::string msg = ": Received invalid nullptr argument.";
                throw std::invalid_argument(f + msg);
            }
        }

        /// Private Static Validators \\\

        [[nodiscard]] static bool ValidatePointer(_In_opt_ const T* const pStr) noexcept
        {
            return !!pStr;
        }

        [[nodiscard]] static bool ValidateLength(_In_ const size_t& len) noexcept
        {
            return (len != ms_InvalidLen);
        }

        [[nodiscard]] static bool ValidatePointerAndLength(_In_opt_ const T* const pStr, _In_ const size_t& len) noexcept
        {
            return ValidatePointer(pStr) && ValidateLength(len);
        }

        /// Static Private Helper Methods \\\

        [[nodiscard]] bool IsNullTerminator(_In_ const T& tc) noexcept
        {
            return (tc == ms_NullTerminator) ? true : false;
        }

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

        static void CopyFromStringObj(_Inout_ String<T>& dst, _In_ const String<T>& src) noexcept
        {
            if ( src.IsUsingSSOArray(src) )
            {
                dst.m_DynBuf.Free( );
                memcpy(dst.m_SSOArr, src.m_SSOArr, sizeof(T) * (src.m_SSOPos + 1));
                dst.m_SSOPos = src.m_SSOPos;
            }
            else
            {
                dst.m_DynBuf = src.m_DynBuf;
            }
        }

        static void MoveFromStringObj(_Inout_ String<T>& dst, _Inout_ String<T>&& src) noexcept
        {
            if ( src.IsUsingSSOArray(src) )
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
        }

        [[nodiscard]] static bool CopyFromRawPointer(_Inout_ String<T>& dst, _In_reads_(len) const T* const pStr, _In_ const size_t& len) noexcept
        {
            bool bRet = true;
            if ( !ValidatePointerAndLength(pStr, len) )
            {
                return false;
            }

            if ( len < ms_SSOCap )
            {
                // We can fit pStr and null-terminator in SSO array.
                dst.m_DynBuf.Free( );
                memcpy(dst.m_SSOArr, pStr, sizeof(T) * len);
                dst.m_SSOPos = len;
                dst.m_SSOArr[dst.m_SSOPos] = ms_NullTerminator;
            }
            else
            {
                // SSO array won't fit whole string and null-terminator - use dynamic buffer.
                bRet = bRet && dst.m_DynBuf.Write(pStr, len);
                bRet = bRet && dst.m_DynBuf.Write(ms_NullTerminator);
                bRet = bRet && dst.DecrementDynBufWritePos( );
            }

            return bRet;
        }

        [[nodiscard]] static bool AppendUsingRawPointer(_Inout_ String<T>& dst, _In_reads_(len) const T* const pStr, _In_ const size_t len) noexcept
        {
            bool bRet = true;
            size_t newPos = ms_InvalidPos;
            if ( !ValidatePointerAndLength(pStr, len) )
            {
                return false;
            }

            newPos = m_SSOPos + len;
            if ( dst.IsUsingSSOArray( ) )
            {
                if ( dst.newSSOPos < ms_SSOCap )
                {
                    memcpy(&dst.m_SSOArr[dst.m_SSOPos], pStr, len);
                    dst.m_SSOPos = newPos;
                    dst.m_SSOArr[dst.m_SSOPos] = ms_NullTerminator;
                }
                else
                {
                    dst.m_DynBuf = DynamicBuffer<T>(newPos);
                    bRet = dst.m_DynBuf;
                    bRet = bRet && dst.m_DynBuf.Write(dst.m_SSOArr, dst.m_SSOPos);
                    bRet = bRet && dst.m_DynBuf.Write(pStr, len);
                    bRet = bRet && dst.m_DynBuf.Write(ms_NullTerminator);
                    bRet = bRet && dst.DecrementDynBufWritePos( );
                }
            }
            else
            {
                bRet = bRet && dst.m_DynBuf.Write(pStr, len);
                bRet = bRet && dst.m_DynBuf.Write(ms_NullTerminator);
                bRet = bRet && dst.DecrementDynBufWritePos( );
            }

            return bRet;
        }

        [[nodiscard]] static bool AppendUsingStringObj(_Inout_ String<T>& dst, _In_ const String<T>& src) noexcept
        {
            return AppendUsingRawPointer(dst, src.CStr( ), src.Length( ));
        }

        [[nodiscard]] static bool CompareStrings(_In_ const String<T>& lhs, _In_ const String<T>& rhs, _In_ const bool& bCI = false) noexcept
        {
            return (lhs.Length( ) == rhs.Length( )) && CompareStrings(lhs.CStr( ), lhs.Length( ), rhs.CStr( ), lhs.Length( ), bCI);
        }

        [[nodiscard]] static bool CompareStrings(_In_ const String<T>& lhs, _In_ const T* const pRhs, _In_ const size_t& rhsLen, _In_ const bool& bCI = false) noexcept
        {
            return CompareStrings(lhs.CStr( ), lhs.Length( ), pRhs, rhsLen, bCI);
        }

        [[nodiscard]] static bool CompareStrings(_In_ const T* const pLhs, _In_ const size_t& lhsLen, _In_ const String<T>& rhs, _In_ const bool& bCI = false) noexcept
        {
            return CompareStrings(pLhs, lhsLen, rhs.CStr( ), lhs.Length( ), bCI);
        }

        [[nodiscard]] static bool CompareStrings(_In_ const T* const pLhs, _In_ const size_t& lhsLen, _In_ const T* const pRhs, _In_ const size_t& rhsLen, _In_ const bool& bCI = false) noexcept
        {
            if ( pLhs == pRhs )
            {
                return true;
            }

            if ( !pLhs || !pRhs )
            {
                return false;
            }

            if ( !bCI )
            {
                return memcmp(pLhs, pRhs, sizeof(T) * lhsLen) == 0;
            }
            else
            {
                if constexpr ( std::is_same_v<T, utf8> )
                {
                    return _strnicmp(pLhs, pRhs, lhsLen) == 0;
                }
                else if constexpr ( std::is_same_v<T, utf16> )
                {
                    return _wcsnicmp(pLhs, pRhs, lhsLen) == 0;
                }
                else
                {
                    static_assert(false, __FUNCSIG__": Unsupported character type.");
                }
            }
        }

        /// Private Helper Methods \\\

        [[nodiscard]] bool IsUsingSSOArray( ) const noexcept
        {
            return !m_DynBuf;
        }

        [[nodiscard]] _Success_(return) bool DecrementDynBufWritePos( ) noexcept
        {
            return m_DynBuf.SetWritePosition(m_DynBuf.WritePosition( ) - 1);
        }

    public:

        /// Constructors \\\

        String( ) noexcept :
            m_SSOPos(0)
        {
            m_SSOArr[0] = ms_NullTerminator;
        }

        String(_In_ const size_t& len) noexcept :
            String( )
        {
            if ( len >= ms_SSOCap )
            {
                m_DynBuf = DynamicBuffer<T>(len);
            }
        }

        String(_In_z_ const T* const pCStr) noexcept :
            String( )
        {
            if ( !CopyFromRawPointer(pCStr, GetStringLength(pCStr)) )
            {
                Clear( );
            }
        }

        String(_In_reads_(len) const T* const pStr, _In_ size_t len) noexcept :
            String( )
        {
            if ( !CopyFromRawPointer(pStr, len) )
            {
                Clear( );
            }
        }

        String(_In_ const String<T>& src) noexcept :
            String( )
        {
            if ( !CopyStringObj(*this, src) )
            {
                Clear( );
            }
        }

        String(_Inout_ String<T>&& src) noexcept :
            String( )
        {
            if ( !MoveStringObj(*this, std::move(src)) )
            {
                Clear( );
            }
        }

        /// Destructor \\\

        ~String( ) noexcept = default;

        /// Assignment Overloads \\\

        String<T>& operator=(_In_ const String<T>& src) noexcept
        {
            if ( this != &src )
            {
                CopyFromStringObj(*this, src);
            }

            return *this;
        }

        String<T>& operator=(_Inout_ String<T>&& src) noexcept
        {
            if ( this != &src )
            {
                MoveFromStringObj(*this, src);
            }

            return *this;
        }

        String<T>& operator=(_In_z_ const T* pCStr)
        {
            ValidatePointerT(__FUNCSIG__, pCStr);
            if ( !CopyFromRawPointer(*this, pCStr, GetStringLength(pCStr)) )
            {
                throw std::exception(__FUNCSIG__": Failed to assign using C-string.");
            }

            return *this;
        }

        /// Operator Overloads \\\

        [[nodiscard]] explicit operator bool( ) const noexcept
        {
            return IsEmpty( );
        }

        [[nodiscard]] _Ret_z_ operator T*() noexcept
        {
            return CStr( );
        }

        [[nodiscard]] _Ret_z_ operator const T*() const noexcept
        {
            return CStr( );
        }

        String<T>& operator+=(_In_z_ const T* pCStr)
        {
            ValidatePointerT(__FUNCSIG__, pCStr);
            if ( !AppendUsingRawPointer(*this, pCStr, GetStringLength( )) )
            {
                throw std::exception(__FUNCSIG__": Failed to append using C-string.");
            }
            return *this;
        }

        String<T>& operator+=(_In_ const String<T>& src)
        {
            ValidatePointerT(__FUNCSIG__, pCStr);
            if ( !AppendUsingPointerObj(*this, src) )
            {
                throw std::exception(__FUNCSIG__": Failed to append using CC::String object.");
            }

            return *this;
        }

        [[nodiscard]] T& operator[](_In_ const size_t& idx)
        {
            ValidateIndexT(__FUNCSIG__, idx);
            return CStr[idx];
        }

        [[nodiscard]] const T& operator[](_In_ const size_t& idx) const
        {
            ValidateIndexT(__FUNCSIG__, idx);
            return CStr[idx];
        }

        [[nodiscard]] bool operator==(_In_ const T* pCStr) const noexcept
        {
            size_t len = 0;
            if ( !pCStr )
            {
                return false;
            }

            len = GetStringLength(pCStr);
            if ( len != Length( ) )
            {
                return false;
            }

            return CompareStrings(*this, pCStr, len);
        }

        [[nodiscard]] bool operator==(_In_ const String<T>& other) const noexcept
        {
            return CompareStrings(*this, other);
        }

        /// Getters \\\

        [[nodiscard]] _Ret_z_ T* CStr( ) const noexcept
        {
            return (IsUsingSSOArray( )) ? m_SSOArr : m_DynBuf.Get( );
        }

        [[nodiscard]] _Ret_z_ const T* CStr( ) const noexcept
        {
            return (IsUsingSSOArray( )) ? m_SSOArr : m_DynBuf.Get( );
        }

        [[nodiscard]] const size_t& Length( ) const noexcept
        {
            return (IsUsingSSOArray( )) ? m_SSOPos : m_DynBuf.WritePosition( );
        }

        [[nodiscard]] const size_t& Capacity( ) const noexcept
        {
            return (IsUsingSSOArray( )) ? ms_SSOCap : m_DynBuf.Length( );
        }

        /// Public Methods \\\

        void Clear( ) noexcept
        {
            m_DynBuf.Free( );
            m_SSOArr[0] = ms_NullTerminator;
            m_SSOPos = 0;
        }

        [[nodiscard]] bool IsEmpty( ) noexcept
        {
            return Length( ) != 0;
        }

        [[nodiscard]] bool Assign(_In_ const T& tc) noexcept
        {
            return Assign(&tc, 1);
        }

        [[nodiscard]] bool Assign(_In_z_ const T* const pCStr) noexcept
        {
            return Assign(pCStr, GetStringLength(pCStr));
        }

        [[nodiscard]] bool Assign(_In_ const T* const pStr, _In_ const size_t& len) noexcept
        {
            return CopyFromRawPointer(*this, pStr, len);
        }

        [[nodiscard]] bool Assign(_In_ const String<T>& src) noexcept
        {
            return CopyFromStringObj(*this, src);
        }

        [[nodiscard]] bool Assign(_In_ String<T>&& src) noexcept
        {
            return MoveFromStringObj(*this, std::move(src));
        }

        [[nodiscard]] bool Append(_In_ const T& tc) noexcept
        {
            return Append(&tc, 1);
        }

        [[nodiscard]] bool Append(_In_z_ const T* const pCStr) noexcept
        {
            return Append(pCStr, GetStringLength(pCStr));
        }

        [[nodiscard]] bool Append(_In_ const T* const pCStr, _In_ const size_t& len) noexcept
        {
            return AppendUsingRawPointer(*this, pCStr, len);
        }

        [[nodiscard]] bool Append(_In_ const String<T>& src) noexcept
        {
            return AppendUsingStringObj(src);
        }

        [[nodiscard]] T& Front( ) noexcept
        {
            return CStr( )[0];
        }

        [[nodiscard]] const T& Front( ) const noexcept
        {
            return CStr( )[0];
        }

        [[nodiscard]] T& Back( ) noexcept
        {
            return CStr( )[Length( ) - 1];
        }

        [[nodiscard]] const T& Back( ) const noexcept
        {
            return CStr( )[Length( ) - 1];
        }

        [[nodiscard]] bool EqualTo(_In_z_ const T* const pCStr, _In_ const bool& bCaseInsensitive = false) const noexcept
        {
            size_t len = GetStringLength(pCStr);
            return (len != ms_InvalidLen) && CompareStrings(*this, pCStr, GetStringLength(pCStr), bCaseInsensitive);
        }

        [[nodiscard]] bool EqualTo(_In_ const T* const pStr, _In_ const size_t& len, _In_ const bool& bCaseInsensitive = false) const noexcept
        {
            return CompareStrings(*this, pStr, len, bCaseInsensitive);
        }

        [[nodiscard]] bool EqualTo(_In_ const String<T>& rhs, _In_ const bool& bCaseInsensitive = false) const noexcept
        {
            return CompareStrings(*this, rhs, bCaseInsensitive);
        }
    };
}