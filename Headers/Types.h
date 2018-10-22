#pragma once

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
using utf32 = int32;

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

/// Sanity Checks For Expected Type Sizes \\\

namespace
{
    // Basic Signed Types
    static_assert(sizeof(int8)      == 1);
    static_assert(sizeof(int16)     == sizeof(int8) << 1);
    static_assert(sizeof(int32)     == sizeof(int16) << 1);
    static_assert(sizeof(int64)     == sizeof(int32) << 1);

    // Basic Unsigned Types
    static_assert(sizeof(uint8)     == 1);
    static_assert(sizeof(uint16)    == sizeof(uint8) << 1);
    static_assert(sizeof(uint32)    == sizeof(uint16) << 1);
    static_assert(sizeof(uint64)    == sizeof(uint32) << 1);

    // String Types
    static_assert(sizeof(ascii)     == 1);
    static_assert(sizeof(utf8)      == sizeof(ascii));
    static_assert(sizeof(utf16)     == sizeof(utf8) << 1);
    static_assert(sizeof(utf32)     == sizeof(utf16) << 1);
}
