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

#pragma warning(suppress : 6262) // Suppress high stack memory usage warning.
    static const UTList tests
    {
        StringTests::ConstructorTests::DefaultCtor<utf8>,
        StringTests::ConstructorTests::DefaultCtor<utf16>,

        StringTests::ConstructorTests::LenAndCharCtor<utf8, TQ::Zero>,
        StringTests::ConstructorTests::LenAndCharCtor<utf8, TQ::One>,
        StringTests::ConstructorTests::LenAndCharCtor<utf8, TQ::Many>,
        StringTests::ConstructorTests::LenAndCharCtor<utf16, TQ::Zero>,
        StringTests::ConstructorTests::LenAndCharCtor<utf16, TQ::One>,
        StringTests::ConstructorTests::LenAndCharCtor<utf16, TQ::Many>,

        StringTests::ConstructorTests::CStrCtorNullptr<utf8>,
        StringTests::ConstructorTests::CStrCtorNullptr<utf16>,

        StringTests::ConstructorTests::CStrCtor<utf8, TQ::Zero>,
        StringTests::ConstructorTests::CStrCtor<utf8, TQ::One>,
        StringTests::ConstructorTests::CStrCtor<utf8, TQ::Many>,
        StringTests::ConstructorTests::CStrCtor<utf16, TQ::Zero>,
        StringTests::ConstructorTests::CStrCtor<utf16, TQ::One>,
        StringTests::ConstructorTests::CStrCtor<utf16, TQ::Many>,

        StringTests::ConstructorTests::PtrAndLenCtorNullptr<utf8, TQ::Zero>,
        StringTests::ConstructorTests::PtrAndLenCtorNullptr<utf8, TQ::One>,
        StringTests::ConstructorTests::PtrAndLenCtorNullptr<utf8, TQ::Many>,
        StringTests::ConstructorTests::PtrAndLenCtorNullptr<utf16, TQ::Zero>,
        StringTests::ConstructorTests::PtrAndLenCtorNullptr<utf16, TQ::One>,
        StringTests::ConstructorTests::PtrAndLenCtorNullptr<utf16, TQ::Many>,

        StringTests::ConstructorTests::PtrAndLenCtor<utf8, TQ::Zero>,
        StringTests::ConstructorTests::PtrAndLenCtor<utf8, TQ::One>,
        StringTests::ConstructorTests::PtrAndLenCtor<utf8, TQ::Many>,
        StringTests::ConstructorTests::PtrAndLenCtor<utf16, TQ::Zero>,
        StringTests::ConstructorTests::PtrAndLenCtor<utf16, TQ::One>,
        StringTests::ConstructorTests::PtrAndLenCtor<utf16, TQ::Many>,

        StringTests::ConstructorTests::CopyCtor<utf8, TQ::Zero>,
        StringTests::ConstructorTests::CopyCtor<utf8, TQ::One>,
        StringTests::ConstructorTests::CopyCtor<utf8, TQ::Many>,
        StringTests::ConstructorTests::CopyCtor<utf16, TQ::Zero>,
        StringTests::ConstructorTests::CopyCtor<utf16, TQ::One>,
        StringTests::ConstructorTests::CopyCtor<utf16, TQ::Many>,

        StringTests::ConstructorTests::MoveCtor<utf8, TQ::Zero>,
        StringTests::ConstructorTests::MoveCtor<utf8, TQ::One>,
        StringTests::ConstructorTests::MoveCtor<utf8, TQ::Many>,
        StringTests::ConstructorTests::MoveCtor<utf16, TQ::Zero>,
        StringTests::ConstructorTests::MoveCtor<utf16, TQ::One>,
        StringTests::ConstructorTests::MoveCtor<utf16, TQ::Many>,

        StringTests::AssignmentOperatorTests::CStrAssignOpNullptr<utf8, TQ::Zero>,
        StringTests::AssignmentOperatorTests::CStrAssignOpNullptr<utf8, TQ::One>,
        StringTests::AssignmentOperatorTests::CStrAssignOpNullptr<utf8, TQ::Many>,
        StringTests::AssignmentOperatorTests::CStrAssignOpNullptr<utf16, TQ::Zero>,
        StringTests::AssignmentOperatorTests::CStrAssignOpNullptr<utf16, TQ::One>,
        StringTests::AssignmentOperatorTests::CStrAssignOpNullptr<utf16, TQ::Many>,

        StringTests::AssignmentOperatorTests::CStrAssignOp<utf8, TQ::Zero>,
        StringTests::AssignmentOperatorTests::CStrAssignOp<utf8, TQ::One>,
        StringTests::AssignmentOperatorTests::CStrAssignOp<utf8, TQ::Many>,
        StringTests::AssignmentOperatorTests::CStrAssignOp<utf16, TQ::Zero>,
        StringTests::AssignmentOperatorTests::CStrAssignOp<utf16, TQ::One>,
        StringTests::AssignmentOperatorTests::CStrAssignOp<utf16, TQ::Many>,

        StringTests::AssignmentOperatorTests::CopyAssignOp<utf8, TQ::Zero>,
        StringTests::AssignmentOperatorTests::CopyAssignOp<utf8, TQ::One>,
        StringTests::AssignmentOperatorTests::CopyAssignOp<utf8, TQ::Many>,
        StringTests::AssignmentOperatorTests::CopyAssignOp<utf16, TQ::Zero>,
        StringTests::AssignmentOperatorTests::CopyAssignOp<utf16, TQ::One>,
        StringTests::AssignmentOperatorTests::CopyAssignOp<utf16, TQ::Many>,

        StringTests::AssignmentOperatorTests::MoveAssignOp<utf8, TQ::Zero>,
        StringTests::AssignmentOperatorTests::MoveAssignOp<utf8, TQ::One>,
        StringTests::AssignmentOperatorTests::MoveAssignOp<utf8, TQ::Many>,
        StringTests::AssignmentOperatorTests::MoveAssignOp<utf16, TQ::Zero>,
        StringTests::AssignmentOperatorTests::MoveAssignOp<utf16, TQ::One>,
        StringTests::AssignmentOperatorTests::MoveAssignOp<utf16, TQ::Many>,

        StringTests::AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf8, TQ::Zero>,
        StringTests::AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf8, TQ::One>,
        StringTests::AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf8, TQ::Many>,
        StringTests::AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf16, TQ::Zero>,
        StringTests::AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf16, TQ::One>,
        StringTests::AddAssignmentOperatorTests::CStrAddAssignOpNullptr<utf16, TQ::Many>,

        StringTests::AddAssignmentOperatorTests::CStrAddAssignOp<utf8, TQ::Zero>,
        StringTests::AddAssignmentOperatorTests::CStrAddAssignOp<utf8, TQ::One>,
        StringTests::AddAssignmentOperatorTests::CStrAddAssignOp<utf8, TQ::Many>,
        StringTests::AddAssignmentOperatorTests::CStrAddAssignOp<utf16, TQ::Zero>,
        StringTests::AddAssignmentOperatorTests::CStrAddAssignOp<utf16, TQ::One>,
        StringTests::AddAssignmentOperatorTests::CStrAddAssignOp<utf16, TQ::Many>,

        StringTests::AddAssignmentOperatorTests::StrObjAddAssignOp<utf8, TQ::Zero>,
        StringTests::AddAssignmentOperatorTests::StrObjAddAssignOp<utf8, TQ::One>,
        StringTests::AddAssignmentOperatorTests::StrObjAddAssignOp<utf8, TQ::Many>,
        StringTests::AddAssignmentOperatorTests::StrObjAddAssignOp<utf16, TQ::Zero>,
        StringTests::AddAssignmentOperatorTests::StrObjAddAssignOp<utf16, TQ::One>,
        StringTests::AddAssignmentOperatorTests::StrObjAddAssignOp<utf16, TQ::Many>,

        StringTests::SubscriptOperatorTests::SubscriptOp<utf8, TQ::Zero>,
        StringTests::SubscriptOperatorTests::SubscriptOp<utf8, TQ::One>,
        StringTests::SubscriptOperatorTests::SubscriptOp<utf8, TQ::Many>,
        StringTests::SubscriptOperatorTests::SubscriptOp<utf16, TQ::Zero>,
        StringTests::SubscriptOperatorTests::SubscriptOp<utf16, TQ::One>,
        StringTests::SubscriptOperatorTests::SubscriptOp<utf16, TQ::Many>,

        StringTests::EquivalencyOperatorTests::CStrEquivOp<utf8, TQ::Zero>,
        StringTests::EquivalencyOperatorTests::CStrEquivOp<utf8, TQ::One>,
        StringTests::EquivalencyOperatorTests::CStrEquivOp<utf8, TQ::Many>,
        StringTests::EquivalencyOperatorTests::CStrEquivOp<utf16, TQ::Zero>,
        StringTests::EquivalencyOperatorTests::CStrEquivOp<utf16, TQ::One>,
        StringTests::EquivalencyOperatorTests::CStrEquivOp<utf16, TQ::Many>,

        StringTests::EquivalencyOperatorTests::StrObjEquivOp<utf8, TQ::Zero>,
        StringTests::EquivalencyOperatorTests::StrObjEquivOp<utf8, TQ::One>,
        StringTests::EquivalencyOperatorTests::StrObjEquivOp<utf8, TQ::Many>,
        StringTests::EquivalencyOperatorTests::StrObjEquivOp<utf16, TQ::Zero>,
        StringTests::EquivalencyOperatorTests::StrObjEquivOp<utf16, TQ::One>,
        StringTests::EquivalencyOperatorTests::StrObjEquivOp<utf16, TQ::Many>,

        StringTests::GetterTests::CStr<utf8, TQ::Zero>,
        StringTests::GetterTests::CStr<utf8, TQ::One>,
        StringTests::GetterTests::CStr<utf8, TQ::Many>,
        StringTests::GetterTests::CStr<utf16, TQ::Zero>,
        StringTests::GetterTests::CStr<utf16, TQ::One>,
        StringTests::GetterTests::CStr<utf16, TQ::Many>,

        StringTests::GetterTests::Length<utf8, TQ::Zero>,
        StringTests::GetterTests::Length<utf8, TQ::One>,
        StringTests::GetterTests::Length<utf8, TQ::Many>,
        StringTests::GetterTests::Length<utf16, TQ::Zero>,
        StringTests::GetterTests::Length<utf16, TQ::One>,
        StringTests::GetterTests::Length<utf16, TQ::Many>,

        StringTests::GetterTests::Capacity<utf8, TQ::Zero>,
        StringTests::GetterTests::Capacity<utf8, TQ::One>,
        StringTests::GetterTests::Capacity<utf8, TQ::Many>,
        StringTests::GetterTests::Capacity<utf16, TQ::Zero>,
        StringTests::GetterTests::Capacity<utf16, TQ::One>,
        StringTests::GetterTests::Capacity<utf16, TQ::Many>,

        StringTests::GetterTests::Front<utf8, TQ::Zero>,
        StringTests::GetterTests::Front<utf8, TQ::One>,
        StringTests::GetterTests::Front<utf8, TQ::Many>,
        StringTests::GetterTests::Front<utf16, TQ::Zero>,
        StringTests::GetterTests::Front<utf16, TQ::One>,
        StringTests::GetterTests::Front<utf16, TQ::Many>,

        StringTests::GetterTests::Back<utf8, TQ::Zero>,
        StringTests::GetterTests::Back<utf8, TQ::One>,
        StringTests::GetterTests::Back<utf8, TQ::Many>,
        StringTests::GetterTests::Back<utf16, TQ::Zero>,
        StringTests::GetterTests::Back<utf16, TQ::One>,
        StringTests::GetterTests::Back<utf16, TQ::Many>,

        StringTests::Assign::ChAssign<utf8, TQ::Zero>,
        StringTests::Assign::ChAssign<utf8, TQ::One>,
        StringTests::Assign::ChAssign<utf8, TQ::Many>,
        StringTests::Assign::ChAssign<utf16, TQ::Zero>,
        StringTests::Assign::ChAssign<utf16, TQ::One>,
        StringTests::Assign::ChAssign<utf16, TQ::Many>,

        StringTests::Assign::CStrAssign<utf8, TQ::Zero>,
        StringTests::Assign::CStrAssign<utf8, TQ::One>,
        StringTests::Assign::CStrAssign<utf8, TQ::Many>,
        StringTests::Assign::CStrAssign<utf16, TQ::Zero>,
        StringTests::Assign::CStrAssign<utf16, TQ::One>,
        StringTests::Assign::CStrAssign<utf16, TQ::Many>,

        StringTests::Assign::PtrAndLenAssign<utf8, TQ::Zero>,
        StringTests::Assign::PtrAndLenAssign<utf8, TQ::One>,
        StringTests::Assign::PtrAndLenAssign<utf8, TQ::Many>,
        StringTests::Assign::PtrAndLenAssign<utf16, TQ::Zero>,
        StringTests::Assign::PtrAndLenAssign<utf16, TQ::One>,
        StringTests::Assign::PtrAndLenAssign<utf16, TQ::Many>,

        StringTests::Assign::StrObjCopyAssign<utf8, TQ::Zero>,
        StringTests::Assign::StrObjCopyAssign<utf8, TQ::One>,
        StringTests::Assign::StrObjCopyAssign<utf8, TQ::Many>,
        StringTests::Assign::StrObjCopyAssign<utf16, TQ::Zero>,
        StringTests::Assign::StrObjCopyAssign<utf16, TQ::One>,
        StringTests::Assign::StrObjCopyAssign<utf16, TQ::Many>,

        StringTests::Assign::StrObjAndLenCopyAssign<utf8, TQ::Zero>,
        StringTests::Assign::StrObjAndLenCopyAssign<utf8, TQ::One>,
        StringTests::Assign::StrObjAndLenCopyAssign<utf8, TQ::Many>,
        StringTests::Assign::StrObjAndLenCopyAssign<utf16, TQ::Zero>,
        StringTests::Assign::StrObjAndLenCopyAssign<utf16, TQ::One>,
        StringTests::Assign::StrObjAndLenCopyAssign<utf16, TQ::Many>,

        StringTests::Assign::StrObjMoveAssign<utf8, TQ::Zero>,
        StringTests::Assign::StrObjMoveAssign<utf8, TQ::One>,
        StringTests::Assign::StrObjMoveAssign<utf8, TQ::Many>,
        StringTests::Assign::StrObjMoveAssign<utf16, TQ::Zero>,
        StringTests::Assign::StrObjMoveAssign<utf16, TQ::One>,
        StringTests::Assign::StrObjMoveAssign<utf16, TQ::Many>,

        StringTests::Append::ChAppend<utf8, TQ::Zero>,
        StringTests::Append::ChAppend<utf8, TQ::One>,
        StringTests::Append::ChAppend<utf8, TQ::Many>,
        StringTests::Append::ChAppend<utf16, TQ::Zero>,
        StringTests::Append::ChAppend<utf16, TQ::One>,
        StringTests::Append::ChAppend<utf16, TQ::Many>,

        StringTests::Append::CStrAppend<utf8, TQ::Zero>,
        StringTests::Append::CStrAppend<utf8, TQ::One>,
        StringTests::Append::CStrAppend<utf8, TQ::Many>,
        StringTests::Append::CStrAppend<utf16, TQ::Zero>,
        StringTests::Append::CStrAppend<utf16, TQ::One>,
        StringTests::Append::CStrAppend<utf16, TQ::Many>,

        StringTests::Append::PtrAndLenAppend<utf8, TQ::Zero>,
        StringTests::Append::PtrAndLenAppend<utf8, TQ::One>,
        StringTests::Append::PtrAndLenAppend<utf8, TQ::Many>,
        StringTests::Append::PtrAndLenAppend<utf16, TQ::Zero>,
        StringTests::Append::PtrAndLenAppend<utf16, TQ::One>,
        StringTests::Append::PtrAndLenAppend<utf16, TQ::Many>,

        StringTests::Append::StrObjAppend<utf8, TQ::Zero>,
        StringTests::Append::StrObjAppend<utf8, TQ::One>,
        StringTests::Append::StrObjAppend<utf8, TQ::Many>,
        StringTests::Append::StrObjAppend<utf16, TQ::Zero>,
        StringTests::Append::StrObjAppend<utf16, TQ::One>,
        StringTests::Append::StrObjAppend<utf16, TQ::Many>,

        StringTests::Append::StrObjAndLenAppend<utf8, TQ::Zero>,
        StringTests::Append::StrObjAndLenAppend<utf8, TQ::One>,
        StringTests::Append::StrObjAndLenAppend<utf8, TQ::Many>,
        StringTests::Append::StrObjAndLenAppend<utf16, TQ::Zero>,
        StringTests::Append::StrObjAndLenAppend<utf16, TQ::One>,
        StringTests::Append::StrObjAndLenAppend<utf16, TQ::Many>,

        StringTests::CompareTests::CStrCompare<utf8, TQ::Zero, false>,
        StringTests::CompareTests::CStrCompare<utf8, TQ::Zero, true>,
        StringTests::CompareTests::CStrCompare<utf8, TQ::One, false>,
        StringTests::CompareTests::CStrCompare<utf8, TQ::One, true>,
        StringTests::CompareTests::CStrCompare<utf8, TQ::Many, false>,
        StringTests::CompareTests::CStrCompare<utf8, TQ::Many, true>,
        StringTests::CompareTests::CStrCompare<utf16, TQ::Zero, false>,
        StringTests::CompareTests::CStrCompare<utf16, TQ::Zero, true>,
        StringTests::CompareTests::CStrCompare<utf16, TQ::One, false>,
        StringTests::CompareTests::CStrCompare<utf16, TQ::One, true>,
        StringTests::CompareTests::CStrCompare<utf16, TQ::Many, false>,
        StringTests::CompareTests::CStrCompare<utf16, TQ::Many, true>,

        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Zero, TQ::Zero, false>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Zero, TQ::Zero, true>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Zero, TQ::One, false>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Zero, TQ::One, true>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Zero, TQ::Many, false>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Zero, TQ::Many, true>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::One, TQ::Zero, false>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::One, TQ::Zero, true>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::One, TQ::One, false>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::One, TQ::One, true>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::One, TQ::Many, false>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::One, TQ::Many, true>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Many, TQ::Zero, false>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Many, TQ::Zero, true>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Many, TQ::One, false>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Many, TQ::One, true>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Many, TQ::Many, false>,
        StringTests::CompareTests::StrAndLenCompare<utf8, TQ::Many, TQ::Many, true>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Zero, TQ::Zero, false>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Zero, TQ::Zero, true>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Zero, TQ::One, false>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Zero, TQ::One, true>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Zero, TQ::Many, false>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Zero, TQ::Many, true>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::One, TQ::Zero, false>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::One, TQ::Zero, true>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::One, TQ::One, false>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::One, TQ::One, true>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::One, TQ::Many, false>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::One, TQ::Many, true>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Many, TQ::Zero, false>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Many, TQ::Zero, true>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Many, TQ::One, false>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Many, TQ::One, true>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Many, TQ::Many, false>,
        StringTests::CompareTests::StrAndLenCompare<utf16, TQ::Many, TQ::Many, true>,

        StringTests::CompareTests::StrObjCompare<utf8, TQ::Zero, false>,
        StringTests::CompareTests::StrObjCompare<utf8, TQ::Zero, true>,
        StringTests::CompareTests::StrObjCompare<utf8, TQ::One, false>,
        StringTests::CompareTests::StrObjCompare<utf8, TQ::One, true>,
        StringTests::CompareTests::StrObjCompare<utf8, TQ::Many, false>,
        StringTests::CompareTests::StrObjCompare<utf8, TQ::Many, true>,
        StringTests::CompareTests::StrObjCompare<utf16, TQ::Zero, false>,
        StringTests::CompareTests::StrObjCompare<utf16, TQ::Zero, true>,
        StringTests::CompareTests::StrObjCompare<utf16, TQ::One, false>,
        StringTests::CompareTests::StrObjCompare<utf16, TQ::One, true>,
        StringTests::CompareTests::StrObjCompare<utf16, TQ::Many, false>,
        StringTests::CompareTests::StrObjCompare<utf16, TQ::Many, true>,

        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Zero, TQ::Zero, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Zero, TQ::Zero, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Zero, TQ::One, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Zero, TQ::One, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Zero, TQ::Many, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Zero, TQ::Many, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::One, TQ::Zero, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::One, TQ::Zero, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::One, TQ::One, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::One, TQ::One, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::One, TQ::Many, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::One, TQ::Many, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Many, TQ::Zero, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Many, TQ::Zero, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Many, TQ::One, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Many, TQ::One, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Many, TQ::Many, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf8, TQ::Many, TQ::Many, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Zero, TQ::Zero, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Zero, TQ::Zero, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Zero, TQ::One, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Zero, TQ::One, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Zero, TQ::Many, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Zero, TQ::Many, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::One, TQ::Zero, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::One, TQ::Zero, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::One, TQ::One, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::One, TQ::One, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::One, TQ::Many, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::One, TQ::Many, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Many, TQ::Zero, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Many, TQ::Zero, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Many, TQ::One, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Many, TQ::One, true>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Many, TQ::Many, false>,
        StringTests::CompareTests::StrObjAndLenCompare<utf16, TQ::Many, TQ::Many, true>,

        StringTests::ClearTests::Clear<utf8, TQ::Zero>,
        StringTests::ClearTests::Clear<utf8, TQ::One>,
        StringTests::ClearTests::Clear<utf8, TQ::Many>,
        StringTests::ClearTests::Clear<utf16, TQ::Zero>,
        StringTests::ClearTests::Clear<utf16, TQ::One>,
        StringTests::ClearTests::Clear<utf16, TQ::Many>,

        StringTests::IsEmptyTests::IsEmpty<utf8, TQ::Zero>,
        StringTests::IsEmptyTests::IsEmpty<utf8, TQ::One>,
        StringTests::IsEmptyTests::IsEmpty<utf8, TQ::Many>,
        StringTests::IsEmptyTests::IsEmpty<utf16, TQ::Zero>,
        StringTests::IsEmptyTests::IsEmpty<utf16, TQ::One>,
        StringTests::IsEmptyTests::IsEmpty<utf16, TQ::Many>,
    };

    return tests;
}