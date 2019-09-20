#include <CCBufferTests.h>


CC::BufferTests::UTList CC::BufferTests::GetTests()
{
#pragma warning(suppress : 6262) // Suppress high stack memory usage warning.
    static const UTList tests
    {
        /// Constructor Tests \\\

        ADD_TESTS_T_TQ1(ConstructorTests::DefaultConstructor),
        ADD_TESTS_T_TQ1(ConstructorTests::LengthConstructor),
        ADD_TESTS_T_TQ1(ConstructorTests::PointerCopyConstructorNull),
        ADD_TESTS_T_TQ1(ConstructorTests::PointerCopyConstructor),
        ADD_TESTS_T_TQ1(ConstructorTests::PointerMoveConstructorNull),
        ADD_TESTS_T_TQ1(ConstructorTests::PointerMoveConstructor),
        ADD_TESTS_T_TQ1(ConstructorTests::CopyConstructor),
        ADD_TESTS_T_TQ1(ConstructorTests::MoveConstructor),

        /// Copy Tests \\\

        ADD_TESTS_T_TQ1(CopyTests::CopyAllBufferDataMembers),
        ADD_TESTS_T_TQ1(CopyTests::CopyNonPointerBufferDataMembers),
        ADD_TESTS_T_TQ1(CopyTests::CopyBuffer),
        ADD_TESTS_T_TQ1(CopyTests::CopyAssignment),

        /// Move Tests \\\

        ADD_TESTS_T_TQ1(MoveTests::TransferBuffer),
        ADD_TESTS_T_TQ1(MoveTests::MoveAssignment),

        /// Subscript Operator Tests \\\

        ADD_TESTS_T_TQ1(SubscriptOperatorTests::SubscriptOperator),

        /// Dereference Operator Tests \\\

        ADD_TESTS_T_TQ1(DereferenceOperatorTests::DereferenceOperator),

        /// Comparison Tests \\\

        ADD_TESTS_T_TQ2(ComparisonTests::ComparisonPtrNull),
        ADD_TESTS_T_TQ2(ComparisonTests::ComparisonPtr),
        ADD_TESTS_T_TQ2(ComparisonTests::Comparison),
        ADD_TESTS_T_TQ2(ComparisonTests::ComparisonOperator),

        /// Set Write Position Tests \\\

        ADD_TESTS_T_TQ1(SetWritePositionTests::SetWritePosition),

        /// Write Tests \\\

        ADD_TESTS_T_TQ1_B(WriteTests::WriteElement),
        ADD_TESTS_T_TQ2(WriteTests::WritePtrNull),
        ADD_TESTS_T_TQ2_B(WriteTests::WritePtr),
        ADD_TESTS_T_TQ2_B(WriteTests::WriteToWritePosition),
        ADD_TESTS_T_TQ2_B(WriteTests::WriteToEnd)
    };

    return tests;
}
