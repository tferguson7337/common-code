// SUTL
#include <UnitTestRunner.h>

// Tests
#include <TestSets.h>


int main(const int argc, const char* argv[ ])
{
	bool bPass = true;
	UnitTestRunner<char> utr;
	std::list<TestSetFunc> testFuncList(GetTestSetFunctions(argc, argv));

	if ( testFuncList.empty( ) )
	{
		printf("No tests selected.\r\n");
	}

	for ( const TestSetFunc& f : testFuncList )
	{
		bPass &= f(utr);
	}

	return bPass ? 0 : 1;
}