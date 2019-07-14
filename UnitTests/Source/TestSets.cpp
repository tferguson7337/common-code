#include <TestSets.h>

// CC Tests
#include <CCBufferTests.h>
#include <CCDynamicBufferTests.h>
#include <CCPointerTests.h>
#include <CCPointerCommonHelpersTests.h>
#include <CCSharedPointerTests.h>
#include <CCStringTests.h>
#include <CCStringUtilTests.h>


static bool RunAllTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests( );
    utr.GetTestSetData( ).SetTestSetName("CommonCode - All");
    utr.AddUnitTests(CC::PointerCommonHelpersTests::GetTests( ));
    utr.AddUnitTests(CC::PointerTests::GetTests( ));
    utr.AddUnitTests(CC::SharedPointerTests::GetTests( ));
    utr.AddUnitTests(CC::BufferTests::GetTests( ));
    utr.AddUnitTests(CC::DynamicBufferTests::GetTests( ));
    utr.AddUnitTests(CC::StringTests::GetTests( ));
    utr.AddUnitTests(CC::StringUtilTests::GetTests( ));
    return utr.RunUnitTests( );
}

static bool RunPointerCommonHelpersTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests( );
    utr.GetTestSetData( ).SetTestSetName("CommonCode - PointerCommonHelpers");
    utr.AddUnitTests(CC::PointerCommonHelpersTests::GetTests( ));
    return utr.RunUnitTests( );
}

static bool RunPointerTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests( );
    utr.GetTestSetData( ).SetTestSetName("CommonCode - Pointer");
    utr.AddUnitTests(CC::PointerTests::GetTests( ));
    return utr.RunUnitTests( );
}

static bool RunSharedPointerTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests( );
    utr.GetTestSetData( ).SetTestSetName("CommonCode - SharedPointer");
    utr.AddUnitTests(CC::SharedPointerTests::GetTests( ));
    return utr.RunUnitTests( );
}

static bool RunBufferTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests( );
    utr.GetTestSetData( ).SetTestSetName("CommonCode - Buffer");
    utr.AddUnitTests(CC::BufferTests::GetTests( ));
    return utr.RunUnitTests( );
}

static bool RunDynamicBufferTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests( );
    utr.GetTestSetData( ).SetTestSetName("CommonCode - Dynamic Buffer");
    utr.AddUnitTests(CC::DynamicBufferTests::GetTests( ));
    return utr.RunUnitTests( );
}

static bool RunStringUtilTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests( );
    utr.GetTestSetData( ).SetTestSetName("CommonCode - StringUtil");
    utr.AddUnitTests(CC::StringUtilTests::GetTests( ));
    return utr.RunUnitTests( );
}

static bool RunStringTests(_Inout_ UnitTestRunner<char>& utr)
{
    utr.ClearUnitTests( );
    utr.GetTestSetData( ).SetTestSetName("CommonCode - String");
    utr.AddUnitTests(CC::StringTests::GetTests( ));
    return utr.RunUnitTests( );
}


// Test-Set-Name To Run-Test-Function Mapping
const TestFuncMap s_TestFuncMap
{
    TestPair("All", RunAllTests),
    TestPair("PointerCommonHelpers", RunPointerCommonHelpersTests),
    TestPair("Pointer", RunPointerTests),
    TestPair("SharedPointer", RunSharedPointerTests),
    TestPair("Buffer", RunBufferTests),
    TestPair("DynamicBuffer", RunDynamicBufferTests),
    TestPair("StringUtil", RunStringUtilTests),
    TestPair("String", RunStringTests)
};


const TestSetFunc GetTestSetFunction(_In_ const std::string& str)
{
    TestSetFunc func;

    for ( TestPair pair : s_TestFuncMap )
    {
        const TestName& testName = pair.first;
        const TestSetFunc& testFunc = pair.second;

        if ( testName.length( ) != str.length( ) )
        {
            continue;
        }

        if ( _strnicmp(str.c_str( ), testName.c_str( ), sizeof(char) * str.length( )) == 0 )
        {
            func = testFunc;
            break;
        }
    }

    return func;
}

std::list<TestSetFunc> GetTestSetFunctions(_In_ const int& argc, _In_count_(argc) const char* argv[ ])
{
    std::list<TestSetFunc> testFuncList;

    if ( argc <= 1 )
    {
        printf(__FUNCTION__ ": No test sets specified, running all tests.\r\n");
        testFuncList.push_back(RunAllTests);
    }
    else
    {
        for ( int i = 1; i < argc; i++ )
        {
            TestSetFunc f(GetTestSetFunction(argv[i]));
            if ( f )
            {
                testFuncList.push_back(f);
            }
            else
            {
                printf(__FUNCTION__": Unknown test name[%s], skipping.\r\n", argv[i]);
            }
        }
    }

    return testFuncList;
}