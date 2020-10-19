#pragma once

// SUTL
#include <UnitTestResult.h>
#include <TestTypes.h>

// STL
#include <algorithm>
#include <functional>
#include <list>
#include <memory>

// Target Class
#include <CCPointer.h>

// Test Helper Utils
#include <TestHelpers.hpp>


namespace CC
{
    class PointerTests
    {
        PointerTests() = delete;
        PointerTests(const PointerTests&) = delete;
        PointerTests(PointerTests&&) = delete;
        ~PointerTests() = delete;
        PointerTests& operator=(const PointerTests&) = delete;
        PointerTests& operator=(PointerTests&&) = delete;

    private:

        // Type aliases
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;

        // Test Subclasses //

        // Tests methods related to freeing memory.
        class DeallocationTests;

        // Tests constructor behavior.
        class ConstructorTests;

        // Tests op-overloads and other methods related to copying.
        class CopyTests;

        // Tests op-overloads and other methods related to moving.
        class MoveTests;

        // Tests behavior of the overloaded dereference operator methods.
        class DereferenceOperatorTests;

    public:

        // Return list of Pointer unit tests.
        static UTList GetTests();
    };

    class PointerTests::DeallocationTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR CopyPointerObj()
        {
            Pointer<T> ptr(GetTQLength<TQ>());
            Pointer<T> srcPtr(GetTQLength<TQ>());

            try
            {
                Pointer<T>::CopyPointerObj(ptr, srcPtr);
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT((ptr.Get() != srcPtr.Get()) == (TQ != TestQuantity::None));
            SUTL_TEST_ASSERT(ptr.Length() == srcPtr.Length());
            for (size_t i = 0; i < GetTQLength<TQ>(); i++)
            {
                SUTL_TEST_ASSERT(ptr.Get()[i] == srcPtr.Get()[i]);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        static UTR TransferPointerObj()
        {
            Pointer<T> ptr(GetTQLength<TQ>());
            Pointer<T> srcPtr(GetTQLength<TQ>());

            try
            {
                Pointer<T>::TransferPointerObj(ptr, srcPtr);
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT((ptr.Get() != srcPtr.Get()) == (TQ != TestQuantity::None));
            SUTL_TEST_ASSERT(srcPtr.Get() == nullptr);
            if constexpr (std::is_same_v<T, Helper>)
            {
                for (size_t i = 0; i < GetTQLength<TQ>(); i++)
                {
                    SUTL_TEST_ASSERT(!ptr.Get()[i].Copied());
                    SUTL_TEST_ASSERT(!ptr.Get()[i].Moved());
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        static UTR Destructor()
        {
            Pointer<T> ptr(GetTQLength<TQ>());

            ptr.~Pointer();

            SUTL_TEST_ASSERT(!ptr.m_pPtr);
            SUTL_TEST_ASSERT(ptr.m_Len == 0);

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        static UTR Free()
        {
            Pointer<T> ptr(GetTQLength<TQ>());

            ptr.Free();

            SUTL_TEST_ASSERT(!ptr.m_pPtr);
            SUTL_TEST_ASSERT(ptr.m_Len == 0);

            SUTL_TEST_SUCCESS();
        }
    };

    class PointerTests::ConstructorTests
    {
    public:

        template <typename T>
        static UTR DefaultCtor()
        {
            Pointer<T> p;

            SUTL_TEST_ASSERT(p.Get() == nullptr);
            SUTL_TEST_ASSERT(p.Length() == 0);
            SUTL_TEST_ASSERT(p.Size() == 0);

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        static UTR LengthCtor()
        {
            constexpr bool bExpectNull = (TQ == TestQuantity::None);
            constexpr size_t len = GetTQLength<TQ>();
            Pointer<T> p(len);

            SUTL_TEST_ASSERT(!p.Get() == bExpectNull);
            SUTL_TEST_ASSERT(p.Length() == len);
            SUTL_TEST_ASSERT(p.Size() == (sizeof(T) * len));

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        static UTR RawPointerCopyCtor()
        {
            constexpr bool bExpectNull = (TQ == TestQuantity::None);
            constexpr size_t len = GetTQLength<TQ>();
            const std::vector<T> testData = GetTestData<T, TQ>();
            Pointer<T>* pPointer = nullptr;

            try
            {
                pPointer = new Pointer<T>(testData.data(), testData.size());
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(!pPointer->Get() == bExpectNull);
            SUTL_TEST_ASSERT(pPointer->Length() == len);
            SUTL_TEST_ASSERT(pPointer->Size() == (sizeof(T) * len));

            if constexpr (len > 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(pPointer->Get()[i] == testData[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(pPointer->Get()[i].Copied());
                        SUTL_TEST_ASSERT(!pPointer->Get()[i].Moved());
                    }
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        static UTR RawPointerStealCtor()
        {
            constexpr bool bExpectNull = (TQ == TestQuantity::None);
            constexpr size_t len = GetTQLength<TQ>();
            const std::vector<T> testData = GetTestData<T, TQ>();
            T* pTestCopy = nullptr;
            T* pOrigPtr = nullptr;
            Pointer<T>* pPointer = nullptr;

            if constexpr (!bExpectNull)
            {
                try
                {
                    pTestCopy = new T[len];
                    pOrigPtr = pTestCopy;
                    for (size_t i = 0; i < len; i++)
                    {
                        pTestCopy[i] = testData[i];
                    }
                }
                catch (const std::exception& e)
                {
                    SUTL_SETUP_EXCEPTION(e.what());
                }
            }

            try
            {
                pPointer = new Pointer<T>(pTestCopy, len);
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(!pPointer->Get() == bExpectNull);
            SUTL_TEST_ASSERT(pPointer->Get() == pOrigPtr);
            SUTL_TEST_ASSERT(!pTestCopy);
            SUTL_TEST_ASSERT(pPointer->Length() == len);
            SUTL_TEST_ASSERT(pPointer->Size() == (sizeof(T) * len));

            if constexpr (len > 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(pPointer->Get()[i] == testData[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(pPointer->Get()[i].Copied());
                        SUTL_TEST_ASSERT(!pPointer->Get()[i].Moved());
                    }
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        static UTR CopyCtor()
        {
            constexpr bool bExpectNull = (TQ == TestQuantity::None);
            constexpr size_t len = GetTQLength<TQ>();
            const std::vector<T> testData = GetTestData<T, TQ>();

            Pointer<T>* pSrc = nullptr;
            Pointer<T>* pPointer = nullptr;

            try
            {
                pSrc = new Pointer<T>(testData.data(), len);
            }
            catch (const std::exception& e)
            {
                SUTL_SETUP_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(!pSrc->Get() == bExpectNull);
            SUTL_TEST_ASSERT(pSrc->Length() == len);
            SUTL_TEST_ASSERT(pSrc->Size() == (sizeof(T) * len));

            try
            {
                pPointer = new Pointer<T>(*pSrc);
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(!pSrc->Get() == bExpectNull);
            SUTL_TEST_ASSERT(pSrc->Length() == len);
            SUTL_TEST_ASSERT(pSrc->Size() == (sizeof(T) * len));

            SUTL_TEST_ASSERT(!pPointer->Get() == bExpectNull);
            SUTL_TEST_ASSERT((pPointer->Get() == pSrc->Get()) == bExpectNull);
            SUTL_TEST_ASSERT(pPointer->Length() == len);
            SUTL_TEST_ASSERT(pPointer->Size() == (sizeof(T) * len));

            if constexpr (len > 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(pPointer->Get()[i] == pSrc->Get()[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(pPointer->Get()[i].Copied());
                        SUTL_TEST_ASSERT(!pPointer->Get()[i].Moved());
                    }
                }
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        static UTR MoveCtor()
        {
            constexpr bool bExpectNull = (TQ == TestQuantity::None);
            constexpr size_t len = GetTQLength<TQ>();
            const std::vector<T> testData = GetTestData<T, TQ>();

            Pointer<T>* pSrc = nullptr;
            Pointer<T>* pPointer = nullptr;

            try
            {
                pSrc = new Pointer<T>(testData.data(), len);
            }
            catch (const std::exception& e)
            {
                SUTL_SETUP_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(!pSrc->Get() == bExpectNull);
            SUTL_TEST_ASSERT(pSrc->Length() == len);
            SUTL_TEST_ASSERT(pSrc->Size() == (sizeof(T) * len));

            try
            {
                pPointer = new Pointer<T>(std::move(*pSrc));
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(!pSrc->Get());
            SUTL_TEST_ASSERT(pSrc->Length() == 0);
            SUTL_TEST_ASSERT(pSrc->Size() == 0);

            SUTL_TEST_ASSERT(!pPointer->Get() == bExpectNull);
            SUTL_TEST_ASSERT((pPointer->Get() == pSrc->Get()) == bExpectNull);
            SUTL_TEST_ASSERT(pPointer->Length() == len);
            SUTL_TEST_ASSERT(pPointer->Size() == (sizeof(T) * len));

            if constexpr (len > 0)
            {
                for (size_t i = 0; i < len; i++)
                {
                    SUTL_TEST_ASSERT(pPointer->Get()[i] == testData[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(pPointer->Get()[i].Copied());
                        SUTL_TEST_ASSERT(!pPointer->Get()[i].Moved());
                    }
                }
            }

            SUTL_TEST_SUCCESS();
        }
    };

    class PointerTests::CopyTests
    {
    public:

        template <typename T, TestQuantity TQDst, TestQuantity TQSrc>
        static UTR CopyAssignment()
        {
            constexpr bool bExpectNullSrc = (TQSrc == TestQuantity::None);
            constexpr bool bExpectNullDst = (TQDst == TestQuantity::None);
            constexpr size_t lenSrc = GetTQLength<TQSrc>();
            constexpr size_t lenDst = GetTQLength<TQDst>();
            const std::vector<T> testDataSrc = GetTestData<T, TQSrc>();
            const std::vector<T> testDataDst = GetTestData<T, TQDst>();

            Pointer<T> src(testDataSrc.data(), lenSrc);
            Pointer<T> dst(testDataDst.data(), lenDst);
            const T* pOrigSrc = src.Get();
            const T* pOrigDst = dst.Get();

            // Pointers should be different unless both should be nullptr.
            SUTL_TEST_ASSERT((pOrigSrc == pOrigDst) == (bExpectNullSrc && bExpectNullDst));

            // Ensure src state is as expected.
            SUTL_TEST_ASSERT(!src.Get() == bExpectNullSrc);
            SUTL_TEST_ASSERT(src.Length() == lenSrc);
            SUTL_TEST_ASSERT(src.Size() == (sizeof(T) * lenSrc));

            if constexpr (lenSrc > 0)
            {
                for (size_t i = 0; i < lenSrc; i++)
                {
                    SUTL_TEST_ASSERT(src.Get()[i] == testDataSrc[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(src.Get()[i].Copied());
                        SUTL_TEST_ASSERT(!src.Get()[i].Moved());
                    }
                }
            }

            // Ensure dst state is as expected.
            SUTL_TEST_ASSERT(!dst.Get() == bExpectNullDst);
            SUTL_TEST_ASSERT(dst.Length() == lenDst);
            SUTL_TEST_ASSERT(dst.Size() == (sizeof(T) * lenDst));

            if constexpr (lenDst > 0)
            {
                for (size_t i = 0; i < lenDst; i++)
                {
                    SUTL_TEST_ASSERT(dst.Get()[i] == testDataDst[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(dst.Get()[i].Copied());
                        SUTL_TEST_ASSERT(!dst.Get()[i].Moved());
                    }
                }
            }

            // Attempt copy assignment.
            try
            {
                dst = src;
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            // Ensure src state is as expected after copy.
            SUTL_TEST_ASSERT(src.Get() == pOrigSrc);
            SUTL_TEST_ASSERT(src.Length() == lenSrc);
            SUTL_TEST_ASSERT(src.Size() == (sizeof(T) * lenSrc));

            if constexpr (lenSrc > 0)
            {
                for (size_t i = 0; i < lenSrc; i++)
                {
                    SUTL_TEST_ASSERT(src.Get()[i] == testDataSrc[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(src.Get()[i].Copied());
                        SUTL_TEST_ASSERT(!src.Get()[i].Moved());
                    }
                }
            }

            // Ensure dst state is as expected after copy.
            SUTL_TEST_ASSERT((dst.Get() == pOrigDst) == (bExpectNullSrc && bExpectNullDst));
            SUTL_TEST_ASSERT(dst.Length() == lenSrc);
            SUTL_TEST_ASSERT(dst.Size() == (sizeof(T) * lenSrc));

            if constexpr (lenSrc > 0)
            {
                for (size_t i = 0; i < lenSrc; i++)
                {
                    SUTL_TEST_ASSERT(dst.Get()[i] == src.Get()[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(dst.Get()[i].Copied());
                        SUTL_TEST_ASSERT(!dst.Get()[i].Moved());
                    }
                }
            }

            SUTL_TEST_SUCCESS();
        }
    };

    class PointerTests::MoveTests
    {
    public:

        template <typename T, TestQuantity TQDst, TestQuantity TQSrc>
        static UTR MoveAssignment()
        {
            constexpr bool bExpectNullSrc = (TQSrc == TestQuantity::None);
            constexpr bool bExpectNullDst = (TQDst == TestQuantity::None);
            constexpr size_t lenSrc = GetTQLength<TQSrc>();
            constexpr size_t lenDst = GetTQLength<TQDst>();
            const std::vector<T> testDataSrc = GetTestData<T, TQSrc>();
            const std::vector<T> testDataDst = GetTestData<T, TQDst>();

            Pointer<T> src(testDataSrc.data(), lenSrc);
            Pointer<T> dst(testDataDst.data(), lenDst);
            const T* pOrigSrc = src.Get();

            // Pointers should be different unless both should be nullptr.
            SUTL_TEST_ASSERT((pOrigSrc == dst.Get()) == (bExpectNullSrc && bExpectNullDst));

            // Ensure src state is as expected.
            SUTL_TEST_ASSERT(!src.Get() == bExpectNullSrc);
            SUTL_TEST_ASSERT(src.Length() == lenSrc);
            SUTL_TEST_ASSERT(src.Size() == (sizeof(T) * lenSrc));

            if constexpr (lenSrc > 0)
            {
                for (size_t i = 0; i < lenSrc; i++)
                {
                    SUTL_TEST_ASSERT(src.Get()[i] == testDataSrc[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(src.Get()[i].Copied());
                        SUTL_TEST_ASSERT(!src.Get()[i].Moved());
                    }
                }
            }

            // Ensure dst state is as expected.
            SUTL_TEST_ASSERT(!dst.Get() == bExpectNullDst);
            SUTL_TEST_ASSERT(dst.Length() == lenDst);
            SUTL_TEST_ASSERT(dst.Size() == (sizeof(T) * lenDst));

            if constexpr (lenDst > 0)
            {
                for (size_t i = 0; i < lenDst; i++)
                {
                    SUTL_TEST_ASSERT(dst.Get()[i] == testDataDst[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(dst.Get()[i].Copied());
                        SUTL_TEST_ASSERT(!dst.Get()[i].Moved());
                    }
                }
            }

            // Attempt move assignment.
            try
            {
                dst = std::move(src);
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            // Ensure src state is as expected after move.
            SUTL_TEST_ASSERT(src.Get() == nullptr);
            SUTL_TEST_ASSERT(src.Length() == 0);
            SUTL_TEST_ASSERT(src.Size() == 0);

            // Ensure dst state is as expected after move.
            SUTL_TEST_ASSERT(dst.Get() == pOrigSrc);
            SUTL_TEST_ASSERT(dst.Length() == lenSrc);
            SUTL_TEST_ASSERT(dst.Size() == (sizeof(T) * lenSrc));

            if constexpr (lenSrc > 0)
            {
                for (size_t i = 0; i < lenSrc; i++)
                {
                    SUTL_TEST_ASSERT(dst.Get()[i] == testDataSrc[i]);
                    if constexpr (std::is_same_v<T, Helper>)
                    {
                        SUTL_TEST_ASSERT(dst.Get()[i].Copied());
                        SUTL_TEST_ASSERT(!dst.Get()[i].Moved());
                    }
                }
            }

            SUTL_TEST_SUCCESS();
        }
    };

    class PointerTests::DereferenceOperatorTests
    {
    public:

        template <typename T, TestQuantity TQ>
        static UTR Dereference()
        {
            constexpr bool bExpectNull = (TQ == TestQuantity::None);
            constexpr size_t len = GetTQLength<TQ>();
            std::vector<T> testData = GetTestData<T, TQ>();
            bool bThrew = false;

            Pointer<T> p(testData.data(), len);
            
            #if defined(__GNUC__)
                #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
            #endif
            T tVal;

            try
            {
                tVal = *p;
            }
            catch (const std::logic_error&)
            {
                bThrew = true;
            }
            catch (const std::exception& e)
            {
                SUTL_TEST_EXCEPTION(e.what());
            }

            SUTL_TEST_ASSERT(bThrew == bExpectNull);
            if constexpr (!bExpectNull)
            {
                SUTL_TEST_ASSERT(tVal == testData.front());
                SUTL_TEST_ASSERT(tVal == p.Get()[0]);
            }

            if constexpr (std::is_same_v<T, Helper>)
            {
            #if defined(__GNUC__)
                #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
            #endif
                bool bCopied = false;
                bool bMoved = false;

                bThrew = false;

                try
                {
                    bCopied = p->Copied();
                    bMoved = p->Moved();
                }
                catch (const std::logic_error&)
                {
                    bThrew = true;
                }
                catch (const std::exception& e)
                {
                    SUTL_TEST_EXCEPTION(e.what());
                }

                SUTL_TEST_ASSERT(bThrew == bExpectNull);
                if constexpr (!bExpectNull)
                {
                    SUTL_TEST_ASSERT(bCopied);
                    SUTL_TEST_ASSERT(!bMoved);
                }
            }

            SUTL_TEST_SUCCESS();
        }
    };
}