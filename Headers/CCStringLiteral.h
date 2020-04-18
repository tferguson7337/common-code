#pragma once

// CC
#include "CCMacros.h"
#include "CCArrayIterator.h"


namespace CC
{
    template <typename T, size_t ArrayLength, CC_ENABLE_IF_SUPPORTED_CHARACTER_TYPE(T)>
    class [[nodiscard]] StringLiteral
    {
        friend class StringLiteralTests;

        static_assert(ArrayLength > 0, "StringLiteral: Somehow received zero-length string literal.");

    public:


    private:

        /// Private Data Members \\\

        const T* const m_pLitStr;

    public:

        /// Ctors \\\

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


        /// Operator Overloads \\\

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return !IsEmpty();
        }

        [[nodiscard]] constexpr T operator[](_In_ const size_t idx) const
        {
            return (idx < Length()) ? m_pLitStr[idx] : throw std::out_of_range(__FUNCSIG__ ": Index out of range.");
        }


        /// Getters \\\

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


        /// Iterator Support \\\

        using ConstIterator = ArrayIterator<const StringLiteral<T, ArrayLength>, T, ptrdiff_t, const T*, const T&>;

        [[nodiscard]] constexpr ConstIterator begin() const noexcept
        {
            return ConstIterator(this, CStr());
        }

        [[nodiscard]] constexpr ConstIterator end() const noexcept
        {
            // End iterator corresponds to the string's null-terminator.
            return ConstIterator(this, CStr() + Length());
        }

        [[nodiscard]] constexpr ConstIterator cbegin() const noexcept
        {
            return begin();
        }

        [[nodiscard]] constexpr ConstIterator cend() const noexcept
        {
            return end();
        }
    };

#define CC_MAKE_STRING_LITERAL(_STR) CC::StringLiteral(_STR)
}
