#include <CCPointerTests.h>

CC::PointerTests::UTList CC::PointerTests::GetTests( )
{
    using TQ = TestQuantity;

    static const UTList tests
    {
        PointerTests::AllocationTests::Allocate<uint8_t, TQ::Zero>,
        PointerTests::AllocationTests::Allocate<uint8_t, TQ::One>,
        PointerTests::AllocationTests::Allocate<uint8_t, TQ::Many>,
        PointerTests::AllocationTests::Allocate<uint16_t, TQ::Zero>,
        PointerTests::AllocationTests::Allocate<uint16_t, TQ::One>,
        PointerTests::AllocationTests::Allocate<uint16_t, TQ::Many>,
        PointerTests::AllocationTests::Allocate<uint32_t, TQ::Zero>,
        PointerTests::AllocationTests::Allocate<uint32_t, TQ::One>,
        PointerTests::AllocationTests::Allocate<uint32_t, TQ::Many>,
        PointerTests::AllocationTests::Allocate<uint64_t, TQ::Zero>,
        PointerTests::AllocationTests::Allocate<uint64_t, TQ::One>,
        PointerTests::AllocationTests::Allocate<uint64_t, TQ::Many>,
        PointerTests::AllocationTests::Allocate<Helper, TQ::Zero>,
        PointerTests::AllocationTests::Allocate<Helper, TQ::One>,
        PointerTests::AllocationTests::Allocate<Helper, TQ::Many>,

        PointerTests::AllocationTests::AllocFromRawPtrNull<uint8_t, TQ::Zero>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<uint8_t, TQ::One>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<uint8_t, TQ::Many>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<uint16_t, TQ::Zero>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<uint16_t, TQ::One>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<uint16_t, TQ::Many>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<uint32_t, TQ::Zero>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<uint32_t, TQ::One>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<uint32_t, TQ::Many>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<uint64_t, TQ::Zero>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<uint64_t, TQ::One>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<uint64_t, TQ::Many>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<Helper, TQ::Zero>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<Helper, TQ::One>,
        PointerTests::AllocationTests::AllocFromRawPtrNull<Helper, TQ::Many>,

        PointerTests::AllocationTests::AllocFromRawPtr<uint8_t, TQ::Zero>,
        PointerTests::AllocationTests::AllocFromRawPtr<uint8_t, TQ::One>,
        PointerTests::AllocationTests::AllocFromRawPtr<uint8_t, TQ::Many>,
        PointerTests::AllocationTests::AllocFromRawPtr<uint16_t, TQ::Zero>,
        PointerTests::AllocationTests::AllocFromRawPtr<uint16_t, TQ::One>,
        PointerTests::AllocationTests::AllocFromRawPtr<uint16_t, TQ::Many>,
        PointerTests::AllocationTests::AllocFromRawPtr<uint32_t, TQ::Zero>,
        PointerTests::AllocationTests::AllocFromRawPtr<uint32_t, TQ::One>,
        PointerTests::AllocationTests::AllocFromRawPtr<uint32_t, TQ::Many>,
        PointerTests::AllocationTests::AllocFromRawPtr<uint64_t, TQ::Zero>,
        PointerTests::AllocationTests::AllocFromRawPtr<uint64_t, TQ::One>,
        PointerTests::AllocationTests::AllocFromRawPtr<uint64_t, TQ::Many>,
        PointerTests::AllocationTests::AllocFromRawPtr<Helper, TQ::Zero>,
        PointerTests::AllocationTests::AllocFromRawPtr<Helper, TQ::One>,
        PointerTests::AllocationTests::AllocFromRawPtr<Helper, TQ::Many>,

    };

    return tests;
}