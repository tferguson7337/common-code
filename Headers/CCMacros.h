#ifndef _CC_MACROS_H_
#define _CC_MACROS_H_

#include "CCTypes.h"

/// String Macro Helpers \\\

#define _STRINGIFY(s)   #s
#define STRINGIFY(s)    _STRINGIFY(s)

#define _CONCATENATE(l, r)  l ## r
#define CONCATENATE(l, r)   _CONCATENATE(l, r)

#define _UTF8_LITERAL_PREFIX     u8
#define _UTF16_LITERAL_PREFIX    L

#define UTF8_LITERAL_CHAR(c)    CONCATENATE(_UTF8_LITERAL_PREFIX, c)
#define UTF16_LITERAL_CHAR(c)   CONCATENATE(_UTF16_LITERAL_PREFIX, c)
#define UTF8_LITERAL_STR(str)   CONCATENATE(_UTF8_LITERAL_PREFIX, str)
#define UTF16_LITERAL_STR(str)  CONCATENATE(_UTF16_LITERAL_PREFIX, str)

#define MAKE_CHAR_TUPLE(c)  SupportedCharacterTuple(UTF8_LITERAL_CHAR(c), UTF16_LITERAL_CHAR(c))
#define MAKE_STR_TUPLE(str) SupportedStringTuple(UTF8_LITERAL_STR(str), UTF16_LITERAL_STR(str))

/// SFINAE Enable-If Helper Macros \\\

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

#endif // _CC_MACROS_H_