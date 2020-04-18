#pragma once

// SUTL
#include <UnitTestResult.h>
#include <TestQuantity.h>

// STL
#include <functional>
#include <list>
#include <vector>

// Target Class
#include <CCStringLiteral.h>

// Test Helper Utils
#include <CCStringUtil.h>
#include <TestHelpers.hpp>


namespace CC
{
    class StringLiteralTests
    {
        StringLiteralTests() = delete;
        StringLiteralTests(const StringLiteralTests&) = delete;
        StringLiteralTests(StringLiteralTests&&) = delete;
        ~StringLiteralTests() = delete;
        StringLiteralTests& operator=(const StringLiteralTests&) = delete;
        StringLiteralTests& operator=(StringLiteralTests&&) = delete;

#define STRING_LITERAL_NARROW_TQ_NONE   ""
#define STRING_LITERAL_NARROW_TQ_LOW    "A"
#define STRING_LITERAL_NARROW_TQ_MID    "MidTestString"

#define STRING_LITERAL_WIDE_TQ_NONE     CC_UTF16_LITERAL_STR(STRING_LITERAL_NARROW_TQ_NONE)
#define STRING_LITERAL_WIDE_TQ_LOW      CC_UTF16_LITERAL_STR(STRING_LITERAL_NARROW_TQ_LOW)
#define STRING_LITERAL_WIDE_TQ_MID      CC_UTF16_LITERAL_STR(STRING_LITERAL_NARROW_TQ_MID)

        template <TestQuantity TQ>
        constexpr static bool IsValidTestQuantity()
        {
            return TQ == TestQuantity::None
                || TQ == TestQuantity::Low
                || TQ == TestQuantity::Mid;
        }


        template <typename T, TestQuantity TQ>
        constexpr static std::pair<const T* const, const size_t> GetTestStringLiteralPtrLenPair()
        {
            static_assert(CC::IsSupportedCharType<T>());
            static_assert(IsValidTestQuantity<TQ>());
            if constexpr (std::is_same_v<T, char>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    return std::pair(STRING_LITERAL_NARROW_TQ_NONE, CC_STRING_LITERAL_LEN(STRING_LITERAL_NARROW_TQ_NONE));
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    return std::pair(STRING_LITERAL_NARROW_TQ_LOW, CC_STRING_LITERAL_LEN(STRING_LITERAL_NARROW_TQ_LOW));
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    return std::pair(STRING_LITERAL_NARROW_TQ_MID, CC_STRING_LITERAL_LEN(STRING_LITERAL_NARROW_TQ_MID));
                }
            }
            else // if constexpr (std::is_same_v<T, wchar_t>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    return std::pair(STRING_LITERAL_WIDE_TQ_NONE, CC_STRING_LITERAL_LEN(STRING_LITERAL_WIDE_TQ_NONE));
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    return std::pair(STRING_LITERAL_WIDE_TQ_LOW, CC_STRING_LITERAL_LEN(STRING_LITERAL_WIDE_TQ_LOW));
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    return std::pair(STRING_LITERAL_WIDE_TQ_MID, CC_STRING_LITERAL_LEN(STRING_LITERAL_WIDE_TQ_MID));
                }
            }
        }

    public:

        // Type aliases
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;

        // Tests constructor behavior.
        class ConstructorTests;

        // Tests overloaded operator behavior.
        class OperatorTests;

        // Tests behavior of various Getter methods.
        class GetterTests;

        [[nodiscard]] static UTList GetTests();
    };


    ///
    /// NOTE:   Compilers can, but are not required, to combine storage for equal/overlapping string literals.
    ///         E.g., { const char* a = "Lit"; const char* b = "Lit"; a == b; } is not necessarily true.
    ///         Due to this, we need to actually do string comparisons here rather just compare the address of each literal.
    ///
    /// TODO:   Fix tests where we're comparing the pointer values.
    ///

    class StringLiteralTests::ConstructorTests
    {
    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR LiteralStringCtor()
        {
            constexpr auto ptrLenPair(GetTestStringLiteralPtrLenPair<T, TQ>());

            if constexpr (std::is_same_v<T, char>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_NONE);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_LOW);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_MID);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                }
            }
            else // if constexpr (std::is_same_v<T, wchar_t>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_NONE);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_LOW);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_MID);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CopyCtor()
        {
            constexpr auto ptrLenPair(GetTestStringLiteralPtrLenPair<T, TQ>());

            if constexpr (std::is_same_v<T, char>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_NARROW_TQ_NONE));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(lit);
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_NARROW_TQ_LOW));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(lit);
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_NARROW_TQ_MID));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(lit);
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
            }
            else // if constexpr (std::is_same_v<T, wchar_t>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_WIDE_TQ_NONE));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(lit);
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_WIDE_TQ_LOW));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(lit);
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_WIDE_TQ_MID));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(lit);
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR MoveCtor()
        {
            constexpr auto ptrLenPair(GetTestStringLiteralPtrLenPair<T, TQ>());

            if constexpr (std::is_same_v<T, char>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_NARROW_TQ_NONE));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(std::move(lit));
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_NARROW_TQ_LOW));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(std::move(lit));
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_NARROW_TQ_MID));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(std::move(lit));
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
            }
            else // if constexpr (std::is_same_v<T, wchar_t>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_WIDE_TQ_NONE));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(std::move(lit));
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_WIDE_TQ_LOW));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(std::move(lit));
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr auto lit(CC_MAKE_STRING_LITERAL(STRING_LITERAL_WIDE_TQ_MID));
                    SUTL_TEST_ASSERT(lit.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);

                    constexpr auto lit2(std::move(lit));
                    SUTL_TEST_ASSERT(lit2.m_pLitStr == ptrLenPair.first);
                    SUTL_TEST_ASSERT(lit2.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr == lit2.m_pLitStr);
                    SUTL_TEST_ASSERT(lit.Length() == lit2.Length());
                }
            }

            SUTL_TEST_SUCCESS();
        }
    };

    class StringLiteralTests::OperatorTests
    {
    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR BoolOp()
        {
            if constexpr (std::is_same_v<T, char>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_NONE);
                    SUTL_TEST_ASSERT(!lit);
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_LOW);
                    SUTL_TEST_ASSERT(!!lit);
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_MID);
                    SUTL_TEST_ASSERT(!!lit);
                }
            }
            else // if constexpr (std::is_same_v<T, wchar_t>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_NONE);
                    SUTL_TEST_ASSERT(!lit);
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_LOW);
                    SUTL_TEST_ASSERT(!!lit);
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_MID);
                    SUTL_TEST_ASSERT(!!lit);
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR SubscriptOp()
        {
            constexpr auto ptrLenPair(GetTestStringLiteralPtrLenPair<T, TQ>());
            bool bThrew = false;
            if constexpr (std::is_same_v<T, char>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_NONE);
                    try
                    {
                        auto c = lit[0];
                        c;
                    }
                    catch (const std::out_of_range&)
                    {
                        bThrew = true;
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }

                    SUTL_TEST_ASSERT(bThrew);
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_LOW);
                    for (size_t i = 0; i < ptrLenPair.second; ++i)
                    {
                        SUTL_TEST_ASSERT(lit[i] == ptrLenPair.first[i]);
                    }

                    try
                    {
                        auto c = lit[ptrLenPair.second];
                        c;
                    }
                    catch (const std::out_of_range&)
                    {
                        bThrew = true;
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }

                    SUTL_TEST_ASSERT(bThrew);
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_MID);
                    for (size_t i = 0; i < ptrLenPair.second; ++i)
                    {
                        SUTL_TEST_ASSERT(lit[i] == ptrLenPair.first[i]);
                    }

                    try
                    {
                        auto c = lit[ptrLenPair.second];
                        c;
                    }
                    catch (const std::out_of_range&)
                    {
                        bThrew = true;
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }

                    SUTL_TEST_ASSERT(bThrew);
                }
            }
            else // if constexpr (std::is_same_v<T, wchar_t>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_NONE);
                    for (size_t i = 0; i < ptrLenPair.second; ++i)
                    {
                        SUTL_TEST_ASSERT(lit[i] == ptrLenPair.first[i]);
                    }

                    try
                    {
                        auto c = lit[ptrLenPair.second];
                        c;
                    }
                    catch (const std::out_of_range&)
                    {
                        bThrew = true;
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }

                    SUTL_TEST_ASSERT(bThrew);
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_LOW);
                    for (size_t i = 0; i < ptrLenPair.second; ++i)
                    {
                        SUTL_TEST_ASSERT(lit[i] == ptrLenPair.first[i]);
                    }

                    try
                    {
                        auto c = lit[ptrLenPair.second];
                        c;
                    }
                    catch (const std::out_of_range&)
                    {
                        bThrew = true;
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }

                    SUTL_TEST_ASSERT(bThrew);
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_MID);
                    for (size_t i = 0; i < ptrLenPair.second; ++i)
                    {
                        SUTL_TEST_ASSERT(lit[i] == ptrLenPair.first[i]);
                    }

                    try
                    {
                        auto c = lit[ptrLenPair.second];
                        c;
                    }
                    catch (const std::out_of_range&)
                    {
                        bThrew = true;
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }

                    SUTL_TEST_ASSERT(bThrew);
                }
            }

            SUTL_TEST_SUCCESS();
        }
    };

    class StringLiteralTests::GetterTests
    {
    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR IsEmpty()
        {
            if constexpr (std::is_same_v<T, char>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_NONE);
                    SUTL_TEST_ASSERT(lit.IsEmpty());
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_LOW);
                    SUTL_TEST_ASSERT(!lit.IsEmpty());
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_MID);
                    SUTL_TEST_ASSERT(!lit.IsEmpty());
                }
            }
            else // if constexpr (std::is_same_v<T, wchar_t>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_NONE);
                    SUTL_TEST_ASSERT(lit.IsEmpty());
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_LOW);
                    SUTL_TEST_ASSERT(!lit.IsEmpty());
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_MID);
                    SUTL_TEST_ASSERT(!lit.IsEmpty());
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CStr()
        {
            constexpr auto ptrLenPair(GetTestStringLiteralPtrLenPair<T, TQ>());
            if constexpr (std::is_same_v<T, char>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_NONE);
                    SUTL_TEST_ASSERT(lit.CStr() == ptrLenPair.first);
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_LOW);
                    SUTL_TEST_ASSERT(lit.CStr() == ptrLenPair.first);
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_MID);
                    SUTL_TEST_ASSERT(lit.CStr() == ptrLenPair.first);
                }
            }
            else // if constexpr (std::is_same_v<T, wchar_t>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_NONE);
                    SUTL_TEST_ASSERT(lit.CStr() == ptrLenPair.first);
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_LOW);
                    SUTL_TEST_ASSERT(lit.CStr() == ptrLenPair.first);
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_MID);
                    SUTL_TEST_ASSERT(lit.CStr() == ptrLenPair.first);
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR Length()
        {
            constexpr auto ptrLenPair(GetTestStringLiteralPtrLenPair<T, TQ>());
            if constexpr (std::is_same_v<T, char>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_NONE);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr[lit.Length() + 1] == '\0');
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_LOW);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr[lit.Length() + 1] == '\0');
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_NARROW_TQ_MID);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr[lit.Length() + 1] == '\0');
                }
            }
            else // if constexpr (std::is_same_v<T, wchar_t>)
            {
                if constexpr (TQ == TestQuantity::None)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_NONE);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr[lit.Length() + 1] == L'\0');
                }
                else if constexpr (TQ == TestQuantity::Low)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_LOW);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr[lit.Length() + 1] == L'\0');
                }
                else // if constexpr (TQ == TestQuantity::Mid)
                {
                    constexpr StringLiteral lit(STRING_LITERAL_WIDE_TQ_MID);
                    SUTL_TEST_ASSERT(lit.Length() == ptrLenPair.second);
                    SUTL_TEST_ASSERT(lit.m_pLitStr[lit.Length() + 1] == L'\0');
                }
            }

            SUTL_TEST_SUCCESS();
        }
    };
}

#undef STRING_LITERAL_NARROW_TQ_NONE
#undef STRING_LITERAL_NARROW_TQ_LOW
#undef STRING_LITERAL_NARROW_TQ_MID
#undef STRING_LITERAL_NARROW_TQ_HIGH

#undef STRING_LITERAL_WIDE_TQ_NONE
#undef STRING_LITERAL_WIDE_TQ_LOW
#undef STRING_LITERAL_WIDE_TQ_MID
#undef STRING_LITERAL_WIDE_TQ_HIGH
