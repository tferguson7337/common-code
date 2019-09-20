#include <CCDynamicBufferTests.h>


namespace CC
{
    CC::DynamicBufferTests::UTList CC::DynamicBufferTests::GetTests()
    {
    #pragma warning(suppress : 6262)
        static const UTList tests
        {
            // Write Tests
            ADD_TESTS_T_TQ1_B(WriteTests::WriteElement),
            ADD_TESTS_T_TQ2(WriteTests::WritePtrNull),
            ADD_TESTS_T_TQ2_B(WriteTests::WritePtr),
            ADD_TESTS_T_TQ2_B(WriteTests::WriteToWritePos),
            ADD_TESTS_T_TQ2_B(WriteTests::WriteToEnd),
        };

        return tests;
    }
}