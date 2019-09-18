// SUTL
#include <UnitTestRunner.h>

// Tests
#include <TestSets.h>

// STL
#include <chrono>


int main(const int argc, const char* argv[])
{
    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<int64_t, std::milli>;
    using TimePoint = std::chrono::time_point<Clock>;

    Duration totalDelta(0);
    UnitTestRunner<char> utr;

    for (const TestSetFunc& f : GetTestSetFunctions(argc, argv))
    {
        bool bTestSetResult = false;
        const char* pSetName = nullptr;
        TimePoint t0, t1;
        Duration delta;

        t0 = Clock::now();
        bTestSetResult = f(utr);
        t1 = Clock::now();

        pSetName = utr.GetTestSetData().GetTestSetName().c_str();
        if (!bTestSetResult)
        {
            printf("  Warning: Test set[%s] failure(s)\r\n", pSetName);
        }

        delta = std::chrono::duration_cast<Duration, int64_t>(t1 - t0);
        totalDelta += delta;
        printf("  Test set[%s] complete - %lld ms\r\n", pSetName, delta.count());
    }

    printf("\r\n  All test(s) complete - %lld ms\r\n", totalDelta.count());

    return static_cast<int>(utr.GetTestSetData().GetTotalFailureCount());
}
