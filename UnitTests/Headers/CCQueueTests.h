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
#include <CCQueue.h>

// Test Helper Utils
#include <TestHelpers.hpp>


namespace CC
{
    class QueueTests
    {
        // Type aliases.
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR()>;
        using UTList = std::list<UTFunc>;

        QueueTests() = delete;
        QueueTests(const QueueTests&) = delete;
        QueueTests(QueueTests&&) = delete;
        ~QueueTests() = delete;
        QueueTests& operator=(const QueueTests&) = delete;
        QueueTests& operator=(QueueTests&&) = delete;

    private:

        /// Test Subclasses \\\

        // Test constructor behavior.
        class ConstructorTests;

        // Tests assignment methods.
        class AssignTests;

        // Tests comparison methods.
        class ComparisonTests;

        // Tests getter methods (e.g., Length, Back, Front, etc.).
        class GetterTests;

        // Tests enqueue and dequeue methods.
        class EnqueueDequeueTests;

    public:

        // Returns list of List unit tests.
        [[nodiscard]] static UTList GetTests();
    };

#define SETUP_QUEUE_WITH_TEST_DATA_VECTOR(_q_, _t_, _l_)                        \
    if constexpr (_l_ != 0)                                                     \
    {                                                                           \
        for (size_t i = 0; i < _l_; i++)                                        \
        {                                                                       \
            SUTL_SETUP_ASSERT(_q_.Enqueue(_t_[i]));                             \
            SUTL_SETUP_ASSERT(!!_q_.m_pHead);                                   \
            SUTL_SETUP_ASSERT(!!_q_.m_pTail);                                   \
            SUTL_SETUP_ASSERT(_q_.m_Len == i + 1);                              \
            SUTL_SETUP_ASSERT((_q_.m_pHead == _q_.m_pTail) == (_q_.m_Len == 1));\
            SUTL_SETUP_ASSERT(_q_.m_pHead->data == _t_[0]);                     \
            SUTL_SETUP_ASSERT(_q_.m_pTail->data == _t_[i]);                     \
        }                                                                       \
                                                                                \
        SUTL_SETUP_ASSERT(!!_q_.m_pHead);                                       \
        SUTL_SETUP_ASSERT(!!_q_.m_pTail);                                       \
        SUTL_SETUP_ASSERT(_q_.m_Len == _l_);                                    \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        SUTL_SETUP_ASSERT(!_q_.m_pHead);                                        \
        SUTL_SETUP_ASSERT(!_q_.m_pTail);                                        \
        SUTL_SETUP_ASSERT(_q_.m_Len == 0);                                      \
    }

    // Test constructor behavior.
    class QueueTests::ConstructorTests
    {
    public:

        template <typename T>
        [[nodiscard]] static UTR DefaultCtor()
        {
            Queue<T> queue;

            SUTL_TEST_ASSERT(!queue.m_pHead);
            SUTL_TEST_ASSERT(!queue.m_pTail);
            SUTL_TEST_ASSERT(queue.m_Len == 0);

            SUTL_TEST_SUCCESS();
        }

        template <typename T>
        [[nodiscard]] static UTR ElementCopyCtor()
        {
            const std::vector<T> testData(GetTestData<T, TestQuantity::VeryLow>());
            Queue<T> queue(testData.back());

            SUTL_TEST_ASSERT(!!queue.m_pHead);
            SUTL_TEST_ASSERT(!!queue.m_pTail);
            SUTL_TEST_ASSERT(queue.m_pHead == queue.m_pTail);
            SUTL_TEST_ASSERT(queue.m_Len == 1);
            SUTL_TEST_ASSERT(!queue.m_pHead->pNext);
            SUTL_TEST_ASSERT(queue.m_pHead->data == testData.back());
            if constexpr (std::is_same_v<T, Helper>)
            {
                SUTL_TEST_ASSERT(queue.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(!queue.m_pHead->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T>
        [[nodiscard]] static UTR ElementMoveCtor()
        {
            std::vector<T> testData(GetTestData<T, TestQuantity::VeryLow>());
            Queue<T> queue(std::move(testData.back()));

            SUTL_TEST_ASSERT(!!queue.m_pHead);
            SUTL_TEST_ASSERT(!!queue.m_pTail);
            SUTL_TEST_ASSERT(queue.m_pHead == queue.m_pTail);
            SUTL_TEST_ASSERT(queue.m_Len == 1);
            SUTL_TEST_ASSERT(!queue.m_pHead->pNext);
            SUTL_TEST_ASSERT(queue.m_pHead->data == testData.back());
            if constexpr (std::is_same_v<T, Helper>)
            {
                SUTL_TEST_ASSERT(!queue.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(queue.m_pHead->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR QueueCopyCtor()
        {
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            Queue<T> src;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(src, testData, srcLen);

            Queue<T> dst(src);
            SUTL_TEST_ASSERT(dst.m_Len == src.m_Len);

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_pHead;
                auto pSrc = src.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);

                    if (i == srcLen - 1)
                    {
                        SUTL_TEST_ASSERT(pDst == dst.m_pTail);
                        SUTL_TEST_ASSERT(pSrc == src.m_pTail);
                    }

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
                SUTL_TEST_ASSERT(!src.m_pHead);
                SUTL_TEST_ASSERT(!src.m_pTail);
                SUTL_TEST_ASSERT(dst.m_Len == 0);
                SUTL_TEST_ASSERT(src.m_Len == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity SrcLen>
        [[nodiscard]] static UTR QueueMoveCtor()
        {
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> testData(GetTestData<T, SrcLen>());
            Queue<T> src;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(src, testData, srcLen);

            auto pSrcHead = src.m_pHead;
            auto pSrcTail = src.m_pTail;
            Queue<T> dst(std::move(src));
            SUTL_TEST_ASSERT(dst.m_pHead == pSrcHead);
            SUTL_TEST_ASSERT(dst.m_pTail == pSrcTail);
            SUTL_TEST_ASSERT(dst.m_Len == srcLen);
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == testData[i]);
                    if (i == srcLen - 1)
                    {
                        SUTL_TEST_ASSERT(pDst == dst.m_pTail);
                    }
                    pDst = pDst->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_Len == 0);
            }

            SUTL_TEST_SUCCESS();
        }
    };


    // Tests assignment methods.
    class QueueTests::AssignTests
    {
    public:

        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR AssignMethodElementCopy()
        {
            constexpr size_t len = GetTQLength<DstLen>();
            const std::vector<T> dstTestData(GetTestData<T, DstLen>());
            const T testData(GetTestData<T, TestQuantity::Low>().back());
            Queue<T> dst;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, dstTestData, len);

            // Save off current queue head.
            auto pHead = dst.m_pHead;

            // Assign the element.
            SUTL_TEST_ASSERT(dst.Assign(testData));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead != pHead);
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
            const std::vector<T> dstTestData(GetTestData<T, DstLen>());
            T testData(GetTestData<T, TestQuantity::Low>().back());
            Queue<T> dst;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, dstTestData, len);

            // Save off current queue head.
            auto pHead = dst.m_pHead;

            // Assign the element.
            SUTL_TEST_ASSERT(dst.Assign(std::move(testData)));
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead != pHead);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure data was copied (not moved).
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignMethodQueueCopy()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> dstTestData(GetTestData<T, DstLen>());
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Queue<T> dst;
            Queue<T> src;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, dstTestData, dstLen);
            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(src, srcTestData, srcLen);

            SUTL_TEST_ASSERT(dst.Assign(src));
            SUTL_TEST_ASSERT(dst.m_Len == src.m_Len);

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_pHead;
                auto pSrc = src.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);
                    SUTL_TEST_ASSERT(pDst->data == srcTestData[i]);

                    if (i == srcLen - 1)
                    {
                        SUTL_TEST_ASSERT(pDst == dst.m_pTail);
                        SUTL_TEST_ASSERT(pSrc == src.m_pTail);
                    }

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
                SUTL_TEST_ASSERT(!src.m_pHead);
                SUTL_TEST_ASSERT(!src.m_pTail);
                SUTL_TEST_ASSERT(dst.m_Len == 0);
                SUTL_TEST_ASSERT(src.m_Len == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignMethodQueueMove()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> dstTestData(GetTestData<T, DstLen>());
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Queue<T> dst;
            Queue<T> src;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, dstTestData, dstLen);
            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(src, srcTestData, srcLen);

            auto pSrc = src.m_pHead;
            SUTL_TEST_ASSERT(dst.Assign(std::move(src)));
            SUTL_TEST_ASSERT(dst.m_pHead == pSrc);
            SUTL_TEST_ASSERT(dst.m_Len == srcLen);
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(src.m_Len == 0);

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == srcTestData[i]);
                    if (i == srcLen - 1)
                    {
                        SUTL_TEST_ASSERT(pDst == dst.m_pTail);
                    }

                    pDst = pDst->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
                SUTL_TEST_ASSERT(!src.m_pHead);
                SUTL_TEST_ASSERT(!src.m_pTail);
                SUTL_TEST_ASSERT(dst.m_Len == 0);
                SUTL_TEST_ASSERT(src.m_Len == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen>
        [[nodiscard]] static UTR AssignOperatorElementCopy()
        {
            constexpr size_t len = GetTQLength<DstLen>();
            const T testData(GetTestData<T, TestQuantity::Low>().back());
            const std::vector<T> srcTestData(GetTestData<T, DstLen>());
            Queue<T> dst;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, srcTestData, len);

            // Save off current queue head.
            auto pHead = dst.m_pHead;

            // Assign the element.
            dst = testData;
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead != pHead);
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
            T testData(GetTestData<T, TestQuantity::Low>().back());
            const std::vector<T> srcTestData(GetTestData<T, DstLen>());
            Queue<T> dst;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, srcTestData, len);

            // Save off current queue head.
            auto pHead = dst.m_pHead;

            // Assign the element.
            dst = std::move(testData);
            SUTL_TEST_ASSERT(!!dst.m_pHead);
            SUTL_TEST_ASSERT(!!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_pHead == dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 1);
            SUTL_TEST_ASSERT(dst.m_pHead != pHead);
            SUTL_TEST_ASSERT(dst.m_pHead->data == testData);

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Ensure data was copied (not moved).
                SUTL_TEST_ASSERT(!dst.m_pHead->data.m_bCopied);
                SUTL_TEST_ASSERT(dst.m_pHead->data.m_bMoved);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignOperatorQueueCopy()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> dstTestData(GetTestData<T, DstLen>());
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Queue<T> dst;
            Queue<T> src;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, dstTestData, dstLen);
            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(src, srcTestData, srcLen);

            dst = src;
            SUTL_TEST_ASSERT(dst.m_Len == src.m_Len);

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_pHead;
                auto pSrc = src.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(!!pSrc);
                    SUTL_TEST_ASSERT(pDst != pSrc);
                    SUTL_TEST_ASSERT(pDst->data == pSrc->data);
                    SUTL_TEST_ASSERT(pDst->data == srcTestData[i]);

                    if (i == srcLen - 1)
                    {
                        SUTL_TEST_ASSERT(pDst == dst.m_pTail);
                        SUTL_TEST_ASSERT(pSrc == src.m_pTail);
                    }

                    pDst = pDst->pNext;
                    pSrc = pSrc->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!src.m_pHead);
                SUTL_TEST_ASSERT(dst.m_Len == 0);
                SUTL_TEST_ASSERT(src.m_Len == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR AssignOperatorQueueMove()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            const std::vector<T> dstTestData(GetTestData<T, DstLen>());
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Queue<T> dst;
            Queue<T> src;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, dstTestData, dstLen);
            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(src, srcTestData, srcLen);

            auto pSrcHead = src.m_pHead;
            auto pSrcTail = src.m_pTail;
            dst = std::move(src);
            SUTL_TEST_ASSERT(dst.m_pHead == pSrcHead);
            SUTL_TEST_ASSERT(dst.m_pTail == pSrcTail);
            SUTL_TEST_ASSERT(dst.m_Len == srcLen);
            SUTL_TEST_ASSERT(!src.m_pHead);
            SUTL_TEST_ASSERT(!src.m_pTail);
            SUTL_TEST_ASSERT(src.m_Len == 0);

            if constexpr (srcLen != 0)
            {
                auto pDst = dst.m_pHead;
                for (size_t i = 0; i < srcLen; i++)
                {
                    SUTL_TEST_ASSERT(!!pDst);
                    SUTL_TEST_ASSERT(pDst->data == srcTestData[i]);
                    if (i == srcLen - 1)
                    {
                        SUTL_TEST_ASSERT(pDst == dst.m_pTail);
                    }

                    pDst = pDst->pNext;
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
                SUTL_TEST_ASSERT(!src.m_pHead);
                SUTL_TEST_ASSERT(!src.m_pTail);
                SUTL_TEST_ASSERT(dst.m_Len == 0);
                SUTL_TEST_ASSERT(src.m_Len == 0);
            }

            SUTL_TEST_SUCCESS();
        }
    };


    // Tests comparison methods.
    class QueueTests::ComparisonTests
    {
    public:

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR ComparisonMethod()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            constexpr bool bExpectMatch = (dstLen == srcLen);
            const std::vector<T> dstTestData(GetTestData<T, DstLen>());
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Queue<T> dst;
            Queue<T> src;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, dstTestData, dstLen);
            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(src, srcTestData, srcLen);

            SUTL_TEST_ASSERT(dst.Compare(src) == bExpectMatch);

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity DstLen, TestQuantity SrcLen>
        [[nodiscard]] static UTR ComparisonOperator()
        {
            constexpr size_t dstLen = GetTQLength<DstLen>();
            constexpr size_t srcLen = GetTQLength<SrcLen>();
            constexpr bool bExpectMatch = (dstLen == srcLen);
            const std::vector<T> dstTestData(GetTestData<T, DstLen>());
            const std::vector<T> srcTestData(GetTestData<T, SrcLen>());
            Queue<T> dst;
            Queue<T> src;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, dstTestData, dstLen);
            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(src, srcTestData, srcLen);

            SUTL_TEST_ASSERT((dst == src) == bExpectMatch);

            SUTL_TEST_SUCCESS();
        }
    };


    // Tests getter methods (e.g., Length, Back, Front, etc.).
    class QueueTests::GetterTests
    {
    public:

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Length()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());
            Queue<T> dst;

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Enqueue(testData[i]));
                    SUTL_TEST_ASSERT(dst.Length() == i + 1);
                }
            }

            SUTL_TEST_ASSERT(dst.Length() == len);

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Back()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());
            Queue<T> dst;

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Enqueue(testData[i]));
                    try
                    {
                        SUTL_TEST_ASSERT(dst.Back() == testData[i]);
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
                    T tmp = dst.Back();
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
        [[nodiscard]] static UTR Front()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());
            Queue<T> dst;

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Enqueue(testData[i]));
                    try
                    {
                        SUTL_TEST_ASSERT(dst.Front() == testData[0]);
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
                    T tmp = dst.Front();
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


    // Tests enqueue and dequeue methods.
    class QueueTests::EnqueueDequeueTests
    {
    public:

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Enqueue()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());
            Queue<T> dst;

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_SETUP_ASSERT(dst.Enqueue(testData[i]));
                    SUTL_TEST_ASSERT(dst.m_Len == (i + 1));
                    SUTL_TEST_ASSERT(!!dst.m_pHead);
                    SUTL_TEST_ASSERT(!!dst.m_pTail);
                    SUTL_TEST_ASSERT((dst.m_pHead == dst.m_pTail) == (dst.m_Len == 1));
                    SUTL_TEST_ASSERT(dst.m_pHead->data == testData[0]);
                    SUTL_TEST_ASSERT(dst.m_pTail->data == testData[i]);
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!dst.m_pHead);
                SUTL_TEST_ASSERT(!dst.m_pTail);
                SUTL_TEST_ASSERT(dst.m_Len == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR Dequeue()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());
            Queue<T> dst;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, testData, len);

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!dst.m_pHead);
                    SUTL_TEST_ASSERT(dst.m_pHead->data == testData[i]);
                    SUTL_TEST_ASSERT(dst.m_pTail->data == testData[len - 1]);
                    SUTL_TEST_ASSERT(dst.m_Len == len - i);

                    auto pNext = dst.m_pHead->pNext;
                    SUTL_TEST_ASSERT(dst.Dequeue());
                    SUTL_TEST_ASSERT(dst.m_pHead == pNext);
                    SUTL_TEST_ASSERT(dst.m_Len == len - i - 1);

                    if (!!pNext)
                    {
                        SUTL_TEST_ASSERT(i < len - 1);
                        SUTL_TEST_ASSERT(dst.m_pHead->data == testData[i + 1]);
                        SUTL_TEST_ASSERT(dst.m_pTail->data == testData[len - 1]);
                    }
                    else
                    {
                        SUTL_TEST_ASSERT(i == len - 1);
                    }
                }
            }

            try
            {
                SUTL_TEST_ASSERT(!dst.Dequeue());
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(!dst.m_pHead);
            SUTL_TEST_ASSERT(!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 0);

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR DequeueAndCopy()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());
            Queue<T> dst;

            SETUP_QUEUE_WITH_TEST_DATA_VECTOR(dst, testData, len);

            if constexpr (len != 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!dst.m_pHead);
                    SUTL_TEST_ASSERT(dst.m_pHead->data == testData[i]);
                    SUTL_TEST_ASSERT(dst.m_pTail->data == testData[len - 1]);
                    SUTL_TEST_ASSERT(dst.m_Len == len - i);

                    T tmp;
                    auto pNext = dst.m_pHead->pNext;
                    SUTL_TEST_ASSERT(dst.Dequeue(tmp));
                    SUTL_TEST_ASSERT(tmp == testData[i]);
                    SUTL_TEST_ASSERT(dst.m_pHead == pNext);
                    SUTL_TEST_ASSERT(dst.m_Len == len - i - 1);

                    if (!!pNext)
                    {
                        SUTL_TEST_ASSERT(i < len - 1);
                        SUTL_TEST_ASSERT(dst.m_pHead->data == testData[i + 1]);
                        SUTL_TEST_ASSERT(dst.m_pTail->data == testData[len - 1]);
                    }
                    else
                    {
                        SUTL_TEST_ASSERT(i == len - 1);
                    }
                }
            }

            try
            {
                SUTL_TEST_ASSERT(!dst.Dequeue());
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(!dst.m_pHead);
            SUTL_TEST_ASSERT(!dst.m_pTail);
            SUTL_TEST_ASSERT(dst.m_Len == 0);

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR EnqueueDequeueStagger()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());

            for (size_t staggerThreshold = 1; staggerThreshold <= static_cast<size_t>(sqrt(len)) + 1; staggerThreshold++)
            {
                Queue<T> queue;
                size_t staggerCount = 0;

                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(queue.Enqueue(testData[i]));
                    SUTL_TEST_ASSERT(queue.m_pHead->data == testData[staggerCount]);
                    SUTL_TEST_ASSERT(queue.m_pTail->data == testData[i]);
                    if ((i % staggerThreshold) == 0)
                    {
                        staggerCount++;
                        SUTL_TEST_ASSERT(queue.Dequeue());
                        if (!!queue.m_pHead)
                        {
                            SUTL_TEST_ASSERT(queue.m_pHead->data == testData[staggerCount]);
                            SUTL_TEST_ASSERT(queue.m_pTail->data == testData[i]);
                            SUTL_TEST_ASSERT(queue.m_Len != 0);
                        }
                        else
                        {
                            SUTL_TEST_ASSERT(queue.m_Len == 0);
                            SUTL_TEST_ASSERT(((i + 1) - staggerCount) == 0);
                        }
                    }

                    SUTL_TEST_ASSERT(queue.m_Len == ((i + 1) - staggerCount));
                }

                for (size_t i = staggerCount; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!queue.m_pHead);
                    SUTL_TEST_ASSERT(queue.m_pHead->data == testData[i]);
                    SUTL_TEST_ASSERT(queue.Dequeue());
                }

                SUTL_TEST_ASSERT(!queue.m_pHead);
                SUTL_TEST_ASSERT(!queue.m_pTail);
                SUTL_TEST_ASSERT(queue.m_Len == 0);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity Len>
        [[nodiscard]] static UTR EnqueueDequeueAndCopyStagger()
        {
            constexpr size_t len = GetTQLength<Len>();
            const std::vector<T> testData(GetTestData<T, Len>());

            for (size_t staggerThreshold = 1; staggerThreshold <= static_cast<size_t>(sqrt(len)) + 1; staggerThreshold++)
            {
                Queue<T> queue;
                T tmp;
                size_t staggerCount = 0;

                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(queue.Enqueue(testData[i]));
                    SUTL_TEST_ASSERT(queue.m_pHead->data == testData[staggerCount]);
                    SUTL_TEST_ASSERT(queue.m_pTail->data == testData[i]);
                    if ((i % staggerThreshold) == 0)
                    {
                        staggerCount++;
                        SUTL_TEST_ASSERT(queue.Dequeue(tmp));
                        SUTL_TEST_ASSERT(tmp == testData[staggerCount - 1]);
                        if constexpr (std::is_same_v<T, Helper>)
                        {
                            SUTL_TEST_ASSERT(!tmp.m_bCopied);
                            SUTL_TEST_ASSERT(tmp.m_bMoved);
                        }

                        if (!!queue.m_pHead)
                        {
                            SUTL_TEST_ASSERT(queue.m_pHead->data == testData[staggerCount]);
                            SUTL_TEST_ASSERT(queue.m_pTail->data == testData[i]);
                            SUTL_TEST_ASSERT(queue.m_Len != 0);
                        }
                        else
                        {
                            SUTL_TEST_ASSERT(queue.m_Len == 0);
                            SUTL_TEST_ASSERT(((i + 1) - staggerCount) == 0);
                        }
                    }

                    SUTL_TEST_ASSERT(queue.m_Len == ((i + 1) - staggerCount));
                }

                for (size_t i = staggerCount; i < len; i++)
                {
                    SUTL_TEST_ASSERT(!!queue.m_pHead);
                    SUTL_TEST_ASSERT(queue.m_pHead->data == testData[i]);
                    SUTL_TEST_ASSERT(queue.Dequeue(tmp));
                    SUTL_TEST_ASSERT(tmp == testData[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(!tmp.m_bCopied);
                        SUTL_TEST_ASSERT(tmp.m_bMoved);
                    }
                }

                SUTL_TEST_ASSERT(!queue.m_pHead);
                SUTL_TEST_ASSERT(!queue.m_pTail);
                SUTL_TEST_ASSERT(queue.m_Len == 0);
            }

            SUTL_TEST_SUCCESS();
        }
    };
}
