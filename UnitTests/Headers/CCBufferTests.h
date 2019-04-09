#pragma once

// SUTL
#include <UnitTestResult.h>

// STL
#include <functional>
#include <list>
#include <memory>

// Target Class
#include <CCBuffer.h>

namespace CC
{
    class BufferTests
    {
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

        enum class TestQuantity : size_t
        {
            Zero = 0,
            One,
            Many,

            _End,
            _Begin = 0
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

        // Tests ZeroBuffer method behavior.
        class ZeroBufferTests;

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

            explicit operator bool() const noexcept
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
                if constexpr ( !bZeroCase )
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
                if constexpr ( bZeroCase )
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
                if constexpr ( len == 0 )
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
                if constexpr ( len == 0 )
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

    class BufferTests::ZeroBufferTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR ZeroBuffer( )
        {
            constexpr const size_t len = GetTQNum<TQ>( );
            constexpr const bool bThrowExpected = (len == 0);
            Buffer<T> buffer(len);
            bool bThrew = false;

            try
            {
                buffer.ZeroBuffer( );
            }
            catch ( const std::exception& e )
            {
                bThrew = true;
                if constexpr ( !bThrowExpected )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }

            SUTL_TEST_ASSERT(bThrew == bThrowExpected);
            if constexpr ( !bThrowExpected )
            {
                try
                {
                    for ( size_t i = 0; i < len; i++ )
                    {
                        if constexpr ( std::is_same_v<T, Helper> )
                        {
                            buffer[i] == Helper( );
                        }
                        else
                        {
                            buffer[i] = static_cast<T>(i);
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
                if constexpr ( !bThrowExpected )
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
                if constexpr ( !bExpectDerefThrow )
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
                if constexpr ( !bExpectSecondWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if constexpr ( !bExpectDerefThrow )
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

            SUTL_TEST_ASSERT(bThrew);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2>
        static UTR WritePtr( )
        {
            constexpr const size_t len1 = GetTQNum<TQ1>( );
            constexpr const size_t len2 = GetTQNum<TQ2>( );
            constexpr const bool bNullBuffer = (len1 == 0);
            constexpr const bool bNullSource = (len2 == 0);
            constexpr const bool bSameLen = (len1 == len2);
            constexpr const bool bExpectDerefThrow = (bNullBuffer || bNullSource);
            constexpr const bool bExpectFirstWriteThrow = (!bExpectDerefThrow && len2 > len1);
            constexpr const bool bExpectSecondWriteThrow = (!bExpectDerefThrow && len2 >= len1);

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
                if constexpr ( !bExpectFirstWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if constexpr ( !bExpectDerefThrow )
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
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == testData.size( ));
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
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
                if constexpr ( !bExpectSecondWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if constexpr ( !bExpectDerefThrow )
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
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == std::min(buffer.Length( ), testData.size( ) * 2));
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
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
            constexpr const bool bNullSrc = (len2 == 0);
            constexpr const bool bExpectSetupDerefThrow = (bNullSrc || writeLen == 0);
            constexpr const bool bExpectDerefThrow = (bNullBuffer || bNullSrc);
            constexpr const bool bExpectFirstWriteThrow = (!bExpectDerefThrow && writeLen > len1);
            constexpr const bool bExpectSecondWriteThrow = (!bExpectDerefThrow && (writeLen << 1) > len1);

            std::vector<T> testData(GetTestData<T, TQ2>( ));
            Buffer<T> buffer(len1);
            Buffer<T> src(len2);
            bool bThrewLE = false;
            bool bThrewOOR = false;

            try
            {
                src.Write(testData.data( ), writeLen);
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if constexpr ( !bExpectSetupDerefThrow )
                {
                    SUTL_SETUP_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            SUTL_SETUP_ASSERT(bThrewLE == bExpectSetupDerefThrow);
            bThrewLE = false;

            try
            {
                buffer.Write(src, true);
            }
            catch ( const std::out_of_range& e )
            {
                bThrewOOR = true;
                if constexpr ( !bExpectFirstWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if constexpr ( !bExpectDerefThrow )
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
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == writeLen);
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
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
                if constexpr ( !bExpectSecondWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if constexpr ( !bExpectDerefThrow )
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
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == writeLen * 2);
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
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
            constexpr const bool bSameLen = (len1 == len2);
            constexpr const bool bExpectSetupDerefThrow = (bNullSource || len2 == 0);
            constexpr const bool bExpectDerefThrow = (bNullBuffer || bNullSource);
            constexpr const bool bExpectFirstWriteThrow = (!bExpectDerefThrow && len2 > len1);
            constexpr const bool bExpectSecondWriteThrow = (!bExpectDerefThrow && len2 >= len1);

            std::vector<T> testData(GetTestData<T, TQ2>( ));
            Buffer<T> buffer(len1);
            Buffer<T> src(len2);
            bool bThrewLE = false;
            bool bThrewOOR = false;

            try
            {
                src.Write(testData.data( ), testData.size( ));
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if constexpr ( !bExpectDerefThrow )
                {
                    SUTL_SETUP_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            SUTL_SETUP_ASSERT(bThrewLE == bExpectSetupDerefThrow);
            bThrewLE = false;

            try
            {
                buffer.Write(src, false);
            }
            catch ( const std::out_of_range& e )
            {
                bThrewOOR = true;
                if constexpr ( !bExpectFirstWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if constexpr ( !bExpectDerefThrow )
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
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == src.Length( ));
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
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
                if constexpr ( !bExpectSecondWriteThrow )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }
            }
            catch ( const std::logic_error& e )
            {
                bThrewLE = true;
                if constexpr ( !bExpectDerefThrow )
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
                SUTL_TEST_ASSERT(buffer.Ptr( ));
                SUTL_TEST_ASSERT(buffer.Length( ) == len1);
                SUTL_TEST_ASSERT(buffer.Size( ) == sizeof(T) * len1);
                SUTL_TEST_ASSERT(buffer.WritePosition( ) == std::min(buffer.Length( ), src.Length( ) * 2));
                SUTL_TEST_ASSERT(buffer.m_FreeFunc == ((len1 == 1) ? Buffer<T>::FreeSingle : Buffer<T>::FreeArray));
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

CC::BufferTests::UTList CC::BufferTests::GetTests( )
{
    static const UTList tests
    {
        /// Allocation Tests \\\

        BufferTests::AllocationTests::Allocate<uint8_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::Allocate<uint16_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::Allocate<uint32_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::Allocate<uint64_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::Allocate<Helper, TestQuantity::Zero>,
        BufferTests::AllocationTests::Allocate<uint8_t, TestQuantity::One>,
        BufferTests::AllocationTests::Allocate<uint16_t, TestQuantity::One>,
        BufferTests::AllocationTests::Allocate<uint32_t, TestQuantity::One>,
        BufferTests::AllocationTests::Allocate<uint64_t, TestQuantity::One>,
        BufferTests::AllocationTests::Allocate<Helper, TestQuantity::One>,
        BufferTests::AllocationTests::Allocate<uint8_t, TestQuantity::Many>,
        BufferTests::AllocationTests::Allocate<uint16_t, TestQuantity::Many>,
        BufferTests::AllocationTests::Allocate<uint32_t, TestQuantity::Many>,
        BufferTests::AllocationTests::Allocate<uint64_t, TestQuantity::Many>,
        BufferTests::AllocationTests::Allocate<Helper, TestQuantity::Many>,

        BufferTests::AllocationTests::AllocFromPtrNull<uint8_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::AllocFromPtrNull<uint16_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::AllocFromPtrNull<uint32_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::AllocFromPtrNull<uint64_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::AllocFromPtrNull<Helper, TestQuantity::Zero>,
        BufferTests::AllocationTests::AllocFromPtrNull<uint8_t, TestQuantity::One>,
        BufferTests::AllocationTests::AllocFromPtrNull<uint16_t, TestQuantity::One>,
        BufferTests::AllocationTests::AllocFromPtrNull<uint32_t, TestQuantity::One>,
        BufferTests::AllocationTests::AllocFromPtrNull<uint64_t, TestQuantity::One>,
        BufferTests::AllocationTests::AllocFromPtrNull<Helper, TestQuantity::One>,
        BufferTests::AllocationTests::AllocFromPtrNull<uint8_t, TestQuantity::Many>,
        BufferTests::AllocationTests::AllocFromPtrNull<uint16_t, TestQuantity::Many>,
        BufferTests::AllocationTests::AllocFromPtrNull<uint32_t, TestQuantity::Many>,
        BufferTests::AllocationTests::AllocFromPtrNull<uint64_t, TestQuantity::Many>,
        BufferTests::AllocationTests::AllocFromPtrNull<Helper, TestQuantity::Many>,

        BufferTests::AllocationTests::AllocFromPtr<uint8_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::AllocFromPtr<uint16_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::AllocFromPtr<uint32_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::AllocFromPtr<uint64_t, TestQuantity::Zero>,
        BufferTests::AllocationTests::AllocFromPtr<Helper, TestQuantity::Zero>,
        BufferTests::AllocationTests::AllocFromPtr<uint8_t, TestQuantity::One>,
        BufferTests::AllocationTests::AllocFromPtr<uint16_t, TestQuantity::One>,
        BufferTests::AllocationTests::AllocFromPtr<uint32_t, TestQuantity::One>,
        BufferTests::AllocationTests::AllocFromPtr<uint64_t, TestQuantity::One>,
        BufferTests::AllocationTests::AllocFromPtr<Helper, TestQuantity::One>,
        BufferTests::AllocationTests::AllocFromPtr<uint8_t, TestQuantity::Many>,
        BufferTests::AllocationTests::AllocFromPtr<uint16_t, TestQuantity::Many>,
        BufferTests::AllocationTests::AllocFromPtr<uint32_t, TestQuantity::Many>,
        BufferTests::AllocationTests::AllocFromPtr<uint64_t, TestQuantity::Many>,
        BufferTests::AllocationTests::AllocFromPtr<Helper, TestQuantity::Many>,

        /// Deallocation Tests \\\

        BufferTests::DeallocationTests::CopyBuffer<uint8_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::CopyBuffer<uint16_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::CopyBuffer<uint32_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::CopyBuffer<uint64_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::CopyBuffer<Helper, TestQuantity::Zero>,
        BufferTests::DeallocationTests::CopyBuffer<uint8_t, TestQuantity::One>,
        BufferTests::DeallocationTests::CopyBuffer<uint16_t, TestQuantity::One>,
        BufferTests::DeallocationTests::CopyBuffer<uint32_t, TestQuantity::One>,
        BufferTests::DeallocationTests::CopyBuffer<uint64_t, TestQuantity::One>,
        BufferTests::DeallocationTests::CopyBuffer<Helper, TestQuantity::One>,
        BufferTests::DeallocationTests::CopyBuffer<uint8_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::CopyBuffer<uint16_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::CopyBuffer<uint32_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::CopyBuffer<uint64_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::CopyBuffer<Helper, TestQuantity::Many>,

        BufferTests::DeallocationTests::TransferBuffer<uint8_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::TransferBuffer<uint16_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::TransferBuffer<uint32_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::TransferBuffer<uint64_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::TransferBuffer<Helper, TestQuantity::Zero>,
        BufferTests::DeallocationTests::TransferBuffer<uint8_t, TestQuantity::One>,
        BufferTests::DeallocationTests::TransferBuffer<uint16_t, TestQuantity::One>,
        BufferTests::DeallocationTests::TransferBuffer<uint32_t, TestQuantity::One>,
        BufferTests::DeallocationTests::TransferBuffer<uint64_t, TestQuantity::One>,
        BufferTests::DeallocationTests::TransferBuffer<Helper, TestQuantity::One>,
        BufferTests::DeallocationTests::TransferBuffer<uint8_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::TransferBuffer<uint16_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::TransferBuffer<uint32_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::TransferBuffer<uint64_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::TransferBuffer<Helper, TestQuantity::Many>,

        BufferTests::DeallocationTests::Destructor<uint8_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::Destructor<uint16_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::Destructor<uint32_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::Destructor<uint64_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::Destructor<Helper, TestQuantity::Zero>,
        BufferTests::DeallocationTests::Destructor<uint8_t, TestQuantity::One>,
        BufferTests::DeallocationTests::Destructor<uint16_t, TestQuantity::One>,
        BufferTests::DeallocationTests::Destructor<uint32_t, TestQuantity::One>,
        BufferTests::DeallocationTests::Destructor<uint64_t, TestQuantity::One>,
        BufferTests::DeallocationTests::Destructor<Helper, TestQuantity::One>,
        BufferTests::DeallocationTests::Destructor<uint8_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::Destructor<uint16_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::Destructor<uint32_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::Destructor<uint64_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::Destructor<Helper, TestQuantity::Many>,

        BufferTests::DeallocationTests::Free<uint8_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::Free<uint16_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::Free<uint32_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::Free<uint64_t, TestQuantity::Zero>,
        BufferTests::DeallocationTests::Free<Helper, TestQuantity::Zero>,
        BufferTests::DeallocationTests::Free<uint8_t, TestQuantity::One>,
        BufferTests::DeallocationTests::Free<uint16_t, TestQuantity::One>,
        BufferTests::DeallocationTests::Free<uint32_t, TestQuantity::One>,
        BufferTests::DeallocationTests::Free<uint64_t, TestQuantity::One>,
        BufferTests::DeallocationTests::Free<Helper, TestQuantity::One>,
        BufferTests::DeallocationTests::Free<uint8_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::Free<uint16_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::Free<uint32_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::Free<uint64_t, TestQuantity::Many>,
        BufferTests::DeallocationTests::Free<Helper, TestQuantity::Many>,

        /// Constructor Tests \\\

        BufferTests::ConstructorTests::DefaultConstructor<uint8_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::DefaultConstructor<uint16_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::DefaultConstructor<uint32_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::DefaultConstructor<uint64_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::DefaultConstructor<Helper, TestQuantity::Zero>,
        BufferTests::ConstructorTests::DefaultConstructor<uint8_t, TestQuantity::One>,
        BufferTests::ConstructorTests::DefaultConstructor<uint16_t, TestQuantity::One>,
        BufferTests::ConstructorTests::DefaultConstructor<uint32_t, TestQuantity::One>,
        BufferTests::ConstructorTests::DefaultConstructor<uint64_t, TestQuantity::One>,
        BufferTests::ConstructorTests::DefaultConstructor<Helper, TestQuantity::One>,
        BufferTests::ConstructorTests::DefaultConstructor<uint8_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::DefaultConstructor<uint16_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::DefaultConstructor<uint32_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::DefaultConstructor<uint64_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::DefaultConstructor<Helper, TestQuantity::Many>,
            
        BufferTests::ConstructorTests::LengthConstructor<uint8_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::LengthConstructor<uint16_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::LengthConstructor<uint32_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::LengthConstructor<uint64_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::LengthConstructor<Helper, TestQuantity::Zero>,
        BufferTests::ConstructorTests::LengthConstructor<uint8_t, TestQuantity::One>,
        BufferTests::ConstructorTests::LengthConstructor<uint16_t, TestQuantity::One>,
        BufferTests::ConstructorTests::LengthConstructor<uint32_t, TestQuantity::One>,
        BufferTests::ConstructorTests::LengthConstructor<uint64_t, TestQuantity::One>,
        BufferTests::ConstructorTests::LengthConstructor<Helper, TestQuantity::One>,
        BufferTests::ConstructorTests::LengthConstructor<uint8_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::LengthConstructor<uint16_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::LengthConstructor<uint32_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::LengthConstructor<uint64_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::LengthConstructor<Helper, TestQuantity::Many>,

        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint8_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint16_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint32_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint64_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<Helper, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint8_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint16_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint32_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint64_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<Helper, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint8_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint16_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint32_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<uint64_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerCopyConstructorNull<Helper, TestQuantity::Many>,

        BufferTests::ConstructorTests::PointerCopyConstructor<uint8_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerCopyConstructor<uint16_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerCopyConstructor<uint32_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerCopyConstructor<uint64_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerCopyConstructor<Helper, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerCopyConstructor<uint8_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerCopyConstructor<uint16_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerCopyConstructor<uint32_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerCopyConstructor<uint64_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerCopyConstructor<Helper, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerCopyConstructor<uint8_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerCopyConstructor<uint16_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerCopyConstructor<uint32_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerCopyConstructor<uint64_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerCopyConstructor<Helper, TestQuantity::Many>,

        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint8_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint16_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint32_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint64_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<Helper, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint8_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint16_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint32_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint64_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<Helper, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint8_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint16_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint32_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<uint64_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerMoveConstructorNull<Helper, TestQuantity::Many>,

        BufferTests::ConstructorTests::PointerMoveConstructor<uint8_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerMoveConstructor<uint16_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerMoveConstructor<uint32_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerMoveConstructor<uint64_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerMoveConstructor<Helper, TestQuantity::Zero>,
        BufferTests::ConstructorTests::PointerMoveConstructor<uint8_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerMoveConstructor<uint16_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerMoveConstructor<uint32_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerMoveConstructor<uint64_t, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerMoveConstructor<Helper, TestQuantity::One>,
        BufferTests::ConstructorTests::PointerMoveConstructor<uint8_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerMoveConstructor<uint16_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerMoveConstructor<uint32_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerMoveConstructor<uint64_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::PointerMoveConstructor<Helper, TestQuantity::Many>,

        BufferTests::ConstructorTests::CopyConstructor<uint8_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::CopyConstructor<uint16_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::CopyConstructor<uint32_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::CopyConstructor<uint64_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::CopyConstructor<Helper, TestQuantity::Zero>,
        BufferTests::ConstructorTests::CopyConstructor<uint8_t, TestQuantity::One>,
        BufferTests::ConstructorTests::CopyConstructor<uint16_t, TestQuantity::One>,
        BufferTests::ConstructorTests::CopyConstructor<uint32_t, TestQuantity::One>,
        BufferTests::ConstructorTests::CopyConstructor<uint64_t, TestQuantity::One>,
        BufferTests::ConstructorTests::CopyConstructor<Helper, TestQuantity::One>,
        BufferTests::ConstructorTests::CopyConstructor<uint8_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::CopyConstructor<uint16_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::CopyConstructor<uint32_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::CopyConstructor<uint64_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::CopyConstructor<Helper, TestQuantity::Many>,
             
        BufferTests::ConstructorTests::MoveConstructor<uint8_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::MoveConstructor<uint16_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::MoveConstructor<uint32_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::MoveConstructor<uint64_t, TestQuantity::Zero>,
        BufferTests::ConstructorTests::MoveConstructor<Helper, TestQuantity::Zero>,
        BufferTests::ConstructorTests::MoveConstructor<uint8_t, TestQuantity::One>,
        BufferTests::ConstructorTests::MoveConstructor<uint16_t, TestQuantity::One>,
        BufferTests::ConstructorTests::MoveConstructor<uint32_t, TestQuantity::One>,
        BufferTests::ConstructorTests::MoveConstructor<uint64_t, TestQuantity::One>,
        BufferTests::ConstructorTests::MoveConstructor<Helper, TestQuantity::One>,
        BufferTests::ConstructorTests::MoveConstructor<uint8_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::MoveConstructor<uint16_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::MoveConstructor<uint32_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::MoveConstructor<uint64_t, TestQuantity::Many>,
        BufferTests::ConstructorTests::MoveConstructor<Helper, TestQuantity::Many>,

        /// Copy Tests \\\

        BufferTests::CopyTests::CopyAllBufferDataMembers<uint8_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<uint16_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<uint32_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<uint64_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<Helper, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<uint8_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<uint16_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<uint32_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<uint64_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<Helper, TestQuantity::One>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<uint8_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<uint16_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<uint32_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<uint64_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyAllBufferDataMembers<Helper, TestQuantity::Many>,

        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint8_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint16_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint32_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint64_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<Helper, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint8_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint16_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint32_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint64_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<Helper, TestQuantity::One>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint8_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint16_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint32_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<uint64_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyNonPointerBufferDataMembers<Helper, TestQuantity::Many>,

        BufferTests::CopyTests::CopyBuffer<uint8_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyBuffer<uint16_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyBuffer<uint32_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyBuffer<uint64_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyBuffer<Helper, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyBuffer<uint8_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyBuffer<uint16_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyBuffer<uint32_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyBuffer<uint64_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyBuffer<Helper, TestQuantity::One>,
        BufferTests::CopyTests::CopyBuffer<uint8_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyBuffer<uint16_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyBuffer<uint32_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyBuffer<uint64_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyBuffer<Helper, TestQuantity::Many>,

        BufferTests::CopyTests::CopyAssignment<uint8_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyAssignment<uint16_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyAssignment<uint32_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyAssignment<uint64_t, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyAssignment<Helper, TestQuantity::Zero>,
        BufferTests::CopyTests::CopyAssignment<uint8_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyAssignment<uint16_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyAssignment<uint32_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyAssignment<uint64_t, TestQuantity::One>,
        BufferTests::CopyTests::CopyAssignment<Helper, TestQuantity::One>,
        BufferTests::CopyTests::CopyAssignment<uint8_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyAssignment<uint16_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyAssignment<uint32_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyAssignment<uint64_t, TestQuantity::Many>,
        BufferTests::CopyTests::CopyAssignment<Helper, TestQuantity::Many>,

        /// Move Tests \\\

        BufferTests::MoveTests::TransferBuffer<uint8_t, TestQuantity::Zero>,
        BufferTests::MoveTests::TransferBuffer<uint16_t, TestQuantity::Zero>,
        BufferTests::MoveTests::TransferBuffer<uint32_t, TestQuantity::Zero>,
        BufferTests::MoveTests::TransferBuffer<uint64_t, TestQuantity::Zero>,
        BufferTests::MoveTests::TransferBuffer<Helper, TestQuantity::Zero>,
        BufferTests::MoveTests::TransferBuffer<uint8_t, TestQuantity::One>,
        BufferTests::MoveTests::TransferBuffer<uint16_t, TestQuantity::One>,
        BufferTests::MoveTests::TransferBuffer<uint32_t, TestQuantity::One>,
        BufferTests::MoveTests::TransferBuffer<uint64_t, TestQuantity::One>,
        BufferTests::MoveTests::TransferBuffer<Helper, TestQuantity::One>,
        BufferTests::MoveTests::TransferBuffer<uint8_t, TestQuantity::Many>,
        BufferTests::MoveTests::TransferBuffer<uint16_t, TestQuantity::Many>,
        BufferTests::MoveTests::TransferBuffer<uint32_t, TestQuantity::Many>,
        BufferTests::MoveTests::TransferBuffer<uint64_t, TestQuantity::Many>,
        BufferTests::MoveTests::TransferBuffer<Helper, TestQuantity::Many>,

        BufferTests::MoveTests::MoveAssignment<uint8_t, TestQuantity::Zero>,
        BufferTests::MoveTests::MoveAssignment<uint16_t, TestQuantity::Zero>,
        BufferTests::MoveTests::MoveAssignment<uint32_t, TestQuantity::Zero>,
        BufferTests::MoveTests::MoveAssignment<uint64_t, TestQuantity::Zero>,
        BufferTests::MoveTests::MoveAssignment<Helper, TestQuantity::Zero>,
        BufferTests::MoveTests::MoveAssignment<uint8_t, TestQuantity::One>,
        BufferTests::MoveTests::MoveAssignment<uint16_t, TestQuantity::One>,
        BufferTests::MoveTests::MoveAssignment<uint32_t, TestQuantity::One>,
        BufferTests::MoveTests::MoveAssignment<uint64_t, TestQuantity::One>,
        BufferTests::MoveTests::MoveAssignment<Helper, TestQuantity::One>,
        BufferTests::MoveTests::MoveAssignment<uint8_t, TestQuantity::Many>,
        BufferTests::MoveTests::MoveAssignment<uint16_t, TestQuantity::Many>,
        BufferTests::MoveTests::MoveAssignment<uint32_t, TestQuantity::Many>,
        BufferTests::MoveTests::MoveAssignment<uint64_t, TestQuantity::Many>,
        BufferTests::MoveTests::MoveAssignment<Helper, TestQuantity::Many>,

        /// Subscript Operator Tests \\\

        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint8_t, TestQuantity::Zero>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint16_t, TestQuantity::Zero>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint32_t, TestQuantity::Zero>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint64_t, TestQuantity::Zero>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<Helper, TestQuantity::Zero>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint8_t, TestQuantity::One>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint16_t, TestQuantity::One>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint32_t, TestQuantity::One>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint64_t, TestQuantity::One>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<Helper, TestQuantity::One>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint8_t, TestQuantity::Many>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint16_t, TestQuantity::Many>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint32_t, TestQuantity::Many>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<uint64_t, TestQuantity::Many>,
        BufferTests::SubscriptOperatorTests::SubscriptOperator<Helper, TestQuantity::Many>,

        /// Dereference Operator Tests \\\

        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint8_t, TestQuantity::Zero>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint16_t, TestQuantity::Zero>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint32_t, TestQuantity::Zero>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint64_t, TestQuantity::Zero>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<Helper, TestQuantity::Zero>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint8_t, TestQuantity::One>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint16_t, TestQuantity::One>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint32_t, TestQuantity::One>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint64_t, TestQuantity::One>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<Helper, TestQuantity::One>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint8_t, TestQuantity::Many>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint16_t, TestQuantity::Many>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint32_t, TestQuantity::Many>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<uint64_t, TestQuantity::Many>,
        BufferTests::DereferenceOperatorTests::DereferenceOperator<Helper, TestQuantity::Many>,

        /// Zero Buffer Tests \\\

        BufferTests::ZeroBufferTests::ZeroBuffer<uint8_t, TestQuantity::Zero>,
        BufferTests::ZeroBufferTests::ZeroBuffer<uint16_t, TestQuantity::Zero>,
        BufferTests::ZeroBufferTests::ZeroBuffer<uint32_t, TestQuantity::Zero>,
        BufferTests::ZeroBufferTests::ZeroBuffer<uint64_t, TestQuantity::Zero>,
        BufferTests::ZeroBufferTests::ZeroBuffer<Helper, TestQuantity::Zero>,
        BufferTests::ZeroBufferTests::ZeroBuffer<uint8_t, TestQuantity::One>,
        BufferTests::ZeroBufferTests::ZeroBuffer<uint16_t, TestQuantity::One>,
        BufferTests::ZeroBufferTests::ZeroBuffer<uint32_t, TestQuantity::One>,
        BufferTests::ZeroBufferTests::ZeroBuffer<uint64_t, TestQuantity::One>,
        BufferTests::ZeroBufferTests::ZeroBuffer<Helper, TestQuantity::One>,
        BufferTests::ZeroBufferTests::ZeroBuffer<uint8_t, TestQuantity::Many>,
        BufferTests::ZeroBufferTests::ZeroBuffer<uint16_t, TestQuantity::Many>,
        BufferTests::ZeroBufferTests::ZeroBuffer<uint32_t, TestQuantity::Many>,
        BufferTests::ZeroBufferTests::ZeroBuffer<uint64_t, TestQuantity::Many>,
        BufferTests::ZeroBufferTests::ZeroBuffer<Helper, TestQuantity::Many>,

        /// Comparison Tests \\\

        BufferTests::ComparisonTests::ComparisonPtrNull<uint8_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint8_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint8_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint8_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint8_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint8_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint8_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint8_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint8_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint16_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint16_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint16_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint16_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint16_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint16_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint16_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint16_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint16_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint32_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint32_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint32_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint32_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint32_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint32_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint32_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint32_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint32_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint64_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint64_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint64_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint64_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint64_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint64_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint64_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint64_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<uint64_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<Helper, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<Helper, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<Helper, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<Helper, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<Helper, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<Helper, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtrNull<Helper, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtrNull<Helper, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtrNull<Helper, TestQuantity::Many, TestQuantity::Many>,

        BufferTests::ComparisonTests::ComparisonPtr<uint8_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint8_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint8_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<uint8_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint8_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint8_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<uint8_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint8_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint8_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<uint16_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint16_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint16_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<uint16_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint16_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint16_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<uint16_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint16_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint16_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<uint32_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint32_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint32_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<uint32_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint32_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint32_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<uint32_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint32_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint32_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<uint64_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint64_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint64_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<uint64_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint64_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint64_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<uint64_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<uint64_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<uint64_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<Helper, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<Helper, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<Helper, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<Helper, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<Helper, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<Helper, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonPtr<Helper, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonPtr<Helper, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonPtr<Helper, TestQuantity::Many, TestQuantity::Many>,

        BufferTests::ComparisonTests::Comparison<uint8_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint8_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint8_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<uint8_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint8_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint8_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<uint8_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint8_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint8_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<uint16_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint16_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint16_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<uint16_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint16_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint16_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<uint16_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint16_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint16_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<uint32_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint32_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint32_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<uint32_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint32_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint32_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<uint32_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint32_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint32_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<uint64_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint64_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint64_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<uint64_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint64_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint64_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<uint64_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<uint64_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<uint64_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<Helper, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<Helper, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<Helper, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<Helper, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<Helper, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<Helper, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::Comparison<Helper, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::Comparison<Helper, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::Comparison<Helper, TestQuantity::Many, TestQuantity::Many>,

        BufferTests::ComparisonTests::ComparisonOperator<uint8_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint8_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint8_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<uint8_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint8_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint8_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<uint8_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint8_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint8_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<uint16_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint16_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint16_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<uint16_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint16_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint16_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<uint16_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint16_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint16_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<uint32_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint32_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint32_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<uint32_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint32_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint32_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<uint32_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint32_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint32_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<uint64_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint64_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint64_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<uint64_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint64_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint64_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<uint64_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<uint64_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<uint64_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<Helper, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<Helper, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<Helper, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<Helper, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<Helper, TestQuantity::One, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<Helper, TestQuantity::One, TestQuantity::Many>,
        BufferTests::ComparisonTests::ComparisonOperator<Helper, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::ComparisonTests::ComparisonOperator<Helper, TestQuantity::Many, TestQuantity::One>,
        BufferTests::ComparisonTests::ComparisonOperator<Helper, TestQuantity::Many, TestQuantity::Many>,

        /// Set Write Position Tests \\\

        BufferTests::SetWritePositionTests::SetWritePosition<uint8_t, TestQuantity::Zero>,
        BufferTests::SetWritePositionTests::SetWritePosition<uint16_t, TestQuantity::Zero>,
        BufferTests::SetWritePositionTests::SetWritePosition<uint32_t, TestQuantity::Zero>,
        BufferTests::SetWritePositionTests::SetWritePosition<uint64_t, TestQuantity::Zero>,
        BufferTests::SetWritePositionTests::SetWritePosition<Helper, TestQuantity::Zero>,
        BufferTests::SetWritePositionTests::SetWritePosition<uint8_t, TestQuantity::One>,
        BufferTests::SetWritePositionTests::SetWritePosition<uint16_t, TestQuantity::One>,
        BufferTests::SetWritePositionTests::SetWritePosition<uint32_t, TestQuantity::One>,
        BufferTests::SetWritePositionTests::SetWritePosition<uint64_t, TestQuantity::One>,
        BufferTests::SetWritePositionTests::SetWritePosition<Helper, TestQuantity::One>,
        BufferTests::SetWritePositionTests::SetWritePosition<uint8_t, TestQuantity::Many>,
        BufferTests::SetWritePositionTests::SetWritePosition<uint16_t, TestQuantity::Many>,
        BufferTests::SetWritePositionTests::SetWritePosition<uint32_t, TestQuantity::Many>,
        BufferTests::SetWritePositionTests::SetWritePosition<uint64_t, TestQuantity::Many>,
        BufferTests::SetWritePositionTests::SetWritePosition<Helper, TestQuantity::Many>,

        /// Write Tests \\\

        BufferTests::WriteTests::WriteElement<uint8_t, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteElement<uint16_t, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteElement<uint32_t, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteElement<uint64_t, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteElement<Helper, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteElement<uint8_t, TestQuantity::One>,
        BufferTests::WriteTests::WriteElement<uint16_t, TestQuantity::One>,
        BufferTests::WriteTests::WriteElement<uint32_t, TestQuantity::One>,
        BufferTests::WriteTests::WriteElement<uint64_t, TestQuantity::One>,
        BufferTests::WriteTests::WriteElement<Helper, TestQuantity::One>,
        BufferTests::WriteTests::WriteElement<uint8_t, TestQuantity::Many>,
        BufferTests::WriteTests::WriteElement<uint16_t, TestQuantity::Many>,
        BufferTests::WriteTests::WriteElement<uint32_t, TestQuantity::Many>,
        BufferTests::WriteTests::WriteElement<uint64_t, TestQuantity::Many>,
        BufferTests::WriteTests::WriteElement<Helper, TestQuantity::Many>,

        BufferTests::WriteTests::WritePtrNull<uint8_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint8_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint8_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<uint8_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint8_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint8_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<uint8_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint8_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint8_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<uint16_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint16_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint16_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<uint16_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint16_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint16_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<uint16_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint16_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint16_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<uint32_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint32_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint32_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<uint32_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint32_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint32_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<uint32_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint32_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint32_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<uint64_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint64_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint64_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<uint64_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint64_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint64_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<uint64_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<uint64_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<uint64_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<Helper, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<Helper, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<Helper, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<Helper, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<Helper, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<Helper, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtrNull<Helper, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtrNull<Helper, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WritePtrNull<Helper, TestQuantity::Many, TestQuantity::Many>,

        BufferTests::WriteTests::WritePtr<uint8_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint8_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint8_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<uint8_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint8_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint8_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<uint8_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint8_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint8_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<uint16_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint16_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint16_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<uint16_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint16_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint16_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<uint16_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint16_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint16_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<uint32_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint32_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint32_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<uint32_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint32_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint32_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<uint32_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint32_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint32_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<uint64_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint64_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint64_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<uint64_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint64_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint64_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<uint64_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<uint64_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<uint64_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<Helper, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<Helper, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<Helper, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<Helper, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<Helper, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<Helper, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WritePtr<Helper, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WritePtr<Helper, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WritePtr<Helper, TestQuantity::Many, TestQuantity::Many>,
        
        BufferTests::WriteTests::WriteToWritePosition<uint8_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint8_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint8_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<uint8_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint8_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint8_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<uint8_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint8_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint8_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<uint16_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint16_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint16_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<uint16_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint16_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint16_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<uint16_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint16_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint16_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<uint32_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint32_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint32_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<uint32_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint32_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint32_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<uint32_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint32_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint32_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<uint64_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint64_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint64_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<uint64_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint64_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint64_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<uint64_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<uint64_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<uint64_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<Helper, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<Helper, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<Helper, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<Helper, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<Helper, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<Helper, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToWritePosition<Helper, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToWritePosition<Helper, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WriteToWritePosition<Helper, TestQuantity::Many, TestQuantity::Many>,
        
        BufferTests::WriteTests::WriteToEnd<uint8_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint8_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint8_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<uint8_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint8_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint8_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<uint8_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint8_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint8_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<uint16_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint16_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint16_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<uint16_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint16_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint16_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<uint16_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint16_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint16_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<uint32_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint32_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint32_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<uint32_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint32_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint32_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<uint32_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint32_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint32_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<uint64_t, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint64_t, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint64_t, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<uint64_t, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint64_t, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint64_t, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<uint64_t, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<uint64_t, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<uint64_t, TestQuantity::Many, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<Helper, TestQuantity::Zero, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<Helper, TestQuantity::Zero, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<Helper, TestQuantity::Zero, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<Helper, TestQuantity::One, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<Helper, TestQuantity::One, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<Helper, TestQuantity::One, TestQuantity::Many>,
        BufferTests::WriteTests::WriteToEnd<Helper, TestQuantity::Many, TestQuantity::Zero>,
        BufferTests::WriteTests::WriteToEnd<Helper, TestQuantity::Many, TestQuantity::One>,
        BufferTests::WriteTests::WriteToEnd<Helper, TestQuantity::Many, TestQuantity::Many>
    };

    return tests;
}
