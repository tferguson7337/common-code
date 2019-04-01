// SUTL
#include <UnitTestRunner.h>

// Tests
#include <StringUtilTests.h>


void GetTests(UnitTestRunner<char>& utr)
{
    utr.AddUnitTests(StringUtilTests::GetTests( ));
}

int main( )
{
    UnitTestRunner<char> utr("Common-Code");

    GetTests(utr);

    return utr.RunUnitTests( ) ? 0 : 1;
}