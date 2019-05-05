#pragma once

// SUTL
#include <UnitTestResult.h>

// STL
#include <algorithm>
#include <functional>
#include <list>
#include <memory>

// Test Helpers
#include <CCBufferTests.h>

// Target Test Class
#include <CCDynamicBuffer.h>

namespace CC
{
    class DynamicBufferTests
    {
    public:

        // Type aliases
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;
        using Helper = BufferTests::Helper;
        using TQ = BufferTests::TestQuantity;

    private:

        template <TQ _TQ>
        static constexpr const size_t GetTQNum( )
        {
            static_assert(BufferTests::IsValidTestQuantity<_TQ>( ), __FUNCTION__": Invalid test quantity.");

            if constexpr ( _TQ == TQ::Zero )
            {
                return 0;
            }
            else if constexpr ( _TQ == TQ::One )
            {
                return 1;
            }
            else if constexpr ( _TQ == TQ::Many )
            {
                return 16;
            }
        }

        template <TQ OrigBufLen, TQ MinInc>
        static constexpr const size_t GetExpectedNewLen( )
        {
            static_assert(BufferTests::IsValidTestQuantity<OrigBufLen>( ), __FUNCTION__": Invalid test quantity.");
            static_assert(BufferTests::IsValidTestQuantity<MinInc>( ), __FUNCTION__": Invalid test quantity.");

            if constexpr ( OrigBufLen == TQ::Zero )
            {
                return 16;
            }
            else if constexpr ( OrigBufLen == TQ::One )
            {
                constexpr const size_t origLen = GetTQNum<OrigBufLen>( );
                constexpr const size_t minInc = GetTQNum<MinInc>( );
                return (MinInc != TQ::Many) ? 16 : (origLen + minInc);
            }
            else if constexpr ( OrigBufLen == TQ::Many )
            {
                constexpr const size_t origLen = GetTQNum<OrigBufLen>( );
                constexpr const size_t minInc = GetTQNum<MinInc>( );
                return (MinInc != TQ::Many) ? ((origLen * 3) >> 1) : (origLen + minInc);
            }
        }

    public:

        // Get list of DynamicBufferTests.
        static UTList GetTests( );

        // Tests methods related to calculating new length for buffer growth.
        class CalculateNewLengthTests;
        
        // Tests methods related to growing the buffer if required.
        class GrowBufferIfRequiredTests;

        // Tests methods related to write operations.
        class WriteTests;
    };

    class DynamicBufferTests::CalculateNewLengthTests
    {
    public:

        template <typename T, TQ OrigBufferLen, TQ MinInc>
        static UTR CalcNewLen( )
        {
            constexpr const size_t origBufLen = GetTQNum<OrigBufferLen>( );
            constexpr const size_t minInc = GetTQNum<MinInc>( );
            constexpr const size_t expNewLen = GetExpectedNewLen<OrigBufferLen, MinInc>( );
            const size_t newLen = DynamicBuffer<T>::CalculateNewLength(origBufLen, minInc);
            SUTL_TEST_ASSERT(expNewLen == newLen);
            SUTL_TEST_SUCCESS( );
        }
    };

    class DynamicBufferTests::GrowBufferIfRequiredTests
    {
    public:

        template <typename T, TQ OrigBufferLen, TQ MinInc>
        static UTR GrowBufIfReq( )
        {
            constexpr const size_t origBufSize = GetTQNum<OrigBufferLen>( );
            constexpr const size_t minInc = GetTQNum<MinInc>( );
            constexpr const size_t expNewLen = GetExpectedNewLen<OrigBufferLen, MinInc>( );
            constexpr const bool bGrowReqZeroWritePos = (minInc > origBufSize);
            constexpr const bool bGrowReqEndWritePos = (minInc != 0);

            SUTL_TEST_ASSERT(minInc == 0 || bGrowReqZeroWritePos || bGrowReqEndWritePos);

            DynamicBuffer<T> dynBuf(origBufSize);
            const T* pBufPtr = dynBuf.Ptr( );
            dynBuf.GrowBufferIfRequired(minInc);
            if constexpr ( bGrowReqZeroWritePos )
            {
                SUTL_TEST_ASSERT(pBufPtr != dynBuf.Ptr( ));
                SUTL_TEST_ASSERT(dynBuf.Length( ) == expNewLen);
            }

            dynBuf = DynamicBuffer<T>(origBufSize);
            dynBuf.m_WritePos = origBufSize;
            pBufPtr = dynBuf.Ptr( );
            dynBuf.GrowBufferIfRequired(minInc);
            if constexpr ( bGrowReqEndWritePos )
            {
                SUTL_TEST_ASSERT(pBufPtr != dynBuf.Ptr( ));
                SUTL_TEST_ASSERT(dynBuf.Length( ) == expNewLen);
            }

            SUTL_TEST_SUCCESS( );
        }
    };

    class DynamicBufferTests::WriteTests
    {
    private:

        template <TQ DstLen, TQ SrcLen>
        static constexpr const size_t GetExpectedFirstPtrWriteLen( )
        {
            constexpr const size_t dst = GetTQNum<DstLen>( );
            constexpr const size_t src = BufferTests::GetTQNum<SrcLen>( );

            static_assert(BufferTests::IsValidTestQuantity<DstLen>( ), __FUNCTION__": Invalid test quantity.");
            static_assert(BufferTests::IsValidTestQuantity<SrcLen>( ), __FUNCTION__": Invalid test quantity.");

            if constexpr ( SrcLen == TQ::Zero )
            {
                return dst;
            }
            else if constexpr ( SrcLen == TQ::One )
            {
                return (DstLen == TQ::Zero) ? 16 : dst;
            }
            else if constexpr ( SrcLen == TQ::Many )
            {
                return dst + src;
            }
        }

        template <TQ DstLen, TQ SrcLen>
        static constexpr const size_t GetExpectedSecondPtrWriteLen( )
        {
            constexpr const size_t dst = GetExpectedFirstPtrWriteLen<DstLen, SrcLen>( );
            constexpr const size_t src = BufferTests::GetTQNum<SrcLen>( );

            static_assert(BufferTests::IsValidTestQuantity<DstLen>( ), __FUNCTION__": Invalid test quantity.");
            static_assert(BufferTests::IsValidTestQuantity<SrcLen>( ), __FUNCTION__": Invalid test quantity.");

            if constexpr ( SrcLen == TQ::Zero )
            {
                return dst;
            }
            else if constexpr ( SrcLen == TQ::One )
            {
                return 16;
            }
            else if constexpr ( SrcLen == TQ::Many )
            {
                return dst + src;
            }
        }

        template <TQ DstLen, TQ SrcWritePos, bool bWriteToWritePos>
        static constexpr const size_t GetExpectedFirstBufWriteLen( )
        {
            constexpr const size_t dst = GetTQNum<DstLen>( );
            constexpr const size_t src = bWriteToWritePos ? GetTQNum<SrcWritePos>( ) : (GetTQNum<SrcWritePos>( ) + BufferTests::GetTQNum<SrcWritePos>( ));

            if constexpr ( bWriteToWritePos )
            {
                if constexpr ( SrcWritePos == TQ::Zero )
                {
                    return dst;
                }
                else if constexpr ( SrcWritePos == TQ::One )
                {
                    return (DstLen == TQ::Zero) ? 16 : dst;
                }
                else if constexpr ( SrcWritePos == TQ::Many )
                {
                    return (DstLen == TQ::Many) ? dst : dst + src;
                }
            }
            else
            {
                if constexpr ( SrcWritePos == TQ::Zero )
                {
                    return dst;
                }
                else if constexpr ( SrcWritePos == TQ::One )
                {
                    return (DstLen == TQ::Zero) ? 16 : dst;
                }
                else if constexpr ( SrcWritePos == TQ::Many )
                {
                    return dst + src;
                }
            }
        }

        template <TQ DstLen, TQ SrcWritePos, bool bWriteToWritePos>
        static constexpr const size_t GetExpectedSecondBufWriteLen( )
        {
            constexpr const size_t dst = GetExpectedFirstBufWriteLen<DstLen, SrcWritePos, bWriteToWritePos>( );
            constexpr const size_t src = bWriteToWritePos ? GetTQNum<SrcWritePos>( ) : (GetTQNum<SrcWritePos>( ) + BufferTests::GetTQNum<SrcWritePos>( ));

            if constexpr ( bWriteToWritePos )
            {
                if constexpr ( SrcWritePos == TQ::Zero )
                {
                    return dst;
                }
                else if constexpr ( SrcWritePos == TQ::One )
                {
                    return (DstLen != TQ::Many) ? 16 : dst;
                }
                else if constexpr ( SrcWritePos == TQ::Many )
                {
                    return dst + src;
                }
            }
            else
            {
                if constexpr ( SrcWritePos == TQ::Zero )
                {
                    return dst;
                }
                else if constexpr ( SrcWritePos == TQ::One )
                {
                    return (DstLen != TQ::Many) ? 16 : dst;
                }
                else if constexpr ( SrcWritePos == TQ::Many )
                {
                    return dst + src;
                }
            }
        }

    public:

        template <typename T, TQ BufLen>
        static UTR WriteElement( )
        {
            constexpr const size_t origBufLen = GetTQNum<BufLen>( );
            constexpr const bool bExpectFirstWriteGrow = (origBufLen == 0);
            constexpr const bool bExpectSecondWriteGrow = (origBufLen == 1);

            const std::vector<T> testData = BufferTests::GetTestData<T, TQ::Many>( );

            DynamicBuffer<T> dynBuf(origBufLen);
            const T* pBufPtr = dynBuf.Ptr( );
            size_t len = dynBuf.Length( );

            try
            {
                dynBuf.Write(testData[0]);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bExpectFirstWriteGrow == (pBufPtr != dynBuf.Ptr( )));
            SUTL_TEST_ASSERT(bExpectFirstWriteGrow == (len != dynBuf.Length( )));
            SUTL_TEST_ASSERT(memcmp(testData.data( ), dynBuf.Ptr( ), sizeof(T)) == 0);
            SUTL_TEST_ASSERT(dynBuf.WritePosition( ) == 1);

            pBufPtr = dynBuf.Ptr( );
            len = dynBuf.Length( );

            try
            {
                dynBuf.Write(testData[1]);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bExpectSecondWriteGrow == (pBufPtr != dynBuf.Ptr( )));
            SUTL_TEST_ASSERT(bExpectSecondWriteGrow == (len != dynBuf.Length( )));
            SUTL_TEST_ASSERT(memcmp(testData.data( ), dynBuf.Ptr( ), sizeof(T) * 2) == 0);
            SUTL_TEST_ASSERT(dynBuf.WritePosition( ) == 2);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TQ DstBufLen, TQ SrcBufLen>
        static UTR WritePtrNull( )
        {
            constexpr const size_t dstLen = GetTQNum<DstBufLen>( );
            constexpr const size_t srcLen = GetTQNum<SrcBufLen>( );
            constexpr const bool bExpectThrow = (srcLen != 0);

            DynamicBuffer<T> dynBuf(dstLen);
            bool bThrew = false;

            try
            {
                dynBuf.Write(nullptr, srcLen);
            }
            catch ( const std::logic_error& )
            {
                bThrew = true;
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bExpectThrow == bThrew);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TQ DstBufLen, TQ SrcBufLen>
        static UTR WritePtr( )
        {
            constexpr const size_t dstLen = GetTQNum<DstBufLen>( );
            constexpr const size_t srcLen = BufferTests::GetTQNum<SrcBufLen>( );
            constexpr const size_t firstWriteLen = GetExpectedFirstPtrWriteLen<DstBufLen, SrcBufLen>( );
            constexpr const size_t secondWriteLen = GetExpectedSecondPtrWriteLen<DstBufLen, SrcBufLen>( );
            constexpr const bool bExpectWrite = (SrcBufLen != TQ::Zero);

            std::vector<T> testData = BufferTests::GetTestData<T, SrcBufLen>( );
            DynamicBuffer<T> dynBuf(dstLen);
            const T* pBufPtr = dynBuf.Ptr( );

            try
            {
                dynBuf.Write(testData.data( ), testData.size( ));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((dstLen != firstWriteLen) == (pBufPtr != dynBuf.Ptr( )));
            SUTL_TEST_ASSERT(dynBuf.Length( ) == firstWriteLen);
            SUTL_TEST_ASSERT(dynBuf.WritePosition( ) == srcLen);
            if constexpr ( bExpectWrite )
            {
                SUTL_TEST_ASSERT(memcmp(testData.data( ), dynBuf.Ptr( ), sizeof(T) * srcLen) == 0);
            }

            pBufPtr = dynBuf.Ptr( );
            try
            {
                dynBuf.Write(testData.data( ), testData.size( ));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((firstWriteLen == secondWriteLen) == (pBufPtr == dynBuf.Ptr( )));
            SUTL_TEST_ASSERT(dynBuf.Length( ) == secondWriteLen);
            SUTL_TEST_ASSERT(dynBuf.WritePosition( ) == srcLen * 2);
            for ( size_t i = 0, srcIdx = 0; i < (srcLen * 2); i++, srcIdx++ )
            {
                if ( srcIdx == srcLen )
                {
                    srcIdx = 0;
                }

                SUTL_TEST_ASSERT(dynBuf[i] == testData[srcIdx]);
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TQ DstBufLen, TQ SrcWritePos>
        static UTR WriteToWritePos( )
        {
            constexpr const size_t dstLen = GetTQNum<DstBufLen>( );
            constexpr const size_t srcLen = BufferTests::GetTQNum<TQ::Many>( );
            constexpr const size_t srcWritePos = GetTQNum<SrcWritePos>( );
            constexpr const size_t firstWriteLen = GetExpectedFirstBufWriteLen<DstBufLen, SrcWritePos, true>( );
            constexpr const size_t secondWriteLen = GetExpectedSecondBufWriteLen<DstBufLen, SrcWritePos, true>( );
            constexpr const bool bExpectWrite = (SrcWritePos != TQ::Zero);

            std::vector<T> testData(BufferTests::GetTestData<T, SrcWritePos>( ));
            DynamicBuffer<T> dstBuf(dstLen);
            DynamicBuffer<T> srcBuf(srcLen);
            const T* pBufPtr = dstBuf.Ptr( );

            try
            {
                srcBuf.Write(testData.data( ), srcWritePos);
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            SUTL_SETUP_ASSERT(srcBuf.WritePosition( ) == srcWritePos);
            if constexpr ( bExpectWrite )
            {
                for ( size_t i = 0; i < srcWritePos; i++ )
                {
                    SUTL_SETUP_ASSERT(srcBuf[i] == testData[i]);
                }
            }

            try
            {
                dstBuf.Write(srcBuf, true);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((dstLen != firstWriteLen) == (pBufPtr != dstBuf.Ptr( )));
            SUTL_TEST_ASSERT(dstBuf.Length( ) == firstWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition( ) == srcWritePos);

            pBufPtr = dstBuf.Ptr( );
            try
            {
                dstBuf.Write(srcBuf, true);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((firstWriteLen == secondWriteLen) == (pBufPtr == dstBuf.Ptr( )));
            SUTL_TEST_ASSERT(dstBuf.Length( ) == secondWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition( ) == srcWritePos * 2);
            for ( size_t i = 0, srcIdx = 0; i < (srcWritePos * 2); i++, srcIdx++ )
            {
                if ( srcIdx == srcWritePos )
                {
                    srcIdx = 0;
                }

                SUTL_TEST_ASSERT(dstBuf[i] == srcBuf[srcIdx]);
            }

            SUTL_TEST_SUCCESS( );
        }
        
        template <typename T, TQ DstBufLen, TQ SrcBufLen>
        static UTR WriteToEnd( )
        {
            constexpr const size_t dstLen = GetTQNum<DstBufLen>( );
            constexpr const size_t srcLen = GetTQNum<SrcBufLen>( );
            constexpr const size_t srcWritePos = BufferTests::GetTQNum<SrcBufLen>( );
            constexpr const size_t firstWriteLen = GetExpectedFirstBufWriteLen<DstBufLen, SrcBufLen, false>( );
            constexpr const size_t secondWriteLen = GetExpectedSecondBufWriteLen<DstBufLen, SrcBufLen, false>( );
            constexpr const bool bExpectWrite = (SrcBufLen != TQ::Zero);

            std::vector<T> testData(BufferTests::GetTestData<T, SrcBufLen>( ));
            DynamicBuffer<T> dstBuf(dstLen);
            DynamicBuffer<T> srcBuf(srcLen);
            const T* pBufPtr = dstBuf.Ptr( );

            try
            {
                srcBuf.Write(testData.data( ), srcWritePos);
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            SUTL_SETUP_ASSERT(srcBuf.WritePosition( ) == srcWritePos);
            if constexpr ( bExpectWrite )
            {
                for ( size_t i = 0; i < srcWritePos; i++ )
                {
                    SUTL_SETUP_ASSERT(srcBuf[i] == testData[i]);
                }
            }

            try
            {
                dstBuf.Write(srcBuf, false);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((dstLen != firstWriteLen) == (pBufPtr != dstBuf.Ptr( )));
            SUTL_TEST_ASSERT(dstBuf.Length( ) == firstWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition( ) == srcBuf.Length( ));

            pBufPtr = dstBuf.Ptr( );
            try
            {
                dstBuf.Write(srcBuf, false);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((firstWriteLen == secondWriteLen) == (pBufPtr == dstBuf.Ptr( )));
            SUTL_TEST_ASSERT(dstBuf.Length( ) == secondWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition( ) == srcBuf.Length( ) * 2);
            for ( size_t i = 0, srcIdx = 0; i < (srcBuf.Length( ) * 2); i++, srcIdx++ )
            {
                if ( srcIdx == srcBuf.Length( ) )
                {
                    srcIdx = 0;
                }

                SUTL_TEST_ASSERT(dstBuf[i] == srcBuf[srcIdx]);
            }

            SUTL_TEST_SUCCESS( );
        }
    };
}