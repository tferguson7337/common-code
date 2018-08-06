#pragma once

#include <type_traits>

///
//
//  Helper constexpr for string template enable-if conditions.
//
///
template <class T>
bool constexpr IsStringTypeValue( )
{
    return std::is_same_v<T, char> || std::is_same_v<T, wchar_t>;
}

///
//
//  Helper macro for enabling string-based template functions
//  for only supported char types (e.g., char and wchar_t)
//
///
#define STRING_TEMPLATE_ENABLE_IF_SUPPORTED_TYPE(T) \
    typename = std::enable_if_t<IsStringTypeValue<T>( )>

