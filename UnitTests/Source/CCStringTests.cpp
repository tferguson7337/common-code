#include <CCStringTests.h>


// Initialize Static Test Helper Data //

const std::vector<CC::SupportedStringTuple> CC::StringTests::ms_TestStrings
{
    CC_MAKE_STR_TUPLE(""),
    CC_MAKE_STR_TUPLE("A short cstring."),
    CC_MAKE_STR_TUPLE("This is a string that has too many chars to fit in an SSO array.")
};

const std::vector<CC::SupportedStringTuple> CC::StringTests::ms_DiffCaseTestStrings
{
    CC_MAKE_STR_TUPLE(""),
    CC_MAKE_STR_TUPLE("A SHORT CSTRING."),
    CC_MAKE_STR_TUPLE("ThIs Is A sTrInG tHaT hAs ToO mAnY cHaRs To FiT iN aN sSo ArRaY.")
};

const std::vector<CC::SupportedStringTuple> CC::StringTests::ms_ShorterTestStrings
{
    CC_MAKE_STR_TUPLE(""),
    CC_MAKE_STR_TUPLE("A short cstr"),
    CC_MAKE_STR_TUPLE("This is a string")
};

const std::vector<CC::SupportedStringTuple> CC::StringTests::ms_LongerTestStrings
{
    CC_MAKE_STR_TUPLE(""),
    CC_MAKE_STR_TUPLE("A short cstring.  Fits in SSO."),
    CC_MAKE_STR_TUPLE("This is a string that has too many chars to fit in an SSO array.  Cool stuff.")
};


// Add-String-Test Helper Macros //

// Add 2 Tests (2 Char types)
#define ADD_STR_TESTS_C(_fn_)\
    _fn_<char>, _fn_<wchar_t>

// Add 6 Tests (2 Char types x 3 TQ)
#define ADD_STR_TESTS_C_TQ1(_fn_)\
    _fn_<char,    TestQuantity::None>, _fn_<char,    TestQuantity::MidLow>, _fn_<char,    TestQuantity::Mid>,\
    _fn_<wchar_t, TestQuantity::None>, _fn_<wchar_t, TestQuantity::MidLow>, _fn_<wchar_t, TestQuantity::Mid>

// Add 12 Tests (2 Char types x 3 TQ x T/F)
#define ADD_STR_TESTS_C_TQ1_B(_fn_)\
    _fn_<char,    TestQuantity::None,   false>, _fn_<char,    TestQuantity::None,   true>,\
    _fn_<char,    TestQuantity::MidLow, false>, _fn_<char,    TestQuantity::MidLow, true>,\
    _fn_<char,    TestQuantity::Mid,    false>, _fn_<char,    TestQuantity::Mid,    true>,\
    _fn_<wchar_t, TestQuantity::None,   false>, _fn_<wchar_t, TestQuantity::None,   true>,\
    _fn_<wchar_t, TestQuantity::MidLow, false>, _fn_<wchar_t, TestQuantity::MidLow, true>,\
    _fn_<wchar_t, TestQuantity::Mid,    false>, _fn_<wchar_t, TestQuantity::Mid,    true>

// Add 36 Tests (2 Char Types x 3 TQ x 3 TQ x T/F)
#define ADD_STR_TESTS_C_TQ2_B(_fn_)\
    _fn_<char,    TestQuantity::None,   TestQuantity::None,   false>, _fn_<char,     TestQuantity::None,   TestQuantity::None,   true>,\
    _fn_<char,    TestQuantity::None,   TestQuantity::MidLow, false>, _fn_<char,     TestQuantity::None,   TestQuantity::MidLow, true>,\
    _fn_<char,    TestQuantity::None,   TestQuantity::Mid,    false>, _fn_<char,     TestQuantity::None,   TestQuantity::Mid,    true>,\
    _fn_<char,    TestQuantity::MidLow, TestQuantity::None,   false>, _fn_<char,     TestQuantity::MidLow, TestQuantity::None,   true>,\
    _fn_<char,    TestQuantity::MidLow, TestQuantity::MidLow, false>, _fn_<char,     TestQuantity::MidLow, TestQuantity::MidLow, true>,\
    _fn_<char,    TestQuantity::MidLow, TestQuantity::Mid,    false>, _fn_<char,     TestQuantity::MidLow, TestQuantity::Mid,    true>,\
    _fn_<char,    TestQuantity::Mid,    TestQuantity::None,   false>, _fn_<char,     TestQuantity::Mid,    TestQuantity::None,   true>,\
    _fn_<char,    TestQuantity::Mid,    TestQuantity::MidLow, false>, _fn_<char,     TestQuantity::Mid,    TestQuantity::MidLow, true>,\
    _fn_<char,    TestQuantity::Mid,    TestQuantity::Mid,    false>, _fn_<char,     TestQuantity::Mid,    TestQuantity::Mid,    true>,\
    _fn_<wchar_t, TestQuantity::None,   TestQuantity::None,   false>, _fn_<wchar_t,  TestQuantity::None,   TestQuantity::None,   true>,\
    _fn_<wchar_t, TestQuantity::None,   TestQuantity::MidLow, false>, _fn_<wchar_t,  TestQuantity::None,   TestQuantity::MidLow, true>,\
    _fn_<wchar_t, TestQuantity::None,   TestQuantity::Mid,    false>, _fn_<wchar_t,  TestQuantity::None,   TestQuantity::Mid,    true>,\
    _fn_<wchar_t, TestQuantity::MidLow, TestQuantity::None,   false>, _fn_<wchar_t,  TestQuantity::MidLow, TestQuantity::None,   true>,\
    _fn_<wchar_t, TestQuantity::MidLow, TestQuantity::MidLow, false>, _fn_<wchar_t,  TestQuantity::MidLow, TestQuantity::MidLow, true>,\
    _fn_<wchar_t, TestQuantity::MidLow, TestQuantity::Mid,    false>, _fn_<wchar_t,  TestQuantity::MidLow, TestQuantity::Mid,    true>,\
    _fn_<wchar_t, TestQuantity::Mid,    TestQuantity::None,   false>, _fn_<wchar_t,  TestQuantity::Mid,    TestQuantity::None,   true>,\
    _fn_<wchar_t, TestQuantity::Mid,    TestQuantity::MidLow, false>, _fn_<wchar_t,  TestQuantity::Mid,    TestQuantity::MidLow, true>,\
    _fn_<wchar_t, TestQuantity::Mid,    TestQuantity::Mid,    false>, _fn_<wchar_t,  TestQuantity::Mid,    TestQuantity::Mid,    true>

// Return list of String unit tests.
CC::StringTests::UTList CC::StringTests::GetTests()
{
#pragma warning(suppress : 6262) // Suppress Mid stack memory usage warning.
    static const UTList tests
    {
        // Constructor Tests
        ADD_STR_TESTS_C(ConstructorTests::DefaultCtor),
        ADD_STR_TESTS_C_TQ1(ConstructorTests::LenAndDefaultCharCtor),
        ADD_STR_TESTS_C_TQ1(ConstructorTests::LenAndCharCtor),
        ADD_STR_TESTS_C(ConstructorTests::CStrCtorNullptr),
        ADD_STR_TESTS_C_TQ1(ConstructorTests::CStrCtor),
        ADD_STR_TESTS_C_TQ1(ConstructorTests::PtrAndLenCtorNullptr),
        ADD_STR_TESTS_C_TQ1(ConstructorTests::PtrAndLenCtor),
        ADD_STR_TESTS_C_TQ1(ConstructorTests::CopyCtor),
        ADD_STR_TESTS_C_TQ1(ConstructorTests::MoveCtor),

        // Assignment Operator Tests
        ADD_STR_TESTS_C_TQ1(AssignmentOperatorTests::CStrAssignOpNullptr),
        ADD_STR_TESTS_C_TQ1(AssignmentOperatorTests::CStrAssignOp),
        ADD_STR_TESTS_C_TQ1(AssignmentOperatorTests::CopyAssignOp),
        ADD_STR_TESTS_C_TQ1(AssignmentOperatorTests::MoveAssignOp),

        // Add-Assignment Operator Tests
        ADD_STR_TESTS_C_TQ1(AddAssignmentOperatorTests::CStrAddAssignOpNullptr),
        ADD_STR_TESTS_C_TQ1(AddAssignmentOperatorTests::CStrAddAssignOp),
        ADD_STR_TESTS_C_TQ1(AddAssignmentOperatorTests::StrObjAddAssignOp),

        // Subscript Operator Tests
        ADD_STR_TESTS_C_TQ1(SubscriptOperatorTests::SubscriptOp),
        ADD_STR_TESTS_C_TQ1(SubscriptOperatorTests::SubscriptOp),

        // Equivalency Operator Tests
        ADD_STR_TESTS_C_TQ1(EquivalencyOperatorTests::CStrEquivOp),
        ADD_STR_TESTS_C_TQ1(EquivalencyOperatorTests::StrObjEquivOp),

        // Getter Tests
        ADD_STR_TESTS_C_TQ1(GetterTests::CStr),
        ADD_STR_TESTS_C_TQ1(GetterTests::Length),
        ADD_STR_TESTS_C_TQ1(GetterTests::Capacity),
        ADD_STR_TESTS_C_TQ1(GetterTests::Front),
        ADD_STR_TESTS_C_TQ1(GetterTests::Back),

        // Assignment Tests
        ADD_STR_TESTS_C_TQ1(Assign::ChAssign),
        ADD_STR_TESTS_C_TQ1(Assign::CStrAssign),
        ADD_STR_TESTS_C_TQ1(Assign::PtrAndLenAssign),
        ADD_STR_TESTS_C_TQ1(Assign::StrObjCopyAssign),
        ADD_STR_TESTS_C_TQ1(Assign::StrObjAndLenCopyAssign),
        ADD_STR_TESTS_C_TQ1(Assign::StrObjMoveAssign),

        // Append Tests
        ADD_STR_TESTS_C_TQ1(Append::ChAppend),
        ADD_STR_TESTS_C_TQ1(Append::CStrAppend),
        ADD_STR_TESTS_C_TQ1(Append::PtrAndLenAppend),
        ADD_STR_TESTS_C_TQ1(Append::StrObjAppend),
        ADD_STR_TESTS_C_TQ1(Append::StrObjAndLenAppend),

        // Compare Tests
        ADD_STR_TESTS_C_TQ1_B(CompareTests::CStrCompare),
        ADD_STR_TESTS_C_TQ2_B(CompareTests::StrAndLenCompare),
        ADD_STR_TESTS_C_TQ1_B(CompareTests::StrObjCompare),
        ADD_STR_TESTS_C_TQ2_B(CompareTests::StrObjAndLenCompare),

        // Clear Tests
        ADD_STR_TESTS_C_TQ1(ClearTests::Clear),

        // IsEmpty Tests
        ADD_STR_TESTS_C_TQ1(IsEmptyTests::IsEmpty)
    };

    return tests;
}