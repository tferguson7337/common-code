#include <CCListTests.h>

CC::ListTests::UTList CC::ListTests::GetTests()
{
    using TQ = TestQuantity;

#pragma warning(suppress : 6262) // Suppress high stack memory usage warning.
    static const UTList tests
    {

    };

    return tests;
}
