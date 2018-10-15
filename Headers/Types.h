#pragma once

#include <type_traits>

/// Basic Types \\\

// Basic Signed Types
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;

// Basic Unsigned Types
using uint8 = std::make_unsigned_t<__int8>;
using uint16 = std::make_unsigned_t<__int16>;
using uint32 = std::make_unsigned_t<__int32>;
using uint64 = std::make_unsigned_t<__int64>;

// Basic Character Types
using ascii = char;
using utf8 = char;
using utf16 = wchar_t;
using utf32 = int32;

/// Pointer-To-Basic-Types \\\

// Pointer - Basic Signed Types
using int8_ptr = std::add_pointer_t<int8>;
using int16_ptr = std::add_pointer_t<int16>;
using int32_ptr = std::add_pointer_t<int32>;
using int64_ptr = std::add_pointer_t<int64>;

// Pointer - Basic Unsigned Types
using uint8_ptr = std::add_pointer_t<uint8>;
using uint16_ptr = std::add_pointer_t<uint16>;
using uint32_ptr = std::add_pointer_t<uint32>;
using uint64_ptr = std::add_pointer_t<uint64>;

// Pointer - Basic Character Types
using ascii_ptr = std::add_pointer_t<ascii>;
using utf8_ptr = std::add_pointer_t<utf8>;
using utf16_ptr = std::add_pointer_t<utf16>;
using utf32_ptr = std::add_pointer_t<utf32>;

/// Sanity Checks For Expected Type Sizes \\\

namespace
{
#define TYPE_SASSERT_STR "Unexpected type size - platform may not be supported."

    // Basic Signed Types
    static_assert(sizeof(int8)      == 1, TYPE_SASSERT_STR);
    static_assert(sizeof(int16)     == sizeof(int8) << 1, TYPE_SASSERT_STR);
    static_assert(sizeof(int32)     == sizeof(int16) << 1, TYPE_SASSERT_STR);
    static_assert(sizeof(int64)     == sizeof(int32) << 1, TYPE_SASSERT_STR);

    // Basic Unsigned Types
    static_assert(sizeof(uint8)     == 1, TYPE_SASSERT_STR);
    static_assert(sizeof(uint16)    == sizeof(uint8) << 1, TYPE_SASSERT_STR);
    static_assert(sizeof(uint32)    == sizeof(uint16) << 1, TYPE_SASSERT_STR);
    static_assert(sizeof(uint64)    == sizeof(uint32) << 1, TYPE_SASSERT_STR);

    // String Types
    static_assert(sizeof(ascii)     == 1, TYPE_SASSERT_STR);
    static_assert(sizeof(utf8)      == sizeof(ascii), TYPE_SASSERT_STR);
    static_assert(sizeof(utf16)     == sizeof(utf8) << 1, TYPE_SASSERT_STR);
    static_assert(sizeof(utf32)     == sizeof(utf16) << 1, TYPE_SASSERT_STR);
}
