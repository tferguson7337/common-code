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

        struct Helper
        {
            static const size_t s_ArrSize = 21;

            uint8_t m_u8;
            uint16_t m_u16;
            uint32_t m_u32;
            uint64_t m_u64;
            uint8_t m_u8Arr[s_ArrSize];

            const bool operator==(const Helper& rhs) const noexcept
            {
                return m_u8 == rhs.m_u8
                    && m_u16 == rhs.m_u16
                    && m_u32 == rhs.m_u32
                    && m_u64 == rhs.m_u64
                    && memcmp(m_u8Arr, rhs.m_u8Arr, s_ArrSize) == 0;
            }

            const bool operator!=(const Helper& rhs) const noexcept
            {
                return !operator==(rhs);
            }
        };

        template <typename T>
        static constexpr bool IsValidTestType( )
        {
            return std::is_same_v<T, uint8_t>
                || std::is_same_v<T, uint16_t>
                || std::is_same_v<T, uint32_t>
                || std::is_same_v<T, uint64_t>
                || std::is_same_v<T, Helper>;
        }

        enum class TestQuantity : size_t
        {
            Zero = 0,
            One,
            Many,

            _End,
            _Begin = 0
        };

        template <TestQuantity TQ>
        static constexpr bool IsValidTestQuantity( )
        {
            return TQ >= TestQuantity::_Begin && TQ < TestQuantity::_End;
        }

        template <TestQuantity TQ>
        static constexpr const size_t GetTQNum( )
        {
            static_assert(IsValidTestQuantity<TQ>( ), __FUNCSIG__": Invalid TestQuantity Template Argument.");

            if constexpr ( TQ == TestQuantity::Zero )
            {
                return 0;
            }
            else if constexpr ( TQ == TestQuantity::One )
            {
                return 1;
            }
            else
            {
                return 128;
            }
        }

        template <typename T, TestQuantity TQ>
        static std::vector<T> GetTestData( )
        {
            static_assert(IsValidTestType<T>( ), __FUNCSIG__": Invalid Template Type Argument");
            static_assert(IsValidTestQuantity<TQ>( ), __FUNCSIG__": Invalid TestQuantity Template Argument.");

            std::vector<T> testData;

            if constexpr ( TQ != TestQuantity::Zero )
            {
                for ( size_t i = 0; i < GetTQNum<TQ>( ); i++ )
                {
                    if constexpr ( !std::is_same_v<T, Helper> )
                    {
                        testData.push_back(static_cast<T>(i));
                    }
                    else
                    {
                        size_t val = i;
                        Helper h;
                        h.m_u8 = static_cast<uint8_t>(val++);
                        h.m_u16 = static_cast<uint16_t>(val++);
                        h.m_u32 = static_cast<uint32_t>(val++);
                        h.m_u64 = static_cast<uint64_t>(val++);
                        for ( size_t j = 0; j < Helper::s_ArrSize; j++ )
                        {
                            h.m_u8Arr[j] = static_cast<uint8_t>(val++);
                        }

                        testData.push_back(h);
                    }
                }
            }

            return testData;
        }

    public:

        // Type aliases
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;

        // Returns list of Buffer unit tests.
        static UTList GetTests( );

        /// Test Subclasses \\\

        // Tests methods related to allocating memory.
        class AllocationTests;

        // Tests methods related to freeing memory.
        class DeallocationTests;

        // Tests constructor behavior.
        class ConstructorTests;

        // Tests op-overloads and other methods related to copying.
        class CopyTests;

        // Tests op-overloads and other methods related to moving.
        class MoveTests;

        // Tests behavior of the overloaded subscript operator methods.
        class SubscriptOperatorTests;

        // Tests behavior of the overloaded subscript operator methods.
        class DereferenceOperatorTests;

        // Tests comparison behavior between buffers.
        class ComparisonTests;

        // Tests SetWritePosition behavior.
        class SetWritePositionTests;

        // Tests behavior of the methods that write to the buffer.
        class WriteTests;
    };

    class BufferTests::AllocationTests
    {
    private:

        template <typename T, TestQuantity TQ>
        struct Ptr
        {
            T* ptr;

            Ptr( ) :
                ptr(nullptr)
            { }

            ~Ptr( )
            {
                Free( );
            }

            void Free( )
            {
                if constexpr ( TQ == TestQuantity::Zero )
                {
                    // no op
                }
                else if constexpr ( TQ == TestQuantity::One )
                {
                    delete ptr;
                }
                else
                {
                    delete[ ] ptr;
                }
            }

            explicit operator bool( ) const noexcept
            {
                return ptr != nullptr;
            }

            Ptr& operator=(T* p)
            {
                Free( );
                ptr = p;
                return *this;
            }
        };

    public:
        template <typename T, TestQuantity TQ>
        static UTR Allocate( )
        {
            constexpr const bool bZeroCase = (TQ == TestQuantity::Zero);
            Ptr<T, TQ> p;

            try
            {
                p = Buffer<T>::Allocate(GetTQNum<TQ>( ));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bZeroCase == !p);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR AllocFromPtrNull( )
        {
            constexpr const bool bZeroCase = (TQ == TestQuantity::Zero);
            bool threw = false;
            Ptr<T, TQ> p;

            try
            {
                p = Buffer<T>::AllocateFromPointer(nullptr, GetTQNum<TQ>( ));
            }
            catch ( const std::invalid_argument& e )
            {
                if ( !bZeroCase )
                {
                    threw = true;
                }
                else
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }

            SUTL_TEST_ASSERT(threw != bZeroCase);
            SUTL_TEST_ASSERT(!p);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR AllocFromPtr( )
        {
            constexpr const bool bZeroCase = (TQ == TestQuantity::Zero);
            constexpr TestQuantity dataTQ = bZeroCase ? TestQuantity::One : TQ;
            const std::vector<T> testData = GetTestData<T, dataTQ>( );
            bool threw = false;
            Ptr<T, TQ> p;

            try
            {
                p = Buffer<T>::AllocateFromPointer(testData.data( ), GetTQNum<TQ>( ));
            }
            catch ( const std::invalid_argument& e )
            {
                if ( bZeroCase )
                {
                    threw = true;
                }
                else
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }

            SUTL_TEST_ASSERT(threw == bZeroCase);
            SUTL_TEST_ASSERT(threw || p);

            SUTL_TEST_SUCCESS( );
        }
    };

    class BufferTests::DeallocationTests
    {
    private:

        static size_t& SingleFreeCounter( )
        {
            static size_t freeCounter = 0;
            return freeCounter;
        }

        static size_t& ArrayFreeCounter( )
        {
            static size_t freeCounter = 0;
            return freeCounter;
        }

        static void ResetFreeCounters( )
        {
            SingleFreeCounter( ) = 0;
            ArrayFreeCounter( ) = 0;
        }

        template <typename T>
        static void FreeSingle(T* p)
        {
            SingleFreeCounter( )++;
            Buffer<T>::FreeSingle(p);
        }

        template <typename T>
        static void FreeArray(T* p)
        {
            ArrayFreeCounter( )++;
            Buffer<T>::FreeArray(p);
        }

        template <typename T, TestQuantity TQ>
        static void AssignFreeWrapper(Buffer<T>& buffer)
        {
            static_assert(IsValidTestType<T>( ), __FUNCSIG__": Invalid Template Type Argument");
            static_assert(IsValidTestQuantity<TQ>( ), __FUNCSIG__": Invalid TestQuantity Template Argument.");

            if constexpr ( TQ == TestQuantity::Zero )
            {
                buffer.m_FreeFunc = nullptr;
            }
            else if constexpr ( TQ == TestQuantity::One )
            {
                buffer.m_FreeFunc = FreeSingle<T>;
            }
            else if constexpr ( TQ == TestQuantity::Many )
            {
                buffer.m_FreeFunc = FreeArray<T>;
            }
        }

    public:

        template <typename T, TestQuantity TQ>
        static UTR CopyBuffer( )
        {
            constexpr size_t expectedSingleFreeCount = (TQ == TestQuantity::One) ? 1 : 0;
            constexpr size_t expectedArrayFreeCount = (TQ == TestQuantity::Many) ? 1 : 0;

            Buffer<T> buffer(GetTQNum<TQ>( ));
            Buffer<T> copyBuffer(GetTQNum<TQ>( ));

            ResetFreeCounters( );
            AssignFreeWrapper<T, TQ>(buffer);

            try
            {
                Buffer<T>::CopyBuffer(buffer, copyBuffer);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(SingleFreeCounter( ) == expectedSingleFreeCount);
            SUTL_TEST_ASSERT(ArrayFreeCounter( ) == expectedArrayFreeCount);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR TransferBuffer( )
        {
            constexpr size_t expectedSingleFreeCount = (TQ == TestQuantity::One) ? 1 : 0;
            constexpr size_t expectedArrayFreeCount = (TQ == TestQuantity::Many) ? 1 : 0;

            Buffer<T> buffer(GetTQNum<TQ>( ));
            Buffer<T> transferBuffer(GetTQNum<TQ>( ));

            ResetFreeCounters( );
            AssignFreeWrapper<T, TQ>(buffer);

            try
            {
                Buffer<T>::TransferBuffer(buffer, transferBuffer);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(SingleFreeCounter( ) == expectedSingleFreeCount);
            SUTL_TEST_ASSERT(ArrayFreeCounter( ) == expectedArrayFreeCount);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR Destructor( )
        {
            constexpr size_t expectedSingleFreeCount = (TQ == TestQuantity::One) ? 1 : 0;
            constexpr size_t expectedArrayFreeCount = (TQ == TestQuantity::Many) ? 1 : 0;

            Buffer<T>* pBuffer = nullptr;

            try
            {
                pBuffer = new Buffer<T>(GetTQNum<TQ>( ));
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            ResetFreeCounters( );
            AssignFreeWrapper<T, TQ>(*pBuffer);

            delete pBuffer;
            pBuffer = nullptr;

            SUTL_TEST_ASSERT(SingleFreeCounter( ) == expectedSingleFreeCount);
            SUTL_TEST_ASSERT(ArrayFreeCounter( ) == expectedArrayFreeCount);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR Free( )
        {
            constexpr size_t expectedSingleFreeCount = (TQ == TestQuantity::One) ? 1 : 0;
            constexpr size_t expectedArrayFreeCount = (TQ == TestQuantity::Many) ? 1 : 0;

            Buffer<T> buffer(GetTQNum<TQ>( ));

            ResetFreeCounters( );
            AssignFreeWrapper<T, TQ>(buffer);

            buffer.Free( );

            SUTL_TEST_ASSERT(SingleFreeCounter( ) == expectedSingleFreeCount);
            SUTL_TEST_ASSERT(ArrayFreeCounter( ) == expectedArrayFreeCount);

            SUTL_TEST_SUCCESS( );
        }
    };

    class BufferTests::ConstructorTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR DefaultConstructor( )
        {
            Buffer<T> defaultBuffer;

            SUTL_TEST_ASSERT(defaultBuffer.Ptr( ) == nullptr);
            SUTL_TEST_ASSERT(defaultBuffer.Length( ) == 0);
            SUTL_TEST_ASSERT(defaultBuffer.Size( ) == 0);
            SUTL_TEST_ASSERT(defaultBuffer.WritePosition( ) == 0);
            SUTL_TEST_ASSERT(defaultBuffer.m_FreeFunc == nullptr);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR LengthConstructor( )
        {
            Buffer<T> buffer(GetTQNum<TQ>( ));

            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ) == nullptr);
                SUTL_TEST_ASSERT(buffer.Length( ) == 0);
                SUTL_TEST_ASSERT(buffer.Size( ) == 0);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == nullptr);
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ) != nullptr);
                SUTL_TEST_ASSERT(buffer.Length( ) == GetTQNum<TQ>( ));
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * GetTQNum<TQ>( ));
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                if constexpr ( TQ == TestQuantity::One )
                {
                    SUTL_TEST_ASSERT(buffer.m_FreeFunc == Buffer<T>::FreeSingle);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.m_FreeFunc == Buffer<T>::FreeArray);
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR PointerCopyConstructorNull( )
        {
            constexpr const bool bThrowExpected = (TQ != TestQuantity::Zero) ? true : false;

            bool bThrew = false;
            Buffer<T>* pBuffer = nullptr;

            try
            {
                pBuffer = new Buffer<T>(nullptr, GetTQNum<TQ>( ));
            }
            catch ( const std::invalid_argument& e )
            {
                bThrew = true;
                if ( !bThrowExpected )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrew == bThrowExpected);
            if ( pBuffer )
            {
                SUTL_TEST_ASSERT(pBuffer->Ptr( ) == nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->Size( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->m_FreeFunc == nullptr);
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
                SUTL_TEST_ASSERT(pBuffer->Ptr( ) == nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->Size( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->m_FreeFunc == nullptr);
            }
            else
            {
                SUTL_TEST_ASSERT(pBuffer->Ptr( ) != nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == testData.size( ));
                SUTL_TEST_ASSERT(pBuffer->Size( ) == sizeof(T) * testData.size( ));
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);
                if constexpr ( TQ == TestQuantity::One )
                {
                    SUTL_TEST_ASSERT(pBuffer->m_FreeFunc == Buffer<T>::FreeSingle);
                }
                else
                {
                    SUTL_TEST_ASSERT(pBuffer->m_FreeFunc == Buffer<T>::FreeArray);
                }

                SUTL_TEST_ASSERT(memcmp(pBuffer->Ptr( ), testData.data( ), sizeof(T) * pBuffer->Length( )) == 0);
            }

            delete pBuffer;
            pBuffer = nullptr;

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR PointerMoveConstructorNull( )
        {
            constexpr const bool bThrowExpected = (TQ != TestQuantity::Zero) ? true : false;

            bool bThrew = false;
            Buffer<T>* pBuffer = nullptr;
            T* p = nullptr;

            try
            {
                pBuffer = new Buffer<T>(p, GetTQNum<TQ>( ));
            }
            catch ( const std::invalid_argument& e )
            {
                bThrew = true;
                if ( !bThrowExpected )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrew == bThrowExpected);
            if ( pBuffer )
            {
                SUTL_TEST_ASSERT(pBuffer->Ptr( ) == nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->Size( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->m_FreeFunc == nullptr);
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
                SUTL_TEST_ASSERT(pBuffer->Ptr( ) == nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->Size( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);
                SUTL_TEST_ASSERT(pBuffer->m_FreeFunc == nullptr);
            }
            else
            {
                SUTL_TEST_ASSERT(pBuffer->Ptr( ) != nullptr);
                SUTL_TEST_ASSERT(pBuffer->Length( ) == testData.size( ));
                SUTL_TEST_ASSERT(pBuffer->Size( ) == sizeof(T) * testData.size( ));
                SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == 0);
                if constexpr ( TQ == TestQuantity::One )
                {
                    SUTL_TEST_ASSERT(pBuffer->m_FreeFunc == Buffer<T>::FreeSingle);
                }
                else
                {
                    SUTL_TEST_ASSERT(pBuffer->m_FreeFunc == Buffer<T>::FreeArray);
                }

                SUTL_TEST_ASSERT(memcmp(pBuffer->Ptr( ), testData.data( ), sizeof(T) * pBuffer->Length( )) == 0);
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
                memcpy(srcBuffer.Ptr( ), testData.data( ), srcBuffer.Size( ));
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
            SUTL_TEST_ASSERT(pBuffer->m_FreeFunc == srcBuffer.m_FreeFunc);

            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(pBuffer->Ptr( ) == nullptr);
                SUTL_TEST_ASSERT(pBuffer->Ptr( ) == srcBuffer.Ptr( ));
            }
            else
            {
                SUTL_TEST_ASSERT(srcBuffer);
                SUTL_TEST_ASSERT(memcmp(pBuffer->Ptr( ), srcBuffer.Ptr( ), pBuffer->Size( )) == 0);
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

            const T* srcPtr = srcBuffer.Ptr( );
            const size_t srcLen = srcBuffer.Length( );
            const size_t srcSize = srcBuffer.Size( );
            const size_t srcWritePos = srcBuffer.WritePosition( );
            const Buffer<T>::FreeFunc srcFreeFunc = srcBuffer.m_FreeFunc;

            SUTL_SETUP_ASSERT(srcLen == testData.size( ));
            if ( srcBuffer )
            {
                memcpy(srcBuffer.Ptr( ), testData.data( ), srcBuffer.Size( ));
            }

            try
            {
                pBuffer = new Buffer<T>(std::move(srcBuffer));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(srcBuffer.Ptr( ) == nullptr);
            SUTL_TEST_ASSERT(srcBuffer.Length( ) == 0);
            SUTL_TEST_ASSERT(srcBuffer.Size( ) == 0);
            SUTL_TEST_ASSERT(srcBuffer.WritePosition( ) == 0);
            SUTL_TEST_ASSERT(srcBuffer.m_FreeFunc == nullptr);

            SUTL_TEST_ASSERT(pBuffer->Ptr( ) == srcPtr);
            SUTL_TEST_ASSERT(pBuffer->Length( ) == srcLen);
            SUTL_TEST_ASSERT(pBuffer->Size( ) == srcSize);
            SUTL_TEST_ASSERT(pBuffer->WritePosition( ) == srcWritePos);
            SUTL_TEST_ASSERT(pBuffer->m_FreeFunc == srcFreeFunc);

            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(pBuffer->Ptr( ) == nullptr);
            }
            else
            {
                SUTL_TEST_ASSERT(srcPtr);
                SUTL_TEST_ASSERT(memcmp(pBuffer->Ptr( ), srcPtr, srcSize) == 0);
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

            SUTL_TEST_ASSERT(buffer.Ptr( ) == dst.Ptr( ));
            SUTL_TEST_ASSERT(buffer.Length( ) == dst.Length( ));
            SUTL_TEST_ASSERT(buffer.Size( ) == dst.Size( ));
            SUTL_TEST_ASSERT(buffer.WritePosition( ) == dst.WritePosition( ));
            SUTL_TEST_ASSERT(buffer.m_FreeFunc == dst.m_FreeFunc);

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

            SUTL_TEST_ASSERT(!!buffer == (buffer.Ptr( ) != dst.Ptr( )));
            SUTL_TEST_ASSERT(buffer.Length( ) == dst.Length( ));
            SUTL_TEST_ASSERT(buffer.Size( ) == dst.Size( ));
            SUTL_TEST_ASSERT(buffer.WritePosition( ) == dst.WritePosition( ));
            SUTL_TEST_ASSERT(buffer.m_FreeFunc == dst.m_FreeFunc);

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
            SUTL_TEST_ASSERT(buffer.m_FreeFunc == dst.m_FreeFunc);
            if ( buffer )
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ) != dst.Ptr( ));
                SUTL_TEST_ASSERT(memcmp(buffer.Ptr( ), dst.Ptr( ), dst.Size( )) == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ) == dst.Ptr( ));
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

            SUTL_TEST_ASSERT(buffer.Length( ) == dst.Length( ));
            SUTL_TEST_ASSERT(buffer.Size( ) == dst.Size( ));
            SUTL_TEST_ASSERT(buffer.WritePosition( ) == dst.WritePosition( ));
            SUTL_TEST_ASSERT(buffer.m_FreeFunc == dst.m_FreeFunc);
            if ( buffer )
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ) != dst.Ptr( ));
                SUTL_TEST_ASSERT(memcmp(buffer.Ptr( ), dst.Ptr( ), dst.Size( )) == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ) == dst.Ptr( ));
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

            const T* ptr = buffer.Ptr( );
            const size_t len = buffer.Length( );
            const size_t size = buffer.Size( );
            const size_t pos = buffer.WritePosition( );
            const Buffer<T>::FreeFunc freeFunc = buffer.m_FreeFunc;

            try
            {
                Buffer<T>::TransferBuffer(dst, buffer);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(dst.Ptr( ) == ptr);
            SUTL_TEST_ASSERT(dst.Length( ) == len);
            SUTL_TEST_ASSERT(dst.Size( ) == size);
            SUTL_TEST_ASSERT(dst.WritePosition( ) == pos);
            SUTL_TEST_ASSERT(dst.m_FreeFunc == freeFunc);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR MoveAssignment( )
        {
            Buffer<T> buffer(GetTestData<T, TQ>( ).data( ), GetTQNum<TQ>( ));
            Buffer<T> dst;

            const T* ptr = buffer.Ptr( );
            const size_t len = buffer.Length( );
            const size_t size = buffer.Size( );
            const size_t pos = buffer.WritePosition( );
            const Buffer<T>::FreeFunc freeFunc = buffer.m_FreeFunc;

            try
            {
                dst = std::move(buffer);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(dst.Ptr( ) == ptr);
            SUTL_TEST_ASSERT(dst.Length( ) == len);
            SUTL_TEST_ASSERT(dst.Size( ) == size);
            SUTL_TEST_ASSERT(dst.WritePosition( ) == pos);
            SUTL_TEST_ASSERT(dst.m_FreeFunc == freeFunc);

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

            if ( len > 0 )
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
                buffer[len];
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
            constexpr const bool bThrowExpected = (len2 != 0);

            Buffer<T> buffer(len1);
            bool bThrew = false;

            try
            {
                SUTL_TEST_ASSERT(bMatchExpected == buffer.Compare(nullptr, len2));
            }
            catch ( const std::logic_error& e )
            {
                bThrew = true;
                if ( !bThrowExpected )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrew == bThrowExpected);

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
            bool bThrew = false;

            try
            {
                buffer.SetWritePosition(0);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            try
            {
                buffer.SetWritePosition(len >> 2);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            try
            {
                buffer.SetWritePosition(len >> 1);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            try
            {
                buffer.SetWritePosition(len);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            try
            {
                buffer.SetWritePosition(len + 1);
            }
            catch ( const std::out_of_range& )
            {
                bThrew = true;
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrew);

            SUTL_TEST_SUCCESS( );
        }
    };

    class BufferTests::WriteTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR WriteElement( )
        {
            constexpr const size_t len = GetTQNum<TQ>( );
            constexpr const bool bExpectDerefThrow = (len == 0);
            constexpr const bool bExpectSecondWriteThrow = (len == 1);

            std::vector<T> testData(GetTestData<T, TestQuantity::Many>( ));
            Buffer<T> buffer(len);
            bool bThrewLE = false;
            bool bThrewOOR = false;

            try
            {
                buffer.Write(testData[0]);
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if ( !bExpectDerefThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrewLE == bExpectDerefThrow);
            bThrewLE = false;

            try
            {
                buffer.Write(testData[1]);
            }
            catch ( const std::out_of_range& e )
            {
                bThrewOOR = true;
                if ( !bExpectSecondWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if ( !bExpectDerefThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrewOOR == bExpectSecondWriteThrow);
            SUTL_TEST_ASSERT(bThrewLE == bExpectDerefThrow);
            bThrewOOR = bThrewLE = false;

            if constexpr ( len == 0 )
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ) == nullptr);
                SUTL_TEST_ASSERT(buffer.Length( ) == 0);
                SUTL_TEST_ASSERT(buffer.Size( ) == 0);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == nullptr);
            }
            else
            {
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == ((len == 1) ? 1 : 2));
                SUTL_TEST_ASSERT(memcmp(buffer.Ptr( ), testData.data( ), sizeof(T) * buffer.WritePosition( )) == 0);
                SUTL_TEST_ASSERT(buffer.Length( ) == len);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len);
                if constexpr ( len == 1 )
                {
                    SUTL_TEST_ASSERT(buffer.m_FreeFunc == Buffer<T>::FreeSingle);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.m_FreeFunc == Buffer<T>::FreeArray);
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2>
        static UTR WritePtrNull( )
        {
            constexpr const bool bExpectThrow = (TQ2 != TestQuantity::Zero);
            constexpr const size_t len = GetTQNum<TQ1>( );
            Buffer<T> buffer(len);
            bool bThrew = false;

            try
            {
                buffer.Write(nullptr, GetTQNum<TQ2>( ));
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

        template <typename T, TestQuantity TQ1, TestQuantity TQ2>
        static UTR WritePtr( )
        {
            constexpr const size_t len1 = GetTQNum<TQ1>( );
            constexpr const size_t len2 = GetTQNum<TQ2>( );
            constexpr const bool bNullBuffer = (len1 == 0);
            constexpr const bool bNullSource = (len2 == 0);
            constexpr const bool bExpectDerefThrow = (bNullBuffer && !bNullSource);
            constexpr const bool bExpectFirstWriteThrow = (!bNullSource && !bExpectDerefThrow && len2 > len1);
            constexpr const bool bExpectSecondWriteThrow = (!bNullSource && !bExpectDerefThrow && (2 * len2) > len1);

            std::vector<T> testData(GetTestData<T, TQ2>( ));
            Buffer<T> buffer(len1);
            bool bThrewLE = false;
            bool bThrewOOR = false;

            try
            {
                buffer.Write(testData.data( ), testData.size( ));
            }
            catch ( const std::out_of_range& e )
            {
                bThrewOOR = true;
                if ( !bExpectFirstWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if ( !bExpectDerefThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrewOOR == bExpectFirstWriteThrow);
            SUTL_TEST_ASSERT(bThrewLE == bExpectDerefThrow);
            bThrewOOR = bThrewLE = false;

            if constexpr ( !bExpectFirstWriteThrow && !bExpectDerefThrow )
            {
                SUTL_TEST_ASSERT(bNullSource || buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == testData.size( ));
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 0) ? nullptr : (len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectFirstWriteThrow )
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectDerefThrow )
            {
                if constexpr ( bNullBuffer )
                {
                    SUTL_TEST_ASSERT(!buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == 0);
                    SUTL_TEST_ASSERT(buffer.Size( ) == 0);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(!buffer.m_FreeFunc);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                    SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
                }
            }

            try
            {
                buffer.Write(testData.data( ), testData.size( ));
            }
            catch ( const std::out_of_range& e )
            {
                bThrewOOR = true;
                if ( !bExpectSecondWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if ( !bExpectDerefThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrewOOR == bExpectSecondWriteThrow);
            SUTL_TEST_ASSERT(bThrewLE == bExpectDerefThrow);
            bThrewOOR = bThrewLE = false;

            if constexpr ( !bExpectSecondWriteThrow && !bExpectDerefThrow )
            {
                SUTL_TEST_ASSERT(bNullSource || buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == std::min(buffer.Length( ), testData.size( ) * 2));
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 0) ? nullptr : (len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectSecondWriteThrow )
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == (bExpectFirstWriteThrow ? 0 : std::min(buffer.Length( ), testData.size( ))));
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectDerefThrow )
            {
                if constexpr ( bNullBuffer )
                {
                    SUTL_TEST_ASSERT(!buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == 0);
                    SUTL_TEST_ASSERT(buffer.Size( ) == 0);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(!buffer.m_FreeFunc);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                    SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2>
        static UTR WriteToWritePosition( )
        {
            constexpr const size_t len1 = GetTQNum<TQ1>( );
            constexpr const size_t len2 = GetTQNum<TQ2>( );
            constexpr const size_t writeLen = (len2 == 1) ? len2 : len2 >> 1;
            constexpr const bool bNullBuffer = (len1 == 0);
            constexpr const bool bNullSource = (len2 == 0);
            constexpr const bool bExpectSetupDerefThrow = (bNullSource || writeLen == 0);
            constexpr const bool bExpectDerefThrow = (bNullBuffer && !bNullSource);
            constexpr const bool bExpectFirstWriteThrow = (!bNullSource && !bExpectDerefThrow && writeLen > len1);
            constexpr const bool bExpectSecondWriteThrow = (!bNullSource && !bExpectDerefThrow && (2 * writeLen) > len1);

            std::vector<T> testData(GetTestData<T, TQ2>( ));
            Buffer<T> buffer(len1);
            Buffer<T> src(len2);
            bool bThrewLE = false;
            bool bThrewOOR = false;

            try
            {
                src.Write(testData.data( ), writeLen);
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            try
            {
                buffer.Write(src, true);
            }
            catch ( const std::out_of_range& e )
            {
                bThrewOOR = true;
                if ( !bExpectFirstWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if ( !bExpectDerefThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrewOOR == bExpectFirstWriteThrow);
            SUTL_TEST_ASSERT(bThrewLE == bExpectDerefThrow);

            if constexpr ( !bExpectFirstWriteThrow && !bExpectDerefThrow )
            {
                SUTL_TEST_ASSERT(bNullSource || buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == writeLen);
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 0) ? nullptr : (len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectFirstWriteThrow )
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectDerefThrow )
            {
                if constexpr ( bNullBuffer )
                {
                    SUTL_TEST_ASSERT(!buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == 0);
                    SUTL_TEST_ASSERT(buffer.Size( ) == 0);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(!buffer.m_FreeFunc);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                    SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
                }
            }

            try
            {
                buffer.Write(src, true);
            }
            catch ( const std::out_of_range& e )
            {
                bThrewOOR = true;
                if ( !bExpectSecondWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if ( !bExpectDerefThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrewOOR == bExpectSecondWriteThrow);
            SUTL_TEST_ASSERT(bThrewLE == bExpectDerefThrow);

            if constexpr ( !bExpectSecondWriteThrow && !bExpectDerefThrow )
            {
                SUTL_TEST_ASSERT(bNullSource || buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == writeLen * 2);
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 0) ? nullptr : (len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectSecondWriteThrow )
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == (bExpectFirstWriteThrow ? 0 : writeLen));
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectDerefThrow )
            {
                if constexpr ( bNullBuffer )
                {
                    SUTL_TEST_ASSERT(!buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == 0);
                    SUTL_TEST_ASSERT(buffer.Size( ) == 0);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(!buffer.m_FreeFunc);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                    SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2>
        static UTR WriteToEnd( )
        {
            constexpr const size_t len1 = GetTQNum<TQ1>( );
            constexpr const size_t len2 = GetTQNum<TQ2>( );
            constexpr const bool bNullBuffer = (len1 == 0);
            constexpr const bool bNullSource = (len2 == 0);
            constexpr const bool bExpectDerefThrow = (bNullBuffer && !bNullSource);
            constexpr const bool bExpectFirstWriteThrow = (!bNullSource && !bExpectDerefThrow && len2 > len1);
            constexpr const bool bExpectSecondWriteThrow = (!bNullSource && !bExpectDerefThrow && (2 * len2) > len1);

            std::vector<T> testData(GetTestData<T, TQ2>( ));
            Buffer<T> buffer(len1);
            Buffer<T> src(len2);
            bool bThrewLE = false;
            bool bThrewOOR = false;

            try
            {
                src.Write(testData.data( ), testData.size( ));
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            try
            {
                buffer.Write(src, false);
            }
            catch ( const std::out_of_range& e )
            {
                bThrewOOR = true;
                if ( !bExpectFirstWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if ( !bExpectDerefThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrewOOR == bExpectFirstWriteThrow);
            SUTL_TEST_ASSERT(bThrewLE == bExpectDerefThrow);

            if constexpr ( !bExpectFirstWriteThrow && !bExpectDerefThrow )
            {
                SUTL_TEST_ASSERT(bNullSource || buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == src.Length( ));
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 0) ? nullptr : (len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectFirstWriteThrow )
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectDerefThrow )
            {
                if constexpr ( bNullBuffer )
                {
                    SUTL_TEST_ASSERT(!buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == 0);
                    SUTL_TEST_ASSERT(buffer.Size( ) == 0);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(!buffer.m_FreeFunc);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                    SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
                }
            }

            try
            {
                buffer.Write(src, false);
            }
            catch ( const std::out_of_range& e )
            {
                bThrewOOR = true;
                if ( !bExpectSecondWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if ( !bExpectDerefThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrewOOR == bExpectSecondWriteThrow);
            SUTL_TEST_ASSERT(bThrewLE == bExpectDerefThrow);

            if constexpr ( !bExpectSecondWriteThrow && !bExpectDerefThrow )
            {
                SUTL_TEST_ASSERT(bNullSource || buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == std::min(buffer.Length( ), src.Length( ) * 2));
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 0) ? nullptr : (len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectSecondWriteThrow )
            {
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == (bExpectFirstWriteThrow ? 0 : std::min(buffer.Length( ), src.Length( ))));
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
            }
            else if constexpr ( bExpectDerefThrow )
            {
                if constexpr ( bNullBuffer )
                {
                    SUTL_TEST_ASSERT(!buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == 0);
                    SUTL_TEST_ASSERT(buffer.Size( ) == 0);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(!buffer.m_FreeFunc);
                }
                else
                {
                    SUTL_TEST_ASSERT(buffer.Ptr( ));
                    SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                    SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                    SUTL_TEST_ASSERT(buffer.WritePosition( ) == 0);
                    SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
                }
            }

            SUTL_TEST_SUCCESS( );
        }
    };
}
