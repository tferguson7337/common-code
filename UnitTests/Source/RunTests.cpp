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
    bool bPass = false;

    UnitTestRunner<char> utr("Common-Code");
    
    GetTests(utr);
    
    bPass = utr.RunUnitTests( );

    return bPass ? 0 : 1;
}