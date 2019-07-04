#pragma once

// SUTL
#include <UnitTestResult.h>

// STL
#include <algorithm>
#include <functional>
#include <list>
#include <memory>

// Test Helpers
#include <CCPointerTests.h>
#include <CCBufferTests.h>

// Target Test Class
#include <CCDynamicBuffer.h>

namespace CC
{
    class DynamicBufferTests
    {
    private:

        // Type aliases
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;
        using Helper = PointerTests::Helper;
        using TQ = PointerTests::TestQuantity;

        template <TQ _TQ>
        static constexpr const size_t GetTQNum( )
        {
            static_assert(PointerTests::IsValidTestQuantity<_TQ>( ), __FUNCSIG__": Invalid TestQuantity Template Argument.");

            if constexpr ( _TQ == TQ::Zero )
            {
                return 0;
            }
            else if constexpr ( _TQ == TQ::One )
            {
                return 1;
            }
            else
            {
                return 16;
            }
        }

        template <TQ OrigBufLen, TQ MinInc>
        static constexpr const size_t GetExpectedNewLen( )
        {
            static_assert(PointerTests::IsValidTestQuantity<OrigBufLen>( ), __FUNCTION__": Invalid test quantity.");
            static_assert(PointerTests::IsValidTestQuantity<MinInc>( ), __FUNCTION__": Invalid test quantity.");

            if constexpr ( OrigBufLen != TQ::Many )
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

        template <typename T, TQ _TQ>
        static std::vector<T> GetTestData( )
        {
            return BufferTests::GetTestData<T, _TQ>( );
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
            const T* pBufPtr = dynBuf.Get( );
            SUTL_TEST_ASSERT(dynBuf.GrowBufferIfRequired(minInc));
            if constexpr ( bGrowReqZeroWritePos )
            {
                SUTL_TEST_ASSERT(pBufPtr != dynBuf.Get( ));
                SUTL_TEST_ASSERT(dynBuf.Length( ) == expNewLen);
            }

            dynBuf = DynamicBuffer<T>(origBufSize);
            dynBuf.m_WritePos = origBufSize;
            pBufPtr = dynBuf.Get( );
            SUTL_TEST_ASSERT(dynBuf.GrowBufferIfRequired(minInc));
            if constexpr ( bGrowReqEndWritePos )
            {
                SUTL_TEST_ASSERT(pBufPtr != dynBuf.Get( ));
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

            static_assert(PointerTests::IsValidTestQuantity<DstLen>( ), __FUNCTION__": Invalid test quantity.");
            static_assert(PointerTests::IsValidTestQuantity<SrcLen>( ), __FUNCTION__": Invalid test quantity.");

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

            static_assert(PointerTests::IsValidTestQuantity<DstLen>( ), __FUNCTION__": Invalid test quantity.");
            static_assert(PointerTests::IsValidTestQuantity<SrcLen>( ), __FUNCTION__": Invalid test quantity.");

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

        template <typename T, TQ BufLen, bool Move>
        static UTR WriteElement( )
        {
            constexpr const size_t origBufLen = GetTQNum<BufLen>( );
            constexpr const bool bExpectFirstWriteGrow = (origBufLen == 0);
            constexpr const bool bExpectSecondWriteGrow = (origBufLen == 1);

            std::vector<T> testData = GetTestData<T, TQ::Many>( );

            DynamicBuffer<T> dynBuf(origBufLen);
            const T* pBufPtr = dynBuf.Get( );
            size_t len = dynBuf.Length( );

            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(std::move(testData[0])));
                }
                else
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(testData[0]));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bExpectFirstWriteGrow == (pBufPtr != dynBuf.Get( )));
            SUTL_TEST_ASSERT(bExpectFirstWriteGrow == (len != dynBuf.Length( )));
            SUTL_TEST_ASSERT(dynBuf.WritePosition( ) == 1);

            pBufPtr = dynBuf.Get( );
            len = dynBuf.Length( );

            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(std::move(testData[1])));
                }
                else
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(testData[1]));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bExpectSecondWriteGrow == (pBufPtr != dynBuf.Get( )));
            SUTL_TEST_ASSERT(bExpectSecondWriteGrow == (len != dynBuf.Length( )));
            SUTL_TEST_ASSERT(dynBuf.WritePosition( ) == 2);

            try
            {
                size_t srcIdx = 0;
                for ( size_t i = 0; i < dynBuf.WritePosition( ); i++, srcIdx++ )
                {
                    if ( srcIdx == testData.size( ) )
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT(dynBuf[i] == testData[srcIdx]);
                    if constexpr ( std::is_same_v<T, Helper> )
                    {
                        if ( i == 0 && bExpectSecondWriteGrow )
                        {
                            SUTL_TEST_ASSERT(!dynBuf[i].Copied( ));
                            SUTL_TEST_ASSERT(dynBuf[i].Moved( ));
                        }
                        else
                        {
                            SUTL_TEST_ASSERT(dynBuf[i].Copied( ) == !Move);
                            SUTL_TEST_ASSERT(dynBuf[i].Moved( ) == Move);
                        }
                    }
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TQ DstBufLen, TQ SrcBufLen>
        static UTR WritePtrNull( )
        {
            constexpr const size_t dstLen = GetTQNum<DstBufLen>( );
            constexpr const size_t srcLen = GetTQNum<SrcBufLen>( );

            DynamicBuffer<T> dynBuf(dstLen);

            try
            {
                SUTL_TEST_ASSERT(dynBuf.Write(nullptr, srcLen) == false);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TQ DstBufLen, TQ SrcBufLen, bool Move>
        static UTR WritePtr( )
        {
            constexpr const size_t dstLen = GetTQNum<DstBufLen>( );
            constexpr const size_t srcLen = BufferTests::GetTQNum<SrcBufLen>( );

            constexpr const bool bWriteResult = (srcLen == 0) ? false : true;
            constexpr const size_t firstWriteLen = GetExpectedFirstPtrWriteLen<DstBufLen, SrcBufLen>( );
            constexpr const size_t secondWriteLen = GetExpectedSecondPtrWriteLen<DstBufLen, SrcBufLen>( );

            std::vector<T> testData = GetTestData<T, SrcBufLen>( );
            T* pData = testData.data( );
            const size_t dataLen = testData.size( );

            DynamicBuffer<T> dynBuf(dstLen);
            const T* pFirstBufPtr = dynBuf.Get( );
            const T* pSecondBufPtr = nullptr;

            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(std::move(pData), dataLen) == bWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(pData, dataLen) == bWriteResult);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((dstLen != firstWriteLen) == (pFirstBufPtr != dynBuf.Get( )));
            SUTL_TEST_ASSERT(dynBuf.Length( ) == firstWriteLen);
            SUTL_TEST_ASSERT(dynBuf.WritePosition( ) == srcLen);
            if constexpr ( srcLen > 0 )
            {
                try
                {
                    size_t srcIdx = 0;
                    for ( size_t i = 0; i < srcLen; i++, srcIdx++ )
                    {
                        if ( srcIdx == testData.size( ) )
                        {
                            srcIdx = 0;
                        }

                        SUTL_TEST_ASSERT(dynBuf[i] == testData[i]);
                        if constexpr ( std::is_same_v<T, Helper> )
                        {
                            SUTL_TEST_ASSERT(dynBuf[i].Copied( ) == !Move);
                            SUTL_TEST_ASSERT(dynBuf[i].Moved( ) == Move);
                        }
                    }
                }
                catch ( const std::exception& e )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }

            pSecondBufPtr = dynBuf.Get( );
            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(std::move(pData), dataLen) == bWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(dynBuf.Write(pData, dataLen) == bWriteResult);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((firstWriteLen == secondWriteLen) == (pSecondBufPtr == dynBuf.Get( )));
            SUTL_TEST_ASSERT(dynBuf.Length( ) == secondWriteLen);
            SUTL_TEST_ASSERT(dynBuf.WritePosition( ) == srcLen * 2);
            if constexpr ( srcLen > 0 )
            {
                try
                {
                    size_t srcIdx = 0;
                    for ( size_t i = 0; i < dynBuf.WritePosition( ); i++, srcIdx++ )
                    {
                        if ( srcIdx == testData.size( ) )
                        {
                            srcIdx = 0;
                        }

                        SUTL_TEST_ASSERT(dynBuf[i] == testData[srcIdx]);
                        if constexpr ( std::is_same_v<T, Helper> )
                        {
                            if ( (i < srcLen) && (pSecondBufPtr != dynBuf.Get( )) )
                            {
                                SUTL_TEST_ASSERT(!dynBuf[i].Copied( ));
                                SUTL_TEST_ASSERT(dynBuf[i].Moved( ));
                            }
                            else
                            {
                                SUTL_TEST_ASSERT(dynBuf[i].Copied( ) == !Move);
                                SUTL_TEST_ASSERT(dynBuf[i].Moved( ) == Move);
                            }
                        }
                    }
                }
                catch ( const std::exception& e )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TQ DstBufLen, TQ SrcWritePos, bool Move>
        static UTR WriteToWritePos( )
        {
            constexpr const size_t dstLen = GetTQNum<DstBufLen>( );
            constexpr const size_t srcLen = GetTQNum<TQ::Many>( );
            constexpr const size_t srcWritePos = GetTQNum<SrcWritePos>( );
            constexpr const size_t firstWriteLen = GetExpectedFirstBufWriteLen<DstBufLen, SrcWritePos, true>( );
            constexpr const size_t secondWriteLen = GetExpectedSecondBufWriteLen<DstBufLen, SrcWritePos, true>( );
            constexpr const bool bExpectWrite = (SrcWritePos != TQ::Zero);

            std::vector<T> testData(GetTestData<T, SrcWritePos>( ));
            DynamicBuffer<T> dstBuf(dstLen);
            DynamicBuffer<T> srcBuf(srcLen);

            const T* pFirstBufPtr = nullptr;
            const T* pSecondBufPtr = nullptr;

            try
            {
                SUTL_TEST_ASSERT(srcBuf.Write(testData.data( ), srcWritePos) == bExpectWrite);
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

            pFirstBufPtr = dstBuf.Get( );
            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(std::move(srcBuf), true) == bExpectWrite);
                }
                else
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(srcBuf, true) == bExpectWrite);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((dstLen != firstWriteLen) == (pFirstBufPtr != dstBuf.Get( )));
            SUTL_TEST_ASSERT(dstBuf.Length( ) == firstWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition( ) == srcWritePos);

            pSecondBufPtr = dstBuf.Get( );
            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(std::move(srcBuf), true) == bExpectWrite);
                }
                else
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(srcBuf, true) == bExpectWrite);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((firstWriteLen == secondWriteLen) == (pSecondBufPtr == dstBuf.Get( )));
            SUTL_TEST_ASSERT(dstBuf.Length( ) == secondWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition( ) == srcWritePos * 2);
            try
            {
                size_t srcIdx = 0;
                for ( size_t i = 0; i < dstBuf.WritePosition( ); i++, srcIdx++ )
                {
                    if ( srcIdx == srcBuf.WritePosition( ) )
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT(dstBuf[i] == srcBuf[srcIdx]);
                    if constexpr ( std::is_same_v<T, Helper> )
                    {
                        if ( (i < srcWritePos) && (pSecondBufPtr != dstBuf.Get( )) )
                        {
                            SUTL_TEST_ASSERT(!dstBuf[i].Copied( ));
                            SUTL_TEST_ASSERT(dstBuf[i].Moved( ));
                        }
                        else
                        {
                            SUTL_TEST_ASSERT(dstBuf[i].Copied( ) == !Move);
                            SUTL_TEST_ASSERT(dstBuf[i].Moved( ) == Move);
                        }
                    }
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_SUCCESS( );
        }
        
        template <typename T, TQ DstBufLen, TQ SrcBufLen, bool Move>
        static UTR WriteToEnd( )
        {
            constexpr const size_t dstLen = GetTQNum<DstBufLen>( );
            constexpr const size_t srcLen = GetTQNum<SrcBufLen>( );
            constexpr const size_t srcWritePos = BufferTests::GetTQNum<SrcBufLen>( );
            constexpr const size_t firstWriteLen = GetExpectedFirstBufWriteLen<DstBufLen, SrcBufLen, false>( );
            constexpr const size_t secondWriteLen = GetExpectedSecondBufWriteLen<DstBufLen, SrcBufLen, false>( );
            constexpr const bool bExpectWrite = (SrcBufLen != TQ::Zero);

            std::vector<T> testData(GetTestData<T, SrcBufLen>( ));
            DynamicBuffer<T> dstBuf(dstLen);
            DynamicBuffer<T> srcBuf(srcLen);
            const T* pFirstBufPtr = nullptr;
            const T* pSecondBufPtr = nullptr;

            try
            {
                SUTL_TEST_ASSERT(srcBuf.Write(testData.data( ), srcWritePos) == bExpectWrite);
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

            pFirstBufPtr = dstBuf.Get( );
            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(std::move(srcBuf), false) == bExpectWrite);
                }
                else
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(srcBuf, false) == bExpectWrite);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((dstLen != firstWriteLen) == (pFirstBufPtr != dstBuf.Get( )));
            SUTL_TEST_ASSERT(dstBuf.Length( ) == firstWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition( ) == srcBuf.Length( ));

            pSecondBufPtr = dstBuf.Get( );
            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(std::move(srcBuf), false) == bExpectWrite);
                }
                else
                {
                    SUTL_TEST_ASSERT(dstBuf.Write(srcBuf, false) == bExpectWrite);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT((firstWriteLen == secondWriteLen) == (pSecondBufPtr == dstBuf.Get( )));
            SUTL_TEST_ASSERT(dstBuf.Length( ) == secondWriteLen);
            SUTL_TEST_ASSERT(dstBuf.WritePosition( ) == srcBuf.Length( ) * 2);
            try
            {
                size_t srcIdx = 0;
                for ( size_t i = 0; i < dstBuf.WritePosition( ); i++, srcIdx++ )
                {
                    if ( srcIdx == srcBuf.Length( ) )
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT(dstBuf[i] == srcBuf[srcIdx]);
                    if constexpr ( std::is_same_v<T, Helper> )
                    {
                        if ( (i < srcBuf.Length( )) && (pSecondBufPtr != dstBuf.Get( )) )
                        {
                            SUTL_TEST_ASSERT(!dstBuf[i].Copied( ));
                            SUTL_TEST_ASSERT(dstBuf[i].Moved( ));
                        }
                        else
                        {
                            SUTL_TEST_ASSERT(dstBuf[i].Copied( ) == !Move);
                            SUTL_TEST_ASSERT(dstBuf[i].Moved( ) == Move);
                        }
                    }
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_SUCCESS( );
        }
    };
}