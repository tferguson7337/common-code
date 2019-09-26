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
#include <CCForwardList.h>

// Test Helper Utils
#include <TestHelpers.hpp>


namespace CC
{
    class ForwardListTests
    {
        // Type aliases.
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR()>;
        using UTList = std::list<UTFunc>;

        ForwardListTests() = delete;
        ForwardListTests(const ForwardListTests&) = delete;
        ForwardListTests(ForwardListTests&&) = delete;
        ~ForwardListTests() = delete;
        ForwardListTests& operator=(const ForwardListTests&) = delete;
        ForwardListTests& operator=(ForwardListTests&&) = delete;

    private:

        /// Tests Subclasses \\\

                // Tests constructor behavior.
        class ConstructorTests;

        // Tests assignment methods.
        class AssignTests;

        // Tests insertion methods (e.g., Append, Prepend, InsertAt, etc.).
        class InsertionTests;

        // Tests getter methods (e.g., Front, Back, At, etc.).
        class GetterTests;

        // Tests comparison methods.
        class ComparisonTests;

        // Tests removal methods (e.g., PopFront, PopBack, RemoveAt, etc.).
        class RemovalTests;

    public:

        // Returns list of List unit tests.
        [[nodiscard]] static UTList GetTests();
    };

    // Tests constructor behavior.
    class ForwardListTests::ConstructorTests
    {
    public:

        template <typename T>
        [[nodiscard]] static UTR DefaultCtor()
        {
            ForwardList<T> list;

            SUTL_TEST_ASSERT(!list.m_pHead);
            SUTL_TEST_ASSERT(!list.m_pTail);
            SUTL_TEST_ASSERT(list.m_Len == 0);

            SUTL_TEST_SUCCESS();
        }

        template <typename T>
        [[nodiscard]] static UTR ElementCopyCtor()
        {
            const T testData(GetTestData<T, TestQuantity::VeryLow>().front());
            ForwardList<T> list(testData);

            // Verify single-element conditions.
            SUTL_TEST_ASSERT(!!list.m_pHead);
            SUTL_TEST_ASSERT(!!list.m_pTail);
            SUTL_TEST_ASSERT(list.m_pHead == list.m_pTail);
            SUTL_TEST_ASSERT(list.m_Len == 1);
            SUTL_TEST_ASSERT(!list.m_pHead->pNext);

            // Ensure data matches up.
            SUTL_TEST_ASSERT(list.m_pHead->data == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure the test data was copied (not moved).
                SUTL_TEST_ASSERT(list.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(!list.m_pHead->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T>
        [[nodiscard]] static UTR ElementMoveCtor()
        {
            T testData(GetTestData<T, TestQuantity::VeryLow>().front());
            ForwardList<T> list(std::move(testData));

            // Verify single-element conditions.
            SUTL_TEST_ASSERT(!!list.m_pHead);
            SUTL_TEST_ASSERT(!!list.m_pTail);
            SUTL_TEST_ASSERT(list.m_pHead == list.m_pTail);
            SUTL_TEST_ASSERT(list.m_Len == 1);
            SUTL_TEST_ASSERT(!list.m_pHead->pNext);

            // Ensure data matches up.
            SUTL_TEST_ASSERT(list.m_pHead->data == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure the test data was moved (not copied).
                SUTL_TEST_ASSERT(!list.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(list.m_pHead->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR ListCopyCtor()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            ForwardList<T> src;

            // Setup src list.
            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(src.Append(testData[i]));
                }

                // Verify initial src conditions.
                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            // Copy list.
            ForwardList<T> dst(src);

            // Verify the copy.
            SUTL_TEST_ASSERT((!!dst.m_pHead) == (len != 0));
            SUTL_TEST_ASSERT((!!dst.m_pTail) == (len != 0));
            SUTL_TEST_ASSERT(dst.m_Len == len);

            if constexpr (len != 0)
            {
                auto pSrc = src.m_pHead;
                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(!!pDst);

                    SUTL_TEST_ASSERT(pSrc->data == pDst->data);

                    pSrc = pSrc->pNext;
                    pDst = pDst->pNext;
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR ListMoveCtor()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            ForwardList<T> src;

            // Setup src list.
            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(src.Append(testData[i]));
                }

                // Verify initial src conditions.
                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            // Save off current src head/tail.
            auto pHead = src.m_pHead;
            auto pTail = src.m_pTail;

            // Move list.
            ForwardList<T> dst(std::move(src));

            // Verify the move - empty src.
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);

            // Verify the move - transfered resources.
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pTail == pTail);
            SUTL_TEST_ASSERT(dst.m_Len == len);

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pHead);
                    SUTL_TEST_ASSERT(pHead->data == testData[i]);

                    pHead = pHead->pNext;
                }
            }

            SUTL_TEST_SUCCESS();
        }
    };


    // Tests assignment methods.
    class ForwardListTests::AssignTests
    {
    public:

        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR AssignMethodElementCopy()
        {
            constexpr size_t len = GetTQLength<DstLen>();
            const T testData(GetTestData<T, TestQuantity::VeryLow>().front());
            ForwardList<T> dst;

            // Setup dst list.
            if constexpr (len != 0)
            {
                std::vector<T> srcTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(srcTestData[i]));
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == len);
            }

            // Save off current list head/tail.
            auto pHead = dst.m_pHead;
            auto pTail = dst.m_pTail;

            // Assign the element.
            SUTL_TEST_ASSERT(dst.Assign(testData));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != pHead);
            SUTL_TEST_ASSERT(dst.m_pTail != pTail);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure data was copied (not moved).
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR AssignMethodElementMove()
        {
            constexpr size_t len = GetTQLength<DstLen>();
            T testData(GetTestData<T, TestQuantity::VeryLow>().front());
            ForwardList<T> dst;

            // Setup dst list.
            if constexpr (len != 0)
            {
                std::vector<T> srcTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(srcTestData[i]));
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == len);
            }

            // Save off current list head/tail.
            auto pHead = dst.m_pHead;
            auto pTail = dst.m_pTail;

            // Assign the element.
            SUTL_TEST_ASSERT(dst.Assign(std::move(testData)));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != pHead);
            SUTL_TEST_ASSERT(dst.m_pTail != pTail);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure data was moved (not copied).
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignMethodListCopy()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> dstTestData(GetTestData<T, DstLen>());
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());

            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (dstLen != 0)
            {
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(dstTestData[i]));
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == dstLen);
            }

            if constexpr (srcLen != 0)
            {
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Append(srcTestData[i]));
                }

                // Verify initial src conditions.
                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == srcLen);
            }

            // Save off current dst head/tail.
            auto pDstHead = dst.m_pHead;
            auto pDstTail = dst.m_pTail;

            // Save off current src head/tail.
            auto pSrcHead = src.m_pHead;
            auto pSrcTail = src.m_pTail;

            // Verify list copy assign.
            SUTL_TEST_ASSERT(dst.Assign(src));
            if constexpr (dstLen == 0 && srcLen == 0)
            {
                // Old dst/src and new dst are all empty.
                SUTL_TEST_ASSERT(pDstHead == dst.m_pHead);
                SUTL_TEST_ASSERT(pDstTail == dst.m_pTail);
                SUTL_TEST_ASSERT(pSrcHead == dst.m_pHead);
                SUTL_TEST_ASSERT(pSrcTail == dst.m_pTail);
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_Len == 0);
            }
            else
            {
                // Dst either went from empty to not or vice-versa.
                SUTL_TEST_ASSERT(pDstHead != dst.m_pHead);
                SUTL_TEST_ASSERT(pDstTail != dst.m_pTail);
                if constexpr (srcLen == 0)
                {
                    // Dst is empty due to empty src.
                    SUTL_TEST_ASSERT(pSrcHead == dst.m_pHead);
                    SUTL_TEST_ASSERT(pSrcTail == dst.m_pTail);
                    SUTL_TEST_ASSERT(!dst.m_pHead);
                    SUTL_TEST_ASSERT(!dst.m_pTail);
                    SUTL_TEST_ASSERT(dst.m_Len == 0);
                }
                else
                {
                    // Dst is not empty due to non-empty src.
                    SUTL_TEST_ASSERT(pSrcHead != dst.m_pHead);
                    SUTL_TEST_ASSERT(pSrcTail != dst.m_pTail);
                    SUTL_TEST_ASSERT(!!dst.m_pHead);
                    SUTL_TEST_ASSERT(!!dst.m_pTail);
                    SUTL_TEST_ASSERT(dst.m_Len == srcLen);
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignMethodListMove()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> dstTestData(GetTestData<T, DstLen>());
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());

            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (dstLen != 0)
            {
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(dstTestData[i]));
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == dstLen);
            }

            if constexpr (srcLen != 0)
            {
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Append(srcTestData[i]));
                }

                // Verify initial src conditions.
                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == srcLen);
            }

            // Save off current dst head/tail.
            auto pDstHead = dst.m_pHead;
            auto pDstTail = dst.m_pTail;

            // Save off current src head/tail.
            auto pSrcHead = src.m_pHead;
            auto pSrcTail = src.m_pTail;

            // Verify list move assign.
            SUTL_TEST_ASSERT(dst.Assign(std::move(src)));
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);

            if constexpr (dstLen == 0 && srcLen == 0)
            {
                // Old dst/src and new dst are all empty.
                SUTL_TEST_ASSERT(pDstHead == dst.m_pHead);
                SUTL_TEST_ASSERT(pDstTail == dst.m_pTail);
                SUTL_TEST_ASSERT(pSrcHead == dst.m_pHead);
                SUTL_TEST_ASSERT(pSrcTail == dst.m_pTail);
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_Len == 0);
            }
            else
            {
                // Dst either went from empty to not or vice-versa.
                SUTL_TEST_ASSERT(pDstHead != dst.m_pHead);
                SUTL_TEST_ASSERT(pDstTail != dst.m_pTail);
                SUTL_TEST_ASSERT(pSrcHead == dst.m_pHead);
                SUTL_TEST_ASSERT(pSrcTail == dst.m_pTail);
                if constexpr (srcLen == 0)
                {
                    // Dst is empty due to empty src.
                    SUTL_TEST_ASSERT(!dst.m_pHead);
                    SUTL_TEST_ASSERT(!dst.m_pTail);
                    SUTL_TEST_ASSERT(dst.m_Len == 0);
                }
                else
                {
                    // Dst is not empty due to non-empty src.
                    SUTL_TEST_ASSERT(!!dst.m_pHead);
                    SUTL_TEST_ASSERT(!!dst.m_pTail);
                    SUTL_TEST_ASSERT(dst.m_Len == srcLen);
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR AssignOperatorElementCopy()
        {
            constexpr size_t len = GetTQLength<DstLen>();
            const T testData(GetTestData<T, TestQuantity::VeryLow>().front());
            ForwardList<T> dst;

            // Setup dst list.
            if constexpr (len != 0)
            {
                std::vector<T> srcTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(srcTestData[i]));
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == len);
            }

            // Save off current list head/tail.
            auto pHead = dst.m_pHead;
            auto pTail = dst.m_pTail;

            // Assign the element.
            dst = testData;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != pHead);
            SUTL_TEST_ASSERT(dst.m_pTail != pTail);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure data was copied (not moved).
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR AssignOperatorElementMove()
        {
            constexpr size_t len = GetTQLength<DstLen>();
            T testData(GetTestData<T, TestQuantity::VeryLow>().front());
            ForwardList<T> dst;

            // Setup dst list.
            if constexpr (len != 0)
            {
                std::vector<T> srcTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(srcTestData[i]));
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == len);
            }

            // Save off current list head/tail.
            auto pHead = dst.m_pHead;
            auto pTail = dst.m_pTail;

            // Assign the element.
            dst = std::move(testData);
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != pHead);
            SUTL_TEST_ASSERT(dst.m_pTail != pTail);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure data was moved (not copied).
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignOperatorListCopy()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();

            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (dstLen != 0)
            {
                const std::vector<T> dstTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(dstTestData[i]));
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == dstLen);
            }

            if constexpr (srcLen != 0)
            {
                const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Append(srcTestData[i]));
                }

                // Verify initial src conditions.
                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == srcLen);
            }

            // Save off current dst head/tail.
            auto pDstHead = dst.m_pHead;
            auto pDstTail = dst.m_pTail;

            // Save off current src head/tail.
            auto pSrcHead = src.m_pHead;
            auto pSrcTail = src.m_pTail;

            // Verify list copy assign.
            dst = src;
            if constexpr (dstLen == 0 && srcLen == 0)
            {
                // Old dst/src and new dst are all empty.
                SUTL_TEST_ASSERT(pDstHead == dst.m_pHead);
                SUTL_TEST_ASSERT(pDstTail == dst.m_pTail);
                SUTL_TEST_ASSERT(pSrcHead == dst.m_pHead);
                SUTL_TEST_ASSERT(pSrcTail == dst.m_pTail);
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_Len == 0);
            }
            else
            {
                // Dst either went from empty to not or vice-versa.
                SUTL_TEST_ASSERT(pDstHead != dst.m_pHead);
                SUTL_TEST_ASSERT(pDstTail != dst.m_pTail);
                if constexpr (srcLen == 0)
                {
                    // Dst is empty due to empty src.
                    SUTL_TEST_ASSERT(pSrcHead == dst.m_pHead);
                    SUTL_TEST_ASSERT(pSrcTail == dst.m_pTail);
                    SUTL_TEST_ASSERT(!dst.m_pHead);
                    SUTL_TEST_ASSERT(!dst.m_pTail);
                    SUTL_TEST_ASSERT(dst.m_Len == 0);
                }
                else
                {
                    // Dst is not empty due to non-empty src.
                    SUTL_TEST_ASSERT(pSrcHead != dst.m_pHead);
                    SUTL_TEST_ASSERT(pSrcTail != dst.m_pTail);
                    SUTL_TEST_ASSERT(!!dst.m_pHead);
                    SUTL_TEST_ASSERT(!!dst.m_pTail);
                    SUTL_TEST_ASSERT(dst.m_Len == srcLen);
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignOperatorListMove()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();

            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (dstLen != 0)
            {
                const std::vector<T> dstTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(dstTestData[i]));
                }

                // Verify initial dst conditions.
                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == dstLen);
            }

            if constexpr (srcLen != 0)
            {
                const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Append(srcTestData[i]));
                }

                // Verify initial src conditions.
                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == srcLen);
            }

            // Save off current dst head/tail.
            auto pDstHead = dst.m_pHead;
            auto pDstTail = dst.m_pTail;

            // Save off current src head/tail.
            auto pSrcHead = src.m_pHead;
            auto pSrcTail = src.m_pTail;

            // Verify list move assign.
            dst = std::move(src);
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);

            if constexpr (dstLen == 0 && srcLen == 0)
            {
                // Old dst/src and new dst are all empty.
                SUTL_TEST_ASSERT(pDstHead == dst.m_pHead);
                SUTL_TEST_ASSERT(pDstTail == dst.m_pTail);
                SUTL_TEST_ASSERT(pSrcHead == dst.m_pHead);
                SUTL_TEST_ASSERT(pSrcTail == dst.m_pTail);
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_Len == 0);
            }
            else
            {
                // Dst either went from empty to not or vice-versa.
                SUTL_TEST_ASSERT(pDstHead != dst.m_pHead);
                SUTL_TEST_ASSERT(pDstTail != dst.m_pTail);
                SUTL_TEST_ASSERT(pSrcHead == dst.m_pHead);
                SUTL_TEST_ASSERT(pSrcTail == dst.m_pTail);
                if constexpr (srcLen == 0)
                {
                    // Dst is empty due to empty src.
                    SUTL_TEST_ASSERT(!dst.m_pHead);
                    SUTL_TEST_ASSERT(!dst.m_pTail);
                    SUTL_TEST_ASSERT(dst.m_Len == 0);
                }
                else
                {
                    // Dst is not empty due to non-empty src.
                    SUTL_TEST_ASSERT(!!dst.m_pHead);
                    SUTL_TEST_ASSERT(!!dst.m_pTail);
                    SUTL_TEST_ASSERT(dst.m_Len == srcLen);
                }
            }

            SUTL_TEST_SUCCESS();
        }
    };


    // Tests insertion methods (e.g., Append, Prepend, InsertAt, etc.).
    class ForwardListTests::InsertionTests
    {
    public:

        // Append Method
        template <typename T>
        [[nodiscard]] static UTR AppendMethodElementCopy()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            const std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first append.
            SUTL_TEST_ASSERT(dst.Append(testData[0]));
            auto pHead = dst.m_pHead;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bMoved);
            }

            // Verify second append.
            SUTL_TEST_ASSERT(dst.Append(testData[1]));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->data == testData[1]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->data.m_bMoved);
            }

            // Verify third append.
            SUTL_TEST_ASSERT(dst.Append(testData[2]));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data == testData[2]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T>
        [[nodiscard]] static UTR AppendMethodElementMove()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first append.
            SUTL_TEST_ASSERT(dst.Append(std::move(testData[0])));
            auto pHead = dst.m_pHead;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element move (not copy).
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bMoved);
            }

            // Verify second append.
            SUTL_TEST_ASSERT(dst.Append(std::move(testData[1])));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->data == testData[1]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element move (not copy).
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->data.m_bMoved);
            }

            // Verify third append.
            SUTL_TEST_ASSERT(dst.Append(std::move(testData[2])));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data == testData[2]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element move (not copy).
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR AppendMethodListCopy()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (SrcLen != TestQuantity::None)
            {
                const std::vector<T> testData(GetTestData<T, SrcLen>());
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            SUTL_TEST_ASSERT(dst.Append(src));
            SUTL_TEST_ASSERT(dst.m_Len == len);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                auto pSrc = src.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }

            SUTL_TEST_ASSERT(dst.Append(src));
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                auto pSrc = src.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }

                pSrc = src.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR AppendMethodListMove()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (SrcLen != TestQuantity::None)
            {
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            auto pSrcTail = src.m_pTail;
            SUTL_TEST_ASSERT(dst.Append(std::move(src)));
            SUTL_TEST_ASSERT(dst.m_pTail == pSrcTail);
            SUTL_TEST_ASSERT(dst.m_Len == len);
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
            }

            if constexpr (SrcLen != TestQuantity::None)
            {
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            pSrcTail = src.m_pTail;
            SUTL_TEST_ASSERT(dst.Append(std::move(src)));
            SUTL_TEST_ASSERT(dst.m_pTail == pSrcTail);
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }

                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
            }

            SUTL_TEST_SUCCESS();
        }


        // Append Operator (+=)
        template <typename T>
        [[nodiscard]] static UTR AppendOperatorElementCopy()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            const std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first append.
            dst += testData[0];
            auto pHead = dst.m_pHead;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bMoved);
            }

            // Verify second append.
            dst += testData[1];
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->data == testData[1]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->data.m_bMoved);
            }

            // Verify third append.
            dst += testData[2];
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data == testData[2]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T>
        [[nodiscard]] static UTR AppendOperatorElementMove()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            const std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first append.
            dst += std::move(testData[0]);
            auto pHead = dst.m_pHead;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bMoved);
            }

            // Verify second append.
            dst += std::move(testData[1]);
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->data == testData[1]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->data.m_bMoved);
            }

            // Verify third append.
            dst += std::move(testData[2]);
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data == testData[2]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR AppendOperatorListCopy()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (SrcLen != TestQuantity::None)
            {
                const std::vector<T> testData(GetTestData<T, SrcLen>());
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            dst += src;
            SUTL_TEST_ASSERT(len == 0 || !!dst);
            SUTL_TEST_ASSERT(dst.m_Len == len);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                auto pSrc = src.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }

            dst += src;
            SUTL_TEST_ASSERT(len == 0 || !!dst);
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                auto pSrc = src.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }

                pSrc = src.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR AppendOperatorListMove()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (SrcLen != TestQuantity::None)
            {
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            auto pSrcTail = src.m_pTail;
            dst += std::move(src);
            SUTL_TEST_ASSERT(len == 0 || !!dst);
            SUTL_TEST_ASSERT(pSrcTail == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == len);
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
            }

            if constexpr (SrcLen != TestQuantity::None)
            {
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            pSrcTail = src.m_pTail;
            dst += std::move(src);
            SUTL_TEST_ASSERT(len == 0 || !!dst);
            SUTL_TEST_ASSERT(pSrcTail == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }

                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
            }

            SUTL_TEST_SUCCESS();
        }


        // Prepend
        template <typename T>
        [[nodiscard]] static UTR PrependElementCopy()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            const std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first prepend.
            SUTL_TEST_ASSERT(dst.Prepend(testData[0]));
            auto pTail = dst.m_pTail;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pTail->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pTail->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pTail->data.m_bMoved);
            }

            // Verify second prepend.
            SUTL_TEST_ASSERT(dst.Prepend(testData[1]));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pTail == pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
            }

            // Verify third prepend.
            SUTL_TEST_ASSERT(dst.Prepend(testData[2]));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            SUTL_TEST_ASSERT(dst.m_pTail == pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T>
        [[nodiscard]] static UTR PrependElementMove()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first prepend.
            SUTL_TEST_ASSERT(dst.Prepend(std::move(testData[0])));
            auto pTail = dst.m_pTail;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pTail->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(!dst.m_pTail->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pTail->data.m_bMoved);
            }

            // Verify second prepend.
            SUTL_TEST_ASSERT(dst.Prepend(std::move(testData[1])));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pTail == pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
            }

            // Verify third prepend.
            SUTL_TEST_ASSERT(dst.Prepend(std::move(testData[2])));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            SUTL_TEST_ASSERT(dst.m_pTail == pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR PrependListCopy()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (SrcLen != TestQuantity::None)
            {
                const std::vector<T> testData(GetTestData<T, SrcLen>());
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            SUTL_TEST_ASSERT(dst.Prepend(src));
            SUTL_TEST_ASSERT(dst.m_Len == len);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pTail;
                auto pSrc = src.m_pTail;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                }
            }

            SUTL_TEST_ASSERT(dst.Prepend(src));
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pTail;
                auto pSrc = src.m_pTail;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                }

                pSrc = src.m_pTail;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR PrependListMove()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (SrcLen != TestQuantity::None)
            {
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            auto pSrc = src.m_pHead;
            SUTL_TEST_ASSERT(dst.Prepend(std::move(src)));
            SUTL_TEST_ASSERT(dst.m_pHead == pSrc);
            SUTL_TEST_ASSERT(dst.m_Len == len);
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
            }

            if constexpr (SrcLen != TestQuantity::None)
            {
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            pSrc = src.m_pHead;
            SUTL_TEST_ASSERT(dst.Prepend(std::move(src)));
            SUTL_TEST_ASSERT(dst.m_pHead == pSrc);
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }

                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
            }

            SUTL_TEST_SUCCESS();
        }


        // InsertAt - Front
        template <typename T>
        [[nodiscard]] static UTR InsertAtFrontElementCopy()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            const std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first insert.
            SUTL_TEST_ASSERT(dst.InsertAt(testData[0], 0));
            auto pTail = dst.m_pTail;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pTail->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pTail->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pTail->data.m_bMoved);
            }

            // Verify second insert.
            SUTL_TEST_ASSERT(dst.InsertAt(testData[1], 0));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pTail == pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
            }

            // Verify third insert.
            SUTL_TEST_ASSERT(dst.InsertAt(testData[2], 0));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            SUTL_TEST_ASSERT(dst.m_pTail == pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T>
        [[nodiscard]] static UTR InsertAtFrontElementMove()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            const std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first insert.
            SUTL_TEST_ASSERT(dst.InsertAt(std::move(testData[0]), 0));
            auto pTail = dst.m_pTail;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pTail->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pTail->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pTail->data.m_bMoved);
            }

            // Verify second insert.
            SUTL_TEST_ASSERT(dst.InsertAt(std::move(testData[1]), 0));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pTail == pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
            }

            // Verify third insert.
            SUTL_TEST_ASSERT(dst.InsertAt(std::move(testData[2]), 0));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            SUTL_TEST_ASSERT(dst.m_pTail == pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!dst.m_pTail->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR InsertAtFrontListCopy()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (SrcLen != TestQuantity::None)
            {
                const std::vector<T> testData(GetTestData<T, SrcLen>());
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            SUTL_TEST_ASSERT(dst.InsertAt(src, 0));
            SUTL_TEST_ASSERT(dst.m_Len == len);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pTail;
                auto pSrc = src.m_pTail;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                }
            }

            SUTL_TEST_ASSERT(dst.InsertAt(src, 0));
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pTail;
                auto pSrc = src.m_pTail;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                }

                pSrc = src.m_pTail;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR InsertAtFrontListMove()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (SrcLen != TestQuantity::None)
            {
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            auto pSrc = src.m_pHead;
            SUTL_TEST_ASSERT(dst.InsertAt(std::move(src), 0));
            SUTL_TEST_ASSERT(dst.m_pHead == pSrc);
            SUTL_TEST_ASSERT(dst.m_Len == len);
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
            }

            if constexpr (SrcLen != TestQuantity::None)
            {
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            pSrc = src.m_pHead;
            SUTL_TEST_ASSERT(dst.InsertAt(std::move(src), 0));
            SUTL_TEST_ASSERT(dst.m_pHead == pSrc);
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }

                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
            }

            SUTL_TEST_SUCCESS();
        }

        // InsertAt - Mid
        template <typename T>
        [[nodiscard]] static UTR InsertAtMidElementCopy()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            const std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(testData[0], 0));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            auto pHead = dst.m_pHead;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bMoved);
            }

            // Verify second insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(testData[1], 1));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            auto pTail = dst.m_pTail;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->data == testData[1]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->data.m_bMoved);
            }

            // Verify third insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(testData[2], 1));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }
            auto pMid = (!!dst.m_pHead) ? dst.m_pHead->pNext : nullptr;
            // Verify list is still valid - expected length.
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!pMid);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            // Ensure head and tail didn't change.
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pTail == pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            // Ensure head, mid, and tail data is as expected.
            SUTL_TEST_ASSERT(pHead->data == testData[0]);
            SUTL_TEST_ASSERT(pMid->data == testData[2]);
            SUTL_TEST_ASSERT(pTail->data == testData[1]);
            // Ensure head and tail are linked properly with the middle node.
            SUTL_TEST_ASSERT(pHead->pNext == pMid);
            // Ensure tail is linked properly with the middle node.
            SUTL_TEST_ASSERT(pMid->pNext == pTail);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(pMid->data.m_bCopied);
                SUTL_TEST_ASSERT(!pMid->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T>
        [[nodiscard]] static UTR InsertAtMidElementMove()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(std::move(testData[0]), 0));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            auto pHead = dst.m_pHead;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element move (not copy).
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bMoved);
            }

            // Verify second insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(std::move(testData[1]), 1));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            auto pTail = dst.m_pTail;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->data == testData[1]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element move (not copy).
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->data.m_bMoved);
            }

            // Verify third insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(std::move(testData[2]), 1));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }
            auto pMid = (!!dst.m_pHead) ? dst.m_pHead->pNext : nullptr;
            // Verify list is still valid - expected length.
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!pMid);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            // Ensure head and tail didn't change.
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pTail == pTail);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            // Ensure head, mid, and tail data is as expected.
            SUTL_TEST_ASSERT(pHead->data == testData[0]);
            SUTL_TEST_ASSERT(pMid->data == testData[2]);
            SUTL_TEST_ASSERT(pTail->data == testData[1]);
            // Ensure head and tail are linked properly with the middle node.
            SUTL_TEST_ASSERT(pHead->pNext == pMid);
            // Ensure tail is linked properly with the middle node.
            SUTL_TEST_ASSERT(pMid->pNext == pTail);
            if constexpr (bIsHelper)
            {
                // Verify element move (not copy).
                SUTL_TEST_ASSERT(!pMid->data.m_bCopied);
                SUTL_TEST_ASSERT(pMid->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR InsertAtMidListCopy()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(src.Append(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            SUTL_TEST_ASSERT(dst.InsertAt(src, len / 2));
            SUTL_TEST_ASSERT(dst.m_Len == len);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else if constexpr (len == 1)
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_pHead != src.m_pHead);
                SUTL_TEST_ASSERT(dst.m_pTail != src.m_pTail);
                SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_pHead->data == src.m_pHead->data);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);
                auto pDst = dst.m_pHead;
                auto pSrc = src.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
                SUTL_TEST_ASSERT(!pDst);
                SUTL_TEST_ASSERT(!pSrc);
            }

            SUTL_TEST_ASSERT(dst.InsertAt(src, len / 2));
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else if constexpr (len == 1)
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_pHead != src.m_pHead);
                SUTL_TEST_ASSERT(dst.m_pTail != src.m_pTail);
                SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_pHead->data == src.m_pHead->data);
                SUTL_TEST_ASSERT(dst.m_pTail->data == src.m_pTail->data);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);
                auto pDst = dst.m_pHead;
                auto pSrc = src.m_pHead;
                for (size_t i = 0; i < len / 2; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
                SUTL_TEST_ASSERT(!!pDst);
                SUTL_TEST_ASSERT(!!pSrc);

                auto pSrcTmp = src.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrcTmp);
                    SUTL_TEST_ASSERT(pDst != pSrcTmp);
                    SUTL_TEST_ASSERT(pDst->data == pSrcTmp->data);

                    pDst = pDst->pNext;
                    pSrcTmp = pSrcTmp->pNext;
                }
                SUTL_TEST_ASSERT(!!pDst);
                SUTL_TEST_ASSERT(!pSrcTmp);

                for (size_t i = 0; i < len / 2; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
                SUTL_TEST_ASSERT(!pDst);
                SUTL_TEST_ASSERT(!pSrc);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR InsertAtMidListMove()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(src.Append(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            SUTL_TEST_ASSERT(dst.InsertAt(std::move(src), len / 2));
            SUTL_TEST_ASSERT(dst.m_Len == len);
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else if constexpr (len == 1)
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_pHead != src.m_pHead);
                SUTL_TEST_ASSERT(dst.m_pTail != src.m_pTail);
                SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_pHead->data == testData[0]);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);
                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);

                    pDst = pDst->pNext;
                }
                SUTL_TEST_ASSERT(!pDst);
            }

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(src.Append(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            SUTL_TEST_ASSERT(dst.InsertAt(std::move(src), len / 2));
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else if constexpr (len == 1)
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_pHead->data == testData[0]);
                SUTL_TEST_ASSERT(dst.m_pTail->data == testData[0]);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);
                auto pDst = dst.m_pHead;
                size_t i = 0;
                for (; i < len / 2; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
                SUTL_TEST_ASSERT(!!pDst);

                for (size_t j = 0; j < len; j++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[j]);
                    pDst = pDst->pNext;
                }
                SUTL_TEST_ASSERT(!!pDst);

                for (; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
                SUTL_TEST_ASSERT(!pDst);
            }

            SUTL_TEST_SUCCESS();
        }


        // InsertAt - Back
        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR InsertAtBackElementCopy()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            const std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(testData[0], 1));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }
            auto pHead = dst.m_pHead;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bMoved);
            }

            // Verify second insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(testData[1], 2));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->data == testData[1]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->data.m_bMoved);
            }

            // Verify third insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(testData[2], 3));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data == testData[2]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element copy (not move).
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR InsertAtBackElementMove()
        {
            constexpr bool bIsHelper = std::is_same_v<T, Helper>;
            std::vector<T> testData(GetTestData<T, TestQuantity::Low>());
            ForwardList<T> dst;

            // Verify initial list conditions.
            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            // Verify first insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(std::move(testData[0]), 1));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_ASSERT(e.what());
            }
            auto pHead = dst.m_pHead;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData[0]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element move (not copy).
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bMoved);
            }

            // Verify second insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(std::move(testData[1]), 2));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_ASSERT(e.what());
            }
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 2);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->data == testData[1]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element move (not copy).
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->data.m_bMoved);
            }

            // Verify third insert.
            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(std::move(testData[2]), 3));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_ASSERT(e.what());
            }
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 3);
            SUTL_TEST_ASSERT(dst.m_pHead == pHead);
            SUTL_TEST_ASSERT(dst.m_pHead != dst.m_pTail);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext);
            SUTL_TEST_ASSERT(!!dst.m_pHead->pNext->pNext);
            SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data == testData[2]);
            SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->pNext);
            if constexpr (bIsHelper)
            {
                // Verify element move (not copy).
                SUTL_TEST_ASSERT(!dst.m_pHead->pNext->pNext->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->pNext->pNext->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR InsertAtBackListCopy()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (SrcLen != TestQuantity::None)
            {
                const std::vector<T> testData(GetTestData<T, SrcLen>());
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(src, 1));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }
            SUTL_TEST_ASSERT(dst.m_Len == len);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                auto pSrc = src.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }

            try
            {
                SUTL_TEST_ASSERT(dst.InsertAt(src, len + 1));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                auto pSrc = src.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }

                pSrc = src.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR InsertAtBackListMove()
        {
            constexpr size_t len = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (SrcLen != TestQuantity::None)
            {
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            auto pSrcTail = src.m_pTail;
            SUTL_TEST_ASSERT(dst.InsertAt(std::move(src), 1));
            SUTL_TEST_ASSERT(dst.m_pTail == pSrcTail);
            SUTL_TEST_ASSERT(dst.m_Len == len);
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
            }

            if constexpr (SrcLen != TestQuantity::None)
            {
                for (const auto& datum : testData)
                {
                    SUTL_SETUP_ASSERT(src.Append(datum));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == len);
            }

            pSrcTail = src.m_pTail;
            SUTL_TEST_ASSERT(dst.InsertAt(std::move(src), len + 1));
            SUTL_TEST_ASSERT(dst.m_pTail == pSrcTail);
            SUTL_TEST_ASSERT(dst.m_Len == (2 * len));
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
            }
            else
            {
                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }

                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    pDst = pDst->pNext;
                }
            }

            SUTL_TEST_SUCCESS();
        }
    };


    // Tests getter methods (e.g., Front, Back, At, etc.).
    class ForwardListTests::GetterTests
    {
    public:

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Length()
        {
            constexpr size_t len = GetTQLength<Len>();
            ForwardList<T> dst;

            if constexpr (len != 0)
            {
                const std::vector<T> testData(GetTestData<T, Len>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(dst.Append(testData[i]));
                    SUTL_TEST_ASSERT(dst.Length() == i + 1);
                }
            }

            SUTL_TEST_ASSERT(dst.Length() == len);

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Front()
        {
            constexpr size_t len = GetTQLength<Len>();
            ForwardList<T> dst;

            if constexpr (len != 0)
            {
                const std::vector<T> testData(GetTestData<T, Len>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(dst.Append(testData[i]));
                }

                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(dst.Front() == dst.m_pHead->data);
                SUTL_TEST_ASSERT(dst.Front() == testData.front());
            }
            else
            {
                bool bThrew = false;
                try
                {
                    T& tmp = dst.Front();
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

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Back()
        {
            constexpr size_t len = GetTQLength<Len>();
            ForwardList<T> dst;

            if constexpr (len != 0)
            {
                const std::vector<T> testData(GetTestData<T, Len>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(dst.Append(testData[i]));
                }

                SUTL_TEST_ASSERT(!!dst.m_pTail);
                SUTL_TEST_ASSERT(dst.Back() == dst.m_pTail->data);
                SUTL_TEST_ASSERT(dst.Back() == testData.back());
            }
            else
            {
                bool bThrew = false;
                try
                {
                    T& tmp = dst.Back();
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

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR At()
        {
            constexpr size_t len = GetTQLength<Len>();
            bool bThrew = false;
            ForwardList<T> dst;

            if constexpr (len != 0)
            {
                const std::vector<T> testData(GetTestData<T, Len>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(dst.Append(testData[i]));
                }

                SUTL_TEST_ASSERT(!!dst.m_pHead);
                SUTL_TEST_ASSERT(!!dst.m_pTail);

                try
                {
                    SUTL_TEST_ASSERT(dst.At(0) == testData[0]);
                }
                catch (const std::exception& e)
                {
                    SUTL_TEST_EXCEPTION(e.what());
                }

                try
                {
                    SUTL_TEST_ASSERT(dst.At(len / 2) == testData[len / 2]);
                }
                catch (const std::exception& e)
                {
                    SUTL_TEST_EXCEPTION(e.what());
                }

                try
                {
                    SUTL_TEST_ASSERT(dst.At(len - 1) == testData[len - 1]);
                }
                catch (const std::exception& e)
                {
                    SUTL_TEST_EXCEPTION(e.what());
                }

                try
                {
                    SUTL_TEST_ASSERT(dst.At(len) == testData[0]);
                }
                catch (const std::out_of_range&)
                {
                    bThrew = true;
                }
                catch (const std::exception& e)
                {
                    SUTL_TEST_EXCEPTION(e.what());
                }

                SUTL_TEST_ASSERT(bThrew);

                bThrew = false;
                try
                {
                    SUTL_TEST_ASSERT(dst.At(len + 1) == testData[0]);
                }
                catch (const std::out_of_range&)
                {
                    bThrew = true;
                }
                catch (const std::exception& e)
                {
                    SUTL_TEST_EXCEPTION(e.what());
                }

                SUTL_TEST_ASSERT(bThrew);

                bThrew = false;
                try
                {
                    SUTL_TEST_ASSERT(dst.At(std::numeric_limits<size_t>::max()) == testData[0]);
                }
                catch (const std::out_of_range&)
                {
                    bThrew = true;
                }
                catch (const std::exception& e)
                {
                    SUTL_TEST_EXCEPTION(e.what());
                }

                SUTL_TEST_ASSERT(bThrew);
            }
            else
            {
                try
                {
                    T& tmp = dst.At(0);
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

                bThrew = false;
                try
                {
                    T& tmp = dst.At(1);
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

                bThrew = false;
                try
                {
                    T& tmp = dst.At(std::numeric_limits<size_t>::max());
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


    // Tests comparison methods.
    class ForwardListTests::ComparisonTests
    {
    public:

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR CompareMethod()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            constexpr bool bExpectCompareSuccess = (dstLen == srcLen);

            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (dstLen != 0)
            {
                const std::vector<T> dstTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(dstTestData[i]));
                }

                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == dstLen);
            }

            if constexpr (srcLen != 0)
            {
                const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Append(srcTestData[i]));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == srcLen);
            }

            SUTL_TEST_ASSERT(dst.Compare(src) == bExpectCompareSuccess);

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR CompareOperator()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            constexpr bool bExpectCompareSuccess = (dstLen == srcLen);

            ForwardList<T> dst;
            ForwardList<T> src;

            if constexpr (dstLen != 0)
            {
                const std::vector<T> dstTestData(GetTestData<T, DstLen>());
                for (size_t i = 0; i < dstLen; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(dstTestData[i]));
                }

                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == dstLen);
            }

            if constexpr (srcLen != 0)
            {
                const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_SETUP_ASSERT(src.Append(srcTestData[i]));
                }

                SUTL_SETUP_ASSERT(!!src.m_pHead);
                SUTL_SETUP_ASSERT(!!src.m_pTail);
                SUTL_SETUP_ASSERT(src.m_Len == srcLen);
            }

            SUTL_TEST_ASSERT((dst == src) == bExpectCompareSuccess);

            SUTL_TEST_SUCCESS();
        }
    };


    // Tests removal methods (e.g., PopFront, PopBack, RemoveAt, etc.).
    class ForwardListTests::RemovalTests
    {
    public:

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR PopFront()
        {
            constexpr size_t len = GetTQLength<Len>();

            ForwardList<T> dst;

            if constexpr (len != 0)
            {
                const std::vector<T> testData(GetTestData<T, Len>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == len);

                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(dst.m_pHead->data == testData[i]);
                    try
                    {
                        SUTL_TEST_ASSERT(dst.PopFront());
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }

                    SUTL_TEST_ASSERT(dst.m_Len == (len - i - 1));
                }
            }

            try
            {
                SUTL_TEST_ASSERT(!dst.PopFront());
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR PopFrontMove()
        {
            constexpr size_t len = GetTQLength<Len>();

            ForwardList<T> dst;

            if constexpr (len != 0)
            {
                const std::vector<T> testData(GetTestData<T, Len>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == len);

                T obj;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(dst.m_pHead->data == testData[i]);
                    try
                    {
                        SUTL_TEST_ASSERT(dst.PopFront(obj));
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }

                    SUTL_TEST_ASSERT(obj == testData[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(obj.m_bMoved);
                    }
                    SUTL_TEST_ASSERT(dst.m_Len == (len - i - 1));
                }
            }

            try
            {
                T obj = T();
                SUTL_TEST_ASSERT(!dst.PopFront(obj));
                SUTL_TEST_ASSERT(obj == T());
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR PopBack()
        {
            constexpr size_t len = GetTQLength<Len>();

            ForwardList<T> dst;

            if constexpr (len != 0)
            {
                const std::vector<T> testData(GetTestData<T, Len>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == len);

                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(dst.m_pTail->data == testData[len - i - 1]);
                    try
                    {
                        SUTL_TEST_ASSERT(dst.PopBack());
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }

                    SUTL_TEST_ASSERT(dst.m_Len == (len - i - 1));
                }
            }

            try
            {
                SUTL_TEST_ASSERT(!dst.PopBack());
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR PopBackMove()
        {
            constexpr size_t len = GetTQLength<Len>();

            ForwardList<T> dst;

            if constexpr (len != 0)
            {
                const std::vector<T> testData(GetTestData<T, Len>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == len);

                T obj;
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(dst.m_pTail->data == testData[len - i - 1]);
                    try
                    {
                        SUTL_TEST_ASSERT(dst.PopBack(obj));
                    }
                    catch (const std::exception& e)
                    {
                        SUTL_TEST_EXCEPTION(e.what());
                    }

                    SUTL_TEST_ASSERT(obj == testData[len - i - 1]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(obj.m_bMoved);
                    }
                    SUTL_TEST_ASSERT(dst.m_Len == (len - i - 1));
                }
            }

            try
            {
                T obj = T();
                SUTL_TEST_ASSERT(!dst.PopBack(obj));
                SUTL_TEST_ASSERT(obj == T());
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR RemoveAt()
        {
            constexpr size_t len = GetTQLength<Len>();

            ForwardList<T> dst;

            if constexpr (len != 0)
            {
                std::vector<T> testData(GetTestData<T, Len>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == len);

                size_t curLen = len;
                while (!testData.empty())
                {
                    const size_t pos = curLen / 2;
                    auto itr = testData.cbegin();
                    std::advance(itr, pos);

                    testData.erase(itr);
                    SUTL_TEST_ASSERT(dst.RemoveAt(pos));
                    curLen--;

                    auto pDst = dst.m_pHead;
                    for (size_t i = 0; i < curLen; i++)
                    {
                        SUTL_TEST_ASSERT(testData[i] == pDst->data);
                        pDst = pDst->pNext;
                    }

                    SUTL_TEST_ASSERT(!pDst);
                }
            }

            try
            {
                SUTL_TEST_ASSERT(!dst.RemoveAt(0));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR RemoveAtMove()
        {
            constexpr size_t len = GetTQLength<Len>();

            ForwardList<T> dst;

            if constexpr (len != 0)
            {
                std::vector<T> testData(GetTestData<T, Len>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == len);

                T obj;
                size_t curLen = len;
                while (!testData.empty())
                {
                    const size_t pos = curLen / 2;
                    auto itr = testData.cbegin();
                    std::advance(itr, pos);
                    T copy = *itr;
                    testData.erase(itr);

                    SUTL_TEST_ASSERT(dst.RemoveAt(pos, obj));
                    SUTL_TEST_ASSERT(obj == copy);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(obj.m_bMoved);
                    }

                    curLen--;

                    auto pDst = dst.m_pHead;
                    for (size_t i = 0; i < curLen; i++)
                    {
                        SUTL_TEST_ASSERT(testData[i] == pDst->data);
                        pDst = pDst->pNext;
                    }

                    SUTL_TEST_ASSERT(!pDst);
                }
            }

            try
            {
                T obj = T();
                SUTL_TEST_ASSERT(!dst.RemoveAt(0, obj));
                SUTL_TEST_ASSERT(obj == T());
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Clear()
        {
            constexpr size_t len = GetTQLength<Len>();

            ForwardList<T> dst;

            if constexpr (len != 0)
            {
                const std::vector<T> testData(GetTestData<T, Len>());
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Append(testData[i]));
                }

                SUTL_SETUP_ASSERT(!!dst.m_pHead);
                SUTL_SETUP_ASSERT(!!dst.m_pTail);
                SUTL_SETUP_ASSERT(dst.m_Len == len);
            }

            dst.Clear();

            SUTL_SETUP_ASSERT(!dst.m_pHead);
            SUTL_SETUP_ASSERT(!dst.m_pTail);
            SUTL_SETUP_ASSERT(dst.m_Len == 0);

            SUTL_TEST_SUCCESS();
        }
    };
}
