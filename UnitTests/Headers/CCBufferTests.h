#pragma once

// SUTL
#include <UnitTestResult.h>

// STL
#include <algorithm>
#include <functional>
#include <list>
#include <memory>

// Target Class
#include <CCBuffer.h>

// Test Helper Utils
#include <CCPointerHelpersTests.h>

namespace CC
{
    class BufferTests
    {
        friend class DynamicBufferTests;

        BufferTests( ) = delete;
        BufferTests(const BufferTests&) = delete;
        BufferTests(BufferTests&&) = delete;
        ~BufferTests( ) = delete;
        BufferTests& operator=(const BufferTests&) = delete;
        BufferTests& operator=(BufferTests&&) = delete;

    private:

        // Type aliases
        using PHT = PointerHelpersTests;
        using TestQuantity = PHT::TestQuantity;
        using Helper = PHT::Helper;

        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;

        template <TestQuantity TQ>
        static constexpr const size_t GetTQNum( )
        {
            return PHT::GetTQNum<TQ>( );
        }

        template <typename T, TestQuantity TQ>
        static std::vector<T> GetTestData( )
        {
            return PHT::GetTestData<T, TQ>( );
        }

        /// Test Subclasses \\\

        // Tests constructor behavior.
        class ConstructorTests;

        // Tests op-overloads and other methods related to copying.
        class CopyTests;

        // Tests op-overloads and other methods related to moving.
        class MoveTests;

        // Tests behavior of the overloaded subscript operator methods.
        class SubscriptOperatorTests;

        // Tests behavior of the overloaded dereference operator methods.
        class DereferenceOperatorTests;

        // Tests comparison behavior between buffers.
        class ComparisonTests;

        // Tests SetWritePosition behavior.
        class SetWritePositionTests;

        // Tests behavior of the methods that write to the buffer.
        class WriteTests;

    public:

        // Returns list of Buffer unit tests.
        static UTList GetTests( );
    };

    class BufferTests::ConstructorTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR DefaultConstructor( )
        {
            Buffer<T> defaultBuffer;

            SUTL_TEST_ASSERT(defaultBuffer.Get( ) == nullptr);
            SUTL_TEST_ASSERT(defaultBuffer.Length( ) == 0);
            SUTL_TEST_ASSERT(defaultBuffer.Size( ) == 0);
            SUTL_TEST_ASSERT(defaultBuffer.WritePosition( ) == 0);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR LengthConstructor( )
        {
            Buffer<T> buffer(GetTQNum<TQ>( ));

            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(buffer.Get( ) == nullptr);
                SUTL_TEST_ASSERT(buffer.Length( ) == 0);
                SUTL_TEST_ASSERT(buffer.Size( ) == 0);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.Get( ) != nullptr);
                SUTL_TEST_ASSERT(buffer.Length( ) == GetTQNum<TQ>( ));
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * GetTQNum<TQ>( ));
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR PointerCopyConstructorNull( )
        {
            Buffer<T>* pBuffer = nullptr;

            try
            {
                pBuffer = new Buffer<T>(nullptr, GetTQNum<TQ>( ));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            if ( pBuffer )
            {
                SUTL_TEST_ASSERT(pBuffer->Get( ) == nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->Size( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);
                delete pBuffer;
                pBuffer = nullptr;
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR PointerCopyConstructor( )
        {
            Buffer<T>* pBuffer = nullptr;
            std::vector<T> testData = GetTestData<T, TQ>( );

            try
            {
                pBuffer = new Buffer<T>(testData.data( ), testData.size( ));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(pBuffer);
            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(pBuffer->Get( ) == nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->Size( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(pBuffer->Get( ) != nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == testData.size( ));
                SUTL_TEST_ASSERT(pBuffer->Size( ) == sizeof(T) * testData.size( ));
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);

                size_t srcIdx = 0;
                for ( size_t i = 0; i < pBuffer->WritePosition( ); i++, srcIdx )
                {
                    if ( srcIdx == testData.size( ) )
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT((*pBuffer)[i] == testData[srcIdx]);
                    if constexpr ( std::is_same_v<T, Helper> )
                    {
                        SUTL_TEST_ASSERT((*pBuffer)[i].Copied( ));
                        SUTL_TEST_ASSERT(!(*pBuffer)[i].Moved( ));
                    }
                }
            }

            delete pBuffer;
            pBuffer = nullptr;

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR PointerMoveConstructorNull( )
        {
            Buffer<T>* pBuffer = nullptr;
            T* p = nullptr;

            try
            {
                pBuffer = new Buffer<T>(p, GetTQNum<TQ>( ));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            if ( pBuffer )
            {
                SUTL_TEST_ASSERT(pBuffer->Get( ) == nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->Size( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);

                delete pBuffer;
                pBuffer = nullptr;
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR PointerMoveConstructor( )
        {
            Buffer<T>* pBuffer = nullptr;
            std::vector<T> testData = GetTestData<T, TQ>( );

            constexpr const size_t len = GetTQNum<TQ>( );
            T* p = (len == 0) ? nullptr : (len == 1) ? new T : new T[len];
            if ( p )
            {
                memcpy(p, testData.data( ), sizeof(T) * len);
            }

            try
            {
                pBuffer = new Buffer<T>(p, len);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(pBuffer);
            SUTL_TEST_ASSERT(!p);

            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(pBuffer->Get( ) == nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->Size( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(pBuffer->Get( ) != nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == testData.size( ));
                SUTL_TEST_ASSERT(pBuffer->Size( ) == sizeof(T) * testData.size( ));
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);

                SUTL_TEST_ASSERT(memcmp(pBuffer->Get( ), testData.data( ), sizeof(T) * pBuffer->Length( )) == 0);
            }

            delete pBuffer;
            pBuffer = nullptr;

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR CopyConstructor( )
        {
            Buffer<T> srcBuffer(GetTQNum<TQ>( ));
            std::vector<T> testData = GetTestData<T, TQ>( );
            Buffer<T>* pBuffer = nullptr;

            SUTL_SETUP_ASSERT(srcBuffer.Length( ) == testData.size( ));
            if ( srcBuffer )
            {
                memcpy(srcBuffer.Get( ), testData.data( ), srcBuffer.Size( ));
            }

            try
            {
                pBuffer = new Buffer<T>(srcBuffer);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(pBuffer);
            SUTL_TEST_ASSERT(pBuffer->Length( ) == srcBuffer.Length( ));
            SUTL_TEST_ASSERT(pBuffer->Size( ) == srcBuffer.Size( ));
            SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == srcBuffer.WritePosition( ));

            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(pBuffer->Get( ) == nullptr);
                SUTL_TEST_ASSERT(pBuffer->Get( ) == srcBuffer.Get( ));
            }
            else
            {
                SUTL_TEST_ASSERT(srcBuffer);
                size_t srcIdx = 0;
                for ( size_t i = 0; i < pBuffer->WritePosition( ); i++, srcIdx )
                {
                    if ( srcIdx == srcBuffer.Length( ) )
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT((*pBuffer)[i] == srcBuffer[srcIdx]);
                    if constexpr ( std::is_same_v<T, Helper> )
                    {
                        SUTL_TEST_ASSERT((*pBuffer)[i].Copied( ));
                        SUTL_TEST_ASSERT(!(*pBuffer)[i].Moved( ));
                    }
                }
            }

            delete pBuffer;
            pBuffer = nullptr;

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR MoveConstructor( )
        {
            Buffer<T> srcBuffer(GetTQNum<TQ>( ));
            std::vector<T> testData = GetTestData<T, TQ>( );
            Buffer<T>* pBuffer = nullptr;

            const T* srcPtr = srcBuffer.Get( );
            const size_t srcLen = srcBuffer.Length( );
            const size_t srcSize = srcBuffer.Size( );
            const size_t srcWritePos = srcBuffer.WritePosition( );

            SUTL_SETUP_ASSERT(srcLen == testData.size( ));
            if ( srcBuffer )
            {
                memcpy(srcBuffer.Get( ), testData.data( ), srcBuffer.Size( ));
            }

            try
            {
                pBuffer = new Buffer<T>(std::move(srcBuffer));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(srcBuffer.Get( ) == nullptr);
            SUTL_TEST_ASSERT(srcBuffer.Length( ) == 0);
            SUTL_TEST_ASSERT(srcBuffer.Size( ) == 0);
            SUTL_TEST_ASSERT(srcBuffer.WritePosition( ) == 0);

            SUTL_TEST_ASSERT(pBuffer->Get( ) == srcPtr);
            SUTL_TEST_ASSERT(pBuffer->Length( ) == srcLen);
            SUTL_TEST_ASSERT(pBuffer->Size( ) == srcSize);
            SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == srcWritePos);

            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(pBuffer->Get( ) == nullptr);
            }
            else
            {
                SUTL_TEST_ASSERT(srcPtr);
                SUTL_TEST_ASSERT(memcmp(pBuffer->Get( ), srcPtr, srcSize) == 0);
            }

            delete pBuffer;
            pBuffer = nullptr;

            SUTL_TEST_SUCCESS( );
        }
    };

    class BufferTests::CopyTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR CopyAllBufferDataMembers( )
        {
            constexpr const size_t len = GetTQNum<TQ>( );
            Buffer<T> buffer(len);
            Buffer<T> dst;

            Buffer<T>::CopyAllBufferDataMembers(dst, buffer);

            SUTL_TEST_ASSERT(buffer.Get( ) == dst.Get( ));
            SUTL_TEST_ASSERT(buffer.Length( ) == dst.Length( ));
            SUTL_TEST_ASSERT(buffer.Size( ) == dst.Size( ));
            SUTL_TEST_ASSERT(buffer.WritePosition( ) == dst.WritePosition( ));

            dst.Reset( );

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR CopyNonPointerBufferDataMembers( )
        {
            constexpr const size_t len = GetTQNum<TQ>( );
            Buffer<T> buffer(len);
            Buffer<T> dst;

            Buffer<T>::CopyNonPointerBufferDataMembers(dst, buffer);

            SUTL_TEST_ASSERT(!!buffer == (buffer.Get( ) != dst.Get( )));
            SUTL_TEST_ASSERT(buffer.Length( ) == dst.Length( ));
            SUTL_TEST_ASSERT(buffer.Size( ) == dst.Size( ));
            SUTL_TEST_ASSERT(buffer.WritePosition( ) == dst.WritePosition( ));

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR CopyBuffer( )
        {
            constexpr const size_t len = GetTQNum<TQ>( );
            Buffer<T> buffer(GetTestData<T, TQ>( ).data( ), len);
            Buffer<T> dst;

            try
            {
                Buffer<T>::CopyBuffer(dst, buffer);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(buffer.Length( ) == dst.Length( ));
            SUTL_TEST_ASSERT(buffer.Size( ) == dst.Size( ));
            SUTL_TEST_ASSERT(buffer.WritePosition( ) == dst.WritePosition( ));

            if ( buffer )
            {
                const T* pRawBufPtr = buffer.Get( );
                const T* pRawDstPtr = dst.Get( );
                SUTL_TEST_ASSERT(!!pRawBufPtr);
                SUTL_TEST_ASSERT(!!pRawDstPtr);
                SUTL_TEST_ASSERT(pRawBufPtr != pRawDstPtr);
                for ( size_t i = 0; i < dst.Length( ); i++ )
                {
                    SUTL_TEST_ASSERT(pRawBufPtr[i] == pRawDstPtr[i]);
                }
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.Get( ) == dst.Get( ));
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR CopyAssignment( )
        {
            constexpr const size_t len = GetTQNum<TQ>( );            
            Buffer<T> buffer(GetTestData<T, TQ>( ).data( ), len);
            Buffer<T> dst;

            try
            {
                dst = buffer;
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(dst.Length( ) == len);
            SUTL_TEST_ASSERT(buffer.Length( ) == dst.Length( ));
            SUTL_TEST_ASSERT(buffer.Size( ) == dst.Size( ));
            SUTL_TEST_ASSERT(buffer.WritePosition( ) == dst.WritePosition( ));

            if ( buffer )
            {
                const T* pRawBufPtr = buffer.Get( );
                const T* pRawDstPtr = dst.Get( );
                SUTL_TEST_ASSERT(!!pRawBufPtr);
                SUTL_TEST_ASSERT(!!pRawDstPtr);
                SUTL_TEST_ASSERT(pRawBufPtr != pRawDstPtr);
                for ( size_t i = 0; i < dst.Length( ); i++ )
                {
                    SUTL_TEST_ASSERT(pRawBufPtr[i] == pRawDstPtr[i]);
                }
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.Get( ) == dst.Get( ));
            }

            SUTL_TEST_SUCCESS( );
        }
    };

    class BufferTests::MoveTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR TransferBuffer( )
        {
            Buffer<T> buffer(GetTestData<T, TQ>( ).data( ), GetTQNum<TQ>( ));
            Buffer<T> dst;

            const T* ptr = buffer.Get( );
            const size_t len = buffer.Length( );
            const size_t size = buffer.Size( );
            const size_t pos = buffer.WritePosition( );

            try
            {
                Buffer<T>::TransferBuffer(dst, buffer);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(dst.Get( ) == ptr);
            SUTL_TEST_ASSERT(dst.Length( ) == len);
            SUTL_TEST_ASSERT(dst.Size( ) == size);
            SUTL_TEST_ASSERT(dst.WritePosition( ) == pos);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR MoveAssignment( )
        {
            Buffer<T> buffer(GetTestData<T, TQ>( ).data( ), GetTQNum<TQ>( ));
            Buffer<T> dst;

            const T* ptr = buffer.Get( );
            const size_t len = buffer.Length( );
            const size_t size = buffer.Size( );
            const size_t pos = buffer.WritePosition( );

            try
            {
                dst = std::move(buffer);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(dst.Get( ) == ptr);
            SUTL_TEST_ASSERT(dst.Length( ) == len);
            SUTL_TEST_ASSERT(dst.Size( ) == size);
            SUTL_TEST_ASSERT(dst.WritePosition( ) == pos);

            SUTL_TEST_SUCCESS( );
        }
    };

    class BufferTests::SubscriptOperatorTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR SubscriptOperator( )
        {
            constexpr const size_t len = GetTQNum<TQ>( );
            bool bThrew = false;
            std::vector<T> testData(GetTestData<T, TQ>( ));
            Buffer<T> buffer(testData.data( ), len);

            SUTL_TEST_ASSERT(buffer.Length( ) == testData.size( ));

            if constexpr ( len > 0 )
            {
                try
                {
                    for ( size_t i = 0; i < len; i++ )
                    {
                        SUTL_TEST_ASSERT(buffer[i] == testData[i]);
                    }
                }
                catch ( const std::exception& e )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }

            try
            {
                static_cast<void>(buffer[len]);
            }
            catch ( const std::out_of_range& )
            {
                bThrew = true;
            }
            catch ( const std::logic_error& e )
            {
                if ( len == 0 )
                {
                    bThrew = true;
                }
                else
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrew);

            SUTL_TEST_SUCCESS( );
        }
    };

    class BufferTests::DereferenceOperatorTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR DereferenceOperator( )
        {
            constexpr const size_t len = GetTQNum<TQ>( );
            bool bThrew = false;
            std::vector<T> testData(GetTestData<T, TQ>( ));
            Buffer<T> buffer(testData.data( ), len);

            const T& firstElem = testData.empty( ) ? T( ) : testData.front( );
            SUTL_TEST_ASSERT(buffer.Length( ) == testData.size( ));

            try
            {
                SUTL_TEST_ASSERT(*buffer == firstElem);
            }
            catch ( const std::logic_error& e )
            {
                if ( len == 0 )
                {
                    bThrew = true;
                }
                else
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrew == (len == 0));

            SUTL_TEST_SUCCESS( );
        }
    };

    class BufferTests::ComparisonTests
    {
    public:

        template <typename T, TestQuantity TQ1, TestQuantity TQ2>
        static UTR ComparisonPtrNull( )
        {
            constexpr const size_t len1 = GetTQNum<TQ1>( );
            constexpr const size_t len2 = GetTQNum<TQ2>( );
            constexpr const bool bMatchExpected = (len1 == 0);

            Buffer<T> buffer(len1);

            try
            {
                SUTL_TEST_ASSERT(bMatchExpected == buffer.Compare(nullptr, len2));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2>
        static UTR ComparisonPtr( )
        {
            constexpr const size_t len1 = GetTQNum<TQ1>( );
            constexpr const size_t len2 = GetTQNum<TQ2>( );
            constexpr const bool bBothNull = (len1 == 0 && len2 == 0);
            constexpr const bool bNeitherNull = (len1 != 0 && len2 != 0);
            constexpr const bool bMatchExpected = (bBothNull || (bNeitherNull && len1 >= len2));

            Buffer<T> buffer(GetTestData<T, TQ1>( ).data( ), len1);
            std::vector<T> testData(GetTestData<T, TQ2>( ));

            try
            {
                SUTL_TEST_ASSERT(bMatchExpected == buffer.Compare(testData.data( ), len2));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            if constexpr ( bMatchExpected && bNeitherNull )
            {
                T val;
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    val.m_u64 = std::numeric_limits<uint64_t>::max( );
                }
                else
                {
                    val = std::numeric_limits<T>::max( );
                }

                if constexpr ( len1 == 1 )
                {
                    testData[0] = val;
                }
                else
                {
                    testData[len2 / 2] = val;
                }

                try
                {
                    SUTL_TEST_ASSERT(!buffer.Compare(testData.data( ), len2));
                }
                catch ( const std::exception& e )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2>
        static UTR Comparison( )
        {
            constexpr const size_t len1 = GetTQNum<TQ1>( );
            constexpr const size_t len2 = GetTQNum<TQ2>( );
            constexpr const bool bBothNull = (len1 == 0 && len2 == 0);
            constexpr const bool bNeitherNull = (len1 != 0 && len2 != 0);
            constexpr const bool bMatchExpected = (bBothNull || (bNeitherNull && len1 >= len2));

            Buffer<T> buffer1(GetTestData<T, TQ1>( ).data( ), len1);
            Buffer<T> buffer2(GetTestData<T, TQ2>( ).data( ), len2);

            try
            {
                SUTL_TEST_ASSERT(bMatchExpected == buffer1.Compare(buffer2));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            if constexpr ( bMatchExpected && bNeitherNull )
            {
                T val;
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    val.m_u64 = std::numeric_limits<uint64_t>::max( );
                }
                else
                {
                    val = std::numeric_limits<T>::max( );
                }

                if constexpr ( len1 == 1 )
                {
                    buffer2[0] = val;
                }
                else
                {
                    buffer2[len2 / 2] = val;
                }

                try
                {
                    SUTL_TEST_ASSERT(!buffer1.Compare(buffer2));
                }
                catch ( const std::exception& e )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2>
        static UTR ComparisonOperator( )
        {
            constexpr const size_t len1 = GetTQNum<TQ1>( );
            constexpr const size_t len2 = GetTQNum<TQ2>( );
            constexpr const bool bBothNull = (len1 == 0 && len2 == 0);
            constexpr const bool bNeitherNull = (len1 != 0 && len2 != 0);
            constexpr const bool bMatchExpected = (bBothNull || (bNeitherNull && len1 >= len2));

            Buffer<T> buffer1(GetTestData<T, TQ1>( ).data( ), len1);
            Buffer<T> buffer2(GetTestData<T, TQ2>( ).data( ), len2);

            try
            {
                SUTL_TEST_ASSERT(bMatchExpected == (buffer1 == buffer2));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            if constexpr ( bMatchExpected && bNeitherNull )
            {
                T val;
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    val.m_u64 = std::numeric_limits<uint64_t>::max( );
                }
                else
                {
                    val = std::numeric_limits<T>::max( );
                }

                if constexpr ( len1 == 1 )
                {
                    buffer2[0] = val;
                }
                else
                {
                    buffer2[len2 / 2] = val;
                }

                try
                {
                    SUTL_TEST_ASSERT(buffer1 != buffer2);
                }
                catch ( const std::exception& e )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }

            SUTL_TEST_SUCCESS( );
        }
    };

    class BufferTests::SetWritePositionTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR SetWritePosition( )
        {
            constexpr const size_t len = GetTQNum<TQ>( );

            Buffer<T> buffer(len);

            try
            {
                SUTL_TEST_ASSERT(buffer.SetWritePosition(0));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            try
            {
                SUTL_TEST_ASSERT(buffer.SetWritePosition(len >> 2));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            try
            {
                SUTL_TEST_ASSERT(buffer.SetWritePosition(len >> 1));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            try
            {
                SUTL_TEST_ASSERT(buffer.SetWritePosition(len));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            try
            {
                SUTL_TEST_ASSERT(buffer.SetWritePosition(len + 1) == false);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_SUCCESS( );
        }
    };

    class BufferTests::WriteTests
    {
    public:

        template <typename T, TestQuantity TQ, bool Move>
        static UTR WriteElement( )
        {
            constexpr const size_t len = GetTQNum<TQ>( );
            constexpr const bool bFirstWriteResult = (len == 0) ? false : true;
            constexpr const bool bSecondWriteResult = (len <= 1) ? false : true;

            std::vector<T> testData(GetTestData<T, TestQuantity::Many>( ));
            Buffer<T> buffer(len);

            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(buffer.Write(std::move(testData[0])) == bFirstWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Write(testData[0]) == bFirstWriteResult);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(buffer.Write(std::move(testData[1])) == bSecondWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Write(testData[1]) == bSecondWriteResult);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            if constexpr ( len == 0 )
            {
                SUTL_TEST_ASSERT(buffer.Get( ) == nullptr);
                SUTL_TEST_ASSERT(buffer.Length( ) == 0);
                SUTL_TEST_ASSERT(buffer.Size( ) == 0);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == ((len == 1) ? 1 : 2));
                SUTL_TEST_ASSERT(buffer.Length( ) == len);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len);
            }

            try
            {
                size_t srcIdx = 0;
                for ( size_t i = 0; i < buffer.WritePosition( ); i++, srcIdx++ )
                {
                    if ( srcIdx == testData.size( ) )
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT(buffer[i] == testData[srcIdx]);
                    if constexpr ( std::is_same_v<T, Helper> )
                    {
                        SUTL_TEST_ASSERT(buffer[i].Copied( ) != Move);
                        SUTL_TEST_ASSERT(buffer[i].Moved( ) == Move);
                    }
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2>
        static UTR WritePtrNull( )
        {
            constexpr const size_t len = GetTQNum<TQ1>( );
            Buffer<T> buffer(len);

            try
            {
                SUTL_TEST_ASSERT(buffer.Write(nullptr, GetTQNum<TQ2>( )) == false);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2, bool Move>
        static UTR WritePtr( )
        {
            constexpr const size_t len1 = GetTQNum<TQ1>( );
            constexpr const size_t len2 = GetTQNum<TQ2>( );
            constexpr const bool bNullBuffer = (len1 == 0);
            constexpr const bool bNullSource = (len2 == 0);
            constexpr const bool bFirstWriteResult = (bNullBuffer || bNullSource || len2 > len1) ? false : true;
            constexpr const bool bSecondWriteResult = (bNullBuffer || bNullSource || (2 * len2) > len1) ? false : true;

            static_assert(((bFirstWriteResult == false) && (bSecondWriteResult == true)) != true, __FUNCSIG__": Invalid expected-write-result configuration logic.");

            std::vector<T> testData(GetTestData<T, TQ2>( ));
            T* pData = testData.data( );
            const size_t dataLen = testData.size( );

            Buffer<T> buffer(len1);

            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(buffer.Write(std::move(pData), dataLen) == bFirstWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Write(pData, dataLen) == bFirstWriteResult);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bNullBuffer == !buffer.Get( ));
            SUTL_TEST_ASSERT(buffer.Length( ) == len1);
            SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);

            if constexpr ( bFirstWriteResult == true )
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == testData.size( ));
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
            }

            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(buffer.Write(std::move(pData), dataLen) == bSecondWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Write(pData, dataLen) == bSecondWriteResult);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bNullBuffer == !buffer.Get( ));
            SUTL_TEST_ASSERT(buffer.Length( ) == len1);
            SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);

            if constexpr ( bFirstWriteResult == false )
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
            }
            else if constexpr ( bSecondWriteResult == false )
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == testData.size( ));
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == (2 * testData.size( )));
            }

            try
            {
                size_t srcIdx = 0;
                for ( size_t i = 0; i < buffer.WritePosition( ); i++, srcIdx++ )
                {
                    if ( srcIdx == testData.size( ) )
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT(buffer[i] == testData[srcIdx]);
                    if constexpr ( std::is_same_v<T, Helper> )
                    {
                        SUTL_TEST_ASSERT(buffer[i].Copied( ) == !Move);
                        SUTL_TEST_ASSERT(buffer[i].Moved( ) == Move);
                    }
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2, bool Move>
        static UTR WriteToWritePosition( )
        {
            constexpr const size_t len1 = GetTQNum<TQ1>( );
            constexpr const size_t len2 = GetTQNum<TQ2>( );
            constexpr const size_t writeLen = (len2 == 1) ? len2 : len2 >> 1;
            constexpr const bool bNullBuffer = (len1 == 0);
            constexpr const bool bNullSource = (len2 == 0);

            constexpr const bool bSetupWriteResult = (bNullSource) ? false : true;
            constexpr const bool bFirstWriteResult = ((bSetupWriteResult == false) || bNullBuffer || writeLen > len1) ? false : true;
            constexpr const bool bSecondWriteResult = ((bSetupWriteResult == false) || bNullBuffer || (2 * writeLen) > len1) ? false : true;

            static_assert(((bFirstWriteResult == false) && (bSecondWriteResult == true)) != true, __FUNCSIG__": Invalid expected-write-result configuration logic.");

            const std::vector<T> testData(GetTestData<T, TQ2>( ));
            Buffer<T> buffer(len1);
            Buffer<T> src(len2);

            try
            {
                SUTL_TEST_ASSERT(src.Write(testData.data( ), writeLen) == bSetupWriteResult);
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bNullSource == !src.Get( ));
            SUTL_TEST_ASSERT(src.Length( ) == len2);
            SUTL_TEST_ASSERT(src.Size( ) == sizeof(T) * len2);
            SUTL_TEST_ASSERT(src.WritePosition( ) == ((bSetupWriteResult == true) ? writeLen : 0));

            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(buffer.Write(std::move(src), true) == bFirstWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Write(src, true) == bFirstWriteResult);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bNullSource == !src.Get( ));
            SUTL_TEST_ASSERT(src.Length( ) == len2);
            SUTL_TEST_ASSERT(src.Size( ) == sizeof(T) * len2);
            SUTL_TEST_ASSERT(src.WritePosition( ) == ((bSetupWriteResult == true) ? writeLen : 0));

            SUTL_TEST_ASSERT(bNullBuffer == !buffer.Get( ));
            SUTL_TEST_ASSERT(buffer.Length( ) == len1);
            SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);

            if constexpr ( bFirstWriteResult == false )
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == writeLen);
            }

            try
            {                
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(buffer.Write(std::move(src), true) == bSecondWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Write(src, true) == bSecondWriteResult);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bNullSource == !src.Get( ));
            SUTL_TEST_ASSERT(src.Length( ) == len2);
            SUTL_TEST_ASSERT(src.Size( ) == sizeof(T) * len2);
            SUTL_TEST_ASSERT(src.WritePosition( ) == ((bSetupWriteResult == true) ? writeLen : 0));

            SUTL_TEST_ASSERT(bNullBuffer == !buffer.Get( ));
            SUTL_TEST_ASSERT(buffer.Length( ) == len1);
            SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);

            if constexpr ( bFirstWriteResult == false )
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
            }
            else if ( bSecondWriteResult == false )
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == writeLen);
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == (writeLen * 2));
            }

            try
            {
                size_t srcIdx = 0;
                for ( size_t i = 0; i < buffer.WritePosition( ); i++, srcIdx++ )
                {
                    if ( srcIdx == src.WritePosition( ) )
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT(buffer[i] == src[srcIdx]);
                    if constexpr ( std::is_same_v<T, Helper> )
                    {
                        SUTL_TEST_ASSERT(buffer[i].Copied( ) == !Move);
                        SUTL_TEST_ASSERT(buffer[i].Moved( ) == Move);
                    }
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2, bool Move>
        static UTR WriteToEnd( )
        {
            constexpr const size_t len1 = GetTQNum<TQ1>( );
            constexpr const size_t len2 = GetTQNum<TQ2>( );
            constexpr const bool bNullBuffer = (len1 == 0);
            constexpr const bool bNullSource = (len2 == 0);

            constexpr const bool bSetupWriteResult = (bNullSource) ? false : true;
            constexpr const bool bFirstWriteResult = ((bSetupWriteResult == false) || bNullBuffer || len2 > len1) ? false : true;
            constexpr const bool bSecondWriteResult = ((bSetupWriteResult == false) || bNullBuffer || (2 * len2) > len1) ? false : true;

            const std::vector<T> testData(GetTestData<T, TQ2>( ));
            Buffer<T> buffer(len1);
            Buffer<T> src(len2);

            try
            {
                SUTL_TEST_ASSERT(src.Write(testData.data( ), testData.size( )) == bSetupWriteResult);
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bNullSource == !src.Get( ));
            SUTL_TEST_ASSERT(src.Length( ) == len2);
            SUTL_TEST_ASSERT(src.Size( ) == sizeof(T) * len2);
            SUTL_TEST_ASSERT(src.WritePosition( ) == ((bSetupWriteResult == true) ? len2 : 0));

            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(buffer.Write(std::move(src), false) == bFirstWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Write(src, false) == bFirstWriteResult);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bNullSource == !src.Get( ));
            SUTL_TEST_ASSERT(src.Length( ) == len2);
            SUTL_TEST_ASSERT(src.Size( ) == sizeof(T) * len2);
            SUTL_TEST_ASSERT(src.WritePosition( ) == ((bSetupWriteResult == true) ? len2 : 0));

            SUTL_TEST_ASSERT(bNullBuffer == !buffer.Get( ));
            SUTL_TEST_ASSERT(buffer.Length( ) == len1);
            SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);

            if constexpr ( bFirstWriteResult == false )
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == testData.size( ));
            }

            try
            {
                if constexpr ( Move )
                {
                    SUTL_TEST_ASSERT(buffer.Write(std::move(src), false) == bSecondWriteResult);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Write(src, false) == bSecondWriteResult);
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bNullSource == !src.Get( ));
            SUTL_TEST_ASSERT(src.Length( ) == len2);
            SUTL_TEST_ASSERT(src.Size( ) == sizeof(T) * len2);
            SUTL_TEST_ASSERT(src.WritePosition( ) == ((bSetupWriteResult == true) ? len2 : 0));

            SUTL_TEST_ASSERT(bNullBuffer == !buffer.Get( ));
            SUTL_TEST_ASSERT(buffer.Length( ) == len1);
            SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);

            if constexpr ( bFirstWriteResult == false )
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
            }
            else if constexpr ( bSecondWriteResult == false )
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == testData.size( ));
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == (2 * testData.size( )));
            }

            try
            {
                size_t srcIdx = 0;
                for ( size_t i = 0; i < buffer.WritePosition( ); i++, srcIdx++ )
                {
                    if ( srcIdx == src.WritePosition( ) )
                    {
                        srcIdx = 0;
                    }

                    SUTL_TEST_ASSERT(buffer[i] == src[srcIdx]);
                    if constexpr ( std::is_same_v<T, Helper> )
                    {
                        SUTL_TEST_ASSERT(buffer[i].Copied( ) == !Move);
                        SUTL_TEST_ASSERT(buffer[i].Moved( ) == Move);
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
