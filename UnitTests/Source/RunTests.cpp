// SUTL
#include <UnitTestRunner.h>

// Tests
#include <TestSets.h>

#include <CCStringLiteral.h>

void PrintChar(_In_ const char c)
{
    printf("%c", c);
}

#include <string_view>

int main(const int argc, const char* argv[])
{
    constexpr CC::StringLiteral lit("Test String\n");
    constexpr std::basic_string_view<char> sv(lit.CStr());

    std::for_each(lit.begin(), lit.end(), PrintChar);
    std::for_each(lit.cbegin(), lit.cend(), PrintChar);

    UnitTestRunner<char> utr;
    std::list<TestSetFunc> tests(GetTestSetFunctions(argc, argv));
    std::for_each(tests.cbegin(), tests.cend(), [&utr](const auto& f) { f(utr); });
    return static_cast<int>(utr.GetTestSetData().GetTotalFailureCount());
}
