#ifndef _CC_MACROS_H_
#define _CC_MACROS_H_

#include "CCTypes.h"

/// String Macro Helpers \\\

#define _CC_STRINGIFY(s)   #s
#define CC_STRINGIFY(s)    _CC_STRINGIFY(s)

#define _CC_CONCATENATE(l, r)  l ## r
#define CC_CONCATENATE(l, r)   _CC_CONCATENATE(l, r)

#define _CC_UTF8_LITERAL_PREFIX     u8
#define _CC_UTF16_LITERAL_PREFIX    L

#define CC_UTF8_LITERAL_CHAR(c)    CC_CONCATENATE(_CC_UTF8_LITERAL_PREFIX, c)
#define CC_UTF16_LITERAL_CHAR(c)   CC_CONCATENATE(_CC_UTF16_LITERAL_PREFIX, c)
#define CC_UTF8_LITERAL_STR(str)   CC_CONCATENATE(_CC_UTF8_LITERAL_PREFIX, str)
#define CC_UTF16_LITERAL_STR(str)  CC_CONCATENATE(_CC_UTF16_LITERAL_PREFIX, str)

#define CC_MAKE_CHAR_TUPLE(c)  SupportedCharacterTuple(CC_UTF8_LITERAL_CHAR(c), CC_UTF16_LITERAL_CHAR(c))
#define CC_MAKE_STR_TUPLE(str) SupportedStringTuple(CC_UTF8_LITERAL_STR(str), CC_UTF16_LITERAL_STR(str))


/// Is Nothrow Helper Macros \\\

#define CC_IS_NOTHROW_CTOR(_TYPE)           (std::is_nothrow_constructible_v<_TYPE>)
#define CC_IS_NOTHROW_CTOR_DEFAULT(_TYPE)   (std::is_nothrow_default_constructible_v<_TYPE>)
#define CC_IS_NOTHROW_CTOR_COPY(_TYPE)      (std::is_nothrow_default_constructible_v<_TYPE>)
#define CC_IS_NOTHROW_CTOR_MOVE(_TYPE)      (std::is_nothrow_default_constructible_v<_TYPE>)
#define CC_IS_NOTHROW_COPY(_TYPE)           (std::is_nothrow_copy_assignable_v<_TYPE>)
#define CC_IS_NOTHROW_MOVE(_TYPE)           (std::is_nothrow_move_assignable_v<_TYPE>)


/// SFINAE Enable-If Helper Macros \\\

// Supported Char Type
#define CC_ENABLE_IF_SUPPORTED_CHARACTER_TYPE(T) \
    typename = typename std::enable_if<IsSupportedCharType<T>( )>::type


// Signed Integer Type
#define CC_ENABLE_IF_SIGNED_INTEGER_TYPE(T) \
    typename = typename std::enable_if<IsSignedIntegerType<T>( )>::type


// Unsigned Integer Type
#define CC_ENABLE_IF_UNSIGNED_INTEGER_TYPE(T) \
    typename = typename std::enable_if<IsUnsignedIntegerType<T>( )>::type


// Integer Type
#define CC_ENABLE_IF_INTEGER_TYPE(T) \
    typename = typename std::enable_if<IsIntegerType<T>( )>::type


// Pointer Type
#define CC_ENABLE_IF_POINTER_TYPE(T) \
    typename = typename std::enable_if<IsPointerType<T>( )>::type

// Integer Representable Type
#define CC_ENABLE_IF_INTEGER_REPRESENTABLE_TYPE(T) \
    typename = typename std::enable_if<IsIntegerRepresentableType<T>( )>::type


#endif // _CC_MACROS_H_