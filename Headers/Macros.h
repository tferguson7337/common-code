#pragma once

#include <type_traits>

/// Enable-If constexpr Helper Functions \\\

// Supported Char type.
template <class T>
bool constexpr IsSupportedCharType( )
{
    return std::is_same_v<T, char> || std::is_same_v<T, wchar_t>;
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