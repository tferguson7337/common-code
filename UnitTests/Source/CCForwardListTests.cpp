#include <CCForwardListTests.h>


CC::ForwardListTests::UTList CC::ForwardListTests::GetTests()
{
#pragma warning(suppress : 6262) // Suppress high stack memory usage warning.
    static const UTList tests
    {
        // Constructor Tests
        ADD_TESTS_T(ConstructorTests::DefaultCtor),
        ADD_TESTS_T(ConstructorTests::ElementCopyCtor),
        ADD_TESTS_T(ConstructorTests::ElementMoveCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::ListCopyCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::ListMoveCtor),

        // Assignment Tests
        ADD_TESTS_T_TQ1(AssignTests::AssignMethodElementCopy),
        ADD_TESTS_T_TQ1(AssignTests::AssignMethodElementMove),
        ADD_TESTS_T_TQ2(AssignTests::AssignMethodListCopy),
        ADD_TESTS_T_TQ2(AssignTests::AssignMethodListMove),
        ADD_TESTS_T_TQ1(AssignTests::AssignOperatorElementCopy),
        ADD_TESTS_T_TQ1(AssignTests::AssignOperatorElementMove),
        ADD_TESTS_T_TQ2(AssignTests::AssignOperatorListCopy),
        ADD_TESTS_T_TQ2(AssignTests::AssignOperatorListMove),

        // Insertion Tests - Append
        ADD_TESTS_T(InsertionTests::AppendMethodElementCopy),
        ADD_TESTS_T(InsertionTests::AppendMethodElementMove),
        ADD_TESTS_T_TQ1(InsertionTests::AppendMethodListCopy),
        ADD_TESTS_T_TQ1(InsertionTests::AppendMethodListMove),
        ADD_TESTS_T(InsertionTests::AppendOperatorElementCopy),
        ADD_TESTS_T(InsertionTests::AppendOperatorElementMove),
        ADD_TESTS_T_TQ1(InsertionTests::AppendOperatorListCopy),
        ADD_TESTS_T_TQ1(InsertionTests::AppendOperatorListMove),

        // Insertion Tests - Prepend
        ADD_TESTS_T(InsertionTests::PrependElementCopy),
        ADD_TESTS_T(InsertionTests::PrependElementMove),
        ADD_TESTS_T_TQ1(InsertionTests::PrependListCopy),
        ADD_TESTS_T_TQ1(InsertionTests::PrependListMove),

        // Insertion Tests - InsertAt (Front)
        ADD_TESTS_T(InsertionTests::InsertAtFrontElementCopy),
        ADD_TESTS_T(InsertionTests::InsertAtFrontElementMove),
        ADD_TESTS_T_TQ1(InsertionTests::InsertAtFrontListCopy),
        ADD_TESTS_T_TQ1(InsertionTests::InsertAtFrontListMove),

        // Insertion Tests - InsertAt (Mid)
        ADD_TESTS_T(InsertionTests::InsertAtMidElementCopy),
        ADD_TESTS_T(InsertionTests::InsertAtMidElementMove),
        ADD_TESTS_T_TQ1(InsertionTests::InsertAtMidListCopy),
        ADD_TESTS_T_TQ1(InsertionTests::InsertAtMidListMove),

        // Insertion Tests - InsertAt (Back)
        ADD_TESTS_T_TQ1(InsertionTests::InsertAtBackElementCopy),
        ADD_TESTS_T_TQ1(InsertionTests::InsertAtBackElementMove),
        ADD_TESTS_T_TQ1(InsertionTests::InsertAtBackListCopy),
        ADD_TESTS_T_TQ1(InsertionTests::InsertAtBackListMove),

        // Getter Tests
        ADD_TESTS_T_TQ1(GetterTests::Length),
        ADD_TESTS_T_TQ1(GetterTests::Front),
        ADD_TESTS_T_TQ1(GetterTests::Back),
        ADD_TESTS_T_TQ1(GetterTests::At),

        // Comparison Tests
        ADD_TESTS_T_TQ2(ComparisonTests::CompareMethod),
        ADD_TESTS_T_TQ2(ComparisonTests::CompareOperator),

        // Removal Tests
        ADD_TESTS_T_TQ1(RemovalTests::PopFront),
        ADD_TESTS_T_TQ1(RemovalTests::PopBack),
        ADD_TESTS_T_TQ1(RemovalTests::RemoveAt),
        ADD_TESTS_T_TQ1(RemovalTests::Clear),
    };

    return tests;
}
