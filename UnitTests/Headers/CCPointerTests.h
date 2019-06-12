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

            Helper& operator=(Helper&& src)
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
                    && memcmp(m_u8Arr, rhs.m_u8Arr, s_ArrSize) == 0;
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
        class SubscriptOperatorTests;

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
        static UTR CopyPointer( )
        {
            constexpr size_t expectedSingleFreeCount = (TQ == TestQuantity::One) ? 1 : 0;
            constexpr size_t expectedArrayFreeCount = (TQ == TestQuantity::Many) ? 1 : 0;

            Pointer<T> ptr(GetTQNum<TQ>( ));
            Pointer<T> copyPtr(GetTQNum<TQ>( ));

            ResetFreeCounters( );
            AssignFreeWrapper<T, TQ>(ptr);

            try
            {
                Pointer<T>::CopyPointerObj(ptr, copyPtr);
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
        static UTR TransferPointer( )
        {
            constexpr size_t expectedSingleFreeCount = (TQ == TestQuantity::One) ? 1 : 0;
            constexpr size_t expectedArrayFreeCount = (TQ == TestQuantity::Many) ? 1 : 0;

            Pointer<T> ptr(GetTQNum<TQ>( ));
            Pointer<T> transferPtr(GetTQNum<TQ>( ));

            ResetFreeCounters( );
            AssignFreeWrapper<T, TQ>(ptr);

            try
            {
                Pointer<T>::TransferPointerObj(ptr, transferPtr);
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
}