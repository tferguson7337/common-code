#include <CCStringTests.h>


/// Initialize Static Test Helper Data \\\

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


// Return list of String unit tests.
CC::StringTests::UTList CC::StringTests::GetTests()
{
    using TQ = TestQuantity;

#pragma warning(suppress : 6262) // Suppress Mid stack memory usage warning.
    static const UTList tests
    {
        ConstructorTests::DefaultCtor<utf8>,
        ConstructorTests::DefaultCtor<utf16>,

        ConstructorTests::LenAndCharCtor<utf8, TQ::None>,
        ConstructorTests::LenAndCharCtor<utf8, TQ::MidLow>,
        ConstructorTests::LenAndCharCtor<utf8, TQ::Mid>,
        ConstructorTests::LenAndCharCtor<utf16, TQ::None>,
        ConstructorTests::LenAndCharCtor<utf16, TQ::MidLow>,
        ConstructorTests::LenAndCharCtor<utf16, TQ::Mid>,

        ConstructorTests::CStrCtorNullptr<utf8>,
        ConstructorTests::CStrCtorNullptr<utf16>,

        ConstructorTests::CStrCtor<utf8, TQ::None>,
        ConstructorTests::CStrCtor<utf8, TQ::MidLow>,
        ConstructorTests::CStrCtor<utf8, TQ::Mid>,
        ConstructorTests::CStrCtor<utf16, TQ::None>,
        ConstructorTests::CStrCtor<utf16, TQ::MidLow>,
        ConstructorTests::CStrCtor<utf16, TQ::Mid>,

        ConstructorTests::PtrAndLenCtorNullptr<utf8, TQ::None>,
        ConstructorTests::PtrAndLenCtorNullptr<utf8, TQ::MidLow>,
        ConstructorTests::PtrAndLenCtorNullptr<utf8, TQ::Mid>,
        ConstructorTests::PtrAndLenCtorNullptr<utf16, TQ::None>,
        ConstructorTests::PtrAndLenCtorNullptr<utf16, TQ::MidLow>,
        ConstructorTests::PtrAndLenCtorNullptr<utf16, TQ::Mid>,

        ConstructorTests::PtrAndLenCtor<utf8, TQ::None>,
        ConstructorTests::PtrAndLenCtor<utf8, TQ::MidLow>,
        ConstructorTests::PtrAndLenCtor<utf8, TQ::Mid>,
        ConstructorTests::PtrAndLenCtor<utf16, TQ::None>,
        ConstructorTests::PtrAndLenCtor<utf16, TQ::MidLow>,
        ConstructorTests::PtrAndLenCtor<utf16, TQ::Mid>,

        ConstructorTests::CopyCtor<utf8, TQ::None>,
        ConstructorTests::CopyCtor<utf8, TQ::MidLow>,
        ConstructorTests::CopyCtor<utf8, TQ::Mid>,
        ConstructorTests::CopyCtor<utf16, TQ::None>,
        ConstructorTests::CopyCtor<utf16, TQ::MidLow>,
        ConstructorTests::CopyCtor<utf16, TQ::Mid>,

        ConstructorTests::MoveCtor<utf8, TQ::None>,
        ConstructorTests::MoveCtor<utf8, TQ::MidLow>,
        ConstructorTests::MoveCtor<utf8, TQ::Mid>,
        ConstructorTests::MoveCtor<utf16, TQ::None>,
        ConstructorTests::MoveCtor<utf16, TQ::MidLow>,
        ConstructorTests::MoveCtor<utf16, TQ::Mid>,

        AssignmentOperatorTests::CStrAssignOpNullptr<utf8, TQ::None>,
        AssignmentOperatorTests::CStrAssignOpNullptr<utf8, TQ::MidLow>,
        AssignmentOperatorTests::CStrAssignOpNullptr<utf8, TQ::Mid>,
        AssignmentOperatorTests::CStrAssignOpNullptr<utf16, TQ::None>,
        AssignmentOperatorTests::CStrAssignOpNullptr<utf16, TQ::MidLow>,
        AssignmentOperatorTests::CStrAssignOpNullptr<utf16, TQ::Mid>,

        AssignmentOperatorTests::CStrAssignOp<utf8, TQ::None>,
        AssignmentOperatorTests::CStrAssignOp<utf8, TQ::MidLow>,
        AssignmentOperatorTests::CStrAssignOp<utf8, TQ::Mid>,
        AssignmentOperatorTests::CStrAssignOp<utf16, TQ::None>,
        AssignmentOperatorTests::CStrAssignOp<utf16, TQ::MidLow>,
        AssignmentOperatorTests::CStrAssignOp<utf16, TQ::Mid>,

        AssignmentOperatorTests::CopyAssignOp<utf8, TQ::None>,
        AssignmentOperatorTests::CopyAssignOp<utf8, TQ::MidLow>,
        AssignmentOperatorTests::CopyAssignOp<utf8, TQ::Mid>,
        AssignmentOperatorTests::CopyAssignOp<utf16, TQ::None>,
        AssignmentOperatorTests::CopyAssignOp<utf16, TQ::MidLow>,
        AssignmentOperatorTests::CopyAssignOp<utf16, TQ::Mid>,

        AssignmentOperatorTests::MoveAssignOp<utf8, TQ::None>,
        AssignmentOperatorTests::MoveAssignOp<utf8, TQ::MidLow>,
        AssignmentOperatorTests::MoveAssignOp<utf8, TQ::Mid>,
        AssignmentOperatorTests::MoveAssignOp<utf16, TQ::None>,
        AssignmentOperatorTests::MoveAssignOp<utf16, TQ::MidLow>,
        AssignmentOperatorTests::MoveAssignOp<utf16, TQ::Mid>,

        AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf8, TQ::None>,
        AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf8, TQ::MidLow>,
        AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf8, TQ::Mid>,
        AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf16, TQ::None>,
        AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf16, TQ::MidLow>,
        AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf16, TQ::Mid>,

        AddAssignmentOperatorTests::CStrAddAssignOp<utf8, TQ::None>,
        AddAssignmentOperatorTests::CStrAddAssignOp<utf8, TQ::MidLow>,
        AddAssignmentOperatorTests::CStrAddAssignOp<utf8, TQ::Mid>,
        AddAssignmentOperatorTests::CStrAddAssignOp<utf16, TQ::None>,
        AddAssignmentOperatorTests::CStrAddAssignOp<utf16, TQ::MidLow>,
        AddAssignmentOperatorTests::CStrAddAssignOp<utf16, TQ::Mid>,

        AddAssignmentOperatorTests::StrObjAddAssignOp<utf8, TQ::None>,
        AddAssignmentOperatorTests::StrObjAddAssignOp<utf8, TQ::MidLow>,
        AddAssignmentOperatorTests::StrObjAddAssignOp<utf8, TQ::Mid>,
        AddAssignmentOperatorTests::StrObjAddAssignOp<utf16, TQ::None>,
        AddAssignmentOperatorTests::StrObjAddAssignOp<utf16, TQ::MidLow>,
        AddAssignmentOperatorTests::StrObjAddAssignOp<utf16, TQ::Mid>,

        SubscriptOperatorTests::SubscriptOp<utf8, TQ::None>,
        SubscriptOperatorTests::SubscriptOp<utf8, TQ::MidLow>,
        SubscriptOperatorTests::SubscriptOp<utf8, TQ::Mid>,
        SubscriptOperatorTests::SubscriptOp<utf16, TQ::None>,
        SubscriptOperatorTests::SubscriptOp<utf16, TQ::MidLow>,
        SubscriptOperatorTests::SubscriptOp<utf16, TQ::Mid>,

        EquivalencyOperatorTests::CStrEquivOp<utf8, TQ::None>,
        EquivalencyOperatorTests::CStrEquivOp<utf8, TQ::MidLow>,
        EquivalencyOperatorTests::CStrEquivOp<utf8, TQ::Mid>,
        EquivalencyOperatorTests::CStrEquivOp<utf16, TQ::None>,
        EquivalencyOperatorTests::CStrEquivOp<utf16, TQ::MidLow>,
        EquivalencyOperatorTests::CStrEquivOp<utf16, TQ::Mid>,

        EquivalencyOperatorTests::StrObjEquivOp<utf8, TQ::None>,
        EquivalencyOperatorTests::StrObjEquivOp<utf8, TQ::MidLow>,
        EquivalencyOperatorTests::StrObjEquivOp<utf8, TQ::Mid>,
        EquivalencyOperatorTests::StrObjEquivOp<utf16, TQ::None>,
        EquivalencyOperatorTests::StrObjEquivOp<utf16, TQ::MidLow>,
        EquivalencyOperatorTests::StrObjEquivOp<utf16, TQ::Mid>,

        GetterTests::CStr<utf8, TQ::None>,
        GetterTests::CStr<utf8, TQ::MidLow>,
        GetterTests::CStr<utf8, TQ::Mid>,
        GetterTests::CStr<utf16, TQ::None>,
        GetterTests::CStr<utf16, TQ::MidLow>,
        GetterTests::CStr<utf16, TQ::Mid>,

        GetterTests::Length<utf8, TQ::None>,
        GetterTests::Length<utf8, TQ::MidLow>,
        GetterTests::Length<utf8, TQ::Mid>,
        GetterTests::Length<utf16, TQ::None>,
        GetterTests::Length<utf16, TQ::MidLow>,
        GetterTests::Length<utf16, TQ::Mid>,

        GetterTests::Capacity<utf8, TQ::None>,
        GetterTests::Capacity<utf8, TQ::MidLow>,
        GetterTests::Capacity<utf8, TQ::Mid>,
        GetterTests::Capacity<utf16, TQ::None>,
        GetterTests::Capacity<utf16, TQ::MidLow>,
        GetterTests::Capacity<utf16, TQ::Mid>,

        GetterTests::Front<utf8, TQ::None>,
        GetterTests::Front<utf8, TQ::MidLow>,
        GetterTests::Front<utf8, TQ::Mid>,
        GetterTests::Front<utf16, TQ::None>,
        GetterTests::Front<utf16, TQ::MidLow>,
        GetterTests::Front<utf16, TQ::Mid>,

        GetterTests::Back<utf8, TQ::None>,
        GetterTests::Back<utf8, TQ::MidLow>,
        GetterTests::Back<utf8, TQ::Mid>,
        GetterTests::Back<utf16, TQ::None>,
        GetterTests::Back<utf16, TQ::MidLow>,
        GetterTests::Back<utf16, TQ::Mid>,

        Assign::ChAssign<utf8, TQ::None>,
        Assign::ChAssign<utf8, TQ::MidLow>,
        Assign::ChAssign<utf8, TQ::Mid>,
        Assign::ChAssign<utf16, TQ::None>,
        Assign::ChAssign<utf16, TQ::MidLow>,
        Assign::ChAssign<utf16, TQ::Mid>,

        Assign::CStrAssign<utf8, TQ::None>,
        Assign::CStrAssign<utf8, TQ::MidLow>,
        Assign::CStrAssign<utf8, TQ::Mid>,
        Assign::CStrAssign<utf16, TQ::None>,
        Assign::CStrAssign<utf16, TQ::MidLow>,
        Assign::CStrAssign<utf16, TQ::Mid>,

        Assign::PtrAndLenAssign<utf8, TQ::None>,
        Assign::PtrAndLenAssign<utf8, TQ::MidLow>,
        Assign::PtrAndLenAssign<utf8, TQ::Mid>,
        Assign::PtrAndLenAssign<utf16, TQ::None>,
        Assign::PtrAndLenAssign<utf16, TQ::MidLow>,
        Assign::PtrAndLenAssign<utf16, TQ::Mid>,

        Assign::StrObjCopyAssign<utf8, TQ::None>,
        Assign::StrObjCopyAssign<utf8, TQ::MidLow>,
        Assign::StrObjCopyAssign<utf8, TQ::Mid>,
        Assign::StrObjCopyAssign<utf16, TQ::None>,
        Assign::StrObjCopyAssign<utf16, TQ::MidLow>,
        Assign::StrObjCopyAssign<utf16, TQ::Mid>,

        Assign::StrObjAndLenCopyAssign<utf8, TQ::None>,
        Assign::StrObjAndLenCopyAssign<utf8, TQ::MidLow>,
        Assign::StrObjAndLenCopyAssign<utf8, TQ::Mid>,
        Assign::StrObjAndLenCopyAssign<utf16, TQ::None>,
        Assign::StrObjAndLenCopyAssign<utf16, TQ::MidLow>,
        Assign::StrObjAndLenCopyAssign<utf16, TQ::Mid>,

        Assign::StrObjMoveAssign<utf8, TQ::None>,
        Assign::StrObjMoveAssign<utf8, TQ::MidLow>,
        Assign::StrObjMoveAssign<utf8, TQ::Mid>,
        Assign::StrObjMoveAssign<utf16, TQ::None>,
        Assign::StrObjMoveAssign<utf16, TQ::MidLow>,
        Assign::StrObjMoveAssign<utf16, TQ::Mid>,

        Append::ChAppend<utf8, TQ::None>,
        Append::ChAppend<utf8, TQ::MidLow>,
        Append::ChAppend<utf8, TQ::Mid>,
        Append::ChAppend<utf16, TQ::None>,
        Append::ChAppend<utf16, TQ::MidLow>,
        Append::ChAppend<utf16, TQ::Mid>,

        Append::CStrAppend<utf8, TQ::None>,
        Append::CStrAppend<utf8, TQ::MidLow>,
        Append::CStrAppend<utf8, TQ::Mid>,
        Append::CStrAppend<utf16, TQ::None>,
        Append::CStrAppend<utf16, TQ::MidLow>,
        Append::CStrAppend<utf16, TQ::Mid>,

        Append::PtrAndLenAppend<utf8, TQ::None>,
        Append::PtrAndLenAppend<utf8, TQ::MidLow>,
        Append::PtrAndLenAppend<utf8, TQ::Mid>,
        Append::PtrAndLenAppend<utf16, TQ::None>,
        Append::PtrAndLenAppend<utf16, TQ::MidLow>,
        Append::PtrAndLenAppend<utf16, TQ::Mid>,

        Append::StrObjAppend<utf8, TQ::None>,
        Append::StrObjAppend<utf8, TQ::MidLow>,
        Append::StrObjAppend<utf8, TQ::Mid>,
        Append::StrObjAppend<utf16, TQ::None>,
        Append::StrObjAppend<utf16, TQ::MidLow>,
        Append::StrObjAppend<utf16, TQ::Mid>,

        Append::StrObjAndLenAppend<utf8, TQ::None>,
        Append::StrObjAndLenAppend<utf8, TQ::MidLow>,
        Append::StrObjAndLenAppend<utf8, TQ::Mid>,
        Append::StrObjAndLenAppend<utf16, TQ::None>,
        Append::StrObjAndLenAppend<utf16, TQ::MidLow>,
        Append::StrObjAndLenAppend<utf16, TQ::Mid>,

        CompareTests::CStrCompare<utf8, TQ::None, false>,
        CompareTests::CStrCompare<utf8, TQ::None, true>,
        CompareTests::CStrCompare<utf8, TQ::MidLow, false>,
        CompareTests::CStrCompare<utf8, TQ::MidLow, true>,
        CompareTests::CStrCompare<utf8, TQ::Mid, false>,
        CompareTests::CStrCompare<utf8, TQ::Mid, true>,
        CompareTests::CStrCompare<utf16, TQ::None, false>,
        CompareTests::CStrCompare<utf16, TQ::None, true>,
        CompareTests::CStrCompare<utf16, TQ::MidLow, false>,
        CompareTests::CStrCompare<utf16, TQ::MidLow, true>,
        CompareTests::CStrCompare<utf16, TQ::Mid, false>,
        CompareTests::CStrCompare<utf16, TQ::Mid, true>,

        CompareTests::StrAndLenCompare<utf8, TQ::None, TQ::None, false>,
        CompareTests::StrAndLenCompare<utf8, TQ::None, TQ::None, true>,
        CompareTests::StrAndLenCompare<utf8, TQ::None, TQ::MidLow, false>,
        CompareTests::StrAndLenCompare<utf8, TQ::None, TQ::MidLow, true>,
        CompareTests::StrAndLenCompare<utf8, TQ::None, TQ::Mid, false>,
        CompareTests::StrAndLenCompare<utf8, TQ::None, TQ::Mid, true>,
        CompareTests::StrAndLenCompare<utf8, TQ::MidLow, TQ::None, false>,
        CompareTests::StrAndLenCompare<utf8, TQ::MidLow, TQ::None, true>,
        CompareTests::StrAndLenCompare<utf8, TQ::MidLow, TQ::MidLow, false>,
        CompareTests::StrAndLenCompare<utf8, TQ::MidLow, TQ::MidLow, true>,
        CompareTests::StrAndLenCompare<utf8, TQ::MidLow, TQ::Mid, false>,
        CompareTests::StrAndLenCompare<utf8, TQ::MidLow, TQ::Mid, true>,
        CompareTests::StrAndLenCompare<utf8, TQ::Mid, TQ::None, false>,
        CompareTests::StrAndLenCompare<utf8, TQ::Mid, TQ::None, true>,
        CompareTests::StrAndLenCompare<utf8, TQ::Mid, TQ::MidLow, false>,
        CompareTests::StrAndLenCompare<utf8, TQ::Mid, TQ::MidLow, true>,
        CompareTests::StrAndLenCompare<utf8, TQ::Mid, TQ::Mid, false>,
        CompareTests::StrAndLenCompare<utf8, TQ::Mid, TQ::Mid, true>,
        CompareTests::StrAndLenCompare<utf16, TQ::None, TQ::None, false>,
        CompareTests::StrAndLenCompare<utf16, TQ::None, TQ::None, true>,
        CompareTests::StrAndLenCompare<utf16, TQ::None, TQ::MidLow, false>,
        CompareTests::StrAndLenCompare<utf16, TQ::None, TQ::MidLow, true>,
        CompareTests::StrAndLenCompare<utf16, TQ::None, TQ::Mid, false>,
        CompareTests::StrAndLenCompare<utf16, TQ::None, TQ::Mid, true>,
        CompareTests::StrAndLenCompare<utf16, TQ::MidLow, TQ::None, false>,
        CompareTests::StrAndLenCompare<utf16, TQ::MidLow, TQ::None, true>,
        CompareTests::StrAndLenCompare<utf16, TQ::MidLow, TQ::MidLow, false>,
        CompareTests::StrAndLenCompare<utf16, TQ::MidLow, TQ::MidLow, true>,
        CompareTests::StrAndLenCompare<utf16, TQ::MidLow, TQ::Mid, false>,
        CompareTests::StrAndLenCompare<utf16, TQ::MidLow, TQ::Mid, true>,
        CompareTests::StrAndLenCompare<utf16, TQ::Mid, TQ::None, false>,
        CompareTests::StrAndLenCompare<utf16, TQ::Mid, TQ::None, true>,
        CompareTests::StrAndLenCompare<utf16, TQ::Mid, TQ::MidLow, false>,
        CompareTests::StrAndLenCompare<utf16, TQ::Mid, TQ::MidLow, true>,
        CompareTests::StrAndLenCompare<utf16, TQ::Mid, TQ::Mid, false>,
        CompareTests::StrAndLenCompare<utf16, TQ::Mid, TQ::Mid, true>,

        CompareTests::StrObjCompare<utf8, TQ::None, false>,
        CompareTests::StrObjCompare<utf8, TQ::None, true>,
        CompareTests::StrObjCompare<utf8, TQ::MidLow, false>,
        CompareTests::StrObjCompare<utf8, TQ::MidLow, true>,
        CompareTests::StrObjCompare<utf8, TQ::Mid, false>,
        CompareTests::StrObjCompare<utf8, TQ::Mid, true>,
        CompareTests::StrObjCompare<utf16, TQ::None, false>,
        CompareTests::StrObjCompare<utf16, TQ::None, true>,
        CompareTests::StrObjCompare<utf16, TQ::MidLow, false>,
        CompareTests::StrObjCompare<utf16, TQ::MidLow, true>,
        CompareTests::StrObjCompare<utf16, TQ::Mid, false>,
        CompareTests::StrObjCompare<utf16, TQ::Mid, true>,

        CompareTests::StrObjAndLenCompare<utf8, TQ::None, TQ::None, false>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::None, TQ::None, true>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::None, TQ::MidLow, false>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::None, TQ::MidLow, true>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::None, TQ::Mid, false>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::None, TQ::Mid, true>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::MidLow, TQ::None, false>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::MidLow, TQ::None, true>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::MidLow, TQ::MidLow, false>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::MidLow, TQ::MidLow, true>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::MidLow, TQ::Mid, false>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::MidLow, TQ::Mid, true>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::Mid, TQ::None, false>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::Mid, TQ::None, true>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::Mid, TQ::MidLow, false>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::Mid, TQ::MidLow, true>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::Mid, TQ::Mid, false>,
        CompareTests::StrObjAndLenCompare<utf8, TQ::Mid, TQ::Mid, true>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::None, TQ::None, false>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::None, TQ::None, true>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::None, TQ::MidLow, false>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::None, TQ::MidLow, true>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::None, TQ::Mid, false>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::None, TQ::Mid, true>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::MidLow, TQ::None, false>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::MidLow, TQ::None, true>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::MidLow, TQ::MidLow, false>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::MidLow, TQ::MidLow, true>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::MidLow, TQ::Mid, false>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::MidLow, TQ::Mid, true>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::Mid, TQ::None, false>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::Mid, TQ::None, true>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::Mid, TQ::MidLow, false>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::Mid, TQ::MidLow, true>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::Mid, TQ::Mid, false>,
        CompareTests::StrObjAndLenCompare<utf16, TQ::Mid, TQ::Mid, true>,

        ClearTests::Clear<utf8, TQ::None>,
        ClearTests::Clear<utf8, TQ::MidLow>,
        ClearTests::Clear<utf8, TQ::Mid>,
        ClearTests::Clear<utf16, TQ::None>,
        ClearTests::Clear<utf16, TQ::MidLow>,
        ClearTests::Clear<utf16, TQ::Mid>,

        IsEmptyTests::IsEmpty<utf8, TQ::None>,
        IsEmptyTests::IsEmpty<utf8, TQ::MidLow>,
        IsEmptyTests::IsEmpty<utf8, TQ::Mid>,
        IsEmptyTests::IsEmpty<utf16, TQ::None>,
        IsEmptyTests::IsEmpty<utf16, TQ::MidLow>,
        IsEmptyTests::IsEmpty<utf16, TQ::Mid>,
    };

    return tests;
}