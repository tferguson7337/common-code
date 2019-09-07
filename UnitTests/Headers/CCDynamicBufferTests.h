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
#include <CCDynamicBuffer.h>

// Test Helper Utils
#include <TestHelpers.hpp>


namespace CC
{
    class DynamicBufferTests
    {
        // Type aliases
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;

        using TQ = TestQuantity;

        DynamicBufferTests() = delete;
        DynamicBufferTests(const DynamicBufferTests&) = delete;
        DynamicBufferTests(DynamicBufferTests&&) = delete;
        ~DynamicBufferTests() = delete;
        DynamicBufferTests& operator=(const DynamicBufferTests&) = delete;
        DynamicBufferTests& operator=(DynamicBufferTests&&) = delete;

    private:

        /// Test Subclasses \\\

        // Tests methods related to write operations.
        class WriteTests;

    public:

        // Get list of DynamicBufferTests.
        static UTList GetTests();
    };

    class DynamicBufferTests::WriteTests
    {
    private:

        // Constant-Time Test Helper - Duplicate Logic of DynamicBuffer<T>::CalculateNewLength
        // Calculates new length for growing buffer.
        // If geometric increase is sufficent, then new length will be (m_Len + (m_Len >> 1)) (i.e., +50%)
        // Otherwise, the new length will be std::max(16, m_Len + minInc).
        template <size_t OldLen, size_t MinInc>
        [[nodiscard]] constexpr static size_t CalculateNewLengthCE() noexcept
        {
            constexpr size_t minNewLen = 16;
            constexpr size_t geoNewLen = OldLen + (OldLen >> 1);
            constexpr size_t linearLen = OldLen + MinInc;

            if constexpr (minNewLen >= geoNewLen && minNewLen >= linearLen)
            {
                return minNewLen;
            }
            else if constexpr (geoNewLen >= linearLen)
            {
                return geoNewLen;
            }
            else
            {
                return linearLen;
            }
        }

    public:

        template <typename T, TQ BufLen, bool Move>
        static UTR WriteElement()
        {
            constexpr size_t origBufLen = GetTQLength<BufLen>();
            constexpr bool bExpectFirstWriteGrow = (origBufLen == 0);
            constexpr bool bExpectSecondWriteGrow = (origBufLen == 1);

            std::vector<T> testData = GetTestData<T, TQ::Low>();

            DynamicBuffer<T> dynBuf(origBufLen);
            const T* pBufPtr = dynBuf.Get();
            size_t len = dynBuf.Length();

            try
            {
                if constexpr (Move)
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(std::move(testData[0])));
                }
                else
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(testData[0]));
                }
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(bExpectFirstWriteGrow == (pBufPtr != dynBuf.Get()));
            SUTL_TEST_ASSERT(bExpectFirstWriteGrow == (len != dynBuf.Length()));
            SUTL_TEST_ASSERT(dynBuf.WritePosition() == 1);

            pBufPtr = dynBuf.Get();
            len = dynBuf.Length();

            try
            {
                if constexpr (Move)
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(std::move(testData[1])));
                }
                else
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(testData[1]));
                }
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(bExpectSecondWriteGrow == (pBufPtr != dynBuf.Get()));
            SUTL_TEST_ASSERT(bExpectSecondWriteGrow == (len != dynBuf.Length()));
            SUTL_TEST_ASSERT(dynBuf.WritePosition() == 2);

            try
            {
                size_t srcIdx = 0;
                for (size_t i = 0; i < dynBuf.WritePosition(); i++, srcIdx++)
                {
                    if (srcIdx == testData.size())
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT(dynBuf[i] == testData[srcIdx]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        if (i == 0 && bExpectSecondWriteGrow)
                        {
                            SUTL_TEST_ASSERT(!dynBuf[i].Copied());
                            SUTL_TEST_ASSERT(dynBuf[i].Moved());
                        }
                        else
                        {
                            SUTL_TEST_ASSERT(dynBuf[i].Copied() == !Move);
                            SUTL_TEST_ASSERT(dynBuf[i].Moved() == Move);
                        }
                    }
                }
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TQ DstBufLen, TQ SrcBufLen>
        static UTR WritePtrNull()
        {
            constexpr size_t dstLen = GetTQLength<DstBufLen>();
            constexpr size_t srcLen = GetTQLength<SrcBufLen>();

            DynamicBuffer<T> dynBuf(dstLen);

            try
            {
                SUTL_TEST_ASSERT(dynBuf.Write(nullptr, srcLen) == false);
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TQ DstBufLen, TQ SrcBufLen, bool Move>
        static UTR WritePtr()
        {
            constexpr size_t dstLen = GetTQLength<DstBufLen>();
            constexpr size_t srcLen = GetTQLength<SrcBufLen>();

            constexpr bool bWriteResult = (srcLen == 0) ? false : true;
            constexpr bool bExpectFirstWriteGrow = (dstLen < srcLen);
            constexpr size_t firstWriteLen = (!bExpectFirstWriteGrow) ? dstLen : CalculateNewLengthCE<dstLen, srcLen>();
            constexpr bool bExpectSecondWriteGrow = (firstWriteLen < (2 * srcLen));
            constexpr size_t secondWriteLen = (!bExpectSecondWriteGrow) ? firstWriteLen : CalculateNewLengthCE<firstWriteLen, srcLen>();

            std::vector<T> testData = GetTestData<T, SrcBufLen>();
            T* pData = testData.data();
            const size_t dataLen = testData.size();

            DynamicBuffer<T> dynBuf(dstLen);
            const T* pFirstBufPtr = dynBuf.Get();
            const T* pSecondBufPtr = nullptr;

            try
            {
                if constexpr (Move)
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(std::move(pData), dataLen) == bWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(pData, dataLen) == bWriteResult);
                }
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(bExpectFirstWriteGrow == (pFirstBufPtr != dynBuf.Get()));
            SUTL_TEST_ASSERT(dynBuf.Length() == firstWriteLen);
            SUTL_TEST_ASSERT(dynBuf.WritePosition() == srcLen);
            if constexpr (srcLen > 0)
            {
                try
                {
                    size_t srcIdx = 0;
                    for (size_t i = 0; i < srcLen; i++, srcIdx++)
                    {
                        if (srcIdx == testData.size())
                        {
                            srcIdx = 0;
                        }

                        SUTL_TEST_ASSERT(dynBuf[i] == testData[i]);
                        if constexpr (std::is_same_v<T, Helper>)
                        {
                            SUTL_TEST_ASSERT(dynBuf[i].Copied() == !Move);
                            SUTL_TEST_ASSERT(dynBuf[i].Moved() == Move);
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    SUTL_TEST_EXCEPTION(e.what());
                }
            }

            pSecondBufPtr = dynBuf.Get();
            try
            {
                if constexpr (Move)
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(std::move(pData), dataLen) == bWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(pData, dataLen) == bWriteResult);
                }
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT((firstWriteLen == secondWriteLen) == (pSecondBufPtr == dynBuf.Get()));
            SUTL_TEST_ASSERT(dynBuf.Length() == secondWriteLen);
            SUTL_TEST_ASSERT(dynBuf.WritePosition() == srcLen * 2);
            if constexpr (srcLen > 0)
            {
                try
                {
                    size_t srcIdx = 0;
                    for (size_t i = 0; i < dynBuf.WritePosition(); i++, srcIdx++)
                    {
                        if (srcIdx == testData.size())
                        {
                            srcIdx = 0;
                        }

                        SUTL_TEST_ASSERT(dynBuf[i] == testData[srcIdx]);
                        if constexpr (std::is_same_v<T, Helper>)
                        {
                            if ((i < srcLen) && (pSecondBufPtr != dynBuf.Get()))
                            {
                                SUTL_TEST_ASSERT(!dynBuf[i].Copied());
                                SUTL_TEST_ASSERT(dynBuf[i].Moved());
                            }
                            else
                            {
                                SUTL_TEST_ASSERT(dynBuf[i].Copied() == !Move);
                                SUTL_TEST_ASSERT(dynBuf[i].Moved() == Move);
                            }
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    SUTL_TEST_EXCEPTION(e.what());
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TQ DstBufLen, TQ SrcWritePos, bool Move>
        static UTR WriteToWritePos()
        {
            constexpr size_t dstLen = GetTQLength<DstBufLen>();
            constexpr size_t srcWritePos = GetTQLength<SrcWritePos>(); 
            constexpr bool bExpectWrite = (SrcWritePos != TQ::None);

            constexpr bool bExpectFirstWriteGrow = bExpectWrite && (dstLen < srcWritePos);
            constexpr size_t firstWriteLen = (!bExpectFirstWriteGrow) ? dstLen : CalculateNewLengthCE<dstLen, srcWritePos>();
            constexpr bool bExpectSecondWriteGrow = bExpectWrite && (firstWriteLen < (2 * srcWritePos));
            constexpr size_t secondWriteLen = (!bExpectSecondWriteGrow) ? firstWriteLen : CalculateNewLengthCE<firstWriteLen, srcWritePos>();

            std::vector<T> testData(GetTestData<T, SrcWritePos>());
            DynamicBuffer<T> dstBuf(dstLen);
            DynamicBuffer<T> srcBuf(srcWritePos);

            const T* pFirstBufPtr = nullptr;
            const T* pSecondBufPtr = nullptr;

            try
            {
                SUTL_TEST_ASSERT(srcBuf.Write(testData.data(), srcWritePos) == bExpectWrite);
            }
            catch (const std::exception& e)
            {
                SUTL_SETUP_EXCEPTION(e.what());
            }

            SUTL_SETUP_ASSERT(srcBuf.WritePosition() == srcWritePos);
            if constexpr (bExpectWrite)
            {
                for (size_t i = 0; i < srcWritePos; i++)
                {
                    SUTL_SETUP_ASSERT(srcBuf[i] == testData[i]);
                }
            }

            pFirstBufPtr = dstBuf.Get();
            try
            {
                if constexpr (Move)
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(std::move(srcBuf), true) == bExpectWrite);
                }
                else
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(srcBuf, true) == bExpectWrite);
                }
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT((dstLen != firstWriteLen) == (pFirstBufPtr != dstBuf.Get()));
            SUTL_TEST_ASSERT(dstBuf.Length() == firstWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition() == srcWritePos);

            pSecondBufPtr = dstBuf.Get();
            try
            {
                if constexpr (Move)
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(std::move(srcBuf), true) == bExpectWrite);
                }
                else
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(srcBuf, true) == bExpectWrite);
                }
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT((firstWriteLen == secondWriteLen) == (pSecondBufPtr == dstBuf.Get()));
            SUTL_TEST_ASSERT(dstBuf.Length() == secondWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition() == srcWritePos * 2);
            try
            {
                size_t srcIdx = 0;
                for (size_t i = 0; i < dstBuf.WritePosition(); i++, srcIdx++)
                {
                    if (srcIdx == srcBuf.WritePosition())
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT(dstBuf[i] == srcBuf[srcIdx]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        if ((i < srcWritePos) && (pSecondBufPtr != dstBuf.Get()))
                        {
                            SUTL_TEST_ASSERT(!dstBuf[i].Copied());
                            SUTL_TEST_ASSERT(dstBuf[i].Moved());
                        }
                        else
                        {
                            SUTL_TEST_ASSERT(dstBuf[i].Copied() == !Move);
                            SUTL_TEST_ASSERT(dstBuf[i].Moved() == Move);
                        }
                    }
                }
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TQ DstBufLen, TQ SrcBufLen, bool Move>
        static UTR WriteToEnd()
        {
            constexpr size_t dstLen = GetTQLength<DstBufLen>();
            constexpr size_t srcLen = GetTQLength<SrcBufLen>();
            constexpr size_t srcWritePos = GetTQLength<SrcBufLen>();
            constexpr bool bExpectFirstWriteGrow = (dstLen < srcLen);
            constexpr size_t firstWriteLen = (!bExpectFirstWriteGrow) ? dstLen : CalculateNewLengthCE<dstLen, srcLen>();
            constexpr bool bExpectSecondWriteGrow = (firstWriteLen < (2 * srcLen));
            constexpr size_t secondWriteLen = (!bExpectSecondWriteGrow) ? firstWriteLen : CalculateNewLengthCE<firstWriteLen, srcLen>();
            constexpr bool bExpectWrite = (SrcBufLen != TQ::None);

            std::vector<T> testData(GetTestData<T, SrcBufLen>());
            DynamicBuffer<T> dstBuf(dstLen);
            DynamicBuffer<T> srcBuf(srcLen);
            const T* pFirstBufPtr = nullptr;
            const T* pSecondBufPtr = nullptr;

            try
            {
                SUTL_TEST_ASSERT(srcBuf.Write(testData.data(), srcWritePos) == bExpectWrite);
            }
            catch (const std::exception& e)
            {
                SUTL_SETUP_EXCEPTION(e.what());
            }

            SUTL_SETUP_ASSERT(srcBuf.WritePosition() == srcWritePos);
            if constexpr (bExpectWrite)
            {
                for (size_t i = 0; i < srcWritePos; i++)
                {
                    SUTL_SETUP_ASSERT(srcBuf[i] == testData[i]);
                }
            }

            pFirstBufPtr = dstBuf.Get();
            try
            {
                if constexpr (Move)
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(std::move(srcBuf), false) == bExpectWrite);
                }
                else
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(srcBuf, false) == bExpectWrite);
                }
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT((dstLen != firstWriteLen) == (pFirstBufPtr != dstBuf.Get()));
            SUTL_TEST_ASSERT(dstBuf.Length() == firstWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition() == srcBuf.Length());

            pSecondBufPtr = dstBuf.Get();
            try
            {
                if constexpr (Move)
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(std::move(srcBuf), false) == bExpectWrite);
                }
                else
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(srcBuf, false) == bExpectWrite);
                }
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT((firstWriteLen == secondWriteLen) == (pSecondBufPtr == dstBuf.Get()));
            SUTL_TEST_ASSERT(dstBuf.Length() == secondWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition() == srcBuf.Length() * 2);
            try
            {
                size_t srcIdx = 0;
                for (size_t i = 0; i < dstBuf.WritePosition(); i++, srcIdx++)
                {
                    if (srcIdx == srcBuf.Length())
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT(dstBuf[i] == srcBuf[srcIdx]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        if ((i < srcBuf.Length()) && (pSecondBufPtr != dstBuf.Get()))
                        {
                            SUTL_TEST_ASSERT(!dstBuf[i].Copied());
                            SUTL_TEST_ASSERT(dstBuf[i].Moved());
                        }
                        else
                        {
                            SUTL_TEST_ASSERT(dstBuf[i].Copied() == !Move);
                            SUTL_TEST_ASSERT(dstBuf[i].Moved() == Move);
                        }
                    }
                }
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_SUCCESS();
        }
    };
}