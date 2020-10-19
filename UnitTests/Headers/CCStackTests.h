#pragma once

// SUTL
#include <UnitTestResult.h>
#include <TestTypes.h>

// STL
#include <algorithm>
#include <cmath>
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

        // Test Subclasses //

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
            Stack<T> stack;

            SUTL_TEST_ASSERT(!stack);
            SUTL_TEST_ASSERT(!stack.m_FwdList);

            SUTL_TEST_SUCCESS();
        }

        template <typename T>
        [[nodiscard]] static UTR ElementCopyCtor()
        {
            const std::vector<T> testData(GetTestData<T, TestQuantity::VeryLow>());
            Stack<T> stack(testData.back());

            SUTL_TEST_ASSERT(!!stack);
            SUTL_TEST_ASSERT(stack.Top() == testData.back());
            SUTL_TEST_ASSERT(stack.Length() == 1);
            if constexpr (std::is_same_v<T, Helper>)
            {
                SUTL_TEST_ASSERT(stack.Top().m_bCopied);
                SUTL_TEST_ASSERT(!stack.Top().m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T>
        [[nodiscard]] static UTR ElementMoveCtor()
        {
            std::vector<T> testData(GetTestData<T, TestQuantity::VeryLow>());
            Stack<T> stack(std::move(testData.back()));

            SUTL_TEST_ASSERT(!!stack);
            SUTL_TEST_ASSERT(stack.Top() == testData.back());
            SUTL_TEST_ASSERT(stack.Length() == 1);
            if constexpr (std::is_same_v<T, Helper>)
            {
                SUTL_TEST_ASSERT(!stack.Top().m_bCopied);
                SUTL_TEST_ASSERT(stack.Top().m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR StackCopyCtor()
        {
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            Stack<T> src;

            if constexpr (srcLen != 0)
            {
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Push(testData[i]));
                    SUTL_SETUP_ASSERT(!!src);
                    SUTL_SETUP_ASSERT(src.Length() == i + 1);
                    SUTL_SETUP_ASSERT(src.Top() == testData[i]);
                }

                SUTL_SETUP_ASSERT(!!src);
                SUTL_SETUP_ASSERT(src.Length() == srcLen);
            }
            else
            {
                SUTL_SETUP_ASSERT(!src);
                SUTL_SETUP_ASSERT(src.Length() == 0);
            }

            Stack<T> dst(src);
            SUTL_TEST_ASSERT(dst.Length() == src.Length());

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_FwdList.m_pHead;
                auto pSrc = src.m_FwdList.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);
                    SUTL_TEST_ASSERT(pDst->data == testData[srcLen - i - 1]);
                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst);
                SUTL_TEST_ASSERT(!src);
                SUTL_TEST_ASSERT(dst.Length() == 0);
                SUTL_TEST_ASSERT(src.Length() == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR StackMoveCtor()
        {
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            Stack<T> src;

            if constexpr (srcLen != 0)
            {
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Push(testData[i]));
                    SUTL_SETUP_ASSERT(!!src);
                    SUTL_SETUP_ASSERT(src.Length() == i + 1);
                    SUTL_SETUP_ASSERT(src.Top() == testData[i]);
                }

                SUTL_SETUP_ASSERT(!!src);
                SUTL_SETUP_ASSERT(src.Length() == srcLen);
            }
            else
            {
                SUTL_SETUP_ASSERT(!src);
                SUTL_SETUP_ASSERT(src.Length() == 0);
            }

            auto pSrc = src.m_FwdList.m_pHead;
            Stack<T> dst(std::move(src));
            SUTL_TEST_ASSERT(dst.m_FwdList.m_pHead == pSrc);
            SUTL_TEST_ASSERT(dst.Length() == srcLen);
            SUTL_TEST_ASSERT(!src);
            SUTL_TEST_ASSERT(src.Length() == 0);

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_FwdList.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[srcLen - i - 1]);
                    pDst = pDst->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst);
                SUTL_TEST_ASSERT(dst.Length() == 0);
            }

            SUTL_TEST_SUCCESS();
        }
    };


    // Tests assignment methods.
    class StackTests::AssignTests
    {
    public:

        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR AssignMethodElementCopy()
        {
            constexpr size_t len = GetTQLength<DstLen>();
            const T testData(GetTestData<T, TestQuantity::Low>().back());
            Stack<T> dst;

            // Setup dst stack.
            if constexpr (len != 0)
            {
                const std::vector<T> srcTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(srcTestData[i]));
                    SUTL_SETUP_ASSERT(!!dst);
                    SUTL_SETUP_ASSERT(dst.Length() == i + 1);
                    SUTL_SETUP_ASSERT(dst.Top() == srcTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == len);
            }

            // Save off current stack head.
            auto pHead = dst.m_FwdList.m_pHead;

            // Assign the element.
            SUTL_TEST_ASSERT(dst.Assign(testData));
            SUTL_TEST_ASSERT(!!dst);
            SUTL_TEST_ASSERT(dst.Length() == 1);
            SUTL_TEST_ASSERT(dst.m_FwdList.m_pHead != pHead);
            SUTL_TEST_ASSERT(dst.Top() == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure data was copied (not moved).
                SUTL_TEST_ASSERT(dst.Top().m_bCopied);
                SUTL_TEST_ASSERT(!dst.Top().m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR AssignMethodElementMove()
        {
            constexpr size_t len = GetTQLength<DstLen>();
            T testData(GetTestData<T, TestQuantity::Low>().back());
            Stack<T> dst;

            // Setup dst stack.
            if constexpr (len != 0)
            {
                const std::vector<T> srcTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(srcTestData[i]));
                    SUTL_SETUP_ASSERT(!!dst);
                    SUTL_SETUP_ASSERT(dst.Length() == i + 1);
                    SUTL_SETUP_ASSERT(dst.Top() == srcTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == len);
            }

            // Save off current stack head.
            auto pHead = dst.m_FwdList.m_pHead;

            // Assign the element.
            SUTL_TEST_ASSERT(dst.Assign(std::move(testData)));
            SUTL_TEST_ASSERT(!!dst);
            SUTL_TEST_ASSERT(dst.Length() == 1);
            SUTL_TEST_ASSERT(dst.m_FwdList.m_pHead != pHead);
            SUTL_TEST_ASSERT(dst.Top() == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure data was copied (not moved).
                SUTL_TEST_ASSERT(!dst.Top().m_bCopied);
                SUTL_TEST_ASSERT(dst.Top().m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignMethodStackCopy()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Stack<T> dst;
            Stack<T> src;

            // Setup dst stack.
            if constexpr (dstLen != 0)
            {
                const std::vector<T> dstTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(dstTestData[i]));
                    SUTL_SETUP_ASSERT(!!dst);
                    SUTL_SETUP_ASSERT(dst.Length() == i + 1);
                    SUTL_SETUP_ASSERT(dst.Top() == dstTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == dstLen);
            }

            // Setup src stack.
            if constexpr (srcLen != 0)
            {
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Push(srcTestData[i]));
                    SUTL_SETUP_ASSERT(!!src);
                    SUTL_SETUP_ASSERT(src.Length() == i + 1);
                    SUTL_SETUP_ASSERT(src.Top() == srcTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!src);
                SUTL_SETUP_ASSERT(src.Length() == srcLen);
            }

            SUTL_TEST_ASSERT(dst.Assign(src));
            SUTL_TEST_ASSERT(dst.Length() == src.Length());

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_FwdList.m_pHead;
                auto pSrc = src.m_FwdList.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);
                    SUTL_TEST_ASSERT(pDst->data == srcTestData[srcLen - i - 1]);
                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst);
                SUTL_TEST_ASSERT(!src);
                SUTL_TEST_ASSERT(dst.Length() == 0);
                SUTL_TEST_ASSERT(src.Length() == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignMethodStackMove()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Stack<T> dst;
            Stack<T> src;

            // Setup dst stack.
            if constexpr (dstLen != 0)
            {
                const std::vector<T> dstTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(dstTestData[i]));
                    SUTL_SETUP_ASSERT(!!dst);
                    SUTL_SETUP_ASSERT(dst.Length() == i + 1);
                    SUTL_SETUP_ASSERT(dst.Top() == dstTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == dstLen);
            }

            // Setup src stack.
            if constexpr (srcLen != 0)
            {
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Push(srcTestData[i]));
                    SUTL_SETUP_ASSERT(!!src);
                    SUTL_SETUP_ASSERT(src.Length() == i + 1);
                    SUTL_SETUP_ASSERT(src.Top() == srcTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!src);
                SUTL_SETUP_ASSERT(src.Length() == srcLen);
            }

            auto pSrc = src.m_FwdList.m_pHead;
            SUTL_TEST_ASSERT(dst.Assign(std::move(src)));
            SUTL_TEST_ASSERT(dst.m_FwdList.m_pHead == pSrc);
            SUTL_TEST_ASSERT(dst.Length() == srcLen);
            SUTL_TEST_ASSERT(!src);
            SUTL_TEST_ASSERT(src.Length() == 0);

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_FwdList.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == srcTestData[srcLen - i - 1]);
                    pDst = pDst->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst);
                SUTL_TEST_ASSERT(!src);
                SUTL_TEST_ASSERT(dst.Length() == 0);
                SUTL_TEST_ASSERT(src.Length() == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR AssignOperatorElementCopy()
        {
            constexpr size_t len = GetTQLength<DstLen>();
            const T testData(GetTestData<T, TestQuantity::Low>().back());
            Stack<T> dst;

            // Setup dst stack.
            if constexpr (len != 0)
            {
                const std::vector<T> srcTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(srcTestData[i]));
                    SUTL_SETUP_ASSERT(!!dst);
                    SUTL_SETUP_ASSERT(dst.Length() == i + 1);
                    SUTL_SETUP_ASSERT(dst.Top() == srcTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == len);
            }

            // Save off current stack head.
            auto pHead = dst.m_FwdList.m_pHead;

            // Assign the element.
            dst = testData;
            SUTL_TEST_ASSERT(!!dst);
            SUTL_TEST_ASSERT(dst.Length() == 1);
            SUTL_TEST_ASSERT(dst.m_FwdList.m_pHead != pHead);
            SUTL_TEST_ASSERT(dst.Top() == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure data was copied (not moved).
                SUTL_TEST_ASSERT(dst.Top().m_bCopied);
                SUTL_TEST_ASSERT(!dst.Top().m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR AssignOperatorElementMove()
        {
            constexpr size_t len = GetTQLength<DstLen>();
            T testData(GetTestData<T, TestQuantity::Low>().back());
            Stack<T> dst;

            // Setup dst stack.
            if constexpr (len != 0)
            {
                const std::vector<T> srcTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(srcTestData[i]));
                    SUTL_SETUP_ASSERT(!!dst);
                    SUTL_SETUP_ASSERT(dst.Length() == i + 1);
                    SUTL_SETUP_ASSERT(dst.Top() == srcTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == len);
            }

            // Save off current stack head.
            auto pHead = dst.m_FwdList.m_pHead;

            // Assign the element.
            dst = std::move(testData);
            SUTL_TEST_ASSERT(!!dst);
            SUTL_TEST_ASSERT(dst.Length() == 1);
            SUTL_TEST_ASSERT(dst.m_FwdList.m_pHead != pHead);
            SUTL_TEST_ASSERT(dst.Top() == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure data was copied (not moved).
                SUTL_TEST_ASSERT(!dst.Top().m_bCopied);
                SUTL_TEST_ASSERT(dst.Top().m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignOperatorStackCopy()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Stack<T> dst;
            Stack<T> src;

            // Setup dst stack.
            if constexpr (dstLen != 0)
            {
                const std::vector<T> dstTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(dstTestData[i]));
                    SUTL_SETUP_ASSERT(!!dst);
                    SUTL_SETUP_ASSERT(dst.Length() == i + 1);
                    SUTL_SETUP_ASSERT(dst.Top() == dstTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == dstLen);
            }

            // Setup src stack.
            if constexpr (srcLen != 0)
            {
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Push(srcTestData[i]));
                    SUTL_SETUP_ASSERT(!!src);
                    SUTL_SETUP_ASSERT(src.Length() == i + 1);
                    SUTL_SETUP_ASSERT(src.Top() == srcTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!src);
                SUTL_SETUP_ASSERT(src.Length() == srcLen);
            }

            dst = src;
            SUTL_TEST_ASSERT(dst.Length() == src.Length());

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_FwdList.m_pHead;
                auto pSrc = src.m_FwdList.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);
                    SUTL_TEST_ASSERT(pDst->data == srcTestData[srcLen - i - 1]);
                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst);
                SUTL_TEST_ASSERT(!src);
                SUTL_TEST_ASSERT(dst.Length() == 0);
                SUTL_TEST_ASSERT(src.Length() == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignOperatorStackMove()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Stack<T> dst;
            Stack<T> src;

            // Setup dst stack.
            if constexpr (dstLen != 0)
            {
                const std::vector<T> dstTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(dstTestData[i]));
                    SUTL_SETUP_ASSERT(!!dst);
                    SUTL_SETUP_ASSERT(dst.Length() == i + 1);
                    SUTL_SETUP_ASSERT(dst.Top() == dstTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == dstLen);
            }

            // Setup src stack.
            if constexpr (srcLen != 0)
            {
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Push(srcTestData[i]));
                    SUTL_SETUP_ASSERT(!!src);
                    SUTL_SETUP_ASSERT(src.Length() == i + 1);
                    SUTL_SETUP_ASSERT(src.Top() == srcTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!src);
                SUTL_SETUP_ASSERT(src.Length() == srcLen);
            }

            auto pSrc = src.m_FwdList.m_pHead;
            dst = std::move(src);
            SUTL_TEST_ASSERT(dst.m_FwdList.m_pHead == pSrc);
            SUTL_TEST_ASSERT(dst.Length() == srcLen);
            SUTL_TEST_ASSERT(!src);
            SUTL_TEST_ASSERT(src.Length() == 0);

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_FwdList.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == srcTestData[srcLen - i - 1]);
                    pDst = pDst->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst);
                SUTL_TEST_ASSERT(!src);
                SUTL_TEST_ASSERT(dst.Length() == 0);
                SUTL_TEST_ASSERT(src.Length() == 0);
            }

            SUTL_TEST_SUCCESS();
        }
    };


    // Tests comparison methods.
    class StackTests::ComparisonTests
    {
    public:

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR ComparisonMethod()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            constexpr bool bExpectMatch = (dstLen == srcLen);
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Stack<T> dst;
            Stack<T> src;

            // Setup dst stack.
            if constexpr (dstLen != 0)
            {
                const std::vector<T> dstTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(dstTestData[i]));
                    SUTL_SETUP_ASSERT(!!dst);
                    SUTL_SETUP_ASSERT(dst.Length() == i + 1);
                    SUTL_SETUP_ASSERT(dst.Top() == dstTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == dstLen);
            }

            // Setup src stack.
            if constexpr (srcLen != 0)
            {
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Push(srcTestData[i]));
                    SUTL_SETUP_ASSERT(!!src);
                    SUTL_SETUP_ASSERT(src.Length() == i + 1);
                    SUTL_SETUP_ASSERT(src.Top() == srcTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!src);
                SUTL_SETUP_ASSERT(src.Length() == srcLen);
            }

            SUTL_TEST_ASSERT(dst.Compare(src) == bExpectMatch);

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR ComparisonOperator()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            constexpr bool bExpectMatch = (dstLen == srcLen);
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Stack<T> dst;
            Stack<T> src;

            // Setup dst stack.
            if constexpr (dstLen != 0)
            {
                const std::vector<T> dstTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(dstTestData[i]));
                    SUTL_SETUP_ASSERT(!!dst);
                    SUTL_SETUP_ASSERT(dst.Length() == i + 1);
                    SUTL_SETUP_ASSERT(dst.Top() == dstTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == dstLen);
            }

            // Setup src stack.
            if constexpr (srcLen != 0)
            {
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Push(srcTestData[i]));
                    SUTL_SETUP_ASSERT(!!src);
                    SUTL_SETUP_ASSERT(src.Length() == i + 1);
                    SUTL_SETUP_ASSERT(src.Top() == srcTestData[i]);
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!src);
                SUTL_SETUP_ASSERT(src.Length() == srcLen);
            }

            SUTL_TEST_ASSERT((dst == src) == bExpectMatch);

            SUTL_TEST_SUCCESS();
        }
    };


    // Tests getter methods (e.g., Top, Length, etc.).
    class StackTests::GetterTests
    {
    public:

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Length()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());
            Stack<T> dst;

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(testData[i]));
                    SUTL_TEST_ASSERT(dst.Length() == i + 1);
                }
            }

            SUTL_TEST_ASSERT(dst.Length() == len);

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Top()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());
            Stack<T> dst;

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(testData[i]));
                    try
                    {
                        SUTL_TEST_ASSERT(dst.Top() == testData[i]);
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }
                }
            }
            else
            {
                bool bThrew = false;
                try
                {
                    T tmp = dst.Top();
                    SUTL_TEST_ASSERT(tmp == T());
                }
                catch (const std::logic_error&)
                {
                    bThrew = true;
                }
                catch (const std::exception& e)
                {
                    SUTL_TEST_EXCEPTION(e.what());
                }

                SUTL_TEST_ASSERT(bThrew);
            }

            SUTL_TEST_SUCCESS();
        }
    };


    // Tests push and pop methods.
    class StackTests::PushPopTests
    {
    public:

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Push()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());
            Stack<T> dst;

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(testData[i]));
                    try
                    {
                        SUTL_TEST_ASSERT(dst.Top() == testData[i]);
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst);
                SUTL_TEST_ASSERT(dst.Length() == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Pop()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());
            Stack<T> dst;

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == len);
            }

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!dst);
                    SUTL_TEST_ASSERT(dst.Top() == testData[len - i - 1]);
                    SUTL_TEST_ASSERT(dst.Length() == len - i);

                    auto pNext = dst.m_FwdList.m_pHead->pNext;
                    SUTL_TEST_ASSERT(dst.Pop());
                    SUTL_TEST_ASSERT(dst.m_FwdList.m_pHead == pNext);
                    SUTL_TEST_ASSERT(dst.Length() == len - i - 1);

                    if (!!pNext)
                    {
                        SUTL_TEST_ASSERT(i < len - 1);
                        SUTL_TEST_ASSERT(dst.Top() == testData[len - i - 2]);
                    }
                    else
                    {
                        SUTL_TEST_ASSERT(i == len - 1);
                    }
                }
            }

            try
            {
                SUTL_TEST_ASSERT(!dst.Pop());
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR PopAndMove()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());
            Stack<T> dst;

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Push(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!dst);
                SUTL_SETUP_ASSERT(dst.Length() == len);
            }

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!dst);
                    SUTL_TEST_ASSERT(dst.Top() == testData[len - i - 1]);
                    SUTL_TEST_ASSERT(dst.Length() == len - i);

                    T tmp;
                    auto pNext = dst.m_FwdList.m_pHead->pNext;
                    SUTL_TEST_ASSERT(dst.Pop(tmp));
                    SUTL_TEST_ASSERT(tmp == testData[len - i - 1]);
                    SUTL_TEST_ASSERT(dst.m_FwdList.m_pHead == pNext);
                    SUTL_TEST_ASSERT(dst.Length() == len - i - 1);

                    if (!!pNext)
                    {
                        SUTL_TEST_ASSERT(i < len - 1);
                        SUTL_TEST_ASSERT(dst.Top() == testData[len - i - 2]);
                    }
                    else
                    {
                        SUTL_TEST_ASSERT(i == len - 1);
                    }
                }
            }

            try
            {
                SUTL_TEST_ASSERT(!dst.Pop());
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR PushPopStagger()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());

            for (size_t staggerThreshold = 1; staggerThreshold <= static_cast<size_t>(sqrt(len)) + 1; staggerThreshold++)
            {
                Stack<T> stack;

                if constexpr (len != 0)
                {
                    size_t staggerCount = 0;
                    for (size_t i = 0; i < len; i++)
                    {
                        SUTL_TEST_ASSERT(stack.Push(testData[i]));
                        SUTL_TEST_ASSERT(stack.Top() == testData[i]);
                        if ((i % staggerThreshold) == 0)
                        {
                            staggerCount++;
                            SUTL_TEST_ASSERT(stack.Pop());
                            if (!!stack)
                            {
                                SUTL_TEST_ASSERT(stack.Top() == testData[i - 1]);
                                SUTL_TEST_ASSERT(stack.Length() != 0);
                            }
                            else
                            {
                                SUTL_TEST_ASSERT(stack.Length() == 0);
                                SUTL_TEST_ASSERT(((i + 1) - staggerCount) == 0);
                            }
                        }

                        SUTL_TEST_ASSERT(stack.Length() == ((i + 1) - staggerCount));
                    }

                    for (size_t i = 0; i < len; i++)
                    {
                        if (((len - i - 1) % staggerThreshold) == 0)
                        {
                            continue;
                        }

                        SUTL_TEST_ASSERT(!!stack);
                        SUTL_TEST_ASSERT(stack.Top() == testData[len - i - 1]);
                        SUTL_TEST_ASSERT(stack.Pop());
                    }
                }

                SUTL_TEST_ASSERT(!stack);
                SUTL_TEST_ASSERT(stack.Length() == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR PushPopAndMoveStagger()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());

            for (size_t staggerThreshold = 1; staggerThreshold <= static_cast<size_t>(sqrt(len)) + 1; staggerThreshold++)
            {
                Stack<T> stack;

                if constexpr (len != 0)
                {
                    T tmp;
                    size_t staggerCount = 0;
                    for (size_t i = 0; i < len; i++)
                    {
                        SUTL_TEST_ASSERT(stack.Push(testData[i]));
                        SUTL_TEST_ASSERT(stack.Top() == testData[i]);
                        if ((i % staggerThreshold) == 0)
                        {
                            staggerCount++;
                            SUTL_TEST_ASSERT(stack.Pop(tmp));
                            SUTL_TEST_ASSERT(tmp == testData[i]);
                            if constexpr (std::is_same_v<T, Helper>)
                            {
                                SUTL_TEST_ASSERT(!tmp.m_bCopied);
                                SUTL_TEST_ASSERT(tmp.m_bMoved);
                            }

                            if (!!stack)
                            {
                                SUTL_TEST_ASSERT(stack.Top() == testData[i - 1]);
                                SUTL_TEST_ASSERT(stack.Length() != 0);
                            }
                            else
                            {
                                SUTL_TEST_ASSERT(stack.Length() == 0);
                                SUTL_TEST_ASSERT(((i + 1) - staggerCount) == 0);
                            }
                        }

                        SUTL_TEST_ASSERT(stack.Length() == ((i + 1) - staggerCount));
                    }

                    for (size_t i = 0; i < len; i++)
                    {
                        if (((len - i - 1) % staggerThreshold) == 0)
                        {
                            continue;
                        }

                        SUTL_TEST_ASSERT(!!stack);
                        SUTL_TEST_ASSERT(stack.Top() == testData[len - i - 1]);
                        SUTL_TEST_ASSERT(stack.Pop(tmp));
                        SUTL_TEST_ASSERT(tmp == testData[len - i - 1]);
                        if constexpr (std::is_same_v<T, Helper>)
                        {
                            SUTL_TEST_ASSERT(!tmp.m_bCopied);
                            SUTL_TEST_ASSERT(tmp.m_bMoved);
                        }
                    }
                }

                SUTL_TEST_ASSERT(!stack);
                SUTL_TEST_ASSERT(stack.Length() == 0);
            }

            SUTL_TEST_SUCCESS();
        }
    };
}
