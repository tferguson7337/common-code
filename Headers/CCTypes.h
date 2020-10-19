#pragma once

// STL
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>

// SAL
#include "CCAPIAnnotations.h"


namespace CC
{
    // Template Class Types //

    // Character tuple of character types supported by CC
    using SupportedCharacterTuple = std::tuple<char, wchar_t>;

    // std::basic_string tuple of character types supported by CC
    using SupportedStringTuple = std::tuple<std::string, std::wstring>;

    // SFINAE constexpr Helper Functions //

    // Supported Char type.
    template <class T>
    [[nodiscard]] bool constexpr IsSupportedCharType()
    {
        return std::is_same_v<T, char> || std::is_same_v<T, wchar_t>;
    }


    // Signed integer type.
    template <class T>
    [[nodiscard]] bool constexpr IsSignedIntegerType()
    {
        return std::is_integral_v<T> && std::is_signed_v<T>;
    }


    // Unsigned integer type.
    template <class T>
    [[nodiscard]] bool constexpr IsUnsignedIntegerType()
    {
        return std::is_integral_v<T> && std::is_unsigned_v<T>;
    }


    // Integer type.
    template <class T>
    [[nodiscard]] bool constexpr IsIntegerType()
    {
        return IsSignedIntegerType<T>() || IsUnsignedIntegerType<T>();
    }


    // Pointer type.
    template <class T>
    [[nodiscard]] bool constexpr IsPointerType()
    {
        return std::is_pointer_v<T>;
    }


    // Integer Representable type.
    template <class T>
    [[nodiscard]] bool constexpr IsIntegerRepresentableType()
    {
        return IsIntegerType<T>() || IsPointerType<T>();
    }
}
