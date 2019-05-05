#ifndef _CC_TYPES_H_
#define _CC_TYPES_H_

#include <cstdint>
#include <string>
#include <tuple>

namespace CC
{
    /// Basic Types \\\

    // Basic Signed Types
    using int8  = int8_t;
    using int16 = int16_t;
    using int32 = int32_t;
    using int64 = int64_t;

    // Basic Unsigned Types
    using uint8  = uint8_t;
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    using uint64 = uint64_t;

    // Basic Character Types
    using ascii = char;
    using utf8  = char;
    using utf16 = wchar_t;
    using utf32 = char32_t;

    /// Pointer-To-Basic-Types \\\

    // Pointer - Basic Signed Types
    using int8_ptr  = int8*;
    using int16_ptr = int16*;
    using int32_ptr = int32*;
    using int64_ptr = int64*;

    // Pointer - Basic Unsigned Types
    using uint8_ptr  = uint8*;
    using uint16_ptr = uint16*;
    using uint32_ptr = uint32*;
    using uint64_ptr = uint64*;

    // Pointer - Basic Character Types
    using ascii_ptr = ascii*;
    using utf8_ptr  = utf8*;
    using utf16_ptr = utf16*;
    using utf32_ptr = utf32*;

    /// Template Class Types \\\

    // Character tuple of character types supported by CC
    using SupportedCharacterTuple = std::tuple<utf8, utf16>;

    // std::basic_string tuple of character types supported by CC
    using SupportedStringTuple = std::tuple<std::basic_string<utf8>, std::basic_string<utf16>>;

    /// SFINAE constexpr Helper Functions \\\

    // Supported Char type.
    template <class T>
    bool constexpr IsSupportedCharType( )
    {
        return std::is_same_v<T, utf8> || std::is_same_v<T, utf16>;
    }


    // Signed integer type.
    template <class T>
    bool constexpr IsSignedIntegerType( )
    {
        return std::is_integral_v<T>&& std::is_signed_v<T>;
    }


    // Unsigned integer type.
    template <class T>
    bool constexpr IsUnsignedIntegerType( )
    {
        return std::is_integral_v<T>&& std::is_unsigned_v<T>;
    }


    // Integer type.
    template <class T>
    bool constexpr IsIntegerType( )
    {
        return IsSignedIntegerType<T>( ) || IsUnsignedIntegerType<T>( );
    }


    // Pointer type.
    template <class T>
    bool constexpr IsPointerType( )
    {
        return std::is_pointer_v<T>;
    }


    // Integer Representable type.
    template <class T>
    bool constexpr IsIntegerRepresentableType( )
    {
        return IsIntegerType<T>( ) || IsPointerType<T>( );
    }

    /// Sanity Checks For Expected Type Sizes \\\

#define _SASSERT_TYPE_SIZE_MISMATCH_STR_ "Unexpected type size mismatch detected."

    // Basic Signed Types
    static_assert(sizeof(int8)      == 1, _SASSERT_TYPE_SIZE_MISMATCH_STR_);
    static_assert(sizeof(int16)     == sizeof(int8) << 1, _SASSERT_TYPE_SIZE_MISMATCH_STR_);
    static_assert(sizeof(int32)     == sizeof(int16) << 1, _SASSERT_TYPE_SIZE_MISMATCH_STR_);
    static_assert(sizeof(int64)     == sizeof(int32) << 1, _SASSERT_TYPE_SIZE_MISMATCH_STR_);

    // Basic Unsigned Types
    static_assert(sizeof(uint8)     == 1, _SASSERT_TYPE_SIZE_MISMATCH_STR_);
    static_assert(sizeof(uint16)    == sizeof(uint8) << 1, _SASSERT_TYPE_SIZE_MISMATCH_STR_);
    static_assert(sizeof(uint32)    == sizeof(uint16) << 1, _SASSERT_TYPE_SIZE_MISMATCH_STR_);
    static_assert(sizeof(uint64)    == sizeof(uint32) << 1, _SASSERT_TYPE_SIZE_MISMATCH_STR_);

    // String Types
    static_assert(sizeof(ascii)     == 1, _SASSERT_TYPE_SIZE_MISMATCH_STR_);
    static_assert(sizeof(utf8)      == sizeof(ascii), _SASSERT_TYPE_SIZE_MISMATCH_STR_);
    static_assert(sizeof(utf16)     == sizeof(utf8) << 1, _SASSERT_TYPE_SIZE_MISMATCH_STR_);
    static_assert(sizeof(utf32)     == sizeof(utf16) << 1, _SASSERT_TYPE_SIZE_MISMATCH_STR_);

#undef _SASSERT_TYPE_SIZE_MISMATCH_STR_
}

#endif // _CC_TYPES_H_