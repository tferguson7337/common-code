#ifndef _CC_MACROS_H_
#define _CC_MACROS_H_

#include "CCTypes.h"

/// String Macro Helpers \\\

#define _CC_STRINGIFY(_S)           #_S
#define CC_STRINGIFY(_S)            _CC_STRINGIFY(_S)

#define _CC_CONCATENATE(_L, _R)     _L ## _R
#define CC_CONCATENATE(_L, _R)      _CC_CONCATENATE(_L, _R)

#define _CC_UTF8_LITERAL_PREFIX     u8
#define _CC_UTF16_LITERAL_PREFIX    L

#define CC_UTF8_LITERAL_CHAR(_C)    CC_CONCATENATE(_CC_UTF8_LITERAL_PREFIX, _C)
#define CC_UTF16_LITERAL_CHAR(_C)   CC_CONCATENATE(_CC_UTF16_LITERAL_PREFIX, _C)
#define CC_UTF8_LITERAL_STR(_STR)   CC_CONCATENATE(_CC_UTF8_LITERAL_PREFIX, _STR)
#define CC_UTF16_LITERAL_STR(_STR)	CC_CONCATENATE(_CC_UTF16_LITERAL_PREFIX, _STR)

#define CC_MAKE_CHAR_TUPLE(_C)      SupportedCharacterTuple(CC_UTF8_LITERAL_CHAR(_C), CC_UTF16_LITERAL_CHAR(_C))
#define CC_MAKE_STR_TUPLE(_STR)	    SupportedStringTuple(CC_UTF8_LITERAL_STR(_STR), CC_UTF16_LITERAL_STR(_STR))


/// noexcept is_nothrow Helper Macros \\\

#define CC_IS_NOTHROW_CTOR(_T)          (std::is_nothrow_constructible_v<_T>)
#define CC_IS_NOTHROW_CTOR_A(_T, _A)    (std::is_nothrow_constructible_v<_T, _A...>)
#define CC_IS_NOTHROW_CTOR_DEFAULT(_T)  (std::is_nothrow_default_constructible_v<_T>)
#define CC_IS_NOTHROW_CTOR_COPY(_T)     (std::is_nothrow_copy_constructible_v<_T>)
#define CC_IS_NOTHROW_CTOR_MOVE(_T)     (std::is_nothrow_move_constructible_v<_T>)
#define CC_IS_NOTHROW_COPY(_T)          (std::is_nothrow_copy_assignable_v<_T>)
#define CC_IS_NOTHROW_MOVE(_T)          (std::is_nothrow_move_assignable_v<_T>)
#define CC_IS_NOTHROW_DTOR(_T)          (std::is_nothrow_destructible_v<_T>)


/// SFINAE Enable-If Helper Macros \\\

// Supported Char Type
#define CC_ENABLE_IF_SUPPORTED_CHARACTER_TYPE(_T) \
    typename = typename std::enable_if_t<IsSupportedCharType<_T>()>


// Signed Integer Type
#define CC_ENABLE_IF_SIGNED_INTEGER_TYPE(_T) \
    typename = typename std::enable_if_t<IsSignedIntegerType<_T>()>


// Unsigned Integer Type
#define CC_ENABLE_IF_UNSIGNED_INTEGER_TYPE(_T) \
    typename = typename std::enable_if_t<IsUnsignedIntegerType<_T>()>


// Integer Type
#define CC_ENABLE_IF_INTEGER_TYPE(_T) \
    typename = typename std::enable_if_t<IsIntegerType<_T>()>


// Pointer Type
#define CC_ENABLE_IF_POINTER_TYPE(_T) \
    typename = typename std::enable_if_t<IsPointerType<_T>()>


// Not Pointer Type
#define CC_ENABLE_IF_NOT_POINTER_TYPE(_T) \
    typename = typename std::enable_if_t<!IsPointerType<_T>()>


// Integer Representable Type
#define CC_ENABLE_IF_INTEGER_REPRESENTABLE_TYPE(_T) \
    typename = typename std::enable_if_t<IsIntegerRepresentableType<_T>()>


#endif // _CC_MACROS_H_