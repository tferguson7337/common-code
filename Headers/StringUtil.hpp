#pragma once

#include "Types.h"

#include <string>
#include <memory>

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

    ///
    //
    //  Intended to be used in std::enable_if_t<..> template args, to help 
    //  ensure correct data types are used with StringUtil functions.
    //
    ///
    template <class T>
    static bool constexpr IsStringTypeValue( )
    {
        return std::is_same<T, char>::value || std::is_same<T, wchar_t>::value;
    }

    ///
    //
    //  Define macro, used as default template argument for GetNTStringLength function.
    //  - Helps ensure correct data-type is used with the function.
    //
    ///
#define _GET_NT_STRING_LENGTH_ENABLE_CONDITIONS_ \
    typename = std::enable_if_t<IsStringTypeValue<T_Src>( )>

    ///
    //
    //  Helper class for counting characters in a null-terminated string (including the null).
    //  - Note: Function assumes provided C-string argument is valid (i.e., not nullptr or non-dereferenceable addr).
    //
    ///
    template <class T_Src, _GET_NT_STRING_LENGTH_ENABLE_CONDITIONS_>
    static size_t GetNTStringLength(const T_Src* src)
    {
        // Get element count of source buffer.
        size_t len = 0;
        while ( *src )
        {
            len++;
            src++;
        }

        // Return character count + null.
        return len + 1;
    }

public:

    /// Public Methods \\\

    ///
    //
    //  Define macro, used as default template argument for ConvertAndCopy functions.
    //  - Helps ensure correct data-types are used with the function.
    //
    ///
#define _CONVERT_AND_COPY_ENABLE_CONDITIONS_ \
    typename = std::enable_if_t<IsStringTypeValue<T_Src>( ) && \
                IsStringTypeValue<T_Dst>( ) && \
                !std::is_same<T_Src, T_Dst>::value>

    ///
    //
    //  Creates a copy of source string, converting it to the destination type.
    //
    //  Returned string is contained in a std::unique_ptr object to help avoid
    //  memory leaks in the event the caller forgets to free the returned pointer.
    //
    //  Length argument is expected to be the number of characters (i.e., elements) 
    //  that the string contains, as opposed to total byte-count.  If string is null
    //  terminated, the length should include the null.
    //
    //  - NOTE: FUNCTION ASSUMES CALLER-PROVIDED LENGTH ARGUMENT IS CORRECT.
    //  - - If length is too large, you will overrun the source-string boundaries.
    //  - - If source-string is null-terminated, but length is too small, returned
    //      string will not be null-terminated.
    //
    ///
    template <class T_Dst, class T_Src, _CONVERT_AND_COPY_ENABLE_CONDITIONS_>
    static std::unique_ptr<T_Dst[ ]> ConvertAndCopy(const T_Src* src, size_t len)
    {
        std::unique_ptr<T_Dst[ ]> buf;

        if ( !src || len == 0 )
        {
            return buf;
        }

        buf = std::make_unique<T_Dst[ ]>(len);
        for ( size_t i = 0; i < len; i++ )
        {
            buf[i] = static_cast<T_Dst>(src[i]);
        }

        return buf;
    }

    ///
    //
    //  Creates and returns a copy of source string, converting it to the destination type.
    //
    //  Returned string is contained in a std::unique_ptr object to help avoid
    //  memory leaks in the event the caller forgets to free the returned pointer.
    //
    //  - NOTE: SOURCE ARGUMENT MUST BE NULL-TERMINATED.
    //
    ///
    template <class T_Dst, class T_Src, _CONVERT_AND_COPY_ENABLE_CONDITIONS_>
    static std::unique_ptr<T_Dst[ ]> ConvertAndCopy(const T_Src* src)
    {
        return (src) ? ConvertAndCopy<T_Dst>(src, GetNTStringLength(src)) : nullptr;
    }

    ///
    //
    //  Creates and returns a copy of source string object, converting it to the destination type.
    //
    ///
    template <class T_Dst, class T_Src, _CONVERT_AND_COPY_ENABLE_CONDITIONS_>
    static std::basic_string<T_Dst> ConvertAndCopy(const std::basic_string<T_Src>& src)
    {
        std::basic_string<T_Dst> buf;

        if ( src.empty( ) )
        {
            return buf;
        }

        buf.reserve(src.size( ));
        for ( auto c : src )
        {
            buf.push_back(static_cast<T_Dst>(c));
        }

        return buf;
    }
};
