#pragma once

// SUTL
#include <UnitTestResult.h>
#include <TestQuantity.h>

// STL
#include <algorithm>
#include <functional>
#include <list>
#include <memory>

// Target Class
#include <CCStack.h>

// Test Helper Utils
#include <TestHelpers.hpp>


namespace CC
{
    class StackTests
    {
        // Type aliases.
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR()>;
        using UTList = std::list<UTFunc>;

        StackTests() = delete;
        StackTests(const StackTests&) = delete;
        StackTests(StackTests&&) = delete;
        ~StackTests() = delete;
        StackTests& operator=(const StackTests&) = delete;
        StackTests& operator=(StackTests&&) = delete;

    private:

        /// Test Subclasses \\\

        // Test constructor behavior.
        class ConstructorTests;

        // Tests assignment methods.
        class AssignTests;

        // Tests comparison methods.
        class ComparisonTests;

        // Tests getter methods (e.g., Top, Length, etc.).
        class GetterTests;

        // Tests push and pop methods.
        class PushPopTests;

    public:

        // Returns list of List unit tests.
        [[nodiscard]] static UTList GetTests();
    };


    // Test constructor behavior.
    class StackTests::ConstructorTests
    {
    public:

        template <typename T>
        [[nodiscard]] static UTR DefaultCtor()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T>
        [[nodiscard]] static UTR ElementCopyCtor()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T>
        [[nodiscard]] static UTR ElementMoveCtor()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR StackCopyCtor()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR StackMoveCtor()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }
    };


    // Tests assignment methods.
    class StackTests::AssignTests
    {
    public:

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignMethodElementCopy()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignMethodElementMove()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignMethodStackCopy()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignMethodStackMove()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignOperatorElementCopy()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignOperatorElementMove()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignOperatorStackCopy()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignOperatorStackMove()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }
    };


    // Tests comparison methods.
    class StackTests::ComparisonTests
    {
    public:

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR ComparisonMethod()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR ComparisonOperator()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }
    };


    // Tests getter methods (e.g., Top, Length, etc.).
    class StackTests::GetterTests
    {
    public:

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Length()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Top()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }
    };


    // Tests push and pop methods.
    class StackTests::PushPopTests
    {
    public:

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Push()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Pop()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR PushPopStagger()
        {


            SUTL_SKIP_TEST("Implementation pending...");
        }
    };
}
