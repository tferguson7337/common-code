#pragma once

// SUTL
#include <UnitTestResult.h>

// STL
#include <algorithm>
#include <functional>
#include <list>
#include <memory>

// Target Class
#include <CCPointerHelper.h>

// Test Helper
#include <CCPointer.h>


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

        /// Test Helpers \\\

        struct Helper
        {
            static constexpr size_t s_ArrSize = 21;

            uint8_t m_u8;
            uint16_t m_u16;
            uint32_t m_u32;
            uint64_t m_u64;
            uint8_t m_u8Arr[s_ArrSize];

            bool m_bCopied;
            bool m_bMoved;

            Helper() noexcept :
                m_u8(0), m_u16(0), m_u32(0), m_u64(0),
                m_bCopied(false), m_bMoved(false)
            {
                memset(m_u8Arr, 0, sizeof(m_u8Arr));
            }

            Helper(const uint8_t u8, const uint16_t u16, const uint32_t u32, const uint64_t u64, const uint8_t u8ArrVals) noexcept :
                m_u8(u8), m_u16(u16), m_u32(u32), m_u64(u64)
            {
                memset(m_u8Arr, u8ArrVals, sizeof(uint8_t) * s_ArrSize);
            }

            Helper(const Helper& src) :
                Helper()
            {
                *this = src;
            }

            Helper(Helper&& src) noexcept :
                Helper()
            {
                *this = std::move(src);
            }

            ~Helper() noexcept = default;

            Helper& operator=(const Helper& src)
            {
                if (this != &src)
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
                if (this != &src)
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

            bool operator==(const Helper& rhs) const noexcept
            {
                return m_u8 == rhs.m_u8
                    && m_u16 == rhs.m_u16
                    && m_u32 == rhs.m_u32
                    && m_u64 == rhs.m_u64
                    && memcmp(m_u8Arr, rhs.m_u8Arr, sizeof(m_u8Arr)) == 0;
            }

            bool operator!=(const Helper& rhs) const noexcept
            {
                return !operator==(rhs);
            }

            bool Copied() const noexcept
            {
                return m_bCopied;
            }

            bool Moved() const noexcept
            {
                return m_bMoved;
            }
        };

        template <typename T>
        static constexpr bool IsValidTestType()
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
        static constexpr bool IsValidTestQuantity()
        {
            return TQ >= TestQuantity::_Begin && TQ < TestQuantity::_End;
        }

        template <TestQuantity TQ>
        static constexpr const size_t GetTQNum()
        {
            static_assert(IsValidTestQuantity<TQ>(), __FUNCSIG__": Invalid TestQuantity Template Argument.");

            if constexpr (TQ == TestQuantity::Zero)
            {
                return 0;
            }
            else if constexpr (TQ == TestQuantity::One)
            {
                return 1;
            }
            else
            {
                return 128;
            }
        }

        template <typename T, TestQuantity TQ>
        static std::vector<T> GetTestData()
        {
            static_assert(IsValidTestType<T>(), __FUNCSIG__": Invalid Template Type Argument");
            static_assert(IsValidTestQuantity<TQ>(), __FUNCSIG__": Invalid TestQuantity Template Argument.");

            std::vector<T> testData;

            if constexpr (TQ != TestQuantity::Zero)
            {
                for (size_t i = 0; i < GetTQNum<TQ>(); i++)
                {
                    if constexpr (!std::is_same_v<T, Helper>)
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
                        for (size_t j = 0; j < Helper::s_ArrSize; j++)
                        {
                            h.m_u8Arr[j] = static_cast<uint8_t>(val++);
                        }

                        testData.push_back(h);
                    }
                }
            }

            return testData;
        }

        // Type aliases
        template <typename T>
        using PH = PointerHelper<T>;

        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;

        /// Test Methods \\\

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