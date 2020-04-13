// SUTL
#include <UnitTestRunner.h>

// Tests
#include <TestSets.h>


int main(const int argc, const char* argv[])
{
    UnitTestRunner<char> utr;
    std::list<TestSetFunc> tests(GetTestSetFunctions(argc, argv));
    std::for_each(tests.cbegin(), tests.cend(), [&utr](const auto& f) { f(utr); });
    return static_cast<int>(utr.GetTestSetData().GetTotalFailureCount());
}
