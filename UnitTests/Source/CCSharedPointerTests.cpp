#include <CCSharedPointerTests.h>

CC::SharedPointerTests::UTList CC::SharedPointerTests::GetTests()
{
    using TQ = TestQuantity;

#pragma warning(suppress : 6262) // Suppress high stack memory usage warning.
    static const UTList tests
    {
        // Constructor Tests
        ADD_TESTS_T(ConstructorTests::DefaultCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::LengthCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::RawPointerCopyCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::RawPointerStealCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::CopyCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::MoveCtor),

        // Destructor Tests
        ADD_TESTS_T_TQ1(DestructorTests::Dtor),

        // Assignment Tests
        ADD_TESTS_T_TQ1(AssignmentTests::CopyAssignment),
        ADD_TESTS_T_TQ1(AssignmentTests::MoveAssignment),

        // RefCount Tests
        ADD_TESTS_T_TQ1(RefCountTests::MTIncrement),
        ADD_TESTS_T_TQ1(RefCountTests::MTDecrement),
        ADD_TESTS_T_TQ1(RefCountTests::MTIncrementAndDecrement)
    };

    return tests;
}