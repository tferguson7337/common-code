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
#include <CCPointerHelper.h>

// Test Helpers
#include <CCPointer.h>
#include <TestHelpers.hpp>


namespace CC
{
    class PointerHelperTests
    {
        friend class BufferTests;
        friend class DynamicBufferTests;
        friend class PointerTests;
        friend class SharedPointerTests;
        friend class StringTests;

        PointerHelperTests() = delete;
        PointerHelperTests(const PointerHelperTests&) = delete;
        PointerHelperTests(PointerHelperTests&&) = delete;
        ~PointerHelperTests() = delete;
        PointerHelperTests& operator=(const PointerHelperTests&) = delete;
        PointerHelperTests& operator=(PointerHelperTests&&) = delete;

    private:

        // Type aliases
        template <typename T>
        using PH = PointerHelper<T>;

        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;

        // Test Methods //

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CopyToRawPointer()
        {
            constexpr size_t len = static_cast<size_t>(TQ);
            std::vector<T> srcData = GetTestData<T, TQ>();
            std::vector<T> dstData = GetTestData<T, TQ>();
            T* pSrc = srcData.data();
            T* pDst = dstData.data();

            if constexpr (len != 0)
            {
                SUTL_TEST_ASSERT(!!pSrc);
                SUTL_TEST_ASSERT(!!pDst);

                // Prep for copy - zero-out dst.
            #if defined(__GNUC__)
                // Suppress Warning:
                // ‘void* memcpy(void*, const void*, size_t)’ writing to an
                // object of type ‘struct CC::Helper’ with no trivial copy-assignment;
                // use copy-assignment or copy-initialization instead
                #pragma GCC diagnostic ignored "-Wclass-memaccess"
            #endif
                memset(pDst, 0, sizeof(T) * len);
            }
            else
            {
                SUTL_TEST_ASSERT(!pSrc);
                SUTL_TEST_ASSERT(!pDst);
            }

            PH<T>::CopyToRawPointer(pDst, pSrc, len);
            if constexpr (len != 0)
            {
                SUTL_TEST_ASSERT(!!pSrc);
                SUTL_TEST_ASSERT(!!pDst);

                if constexpr (std::is_same_v<T, Helper>)
                {
                    for (size_t i = 0; i < len; i++)
                    {
                        SUTL_TEST_ASSERT(pDst[i] == pSrc[i]);
                        SUTL_TEST_ASSERT(pDst[i].Copied());
                        SUTL_TEST_ASSERT(!pDst[i].Moved());
                    }
                }
                else
                {
                    SUTL_TEST_ASSERT(memcmp(pDst, pSrc, sizeof(T) * len) == 0);
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!pSrc);
                SUTL_TEST_ASSERT(!pDst);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR MoveToRawPointer()
        {
            constexpr size_t len = static_cast<size_t>(TQ);
            std::vector<T> srcData = GetTestData<T, TQ>();
            std::vector<T> dstData = GetTestData<T, TQ>();
            T* pSrc = srcData.data();
            T* pDst = dstData.data();

            if constexpr (len != 0)
            {
                SUTL_TEST_ASSERT(!!pSrc);
                SUTL_TEST_ASSERT(!!pDst);

                // Prep for copy - zero-out dst.
            #if defined(__GNUC__)
                // Suppress Warning:
                // ‘void* memcpy(void*, const void*, size_t)’ writing to an
                // object of type ‘struct CC::Helper’ with no trivial copy-assignment;
                // use copy-assignment or copy-initialization instead
                #pragma GCC diagnostic ignored "-Wclass-memaccess"
            #endif
                memset(pDst, 0, sizeof(T) * len);
            }
            else
            {
                SUTL_TEST_ASSERT(!pSrc);
                SUTL_TEST_ASSERT(!pDst);
            }

            PH<T>::MoveToRawPointer(pDst, pSrc, len);
            if constexpr (len != 0)
            {
                SUTL_TEST_ASSERT(!!pSrc);
                SUTL_TEST_ASSERT(!!pDst);

                if constexpr (std::is_same_v<T, Helper>)
                {
                    for (size_t i = 0; i < len; i++)
                    {
                        SUTL_TEST_ASSERT(pDst[i] == pSrc[i]);
                        SUTL_TEST_ASSERT(!pDst[i].Copied());
                        SUTL_TEST_ASSERT(pDst[i].Moved());
                    }
                }
                else
                {
                    SUTL_TEST_ASSERT(memcmp(pDst, pSrc, sizeof(T) * len) == 0);
                }
            }
            else
            {
                SUTL_TEST_ASSERT(!pSrc);
                SUTL_TEST_ASSERT(!pDst);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR Allocate()
        {
            constexpr size_t offset = static_cast<size_t>(TQ);
            T* ptr = nullptr;

            if constexpr (std::is_same_v<T, Helper>)
            {
                // Using literals.
                ptr = PH<T>::Allocate(
                    static_cast<uint8_t>(0 + offset),
                    static_cast<uint16_t>(1 + offset),
                    static_cast<uint32_t>(2 + offset),
                    static_cast<uint64_t>(3 + offset),
                    static_cast<uint8_t>(4 + offset)
                );

                SUTL_TEST_ASSERT(!!ptr);
                SUTL_TEST_ASSERT(ptr->m_u8 == static_cast<uint8_t>(0 + offset));
                SUTL_TEST_ASSERT(ptr->m_u16 == static_cast<uint8_t>(1 + offset));
                SUTL_TEST_ASSERT(ptr->m_u32 == static_cast<uint8_t>(2 + offset));
                SUTL_TEST_ASSERT(ptr->m_u64 == static_cast<uint8_t>(3 + offset));
                for (size_t i = 0; i < Helper::s_ArrSize; i++)
                {
                    SUTL_TEST_ASSERT(ptr->m_u8Arr[i] == static_cast<uint8_t>(4 + offset));
                }

                // Prep for next go.
                delete ptr;

                // Using variables.
                const uint8_t expU8 = static_cast<uint8_t>(0 + offset);
                const uint16_t expU16 = static_cast<uint16_t>(1 + offset);
                const uint32_t expU32 = static_cast<uint32_t>(2 + offset);
                const uint64_t expU64 = static_cast<uint64_t>(3 + offset);
                const uint8_t expU8ArrVals = static_cast<uint8_t>(4 + offset);

                ptr = PH<T>::Allocate(expU8, expU16, expU32, expU64, expU8ArrVals);
                SUTL_TEST_ASSERT(!!ptr);
                SUTL_TEST_ASSERT(ptr->m_u8 == expU8);
                SUTL_TEST_ASSERT(ptr->m_u16 == expU16);
                SUTL_TEST_ASSERT(ptr->m_u32 == expU32);
                SUTL_TEST_ASSERT(ptr->m_u64 == expU64);
                for (size_t i = 0; i < Helper::s_ArrSize; i++)
                {
                    SUTL_TEST_ASSERT(ptr->m_u8Arr[i] == expU8ArrVals);
                }
            }
            else
            {
                ptr = PH<T>::Allocate(static_cast<T>(offset));
                SUTL_TEST_ASSERT(!!ptr);
                SUTL_TEST_ASSERT(*ptr == static_cast<T>(offset));
            }

            delete ptr;

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR AllocateLen()
        {
            constexpr size_t len = static_cast<size_t>(TQ);
            T* p = nullptr;

            p = PH<T>::AllocateLen(len);
            if constexpr (len == 0)
            {
                SUTL_TEST_ASSERT(!p);
            }
            else
            {
                SUTL_TEST_ASSERT(!!p);
                if constexpr (len == 1)
                {
                    delete p;
                }
                else
                {
                    delete[] p;
                }

                p = nullptr;
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR AllocateFromRawPointer()
        {
            constexpr size_t len = static_cast<size_t>(TQ);
            std::vector<T> srcData = GetTestData<T, TQ>();
            T* pSrc = srcData.data();
            T* pDst = nullptr;

            if constexpr (len != 0)
            {
                SUTL_TEST_ASSERT(!!pSrc);
            }
            else
            {
                SUTL_TEST_ASSERT(!pSrc);
            }

            pDst = PH<T>::AllocateFromRawPointer(pSrc, len);
            if constexpr (len != 0)
            {
                SUTL_TEST_ASSERT(!!pSrc);
                SUTL_TEST_ASSERT(!!pDst);

                if constexpr (std::is_same_v<T, Helper>)
                {
                    for (size_t i = 0; i < len; i++)
                    {
                        SUTL_TEST_ASSERT(pDst[i] == pSrc[i]);
                        SUTL_TEST_ASSERT(pDst[i].Copied());
                        SUTL_TEST_ASSERT(!pDst[i].Moved());
                    }
                }
                else
                {
                    SUTL_TEST_ASSERT(memcmp(pDst, pSrc, sizeof(T) * len) == 0);
                }

                if constexpr (len == 1)
                {
                    delete pDst;
                }
                else
                {
                    delete[] pDst;
                }

                pDst = nullptr;
            }
            else
            {
                SUTL_TEST_ASSERT(!pSrc);
                SUTL_TEST_ASSERT(!pDst);
            }

            SUTL_TEST_SUCCESS();
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR AllocateFromIPointerObj()
        {
            constexpr size_t len = static_cast<size_t>(TQ);
            std::vector<T> srcData = GetTestData<T, TQ>();
            Pointer<T> srcPtr(PH<T>::AllocateFromRawPointer(srcData.data(), len), len);
            T* pRawSrcPtr = srcPtr.Get();
            T* pDst = nullptr;

            if constexpr (len != 0)
            {
                SUTL_TEST_ASSERT(!!pRawSrcPtr);
            }
            else
            {
                SUTL_TEST_ASSERT(!pRawSrcPtr);
            }

            pDst = PH<T>::AllocateFromIPointerObj(srcPtr);
            if constexpr (len != 0)
            {
                SUTL_TEST_ASSERT(!!pRawSrcPtr);
                SUTL_TEST_ASSERT(!!pDst);

                if constexpr (std::is_same_v<T, Helper>)
                {
                    for (size_t i = 0; i < len; i++)
                    {
                        SUTL_TEST_ASSERT(pDst[i] == pRawSrcPtr[i]);
                        SUTL_TEST_ASSERT(pDst[i].Copied());
                        SUTL_TEST_ASSERT(!pDst[i].Moved());
                    }
                }
                else
                {
                    SUTL_TEST_ASSERT(memcmp(pDst, pRawSrcPtr, sizeof(T) * len) == 0);
                }

                if constexpr (len == 1)
                {
                    delete pDst;
                }
                else
                {
                    delete[] pDst;
                }

                pDst = nullptr;
            }
            else
            {
                SUTL_TEST_ASSERT(!pRawSrcPtr);
                SUTL_TEST_ASSERT(!pDst);
            }

            SUTL_TEST_SUCCESS();
        }

    public:

        // Return list of PointerHelper unit tests.
        static UTList GetTests();
    };
}