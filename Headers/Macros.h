#pragma once

#include <string>
#include <tuple>
#include <type_traits>
#include "Types.h"

/// String Macro Helpers \\\

#define _STRINGIFY(s)   #s
#define STRINGIFY(s)    _STRINGIFY(s)

#define _CONCATENATE(l, r)  l ## r
#define CONCATENATE(l, r)   _CONCATENATE(l, r)

using SupportedCharacterTuple = std::tuple<utf8, utf16>;
using SupportedStringTuple = std::tuple<std::basic_string<utf8>, std::basic_string<utf16>>;

#define _UTF8_LITERAL_PREFIX     u8
#define _UTF16_LITERAL_PREFIX    L

#define UTF8_LITERAL_CHAR(c)    CONCATENATE(_UTF8_LITERAL_PREFIX, c)
#define UTF16_LITERAL_CHAR(c)   CONCATENATE(_UTF16_LITERAL_PREFIX, c)
#define UTF8_LITERAL_STR(str)   CONCATENATE(_UTF8_LITERAL_PREFIX, str)
#define UTF16_LITERAL_STR(str)  CONCATENATE(_UTF16_LITERAL_PREFIX, str)

#define MAKE_CHAR_TUPLE(c)  SupportedCharacterTuple(UTF8_LITERAL_CHAR(c), UTF16_LITERAL_CHAR(c))
#define MAKE_STR_TUPLE(str) SupportedStringTuple(UTF8_LITERAL_STR(str), UTF16_LITERAL_STR(str))

/// Enable-If constexpr Helper Functions \\\

// Supported Char type.
template <class T>
bool constexpr IsSupportedCharType( )
{
    return std::is_same<T, utf8>::value || std::is_same<T, utf16>::value;
}


// Signed integer type.
template <class T>
bool constexpr IsSignedIntegerType( )
{
    return std::is_integral<T>::value && std::is_signed<T>::value;
}


// Unsigned integer type.
template <class T>
bool constexpr IsUnsignedIntegerType( )
{
    return std::is_integral<T>::value && std::is_unsigned<T>::value;
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
    return std::is_pointer<T>::value;
}


// Integer Representable type.
template <class T>
bool constexpr IsIntegerRepresentableType( )
{
    return IsIntegerType<T>( ) || IsPointerType<T>( );
}

/// Enable-If Helper Macros \\\

// Supported Char Type
#define ENABLE_IF_SUPPORTED_CHARACTER_TYPE(T) \
    typename = typename std::enable_if<IsSupportedCharType<T>( )>::type


// Signed Integer Type
#define ENABLE_IF_SIGNED_INTEGER_TYPE(T) \
    typename = typename std::enable_if<IsSignedIntegerType<T>( )>::type


// Unsigned Integer Type
#define ENABLE_IF_UNSIGNED_INTEGER_TYPE(T) \
    typename = typename std::enable_if<IsUnsignedIntegerType<T>( )>::type


// Integer Type
#define ENABLE_IF_INTEGER_TYPE(T) \
    typename = typename std::enable_if<IsIntegerType<T>( )>::type


// Pointer Type
#define ENABLE_IF_POINTER_TYPE(T) \
    typename = typename std::enable_if<IsPointerType<T>( )>::type

// Integer Representable Type
#define ENABLE_IF_INTEGER_REPRESENTABLE_TYPE(T) \
    typename = typename std::enable_if<IsIntegerRepresentableType<T>( )>::type