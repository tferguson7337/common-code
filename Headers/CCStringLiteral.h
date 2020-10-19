#pragma once

// CC
#include "CCMacros.h"

// STL
#include <stdexcept>


namespace CC
{

    template <typename T, size_t ArrayLength, CC_ENABLE_IF_SUPPORTED_CHARACTER_TYPE(T)>
    class [[nodiscard]] StringLiteral
    {
        friend class StringLiteralTests;

        static_assert(ArrayLength > 0, "StringLiteral: Somehow received zero-length string literal.");

    private:

        // Private Data Members //

        const T* const m_pLitStr;

    public:

        // Ctors //

        constexpr explicit StringLiteral(_In_z_ const T(&pLitStr)[ArrayLength]) noexcept :
            m_pLitStr(pLitStr)
        { }

        // Provide support for copy constructing (e.g., constexpr auto lit("Test"); constexpr auto lit2(lit1);).
        constexpr StringLiteral(_In_ const StringLiteral<T, ArrayLength>& src) noexcept :
            m_pLitStr(src.m_pLitStr)
        { }

        // Provide support for constructing via move (e.g., auto lit(CC_MAKE_STRING_LITERAL("Example"));)
        constexpr StringLiteral(_In_ StringLiteral<T, ArrayLength>&& src) noexcept :
            m_pLitStr(src.m_pLitStr)
        { }


        // Operator Overloads //

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return !IsEmpty();
        }

        [[nodiscard]] constexpr T operator[](_In_ const size_t idx) const
        {
            return (idx < Length()) ? m_pLitStr[idx] : throw std::out_of_range("Index out of range.");
        }


        // Getters //

        [[nodiscard]] constexpr bool IsEmpty() const noexcept
        {
            return Length() == 0;
        }

        [[nodiscard]] _Ret_z_ constexpr const T* CStr() const noexcept
        {
            return m_pLitStr;
        }

        [[nodiscard]] constexpr size_t Length() const noexcept
        {
            // ArrayLength includes null-terminator.
            return (ArrayLength - 1);
        }

        // TODO: Implement iterator support.
    };

#define CC_MAKE_STRING_LITERAL(_STR) CC::StringLiteral(_STR)
}
