#pragma once

#include "Macros.h"

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
    //  Define macro, used as default template argument for CopyXyz functions.
    //  - Helps ensure correct data-types are used with the function.
    //
    ///
#define COPY_METHOD_ENABLE_CONDITIONS(T1, T2) \
    typename = std::enable_if_t<IsStringTypeValue<T1>( ) && IsStringTypeValue<T2>( )>


    ///
    //
    //  Perform common work to copy source C-string into destination buffer.
    //  Will perform conversion as necessary if the source/destination types differ.
    //
    ///
    template <class T_Dst, class T_Src, COPY_METHOD_ENABLE_CONDITIONS(T_Dst, T_Src)>
    static std::unique_ptr<T_Dst[ ]> CopyCStringCommon(const T_Src* src, const size_t len)
    {
        std::unique_ptr<T_Dst[ ]> buf;

        if ( src && len > 0 )
        {
            buf = std::make_unique<T_Dst[ ]>(len);
            for ( size_t i = 0; i < len; i++ )
            {
                buf[i] = static_cast<T_Dst>(src[i]);
            }
        }

        return buf;
    }

    ///
    //
    //  Helper class for counting characters in a null-terminated string (including the null).
    //  - Note: Function assumes provided C-string argument is valid (i.e., valid addr and null-terminated).
    //
    ///
    template <class T_Src, STRING_TEMPLATE_ENABLE_IF_SUPPORTED_TYPE(T_Src)>
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
#define CONVERT_AND_COPY_ENABLE_CONDITIONS(T1, T2) \
    typename = std::enable_if_t<IsStringTypeValue<T1>( ) && \
                IsStringTypeValue<T2>( )>

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
    template <class T_Dst, class T_Src, CONVERT_AND_COPY_ENABLE_CONDITIONS(T_Dst, T_Src)>
    static std::unique_ptr<T_Dst[ ]> ConvertAndCopy(const T_Src* src, const size_t len)
    {
        return CopyCStringCommon<T_Dst>(src, len);
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
    template <class T_Dst, class T_Src, CONVERT_AND_COPY_ENABLE_CONDITIONS(T_Dst, T_Src)>
    static std::unique_ptr<T_Dst[ ]> ConvertAndCopy(const T_Src* src)
    {
        return (src) ? CopyCStringCommon<T_Dst>(src, GetNTStringLength(src)) : nullptr;
    }

    ///
    //
    //  Creates and returns a copy of source string object, converting it to the destination type.
    //
    ///
    template <class T_Dst, class T_Src, CONVERT_AND_COPY_ENABLE_CONDITIONS(T_Dst, T_Src)>
    static std::basic_string<T_Dst> ConvertAndCopy(const std::basic_string<T_Src>& src)
    {
        std::basic_string<T_Dst> buf;

        if ( !src.empty( ) )
        {
            buf.reserve(src.size( ));
            for ( auto c : src )
            {
                buf.push_back(static_cast<T_Dst>(c));
            }
        }

        return buf;
    }

    ///
    //
    //  Creates and returns a copy of the source string.
    //  - Note: Length argument is assumed to be correct.
    //
    ///
    template <class T, STRING_TEMPLATE_ENABLE_IF_SUPPORTED_TYPE(T)>
    static std::unique_ptr<T[ ]> CopyCString(const T* src, const size_t len)
    {
        return CopyCStringCommon<T>(src, len);
    }

    ///
    //
    //  Creates and returns a copy of the source string.
    //  - Note: String must be null-terminated.
    //
    ///
    template <class T, STRING_TEMPLATE_ENABLE_IF_SUPPORTED_TYPE(T)>
    static std::unique_ptr<T[ ]> CopyCString(const T* src)
    {
        return (src) ? CopyCStringCommon<T>(src, GetNTStringLength(src)) : nullptr;
    }
};
