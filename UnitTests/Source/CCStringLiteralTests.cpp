#include <CCStringLiteralTests.h>

// Add 2 Tests (2 Char types)
#define ADD_STR_LIT_TESTS_C(_fn_)\
    _fn_<char>, _fn_<wchar_t>

// Add 6 Tests (2 Char types x 3 TQ)
#define ADD_STR_LIT_TESTS_C_TQ1(_fn_)\
    _fn_<char,    TestQuantity::None>, _fn_<char,    TestQuantity::Low>,    _fn_<char,    TestQuantity::Mid>,\
    _fn_<wchar_t, TestQuantity::None>, _fn_<wchar_t, TestQuantity::Low>,    _fn_<wchar_t, TestQuantity::Mid>

CC::StringLiteralTests::UTList CC::StringLiteralTests::GetTests()
{
    static const UTList tests
    {
        ADD_STR_LIT_TESTS_C_TQ1(ConstructorTests::LiteralStringCtor),
        ADD_STR_LIT_TESTS_C_TQ1(ConstructorTests::CopyCtor),
        ADD_STR_LIT_TESTS_C_TQ1(ConstructorTests::MoveCtor),

        ADD_STR_LIT_TESTS_C_TQ1(OperatorTests::BoolOp),
        ADD_STR_LIT_TESTS_C_TQ1(OperatorTests::SubscriptOp),

        ADD_STR_LIT_TESTS_C_TQ1(GetterTests::IsEmpty),
        ADD_STR_LIT_TESTS_C_TQ1(GetterTests::CStr),
        ADD_STR_LIT_TESTS_C_TQ1(GetterTests::Length)
    };

    return tests;
}

#undef ADD_STR_LIT_TESTS_C
#undef ADD_STR_LIT_TESTS_C_TQ1
