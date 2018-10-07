#pragma once

#include <string>
#include <tuple>
#include <type_traits>
#include <Types.h>

/// String Macro Helpers \\\

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

#define _CONCATENATE(l, r) l ## r
#define CONCATENATE(l, r) _CONCATENATE(l, r)

using SupportedStringTuple = std::tuple<std::basic_string<utf8>, std::basic_string<utf16>>;

#define _UTF8_LITERAL_PREFIX     u8

#ifdef _UTF16_IS_WIDE_CHAR
#define _UTF16_LITERAL_PREFIX   L
#else
#define _UTF16_LITERAL_PREFIX   u
#endif

#define UTF8_LITERAL(str)   CONCATENATE(_UTF8_LITERAL_PREFIX, str)
#define UTF16_LITERAL(str)  CONCATENATE(_UTF16_LITERAL_PREFIX, str)

#define MAKE_SUBSTR_TUPLE(str)  SupportedStringTuple(UTF8_LITERAL(str), UTF16_LITERAL(str))

/// Enable-If constexpr Helper Functions \\\

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
    return std::is_integral_v<T> && std::is_signed_v<T>;
}


// Unsigned integer type.
template <class T>
bool constexpr IsUnsignedIntegerType( )
{
    return std::is_integral_v<T> && std::is_unsigned_v<T>;
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

/// Enable-If Helper Macros \\\

// Supported Char Type
#define ENABLE_IF_SUPPORTED_CHARACTER_TYPE(T) \
    typename = std::enable_if_t<IsSupportedCharType<T>( )>


// Signed Integer Type
#define ENABLE_IF_SIGNED_INTEGER_TYPE(T) \
    typename = std::enable_if_t<IsSignedIntegerType<T>( )>


// Unsigned Integer Type
#define ENABLE_IF_UNSIGNED_INTEGER_TYPE(T) \
    typename = std::enable_if_t<IsUnsignedIntegerType<T>( )>


// Integer Type
#define ENABLE_IF_INTEGER_TYPE(T) \
    typename = std::enable_if_t<IsIntegerType<T>( )>


// Pointer Type
#define ENABLE_IF_POINTER_TYPE(T) \
    typename = std::enable_if_t<IsPointerType<T>( )>

// Integer Representable Type
#define ENABLE_IF_INTEGER_REPRESENTABLE_TYPE(T) \
    typename = std::enable_if_t<IsIntegerRepresentableType<T>( )>