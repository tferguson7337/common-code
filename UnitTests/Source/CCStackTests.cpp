#include <CCStackTests.h>


CC::StackTests::UTList CC::StackTests::GetTests()
{
#pragma warning(suppress : 6262) // Suppress high stack memory usage warning.
    static const UTList tests
    {
        // Constructor Tests
        ADD_TESTS_T(ConstructorTests::DefaultCtor),
        ADD_TESTS_T(ConstructorTests::ElementCopyCtor),
        ADD_TESTS_T(ConstructorTests::ElementMoveCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::StackCopyCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::StackMoveCtor),

        // Assignment Tests
        ADD_TESTS_T_TQ1(AssignTests::AssignMethodElementCopy),
        ADD_TESTS_T_TQ1(AssignTests::AssignMethodElementMove),
        ADD_TESTS_T_TQ2(AssignTests::AssignMethodStackCopy),
        ADD_TESTS_T_TQ2(AssignTests::AssignMethodStackMove),
        ADD_TESTS_T_TQ1(AssignTests::AssignOperatorElementCopy),
        ADD_TESTS_T_TQ1(AssignTests::AssignOperatorElementMove),
        ADD_TESTS_T_TQ2(AssignTests::AssignOperatorStackCopy),
        ADD_TESTS_T_TQ2(AssignTests::AssignOperatorStackMove),

        // Comparison Tests
        ADD_TESTS_T_TQ2(ComparisonTests::ComparisonMethod),
        ADD_TESTS_T_TQ2(ComparisonTests::ComparisonOperator),

        // Getter Tests
        ADD_TESTS_T_TQ1(GetterTests::Length),
        ADD_TESTS_T_TQ1(GetterTests::Top),

        // PushPop Tests
        ADD_TESTS_T_TQ1(PushPopTests::Push),
        ADD_TESTS_T_TQ1(PushPopTests::Pop),
        ADD_TESTS_T_TQ1(PushPopTests::PopAndCopy),
        ADD_TESTS_T_TQ1(PushPopTests::PushPopStagger),
        ADD_TESTS_T_TQ1(PushPopTests::PushPopAndCopyStagger),
    };

    return tests;
}
