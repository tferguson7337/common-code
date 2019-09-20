#include <CCPointerHelperTests.h>

CC::PointerHelperTests::UTList CC::PointerHelperTests::GetTests()
{
    using TQ = TestQuantity;

    static const UTList tests
    {
        // Tests
        ADD_TESTS_T_TQ1(CopyToRawPointer),
        ADD_TESTS_T_TQ1(MoveToRawPointer),
        ADD_TESTS_T_TQ1(Allocate),
        ADD_TESTS_T_TQ1(AllocateLen),
        ADD_TESTS_T_TQ1(AllocateFromRawPointer),
        ADD_TESTS_T_TQ1(AllocateFromIPointerObj)
    };

    return tests;
}