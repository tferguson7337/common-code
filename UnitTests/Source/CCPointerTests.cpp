#include <CCPointerTests.h>

CC::PointerTests::UTList CC::PointerTests::GetTests()
{
    using TQ = TestQuantity;

#pragma warning(suppress : 6262) // Suppress high stack memory usage warning.
    static const UTList tests
    {
        // Deallocation Tests
        ADD_TESTS_T_TQ1(DeallocationTests::CopyPointerObj),
        ADD_TESTS_T_TQ1(DeallocationTests::TransferPointerObj),
        ADD_TESTS_T_TQ1(DeallocationTests::Destructor),
        ADD_TESTS_T_TQ1(DeallocationTests::Free),

        // Constructor Tests
        ADD_TESTS_T(ConstructorTests::DefaultCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::LengthCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::RawPointerCopyCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::RawPointerStealCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::CopyCtor),
        ADD_TESTS_T_TQ1(ConstructorTests::MoveCtor),

        // Copy Tests
        ADD_TESTS_T_TQ2(CopyTests::CopyAssignment),

        // Move Tests
        ADD_TESTS_T_TQ2(MoveTests::MoveAssignment),

        // Dereference Tests
        ADD_TESTS_T_TQ1(DereferenceOperatorTests::Dereference)
    };

    return tests;
}