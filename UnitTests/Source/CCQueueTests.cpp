#include <CCQueueTests.h>


CC::QueueTests::UTList CC::QueueTests::GetTests()
{
#pragma warning(suppress : 6262) // Suppress high stack memory usage warning.
    static const UTList tests
    {
        // Constructor Tests
        ADD_TESTS_T(ConstructorTests::DefaultCtor),
        ADD_TESTS_T(ConstructorTests::ElementCopyCtor),
        ADD_TESTS_T(ConstructorTests::ElementMoveCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::QueueCopyCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::QueueMoveCtor),

        // Assignment Tests
        ADD_TESTS_T_TQ1(AssignTests::AssignMethodElementCopy),
        ADD_TESTS_T_TQ1(AssignTests::AssignMethodElementMove),
        ADD_TESTS_T_TQ2(AssignTests::AssignMethodQueueCopy),
        ADD_TESTS_T_TQ2(AssignTests::AssignMethodQueueMove),
        ADD_TESTS_T_TQ1(AssignTests::AssignOperatorElementCopy),
        ADD_TESTS_T_TQ1(AssignTests::AssignOperatorElementMove),
        ADD_TESTS_T_TQ2(AssignTests::AssignOperatorQueueCopy),
        ADD_TESTS_T_TQ2(AssignTests::AssignOperatorQueueMove),

        // Comparison Tests
        ADD_TESTS_T_TQ2(ComparisonTests::ComparisonMethod),
        ADD_TESTS_T_TQ2(ComparisonTests::ComparisonOperator),

        // Getter Tests
        ADD_TESTS_T_TQ1(GetterTests::Length),
        ADD_TESTS_T_TQ1(GetterTests::Back),
        ADD_TESTS_T_TQ1(GetterTests::Front),

        // EnqueueDequeue Tests
        ADD_TESTS_T_TQ1(EnqueueDequeueTests::Enqueue),
        ADD_TESTS_T_TQ1(EnqueueDequeueTests::Dequeue),
        ADD_TESTS_T_TQ1(EnqueueDequeueTests::DequeueAndCopy),
        ADD_TESTS_T_TQ1(EnqueueDequeueTests::EnqueueDequeueStagger),
        ADD_TESTS_T_TQ1(EnqueueDequeueTests::EnqueueDequeueAndCopyStagger)
    };

    return tests;
}
