#pragma once

// SUTL
#include <UnitTestResult.h>

// STL
#include <algorithm>
#include <functional>
#include <list>
#include <memory>

// Target Class
#include <CCPointer.h>

namespace CC
{
    class PointerTests
    {
        friend class BufferTests;
        friend class DynamicBufferTests;

    private:

        // Type aliases
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;

        /// Test Helpers \\\

        struct Helper
        {
            static const size_t s_ArrSize = 21;

            uint8_t m_u8;
            uint16_t m_u16;
            uint32_t m_u32;
            uint64_t m_u64;
            uint8_t m_u8Arr[s_ArrSize];

            bool m_bCopied;
            bool m_bMoved;

            Helper( ) noexcept :
                m_u8(0), m_u16(0), m_u32(0), m_u64(0),
                m_bCopied(false), m_bMoved(false)
            {
                memset(m_u8Arr, 0, sizeof(m_u8Arr));
            }

            Helper(const Helper& src) :
                Helper( )
            {
                *this = src;
            }

            Helper(Helper&& src) noexcept :
                Helper( )
            {
                *this = std::move(src);
            }

            ~Helper( ) noexcept = default;

            Helper& operator=(const Helper& src)
            {
                if ( this != &src )
                {
                    m_u8 = src.m_u8;
                    m_u16 = src.m_u16;
                    m_u32 = src.m_u32;
                    m_u64 = src.m_u64;
                    memcpy(m_u8Arr, src.m_u8Arr, sizeof(m_u8Arr));

                    m_bCopied = true;
                    m_bMoved = false;
                }

                return *this;
            }

            Helper& operator=(Helper&& src) noexcept
            {
                if ( this != &src )
                {
                    m_u8 = src.m_u8;
                    m_u16 = src.m_u16;
                    m_u32 = src.m_u32;
                    m_u64 = src.m_u64;
                    memcpy(m_u8Arr, src.m_u8Arr, sizeof(m_u8Arr));

                    m_bCopied = false;
                    m_bMoved = true;
                }

                return *this;
            }

            const bool operator==(const Helper& rhs) const noexcept
            {
                return m_u8 == rhs.m_u8
                    && m_u16 == rhs.m_u16
                    && m_u32 == rhs.m_u32
                    && m_u64 == rhs.m_u64
                    && memcmp(m_u8Arr, rhs.m_u8Arr, sizeof(m_u8Arr)) == 0;
            }

            const bool operator!=(const Helper& rhs) const noexcept
            {
                return !operator==(rhs);
            }

            bool Copied( ) const noexcept
            {
                return m_bCopied;
            }

            bool Moved( ) const noexcept
            {
                return m_bMoved;
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
            Pointer<T>::FreeSingle(p);
        }

        template <typename T>
        static void FreeArray(T* p)
        {
            ArrayFreeCounter( )++;
            Pointer<T>::FreeArray(p);
        }

        template <typename T, TestQuantity TQ>
        static void AssignFreeWrapper(Pointer<T>& ptr)
        {
            static_assert(IsValidTestType<T>( ), __FUNCSIG__": Invalid Template Type Argument");
            static_assert(IsValidTestQuantity<TQ>( ), __FUNCSIG__": Invalid TestQuantity Template Argument.");

            if constexpr ( TQ == TestQuantity::Zero )
            {
                ptr.m_FreeFunc = nullptr;
            }
            else if constexpr ( TQ == TestQuantity::One )
            {
                ptr.m_FreeFunc = FreeSingle<T>;
            }
            else if constexpr ( TQ == TestQuantity::Many )
            {
                ptr.m_FreeFunc = FreeArray<T>;
            }
        }

    public:

        // Return list of Pointer unit tests.
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
        class DereferenceOperatorTests;
    };

    class PointerTests::AllocationTests
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
                p = Pointer<T>::Allocate(GetTQNum<TQ>( ));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bZeroCase == !p);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR AllocFromRawPtrNull( )
        {
            Ptr<T, TQ> p;

            try
            {
                p = Pointer<T>::AllocateFromRawPointer(nullptr, GetTQNum<TQ>( ));
            }
            catch ( const std::invalid_argument& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(!p);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR AllocFromRawPtr( )
        {
            constexpr const bool bZeroCase = (TQ == TestQuantity::Zero);

            const std::vector<T> testData = GetTestData<T, TQ>( );
            Ptr<T, TQ> p;

            try
            {
                p = Pointer<T>::AllocateFromRawPointer(testData.data( ), GetTQNum<TQ>( ));
            }
            catch ( const std::invalid_argument& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(p || bZeroCase);

            SUTL_TEST_SUCCESS( );
        }
    };

    class PointerTests::DeallocationTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR CopyPointerObj( )
        {
            constexpr size_t expectedSingleFreeCount = (TQ == TestQuantity::One) ? 1 : 0;
            constexpr size_t expectedArrayFreeCount = (TQ == TestQuantity::Many) ? 1 : 0;

            Pointer<T> ptr(GetTQNum<TQ>( ));
            Pointer<T> srcPtr(GetTQNum<TQ>( ));

            ResetFreeCounters( );
            AssignFreeWrapper<T, TQ>(ptr);

            try
            {
                Pointer<T>::CopyPointerObj(ptr, srcPtr);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(SingleFreeCounter( ) == expectedSingleFreeCount);
            SUTL_TEST_ASSERT(ArrayFreeCounter( ) == expectedArrayFreeCount);

            SUTL_TEST_ASSERT((ptr.Ptr( ) != srcPtr.Ptr( )) == (TQ != TestQuantity::Zero));
            SUTL_TEST_ASSERT(ptr.Length( ) == srcPtr.Length( ));
            for ( size_t i = 0; i < GetTQNum<TQ>( ); i++ )
            {
                SUTL_TEST_ASSERT(ptr.Ptr( )[i] == srcPtr.Ptr( )[i]);
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR TransferPointerObj( )
        {
            constexpr size_t expectedSingleFreeCount = (TQ == TestQuantity::One) ? 1 : 0;
            constexpr size_t expectedArrayFreeCount = (TQ == TestQuantity::Many) ? 1 : 0;

            Pointer<T> ptr(GetTQNum<TQ>( ));
            Pointer<T> srcPtr(GetTQNum<TQ>( ));

            ResetFreeCounters( );
            AssignFreeWrapper<T, TQ>(ptr);

            try
            {
                Pointer<T>::TransferPointerObj(ptr, srcPtr);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(SingleFreeCounter( ) == expectedSingleFreeCount);
            SUTL_TEST_ASSERT(ArrayFreeCounter( ) == expectedArrayFreeCount);

            SUTL_TEST_ASSERT((ptr.Ptr( ) != srcPtr.Ptr( )) == (TQ != TestQuantity::Zero));
            SUTL_TEST_ASSERT(srcPtr.Ptr( ) == nullptr);
            if constexpr ( std::is_same_v<T, Helper> )
            {
                for ( size_t i = 0; i < GetTQNum<TQ>( ); i++ )
                {
                    SUTL_TEST_ASSERT(!ptr.Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!ptr.Ptr( )[i].Moved( ));
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR Destructor( )
        {
            constexpr size_t expectedSingleFreeCount = (TQ == TestQuantity::One) ? 1 : 0;
            constexpr size_t expectedArrayFreeCount = (TQ == TestQuantity::Many) ? 1 : 0;

            Pointer<T>* pPtr = nullptr;

            try
            {
                pPtr = new Pointer<T>(GetTQNum<TQ>( ));
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            ResetFreeCounters( );
            AssignFreeWrapper<T, TQ>(*pPtr);

            delete pPtr;
            pPtr = nullptr;

            SUTL_TEST_ASSERT(SingleFreeCounter( ) == expectedSingleFreeCount);
            SUTL_TEST_ASSERT(ArrayFreeCounter( ) == expectedArrayFreeCount);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR Free( )
        {
            constexpr size_t expectedSingleFreeCount = (TQ == TestQuantity::One) ? 1 : 0;
            constexpr size_t expectedArrayFreeCount = (TQ == TestQuantity::Many) ? 1 : 0;

            Pointer<T> ptr(GetTQNum<TQ>( ));

            ResetFreeCounters( );
            AssignFreeWrapper<T, TQ>(ptr);

            ptr.Free( );

            SUTL_TEST_ASSERT(SingleFreeCounter( ) == expectedSingleFreeCount);
            SUTL_TEST_ASSERT(ArrayFreeCounter( ) == expectedArrayFreeCount);

            SUTL_TEST_SUCCESS( );
        }
    };

    class PointerTests::ConstructorTests
    {
    public:

        template <typename T>
        static UTR DefaultCtor( )
        {
            Pointer<T> p;

            SUTL_TEST_ASSERT(p.Ptr( ) == nullptr);
            SUTL_TEST_ASSERT(p.Length( ) == 0);
            SUTL_TEST_ASSERT(p.Size( ) == 0);
            SUTL_TEST_ASSERT(p.m_FreeFunc == nullptr);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR LengthCtor( )
        {
            constexpr const bool bExpectNull = (TQ == TestQuantity::Zero);
            constexpr const size_t len = GetTQNum<TQ>( );
            Pointer<T> p(len);

            SUTL_TEST_ASSERT(!p.Ptr( ) == bExpectNull);
            SUTL_TEST_ASSERT(p.Length( ) == len);
            SUTL_TEST_ASSERT(p.Size( ) == (sizeof(T) * len));
            if constexpr ( bExpectNull )
            {
                SUTL_TEST_ASSERT(p.m_FreeFunc == nullptr);
            }
            else
            {
                SUTL_TEST_ASSERT(p.m_FreeFunc == ((len == 1) ? Pointer<T>::FreeSingle : Pointer<T>::FreeArray));
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR RawPointerCopyCtor( )
        {
            constexpr const bool bExpectNull = (TQ == TestQuantity::Zero);
            constexpr const size_t len = GetTQNum<TQ>( );
            const std::vector<T> testData = GetTestData<T, TQ>( );
            Pointer<T>* pPointer = nullptr;

            try
            {
                pPointer = new Pointer<T>(testData.data( ), testData.size( ));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(!pPointer->Ptr( ) == bExpectNull);
            SUTL_TEST_ASSERT(pPointer->Length( ) == len);
            SUTL_TEST_ASSERT(pPointer->Size( ) == (sizeof(T) * len));
            if constexpr ( bExpectNull )
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == nullptr);
            }
            else if constexpr ( len == 1 )
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == Pointer<T>::FreeArray);
            }

            for ( size_t i = 0; i < len; i++ )
            {
                SUTL_TEST_ASSERT(pPointer->Ptr( )[i] == testData[i]);
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    SUTL_TEST_ASSERT(pPointer->Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!pPointer->Ptr( )[i].Moved( ));
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR RawPointerStealCtor( )
        {
            constexpr const bool bExpectNull = (TQ == TestQuantity::Zero);
            constexpr const size_t len = GetTQNum<TQ>( );
            const std::vector<T> testData = GetTestData<T, TQ>( );
            T* pTestCopy = nullptr;
            T* pOrigPtr = nullptr;
            Pointer<T>* pPointer = nullptr;

            if constexpr ( !bExpectNull )
            {
                try
                {
                    pTestCopy = new T[len];
                    pOrigPtr = pTestCopy;
                    for ( size_t i = 0; i < len; i++ )
                    {
                        pTestCopy[i] = testData[i];
                    }
                }
                catch ( const std::exception& e )
                {
                    SUTL_SETUP_EXCEPTION(e.what( ));
                }
            }

            try
            {
                pPointer = new Pointer<T>(pTestCopy, len);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(!pPointer->Ptr( ) == bExpectNull);
            SUTL_TEST_ASSERT(pPointer->Ptr( ) == pOrigPtr);
            SUTL_TEST_ASSERT(!pTestCopy);
            SUTL_TEST_ASSERT(pPointer->Length( ) == len);
            SUTL_TEST_ASSERT(pPointer->Size( ) == (sizeof(T) * len));
            if constexpr ( bExpectNull )
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == nullptr);
            }
            else if constexpr ( len == 1 )
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == Pointer<T>::FreeArray);
            }

            for ( size_t i = 0; i < len; i++ )
            {
                SUTL_TEST_ASSERT(pPointer->Ptr( )[i] == testData[i]);
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    SUTL_TEST_ASSERT(pPointer->Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!pPointer->Ptr( )[i].Moved( ));
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR CopyCtor( )
        {
            constexpr const bool bExpectNull = (TQ == TestQuantity::Zero);
            constexpr const size_t len = GetTQNum<TQ>( );
            const std::vector<T> testData = GetTestData<T, TQ>( );

            Pointer<T>* pSrc = nullptr;
            Pointer<T>* pPointer = nullptr;

            try
            {
                pSrc = new Pointer<T>(testData.data( ), len);
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(!pSrc->Ptr( ) == bExpectNull);
            SUTL_TEST_ASSERT(pSrc->Length( ) == len);
            SUTL_TEST_ASSERT(pSrc->Size( ) == (sizeof(T) * len));
            if constexpr ( bExpectNull )
            {
                SUTL_TEST_ASSERT(pSrc->m_FreeFunc == nullptr);
            }
            else if constexpr ( len == 1 )
            {
                SUTL_TEST_ASSERT(pSrc->m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(pSrc->m_FreeFunc == Pointer<T>::FreeArray);
            }

            try
            {
                pPointer = new Pointer<T>(*pSrc);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(!pSrc->Ptr( ) == bExpectNull);
            SUTL_TEST_ASSERT(pSrc->Length( ) == len);
            SUTL_TEST_ASSERT(pSrc->Size( ) == (sizeof(T) * len));
            if constexpr ( bExpectNull )
            {
                SUTL_TEST_ASSERT(pSrc->m_FreeFunc == nullptr);
            }
            else if constexpr ( len == 1 )
            {
                SUTL_TEST_ASSERT(pSrc->m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(pSrc->m_FreeFunc == Pointer<T>::FreeArray);
            }

            SUTL_TEST_ASSERT(!pPointer->Ptr( ) == bExpectNull);
            SUTL_TEST_ASSERT((pPointer->Ptr( ) == pSrc->Ptr( )) == bExpectNull);
            SUTL_TEST_ASSERT(pPointer->Length( ) == len);
            SUTL_TEST_ASSERT(pPointer->Size( ) == (sizeof(T) * len));
            if constexpr ( bExpectNull )
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == nullptr);
            }
            else if constexpr ( len == 1 )
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == Pointer<T>::FreeArray);
            }

            for ( size_t i = 0; i < len; i++ )
            {
                SUTL_TEST_ASSERT(pPointer->Ptr( )[i] == pSrc->Ptr( )[i]);
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    SUTL_TEST_ASSERT(pPointer->Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!pPointer->Ptr( )[i].Moved( ));
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        static UTR MoveCtor( )
        {
            constexpr const bool bExpectNull = (TQ == TestQuantity::Zero);
            constexpr const size_t len = GetTQNum<TQ>( );
            const std::vector<T> testData = GetTestData<T, TQ>( );

            Pointer<T>* pSrc = nullptr;
            Pointer<T>* pPointer = nullptr;

            try
            {
                pSrc = new Pointer<T>(testData.data( ), len);
            }
            catch ( const std::exception& e )
            {
                SUTL_SETUP_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(!pSrc->Ptr( ) == bExpectNull);
            SUTL_TEST_ASSERT(pSrc->Length( ) == len);
            SUTL_TEST_ASSERT(pSrc->Size( ) == (sizeof(T) * len));
            if constexpr ( bExpectNull )
            {
                SUTL_TEST_ASSERT(pSrc->m_FreeFunc == nullptr);
            }
            else if constexpr ( len == 1 )
            {
                SUTL_TEST_ASSERT(pSrc->m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(pSrc->m_FreeFunc == Pointer<T>::FreeArray);
            }

            try
            {
                pPointer = new Pointer<T>(std::move(*pSrc));
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(!pSrc->Ptr( ));
            SUTL_TEST_ASSERT(pSrc->Length( ) == 0);
            SUTL_TEST_ASSERT(pSrc->Size( ) == 0);
            SUTL_TEST_ASSERT(pSrc->m_FreeFunc == nullptr);

            SUTL_TEST_ASSERT(!pPointer->Ptr( ) == bExpectNull);
            SUTL_TEST_ASSERT((pPointer->Ptr( ) == pSrc->Ptr( )) == bExpectNull);
            SUTL_TEST_ASSERT(pPointer->Length( ) == len);
            SUTL_TEST_ASSERT(pPointer->Size( ) == (sizeof(T) * len));
            if constexpr ( bExpectNull )
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == nullptr);
            }
            else if constexpr ( len == 1 )
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(pPointer->m_FreeFunc == Pointer<T>::FreeArray);
            }

            for ( size_t i = 0; i < len; i++ )
            {
                SUTL_TEST_ASSERT(pPointer->Ptr( )[i] == testData[i]);
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    SUTL_TEST_ASSERT(pPointer->Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!pPointer->Ptr( )[i].Moved( ));
                }
            }

            SUTL_TEST_SUCCESS( );
        }
    };

    class PointerTests::CopyTests
    {
    public:

        template <typename T, TestQuantity TQDst, TestQuantity TQSrc>
        static UTR CopyAssignment( )
        {
            constexpr const bool bExpectNullSrc = (TQSrc == TestQuantity::Zero);
            constexpr const bool bExpectNullDst = (TQDst == TestQuantity::Zero);
            constexpr const size_t lenSrc = GetTQNum<TQSrc>( );
            constexpr const size_t lenDst = GetTQNum<TQDst>( );
            const std::vector<T> testDataSrc = GetTestData<T, TQSrc>( );
            const std::vector<T> testDataDst = GetTestData<T, TQDst>( );

            Pointer<T> src(testDataSrc.data( ), lenSrc);
            Pointer<T> dst(testDataDst.data( ), lenDst);
            const T* pOrigSrc = src.Ptr( );
            const T* pOrigDst = dst.Ptr( );

            // Pointers should be different unless both should be nullptr.
            SUTL_TEST_ASSERT((pOrigSrc == pOrigDst) == (bExpectNullSrc && bExpectNullDst));

            // Ensure src state is as expected.
            SUTL_TEST_ASSERT(!src.Ptr( ) == bExpectNullSrc);
            SUTL_TEST_ASSERT(src.Length( ) == lenSrc);
            SUTL_TEST_ASSERT(src.Size( ) == (sizeof(T) * lenSrc));
            if constexpr ( bExpectNullSrc )
            {
                SUTL_TEST_ASSERT(src.m_FreeFunc == nullptr);
            }
            else if constexpr ( lenSrc == 1 )
            {
                SUTL_TEST_ASSERT(src.m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(src.m_FreeFunc == Pointer<T>::FreeArray);
            }

            for ( size_t i = 0; i < lenSrc; i++ )
            {
                SUTL_TEST_ASSERT(src.Ptr( )[i] == testDataSrc[i]);
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    SUTL_TEST_ASSERT(src.Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!src.Ptr( )[i].Moved( ));
                }
            }

            // Ensure dst state is as expected.
            SUTL_TEST_ASSERT(!dst.Ptr( ) == bExpectNullDst);
            SUTL_TEST_ASSERT(dst.Length( ) == lenDst);
            SUTL_TEST_ASSERT(dst.Size( ) == (sizeof(T) * lenDst));
            if constexpr ( bExpectNullDst )
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == nullptr);
            }
            else if constexpr ( lenDst == 1 )
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == Pointer<T>::FreeArray);
            }

            for ( size_t i = 0; i < lenDst; i++ )
            {
                SUTL_TEST_ASSERT(dst.Ptr( )[i] == testDataDst[i]);
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    SUTL_TEST_ASSERT(dst.Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!dst.Ptr( )[i].Moved( ));
                }
            }

            // Attempt copy assignment.
            try
            {
                dst = src;
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            // Ensure src state is as expected after copy.
            SUTL_TEST_ASSERT(src.Ptr( ) == pOrigSrc);
            SUTL_TEST_ASSERT(src.Length( ) == lenSrc);
            SUTL_TEST_ASSERT(src.Size( ) == (sizeof(T) * lenSrc));
            if constexpr ( bExpectNullSrc )
            {
                SUTL_TEST_ASSERT(src.m_FreeFunc == nullptr);
            }
            else if constexpr ( lenSrc == 1 )
            {
                SUTL_TEST_ASSERT(src.m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(src.m_FreeFunc == Pointer<T>::FreeArray);
            }

            for ( size_t i = 0; i < lenSrc; i++ )
            {
                SUTL_TEST_ASSERT(src.Ptr( )[i] == testDataSrc[i]);
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    SUTL_TEST_ASSERT(src.Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!src.Ptr( )[i].Moved( ));
                }
            }

            // Ensure dst state is as expected after copy.
            SUTL_TEST_ASSERT((dst.Ptr( ) == pOrigDst) == (bExpectNullSrc && bExpectNullDst));
            SUTL_TEST_ASSERT(dst.Length( ) == lenSrc);
            SUTL_TEST_ASSERT(dst.Size( ) == (sizeof(T) * lenSrc));
            if constexpr ( bExpectNullSrc )
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == nullptr);
            }
            else if constexpr ( lenSrc == 1 )
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == Pointer<T>::FreeArray);
            }

            for ( size_t i = 0; i < lenSrc; i++ )
            {
                SUTL_TEST_ASSERT(dst.Ptr( )[i] == src.Ptr( )[i]);
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    SUTL_TEST_ASSERT(dst.Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!dst.Ptr( )[i].Moved( ));
                }
            }

            SUTL_TEST_SUCCESS( );
        }
    };

    class PointerTests::MoveTests
    {
    public:

        template <typename T, TestQuantity TQDst, TestQuantity TQSrc>
        static UTR MoveAssignment( )
        {
            constexpr const bool bExpectNullSrc = (TQSrc == TestQuantity::Zero);
            constexpr const bool bExpectNullDst = (TQDst == TestQuantity::Zero);
            constexpr const size_t lenSrc = GetTQNum<TQSrc>( );
            constexpr const size_t lenDst = GetTQNum<TQDst>( );
            const std::vector<T> testDataSrc = GetTestData<T, TQSrc>( );
            const std::vector<T> testDataDst = GetTestData<T, TQDst>( );

            Pointer<T> src(testDataSrc.data( ), lenSrc);
            Pointer<T> dst(testDataDst.data( ), lenDst);
            const T* pOrigSrc = src.Ptr( );

            // Pointers should be different unless both should be nullptr.
            SUTL_TEST_ASSERT((pOrigSrc == dst.Ptr( )) == (bExpectNullSrc && bExpectNullDst));

            // Ensure src state is as expected.
            SUTL_TEST_ASSERT(!src.Ptr( ) == bExpectNullSrc);
            SUTL_TEST_ASSERT(src.Length( ) == lenSrc);
            SUTL_TEST_ASSERT(src.Size( ) == (sizeof(T) * lenSrc));
            if constexpr ( bExpectNullSrc )
            {
                SUTL_TEST_ASSERT(src.m_FreeFunc == nullptr);
            }
            else if constexpr ( lenSrc == 1 )
            {
                SUTL_TEST_ASSERT(src.m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(src.m_FreeFunc == Pointer<T>::FreeArray);
            }

            for ( size_t i = 0; i < lenSrc; i++ )
            {
                SUTL_TEST_ASSERT(src.Ptr( )[i] == testDataSrc[i]);
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    SUTL_TEST_ASSERT(src.Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!src.Ptr( )[i].Moved( ));
                }
            }

            // Ensure dst state is as expected.
            SUTL_TEST_ASSERT(!dst.Ptr( ) == bExpectNullDst);
            SUTL_TEST_ASSERT(dst.Length( ) == lenDst);
            SUTL_TEST_ASSERT(dst.Size( ) == (sizeof(T) * lenDst));
            if constexpr ( bExpectNullDst )
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == nullptr);
            }
            else if constexpr ( lenDst == 1 )
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == Pointer<T>::FreeArray);
            }

            for ( size_t i = 0; i < lenDst; i++ )
            {
                SUTL_TEST_ASSERT(dst.Ptr( )[i] == testDataDst[i]);
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    SUTL_TEST_ASSERT(dst.Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!dst.Ptr( )[i].Moved( ));
                }
            }

            // Attempt move assignment.
            try
            {
                dst = std::move(src);
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            // Ensure src state is as expected after move.
            SUTL_TEST_ASSERT(src.Ptr( ) == nullptr);
            SUTL_TEST_ASSERT(src.Length( ) == 0);
            SUTL_TEST_ASSERT(src.Size( ) == 0);
            SUTL_TEST_ASSERT(src.m_FreeFunc == nullptr);

            // Ensure dst state is as expected after move.
            SUTL_TEST_ASSERT(dst.Ptr( ) == pOrigSrc);
            SUTL_TEST_ASSERT(dst.Length( ) == lenSrc);
            SUTL_TEST_ASSERT(dst.Size( ) == (sizeof(T) * lenSrc));
            if constexpr ( bExpectNullSrc )
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == nullptr);
            }
            else if constexpr ( lenSrc == 1 )
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == Pointer<T>::FreeSingle);
            }
            else
            {
                SUTL_TEST_ASSERT(dst.m_FreeFunc == Pointer<T>::FreeArray);
            }

            for ( size_t i = 0; i < lenSrc; i++ )
            {
                SUTL_TEST_ASSERT(dst.Ptr( )[i] == testDataSrc[i]);
                if constexpr ( std::is_same_v<T, Helper> )
                {
                    SUTL_TEST_ASSERT(dst.Ptr( )[i].Copied( ));
                    SUTL_TEST_ASSERT(!dst.Ptr( )[i].Moved( ));
                }
            }

            SUTL_TEST_SUCCESS( );
        }
    };

    class PointerTests::DereferenceOperatorTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR Dereference( )
        {
            constexpr const bool bExpectNull = (TQ == TestQuantity::Zero);
            constexpr const size_t len = GetTQNum<TQ>( );
            std::vector<T> testData = GetTestData<T, TQ>( );
            bool bThrew = false;

            Pointer<T> p(testData.data( ), len);
            T tVal = T( );

            try
            {
                tVal = *p;
            }
            catch ( const std::logic_error& )
            {
                bThrew = true;
            }
            catch ( const std::exception& e )
            {
                SUTL_TEST_EXCEPTION(e.what( ));
            }

            SUTL_TEST_ASSERT(bThrew == bExpectNull);
            if constexpr ( !bExpectNull )
            {
                SUTL_TEST_ASSERT(tVal == testData.front( ));
                SUTL_TEST_ASSERT(tVal == p.Ptr( )[0]);
            }

            if constexpr ( std::is_same_v<T, Helper> )
            {
                bool bCopied = false;
                bool bMoved = false;

                bThrew = false;

                try
                {
                    bCopied = p->Copied( );
                    bMoved = p->Moved( );
                }
                catch ( const std::logic_error& )
                {
                    bThrew = true;
                }
                catch ( const std::exception& e )
                {
                    SUTL_TEST_EXCEPTION(e.what( ));
                }

                SUTL_TEST_ASSERT(bThrew == bExpectNull);
                if constexpr ( !bExpectNull )
                {
                    SUTL_TEST_ASSERT(bCopied);
                    SUTL_TEST_ASSERT(!bMoved);
                }
            }

            SUTL_TEST_SUCCESS( );
        }
    };
}