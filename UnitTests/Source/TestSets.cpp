#include <TestSets.h>

// CC Tests
#include <CCBufferTests.h>
#include <CCDynamicBufferTests.h>
#include <CCListTests.h>
#include <CCPointerTests.h>
#include <CCPointerHelperTests.h>
#include <CCQueueTests.h>
#include <CCSharedPointerTests.h>
#include <CCStackTests.h>
#include <CCStringTests.h>
#include <CCStringUtilTests.h>


static bool RunAllTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests();
    utr.GetTestSetData().SetTestSetName("CommonCode - All");

    utr.AddUnitTests(CC::PointerHelperTests::GetTests());
    utr.AddUnitTests(CC::PointerTests::GetTests());
    utr.AddUnitTests(CC::SharedPointerTests::GetTests());
    utr.AddUnitTests(CC::BufferTests::GetTests());
    utr.AddUnitTests(CC::DynamicBufferTests::GetTests());
    utr.AddUnitTests(CC::StringTests::GetTests());
    utr.AddUnitTests(CC::StringUtilTests::GetTests());
    utr.AddUnitTests(CC::ListTests::GetTests());
    utr.AddUnitTests(CC::StackTests::GetTests());
    utr.AddUnitTests(CC::QueueTests::GetTests());

    return utr.RunUnitTests();
}

static bool RunPointerHelperTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests();
    utr.GetTestSetData().SetTestSetName("CommonCode - PointerHelper");

    utr.AddUnitTests(CC::PointerHelperTests::GetTests());

    return utr.RunUnitTests();
}

static bool RunPointerTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests();
    utr.GetTestSetData().SetTestSetName("CommonCode - Pointer");

    utr.AddUnitTests(CC::PointerTests::GetTests());

    return utr.RunUnitTests();
}

static bool RunSharedPointerTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests();
    utr.GetTestSetData().SetTestSetName("CommonCode - SharedPointer");

    utr.AddUnitTests(CC::SharedPointerTests::GetTests());

    return utr.RunUnitTests();
}

static bool RunBufferTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests();
    utr.GetTestSetData().SetTestSetName("CommonCode - Buffer");

    utr.AddUnitTests(CC::BufferTests::GetTests());

    return utr.RunUnitTests();
}

static bool RunDynamicBufferTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests();
    utr.GetTestSetData().SetTestSetName("CommonCode - Dynamic Buffer");

    utr.AddUnitTests(CC::DynamicBufferTests::GetTests());

    return utr.RunUnitTests();
}

static bool RunStringUtilTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests();
    utr.GetTestSetData().SetTestSetName("CommonCode - StringUtil");

    utr.AddUnitTests(CC::StringUtilTests::GetTests());

    return utr.RunUnitTests();
}

static bool RunStringTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests();
    utr.GetTestSetData().SetTestSetName("CommonCode - String");

    utr.AddUnitTests(CC::StringTests::GetTests());

    return utr.RunUnitTests();
}

static bool RunListTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests();
    utr.GetTestSetData().SetTestSetName("CommonCode - List");

    utr.AddUnitTests(CC::ListTests::GetTests());

    return utr.RunUnitTests();
}

static bool RunStackTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests();
    utr.GetTestSetData().SetTestSetName("CommonCode - Stack");

    utr.AddUnitTests(CC::StackTests::GetTests());

    return utr.RunUnitTests();
}

static bool RunQueueTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests();
    utr.GetTestSetData().SetTestSetName("CommonCode - Queue");

    utr.AddUnitTests(CC::QueueTests::GetTests());

    return utr.RunUnitTests();
}


// Test-Set-Name To Run-Test-Function Mapping
const TestFuncMap s_TestFuncMap
{
    TestPair("All", RunAllTests),
    TestPair("PointerHelper", RunPointerHelperTests),
    TestPair("Pointer", RunPointerTests),
    TestPair("SharedPointer", RunSharedPointerTests),
    TestPair("Buffer", RunBufferTests),
    TestPair("DynamicBuffer", RunDynamicBufferTests),
    TestPair("StringUtil", RunStringUtilTests),
    TestPair("String", RunStringTests),
    TestPair("List", RunListTests),
    TestPair("Stack", RunStackTests),
    TestPair("Queue", RunQueueTests)
};


const TestSetFunc GetTestSetFunction(_In_ const std::string& str)
{
    TestSetFunc func;

    for (const TestPair& pair : s_TestFuncMap)
    {
        const TestName& testName = pair.first;
        const TestSetFunc& testFunc = pair.second;

        if (testName.length() != str.length())
        {
            continue;
        }

        if (_strnicmp(str.c_str(), testName.c_str(), str.length()) == 0)
        {
            func = testFunc;
            break;
        }
    }

    return func;
}

std::list<TestSetFunc> GetTestSetFunctions(_In_ const int& argc, _In_z_count_(argc) const char* argv[])
{
    std::list<TestSetFunc> testFuncList;

    if (argc <= 1)
    {
        printf(__FUNCTION__ ": No test sets specified, running all tests.\r\n");
        testFuncList.push_back(RunAllTests);
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            TestSetFunc f(GetTestSetFunction(argv[i]));
            if (f)
            {
                testFuncList.push_back(f);
            }
            else
            {
                printf(__FUNCTION__": Unknown test name[%s], skipping.\r\n", argv[i]);
            }
        }
    }

    if (testFuncList.empty())
    {
        printf(__FUNCTION__": No tests selected.\r\n");
    }

    return testFuncList;
}