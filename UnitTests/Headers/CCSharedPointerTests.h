#pragma once

// SUTL
#include <UnitTestResult.h>

// STL
#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <thread>

// Target Class
#include <CCSharedPointer.h>

// Test Helper Utils
#include <CCPointerCommonHelpersTests.h>

namespace CC
{
    class SharedPointerTests
    {
        SharedPointerTests( ) = delete;
        SharedPointerTests(const SharedPointerTests&) = delete;
        SharedPointerTests(SharedPointerTests&&) = delete;
        ~SharedPointerTests( ) = delete;
        SharedPointerTests& operator=(const SharedPointerTests&) = delete;
        SharedPointerTests& operator=(SharedPointerTests&&) = delete;

    private:

        // Type aliases
        using PCHT = PointerCommonHelpersTests;
        using TestQuantity = PCHT::TestQuantity;
        using Helper = PCHT::Helper;

        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;

        // Helper class
        template <class T>
        class CleanupHelper
        {
        private:
            T m_Data;
            bool m_bCopied;
            bool m_bMoved;

        public:

            CleanupHelper( ) noexcept :
                m_Data(T( )),
                m_bCopied(false),
                m_bMoved(false)
            { }

            CleanupHelper(const CleanupHelper<T>& src) noexcept :
                m_Data(src.m_Data),
                m_bCopied(true),
                m_bMoved(false)
            { }

            CleanupHelper(CleanupHelper<T>&& src) noexcept :
                m_Data(std::move(src.m_Data)),
                m_bCopied(false),
                m_bMoved(true)
            { }

            ~CleanupHelper( ) noexcept
            {
                GetDestructionCounter( )++;
            }

            CleanupHelper<T>& operator=(const CleanupHelper<T>& src) noexcept
            {
                if ( this != &src )
                {
                    m_Data = src.m_Data;
                    m_bCopied = true;
                    m_bMoved = false;
                }

                return *this;
            }

            CleanupHelper<T>& operator=(CleanupHelper<T>&& src) noexcept
            {
                if ( this != &src )
                {
                    m_Data = std::move(src.m_Data);
                    m_bCopied = false;
                    m_bMoved = true;
                }

                return *this;
            }

            static size_t& GetDestructionCounter( ) noexcept
            {
                static size_t sCounter = 0;
                return sCounter;
            }

            T& GetData( ) noexcept
            {
                return m_Data;
            }

            const T& GetData( ) const noexcept
            {
                return m_Data;
            }

            const bool& Copied( ) const noexcept
            {
                return m_bCopied;
            }

            const bool& Moved( ) const noexcept
            {
                return m_bMoved;
            }
        };

        /// Test Subclasses \\\

        class ConstructorTests;

        class DestructorTests;

        class AssignmentTests;

        class RefCountTests;

    public:

        // Returns list of SharedPointer unit tests.
        static UTList GetTests( );
    };

    class SharedPointerTests::ConstructorTests
    {
    public:

        template <typename T>
        [[nodiscard]] static UTR DefaultCtor( )
        {
            SharedPointer<T> p;

            SUTL_TEST_ASSERT(!p.m_pPtr);
            SUTL_TEST_ASSERT(!p.m_pRefCount);
            SUTL_TEST_ASSERT(p.m_Len == 0);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR LengthCtor( )
        {
            constexpr size_t len = PCHT::GetTQNum<TQ>( );
            SharedPointer<T> p(len);

            if constexpr ( len == 0 )
            {
                SUTL_TEST_ASSERT(!p.m_pPtr);
                SUTL_TEST_ASSERT(!p.m_pRefCount);
                SUTL_TEST_ASSERT(p.m_Len == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(!!p.m_pPtr);
                SUTL_TEST_ASSERT(!!p.m_pRefCount);
                SUTL_TEST_ASSERT((*p.m_pRefCount) == 1);
                SUTL_TEST_ASSERT(p.m_Len == len);
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR RawPointerCopyCtor( )
        {
            constexpr size_t len = PCHT::GetTQNum<TQ>( );
            std::vector<T> testData(PCHT::GetTestData<T, TQ>( ));
            SharedPointer<T> p(testData.data( ), len);

            if constexpr ( len == 0 )
            {
                SUTL_TEST_ASSERT(!p.m_pPtr);
                SUTL_TEST_ASSERT(!p.m_pRefCount);
                SUTL_TEST_ASSERT(p.m_Len == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(!!p.m_pPtr);
                SUTL_TEST_ASSERT(!!p.m_pRefCount);
                SUTL_TEST_ASSERT((*p.m_pRefCount) == 1);
                SUTL_TEST_ASSERT(p.m_Len == len);

                if constexpr ( std::is_same_v<T, Helper> )
                {
                    for ( size_t i = 0; i < len; i++ )
                    {
                        SUTL_TEST_ASSERT(p.m_pPtr[i] == testData[i]);
                        SUTL_TEST_ASSERT(p.m_pPtr[i].Copied( ));
                        SUTL_TEST_ASSERT(!p.m_pPtr[i].Moved( ));
                    }
                }
                else
                {
                    SUTL_TEST_ASSERT(memcmp(p.m_pPtr, testData.data( ), sizeof(T) * len) == 0);
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR RawPointerStealCtor( )
        {
            constexpr size_t len = PCHT::GetTQNum<TQ>( );
            std::vector<T> testData(PCHT::GetTestData<T, TQ>( ));
            T* pRawPtrCopy = PointerCommonHelpers<T>::AllocateFromRawPointer(testData.data( ), len);
            T* pStealPtr = pRawPtrCopy;
            SharedPointer<T> p(pStealPtr, len);

            SUTL_TEST_ASSERT(!pStealPtr);

            if constexpr ( len == 0 )
            {
                SUTL_TEST_ASSERT(!pRawPtrCopy);
                SUTL_TEST_ASSERT(!p.m_pPtr);
                SUTL_TEST_ASSERT(!p.m_pRefCount);
                SUTL_TEST_ASSERT(p.m_Len == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(!!p.m_pPtr);
                SUTL_TEST_ASSERT(!!p.m_pRefCount);
                SUTL_TEST_ASSERT((*p.m_pRefCount) == 1);
                SUTL_TEST_ASSERT(p.m_Len == len);

                SUTL_TEST_ASSERT(p.m_pPtr == pRawPtrCopy);

                if constexpr ( std::is_same_v<T, Helper> )
                {
                    for ( size_t i = 0; i < len; i++ )
                    {
                        SUTL_TEST_ASSERT(p.m_pPtr[i] == testData[i]);
                        SUTL_TEST_ASSERT(p.m_pPtr[i].Copied( ));
                        SUTL_TEST_ASSERT(!p.m_pPtr[i].Moved( ));
                    }
                }
                else
                {
                    SUTL_TEST_ASSERT(memcmp(p.m_pPtr, testData.data( ), sizeof(T) * len) == 0);
                }
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CopyCtor( )
        {
            constexpr size_t len = PCHT::GetTQNum<TQ>( );
            SharedPointer<T> p1(len);
            SharedPointer<T> p2(p1);

            SUTL_TEST_ASSERT(p1.m_pPtr == p2.m_pPtr);
            SUTL_TEST_ASSERT(p1.m_pRefCount == p2.m_pRefCount);
            SUTL_TEST_ASSERT(p1.m_Len == p2.m_Len);

            if constexpr ( len == 0 )
            {
                SUTL_TEST_ASSERT(!p2.m_pPtr);
                SUTL_TEST_ASSERT(!p2.m_pRefCount);
                SUTL_TEST_ASSERT(p2.m_Len == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(!!p2.m_pPtr);
                SUTL_TEST_ASSERT(!!p2.m_pRefCount);
                SUTL_TEST_ASSERT((*p2.m_pRefCount) == 2);
                SUTL_TEST_ASSERT(p2.m_Len == len);
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR MoveCtor( )
        {
            constexpr size_t len = PCHT::GetTQNum<TQ>( );
            SharedPointer<T> p1(len);
            T* pOrigPtr = p1.m_pPtr;
            auto pOrigRefCounter = p1.m_pRefCount;
            size_t origLen = p1.m_Len;

            SharedPointer<T> p2(std::move(p1));

            SUTL_TEST_ASSERT(!p1.m_pPtr);
            SUTL_TEST_ASSERT(!p1.m_pRefCount);
            SUTL_TEST_ASSERT(p1.m_Len == 0);

            SUTL_TEST_ASSERT(p2.m_pPtr == pOrigPtr);
            SUTL_TEST_ASSERT(p2.m_pRefCount == pOrigRefCounter);
            SUTL_TEST_ASSERT(p2.m_Len == origLen);

            if constexpr ( len == 0 )
            {
                SUTL_TEST_ASSERT(!p2.m_pPtr);
                SUTL_TEST_ASSERT(!p2.m_pRefCount);
                SUTL_TEST_ASSERT(p2.m_Len == 0);
            }
            else
            {
                SUTL_TEST_ASSERT(p2.m_pPtr);
                SUTL_TEST_ASSERT(!!p2.m_pRefCount);
                SUTL_TEST_ASSERT((*p2.m_pRefCount) == 1);
                SUTL_TEST_ASSERT(p2.m_Len == len);
            }

            SUTL_TEST_SUCCESS( );
        }
    };

    class SharedPointerTests::DestructorTests
    {
    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR Dtor( )
        {
            constexpr size_t len = PCHT::GetTQNum<TQ>( );
            constexpr size_t arrLen = PCHT::GetTQNum<TestQuantity::Many>( );

            CleanupHelper<T>::GetDestructionCounter( ) = 0;
            SharedPointer<CleanupHelper<T>> p(len);
            std::vector<SharedPointer<CleanupHelper<T>>> arr;

            SUTL_SETUP_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);

            for ( size_t i = 0; i < arrLen; i++ )
            {
                arr.push_back(p);
            }

            for ( size_t i = 0; i < arrLen; i++ )
            {
                SUTL_SETUP_ASSERT(p.m_pPtr == arr[i].m_pPtr);
                SUTL_SETUP_ASSERT(p.m_pRefCount == arr[i].m_pRefCount);
                SUTL_SETUP_ASSERT(p.m_Len == arr[i].m_Len);
            }

            if constexpr ( len == 0 )
            {
                SUTL_SETUP_ASSERT(!p.m_pPtr);
                SUTL_SETUP_ASSERT(!p.m_pRefCount);
                SUTL_SETUP_ASSERT(p.m_Len == 0);
            }
            else
            {
                SUTL_SETUP_ASSERT(!!p.m_pPtr);
                SUTL_SETUP_ASSERT(!!p.m_pRefCount);
                SUTL_SETUP_ASSERT((*p.m_pRefCount) == arrLen + 1);
                SUTL_SETUP_ASSERT(p.m_Len == len);
            }

            for ( size_t i = 0; i < arrLen; i++ )
            {
                arr[i].~SharedPointer( );
                SUTL_TEST_ASSERT(!p.m_pRefCount || (static_cast<size_t>(*p.m_pRefCount) == arrLen - i));
                SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);
            }

            p.~SharedPointer( );
            SUTL_SETUP_ASSERT(!p.m_pPtr);
            SUTL_SETUP_ASSERT(!p.m_pRefCount);
            SUTL_SETUP_ASSERT(p.m_Len == 0);
            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == len);

            CleanupHelper<T>::GetDestructionCounter( ) = 0;

            SUTL_TEST_SUCCESS( );
        }
    };

    class SharedPointerTests::AssignmentTests
    {
    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CopyAssignment( )
        {
            constexpr size_t len = PCHT::GetTQNum<TQ>( );
            SharedPointer<CleanupHelper<T>> p1(len);
            SharedPointer<CleanupHelper<T>> p2;
            SharedPointer<CleanupHelper<T>> p3;
            SharedPointer<CleanupHelper<T>> p4;

            CleanupHelper<T>::GetDestructionCounter( ) = 0;

            SUTL_SETUP_ASSERT(!p2.m_pPtr);
            SUTL_SETUP_ASSERT(!p2.m_pRefCount);
            SUTL_SETUP_ASSERT(p2.m_Len == 0);
            SUTL_SETUP_ASSERT(!p3.m_pPtr);
            SUTL_SETUP_ASSERT(!p3.m_pRefCount);
            SUTL_SETUP_ASSERT(p3.m_Len == 0);
            SUTL_SETUP_ASSERT(!p4.m_pPtr);
            SUTL_SETUP_ASSERT(!p4.m_pRefCount);
            SUTL_SETUP_ASSERT(p4.m_Len == 0);

            if constexpr ( len == 0 )
            {
                SUTL_SETUP_ASSERT(!p1.m_pPtr);
                SUTL_SETUP_ASSERT(!p1.m_pRefCount);
                SUTL_SETUP_ASSERT(p1.m_Len == 0);
            }
            else
            {
                SUTL_SETUP_ASSERT(!!p1.m_pPtr);
                SUTL_SETUP_ASSERT(!!p1.m_pRefCount);
                SUTL_SETUP_ASSERT(p1.m_Len == len);
            }

            p2 = p1;

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);

            SUTL_TEST_ASSERT(p2.m_pPtr == p1.m_pPtr);
            SUTL_TEST_ASSERT(p2.m_pRefCount == p1.m_pRefCount);
            SUTL_TEST_ASSERT(p2.m_Len == p1.m_Len);

            SUTL_TEST_ASSERT(!p2.m_pRefCount || (*p2.m_pRefCount) == 2);

            p3 = p2;

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);

            SUTL_TEST_ASSERT(p3.m_pPtr == p2.m_pPtr);
            SUTL_TEST_ASSERT(p3.m_pRefCount == p2.m_pRefCount);
            SUTL_TEST_ASSERT(p3.m_Len == p2.m_Len);

            SUTL_TEST_ASSERT(!p3.m_pRefCount || (*p3.m_pRefCount) == 3);

            p3 = p1;

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);

            SUTL_TEST_ASSERT(p3.m_pPtr == p1.m_pPtr);
            SUTL_TEST_ASSERT(p3.m_pRefCount == p1.m_pRefCount);
            SUTL_TEST_ASSERT(p3.m_Len == p1.m_Len);

            SUTL_TEST_ASSERT(!p3.m_pRefCount || (*p3.m_pRefCount) == 3);

            p3 = p4;

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);

            SUTL_TEST_ASSERT(!p3.m_pPtr);
            SUTL_TEST_ASSERT(!p3.m_pRefCount);
            SUTL_TEST_ASSERT(p3.m_Len == 0);

            SUTL_TEST_ASSERT(!p1.m_pRefCount || (*p1.m_pRefCount) == 2);

            p2 = p4;

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);

            SUTL_TEST_ASSERT(!p2.m_pPtr);
            SUTL_TEST_ASSERT(!p2.m_pRefCount);
            SUTL_TEST_ASSERT(p2.m_Len == 0);

            SUTL_TEST_ASSERT(!p1.m_pRefCount || (*p1.m_pRefCount) == 1);

            p1 = p4;

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == len);

            SUTL_TEST_ASSERT(!p1.m_pPtr);
            SUTL_TEST_ASSERT(!p1.m_pRefCount);
            SUTL_TEST_ASSERT(p1.m_Len == 0);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR MoveAssignment( )
        {
            constexpr size_t len = PCHT::GetTQNum<TQ>( );
            SharedPointer<CleanupHelper<T>> p1(len);
            SharedPointer<CleanupHelper<T>> p2;
            SharedPointer<CleanupHelper<T>> p3;
            SharedPointer<CleanupHelper<T>> p4;

            CleanupHelper<T>* pOrig = p1.m_pPtr;
            auto pOrigRefCount = p1.m_pRefCount;

            CleanupHelper<T>::GetDestructionCounter( ) = 0;

            SUTL_SETUP_ASSERT(!p2.m_pPtr);
            SUTL_SETUP_ASSERT(!p2.m_pRefCount);
            SUTL_SETUP_ASSERT(p2.m_Len == 0);
            SUTL_SETUP_ASSERT(!p3.m_pPtr);
            SUTL_SETUP_ASSERT(!p3.m_pRefCount);
            SUTL_SETUP_ASSERT(p3.m_Len == 0);
            SUTL_SETUP_ASSERT(!p4.m_pPtr);
            SUTL_SETUP_ASSERT(!p4.m_pRefCount);
            SUTL_SETUP_ASSERT(p4.m_Len == 0);

            if constexpr ( len == 0 )
            {
                SUTL_SETUP_ASSERT(!p1.m_pPtr);
                SUTL_SETUP_ASSERT(!p1.m_pRefCount);
                SUTL_SETUP_ASSERT(p1.m_Len == 0);
            }
            else
            {
                SUTL_SETUP_ASSERT(!!p1.m_pPtr);
                SUTL_SETUP_ASSERT(!!p1.m_pRefCount);
                SUTL_SETUP_ASSERT(p1.m_Len == len);
            }

            p2 = std::move(p1);

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);

            SUTL_SETUP_ASSERT(!p1.m_pPtr);
            SUTL_SETUP_ASSERT(!p1.m_pRefCount);
            SUTL_SETUP_ASSERT(p1.m_Len == 0);

            SUTL_TEST_ASSERT(p2.m_pPtr == pOrig);
            SUTL_TEST_ASSERT(p2.m_pRefCount == pOrigRefCount);
            SUTL_TEST_ASSERT(p2.m_Len == len);

            SUTL_TEST_ASSERT(!p2.m_pRefCount || (*p2.m_pRefCount) == 1);

            p3 = std::move(p2);

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);

            SUTL_SETUP_ASSERT(!p2.m_pPtr);
            SUTL_SETUP_ASSERT(!p2.m_pRefCount);
            SUTL_SETUP_ASSERT(p2.m_Len == 0);

            SUTL_TEST_ASSERT(p3.m_pPtr == pOrig);
            SUTL_TEST_ASSERT(p3.m_pRefCount == pOrigRefCount);
            SUTL_TEST_ASSERT(p3.m_Len == len);

            SUTL_TEST_ASSERT(!p3.m_pRefCount || (*p3.m_pRefCount) == 1);

            p4 = std::move(p3);

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);

            SUTL_SETUP_ASSERT(!p3.m_pPtr);
            SUTL_SETUP_ASSERT(!p3.m_pRefCount);
            SUTL_SETUP_ASSERT(p3.m_Len == 0);

            SUTL_TEST_ASSERT(p4.m_pPtr == pOrig);
            SUTL_TEST_ASSERT(p4.m_pRefCount == pOrigRefCount);
            SUTL_TEST_ASSERT(p4.m_Len == len);

            SUTL_TEST_ASSERT(!p4.m_pRefCount || (*p4.m_pRefCount) == 1);

            p4 = std::move(p1);

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == len);

            SUTL_TEST_ASSERT(!p4.m_pPtr);
            SUTL_TEST_ASSERT(!p4.m_pRefCount);
            SUTL_TEST_ASSERT(p4.m_Len == 0);

            CleanupHelper<T>::GetDestructionCounter( ) = 0;

            SUTL_TEST_SUCCESS( );
        }
    };

    class SharedPointerTests::RefCountTests
    {
    private:

        template <typename T>
        static void Inc(_Inout_ SharedPointer<CleanupHelper<T>>& dst, _In_ const SharedPointer<CleanupHelper<T>>& src)
        {
            if ( dst )
            {
                throw std::invalid_argument(__FUNCSIG__": SharedPointer dst is not empty.");
            }

            dst = src;
        }

        template <typename T>
        static void Dec(_Inout_ SharedPointer<CleanupHelper<T>>& dst)
        {
            dst = SharedPointer<CleanupHelper<T>>( );
        }

        template <typename T>
        static void IncAndDec(_Inout_ SharedPointer<CleanupHelper<T>>& dst, _In_ const SharedPointer<CleanupHelper<T>>& src)
        {
            Inc<T>(dst, src);
            Dec<T>(dst);
        }

    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR MTIncrement( )
        {
            constexpr size_t ptrLen = PCHT::GetTQNum<TQ>( );
            constexpr size_t iterations = 16;
            const size_t arrLen = static_cast<size_t>(std::thread::hardware_concurrency( ) << 1);

            SharedPointer<CleanupHelper<T>> p;
            std::vector<SharedPointer<CleanupHelper<T>>> sPArr(arrLen);
            std::vector<std::thread> tArr(arrLen);

            CleanupHelper<T>::GetDestructionCounter( ) = 0;

            p = SharedPointer<CleanupHelper<T>>(ptrLen);
            
            if constexpr ( ptrLen == 0 )
            {
                SUTL_SETUP_ASSERT(!p.m_pPtr);
                SUTL_SETUP_ASSERT(!p.m_pRefCount);
                SUTL_SETUP_ASSERT(p.m_Len == 0);
            }
            else
            {
                SUTL_SETUP_ASSERT(!!p.m_pPtr);
                SUTL_SETUP_ASSERT(!!p.m_pRefCount);
                SUTL_SETUP_ASSERT(p.m_Len == ptrLen);
            }

            for ( size_t c = 0; c < iterations; c++ )
            {
                // Launch threads.
                for ( size_t i = 0; i < arrLen; i++ )
                {
                    tArr[i] = std::thread(Inc<T>, std::ref(sPArr[i]), std::ref(p));
                    SUTL_SETUP_ASSERT(tArr[i].joinable( ));
                }

                // Join threads.
                for ( size_t i = 0; i < arrLen; i++ )
                {
                    tArr[i].join( );
                }

                // Validate current ref count (if applicable).
                if constexpr ( ptrLen != 0 )
                {
                    SUTL_TEST_ASSERT(static_cast<size_t>(*p.m_pRefCount) == (arrLen + 1));
                }

                // Validate pointer values in sPArr.
                for ( size_t i = 0; i < arrLen; i++ )
                {
                    SharedPointer<CleanupHelper<T>>& testPtr = sPArr[i];
                    SUTL_TEST_ASSERT(testPtr.m_pPtr == p.m_pPtr);
                    SUTL_TEST_ASSERT(testPtr.m_pRefCount == p.m_pRefCount);
                    SUTL_TEST_ASSERT(testPtr.m_Len == p.m_Len);

                    if constexpr ( ptrLen == 0 )
                    {
                        SUTL_TEST_ASSERT(!testPtr.m_pPtr);
                        SUTL_TEST_ASSERT(!testPtr.m_pRefCount);
                        SUTL_TEST_ASSERT(testPtr.m_Len == 0);
                    }
                    else
                    {
                        SUTL_TEST_ASSERT(!!testPtr.m_pPtr);
                        SUTL_TEST_ASSERT(!!testPtr.m_pRefCount);
                        SUTL_TEST_ASSERT(testPtr.m_Len == ptrLen);

                        testPtr.Free( );

                        SUTL_TEST_ASSERT(!testPtr.m_pPtr);
                        SUTL_TEST_ASSERT(!testPtr.m_pRefCount);
                        SUTL_TEST_ASSERT(testPtr.m_Len == 0);
                        SUTL_TEST_ASSERT(static_cast<size_t>(*p.m_pRefCount) == (arrLen - i));
                    }
                }

                // Validate current ref count (if applicable).
                if constexpr ( ptrLen != 0 )
                {
                    SUTL_TEST_ASSERT((*p.m_pRefCount) == 1);
                }

                SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);
            }

            if constexpr ( ptrLen != 0 )
            {
                p.Free( );
                SUTL_TEST_ASSERT(!p.m_pPtr);
                SUTL_TEST_ASSERT(!p.m_pRefCount);
                SUTL_TEST_ASSERT(p.m_Len == 0);
            }

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == ptrLen);

            CleanupHelper<T>::GetDestructionCounter( ) = 0;

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR MTDecrement( )
        {
            constexpr size_t ptrLen = PCHT::GetTQNum<TQ>( );
            constexpr size_t iterations = 8;
            const size_t arrLen = static_cast<size_t>(std::thread::hardware_concurrency( ) << 1);

            SharedPointer<CleanupHelper<T>> p;
            std::vector<SharedPointer<CleanupHelper<T>>> sPArr(arrLen);
            std::vector<std::thread> tArr(arrLen);

            CleanupHelper<T>::GetDestructionCounter( ) = 0;

            p = SharedPointer<CleanupHelper<T>>(ptrLen);

            if constexpr ( ptrLen == 0 )
            {
                SUTL_SETUP_ASSERT(!p.m_pPtr);
                SUTL_SETUP_ASSERT(!p.m_pRefCount);
                SUTL_SETUP_ASSERT(p.m_Len == 0);
            }
            else
            {
                SUTL_SETUP_ASSERT(!!p.m_pPtr);
                SUTL_SETUP_ASSERT(!!p.m_pRefCount);
                SUTL_SETUP_ASSERT(p.m_Len == ptrLen);
            }

            for ( size_t c = 0; c < iterations; c++ )
            {
                // Copy SharedPointers, validate incremented ref count (if applicable).
                for ( size_t i = 0; i < arrLen; i++ )
                {
                    SharedPointer<CleanupHelper<T>>& testPtr = sPArr[i];
                    testPtr = p;

                    SUTL_TEST_ASSERT(testPtr.m_pPtr == p.m_pPtr);
                    SUTL_TEST_ASSERT(testPtr.m_pRefCount == p.m_pRefCount);
                    SUTL_TEST_ASSERT(testPtr.m_Len == p.m_Len);

                    if constexpr ( ptrLen == 0 )
                    {
                        SUTL_TEST_ASSERT(!testPtr.m_pPtr);
                        SUTL_TEST_ASSERT(!testPtr.m_pRefCount);
                        SUTL_TEST_ASSERT(testPtr.m_Len == 0);
                    }
                    else
                    {
                        SUTL_TEST_ASSERT(!!testPtr.m_pPtr);
                        SUTL_TEST_ASSERT(!!testPtr.m_pRefCount);
                        SUTL_TEST_ASSERT(testPtr.m_Len == ptrLen);
                        SUTL_TEST_ASSERT(static_cast<size_t>(*p.m_pRefCount) == ((i + 1) + 1));
                    }
                }

                // Launch threads.
                for ( size_t i = 0; i < arrLen; i++ )
                {
                    tArr[i] = std::thread(Dec<T>, std::ref(sPArr[i]));
                    SUTL_SETUP_ASSERT(tArr[i].joinable( ));
                }

                // Join threads.
                for ( size_t i = 0; i < arrLen; i++ )
                {
                    tArr[i].join( );
                }

                // Validate current ref count (if applicable).
                if constexpr ( ptrLen != 0 )
                {
                    SUTL_TEST_ASSERT((*p.m_pRefCount) == 1);
                }

                // Validate pointer values in sPArr.
                for ( size_t i = 0; i < arrLen; i++ )
                {
                    SUTL_TEST_ASSERT(!sPArr[i].m_pPtr);
                    SUTL_TEST_ASSERT(!sPArr[i].m_pRefCount);
                    SUTL_TEST_ASSERT(sPArr[i].m_Len == 0);
                }

                // Validate current ref count (if applicable).
                if constexpr ( ptrLen != 0 )
                {
                    SUTL_TEST_ASSERT((*p.m_pRefCount) == 1);
                }

                SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);
            }

            if constexpr ( ptrLen != 0 )
            {
                p.Free( );
                SUTL_TEST_ASSERT(!p.m_pPtr);
                SUTL_TEST_ASSERT(!p.m_pRefCount);
                SUTL_TEST_ASSERT(p.m_Len == 0);
            }

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == ptrLen);

            CleanupHelper<T>::GetDestructionCounter( ) = 0;

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR MTIncrementAndDecrement( )
        {
            constexpr size_t ptrLen = PCHT::GetTQNum<TQ>( );
            constexpr size_t iterations = 8;
            const size_t arrLen = static_cast<size_t>(std::thread::hardware_concurrency( ) << 1);

            SharedPointer<CleanupHelper<T>> p;
            std::vector<SharedPointer<CleanupHelper<T>>> sPArr(arrLen);
            std::vector<std::thread> tArr(arrLen);

            CleanupHelper<T>::GetDestructionCounter( ) = 0;

            p = SharedPointer<CleanupHelper<T>>(ptrLen);

            if constexpr ( ptrLen == 0 )
            {
                SUTL_SETUP_ASSERT(!p.m_pPtr);
                SUTL_SETUP_ASSERT(!p.m_pRefCount);
                SUTL_SETUP_ASSERT(p.m_Len == 0);
            }
            else
            {
                SUTL_SETUP_ASSERT(!!p.m_pPtr);
                SUTL_SETUP_ASSERT(!!p.m_pRefCount);
                SUTL_SETUP_ASSERT(p.m_Len == ptrLen);
            }

            for ( size_t c = 0; c < iterations; c++ )
            {
                // Launch threads.
                for ( size_t i = 0; i < arrLen; i++ )
                {
                    tArr[i] = std::thread(IncAndDec<T>, std::ref(sPArr[i]), std::ref(p));
                    SUTL_SETUP_ASSERT(tArr[i].joinable( ));
                }

                // Join threads.
                for ( size_t i = 0; i < arrLen; i++ )
                {
                    tArr[i].join( );
                }

                // Validate current ref count (if applicable).
                if constexpr ( ptrLen != 0 )
                {
                    SUTL_TEST_ASSERT((*p.m_pRefCount) == 1);
                }

                // Validate pointer values in sPArr.
                for ( size_t i = 0; i < arrLen; i++ )
                {
                    SUTL_TEST_ASSERT(!sPArr[i].m_pPtr);
                    SUTL_TEST_ASSERT(!sPArr[i].m_pRefCount);
                    SUTL_TEST_ASSERT(sPArr[i].m_Len == 0);
                }

                // Validate current ref count (if applicable).
                if constexpr ( ptrLen != 0 )
                {
                    SUTL_TEST_ASSERT((*p.m_pRefCount) == 1);
                }

                SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == 0);
            }

            if constexpr ( ptrLen != 0 )
            {
                p.Free( );
                SUTL_TEST_ASSERT(!p.m_pPtr);
                SUTL_TEST_ASSERT(!p.m_pRefCount);
                SUTL_TEST_ASSERT(p.m_Len == 0);
            }

            SUTL_TEST_ASSERT(CleanupHelper<T>::GetDestructionCounter( ) == ptrLen);

            CleanupHelper<T>::GetDestructionCounter( ) = 0;

            SUTL_TEST_SUCCESS( );
        }
    };
}