#pragma once

// SUTL
#include <UnitTestResult.h>

// STL
#include <algorithm>
#include <functional>
#include <list>
#include <memory>

// Target Class
#include <CCSharedPointer.h>

// Test Helper Utils
#include <CCPointerCommonHelpersTests.h>

namespace CC
{
    class SharedPointerTests
    {
        SharedPointerTests( ) = delete;
        SharedPointerTests(const SharedPointerTests&) = delete;
        SharedPointerTests(SharedPointerTests&&) = delete;
        ~SharedPointerTests( ) = delete;
        SharedPointerTests& operator=(const SharedPointerTests&) = delete;
        SharedPointerTests& operator=(SharedPointerTests&&) = delete;

    private:

        // Type aliases
        using TestQuantity = PointerCommonHelpersTests::TestQuantity;
        using Helper = PointerCommonHelpersTests::Helper;

        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;

    public:

        // Returns list of SharedPointer unit tests.
        static UTList GetTests( );

        /// Test Subclasses \\\


    };
}