#pragma once

// SUTL
#include <TestTypes.h>

// STD
#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>


namespace CC
{
    // Helper struct for testing copy/move behavior with CC data structures.
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

        Helper(const Helper& src) noexcept :
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

        Helper& operator=(const Helper& src) noexcept
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

    template <TestQuantity _TQ>
    static constexpr size_t GetTQLength()
    {
        using TQ = TestQuantity;

        static_assert(IsValidTestQuantity<_TQ>(), "Invalid TestQuantity Template Argument.");

        if constexpr (_TQ == TQ::None)
        {
            return 0;
        }
        else if constexpr (_TQ == TQ::VeryLow)
        {
            return 1;
        }
        else if constexpr (_TQ == TQ::Low)
        {
            return 4;
        }
        else if constexpr (_TQ == TQ::MidLow)
        {
            return 16;
        }
        else if constexpr (_TQ == TQ::Mid)
        {
            return 64;
        }
        else if constexpr (_TQ == TQ::MidHigh)
        {
            return 128;
        }
        else if constexpr (_TQ == TQ::High)
        {
            return 512;
        }
        else if constexpr (_TQ == TQ::VeryHigh)
        {
            return 2048;
        }
        else if constexpr (_TQ == TQ::All)
        {
            return 4096;
        }
        else
        {
            throw std::invalid_argument("Unknown TestQuantity Template Argument.");
        }
    }

    template <typename T, TestQuantity TQ>
    static std::vector<T> GetTestData()
    {
        static_assert(IsValidTestType<T>(), "Invalid Template Type Argument");
        static_assert(IsValidTestQuantity<TQ>(), "Invalid TestQuantity Template Argument.");

        std::vector<T> testData;

        if constexpr (TQ != TestQuantity::None)
        {
            for (size_t i = 0; i < GetTQLength<TQ>(); i++)
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
}


// Add-Test Helper Macros //

// Add 5 tests, (5 types)
#define ADD_TESTS_T(_fn_)\
    _fn_<uint8_t>, _fn_<uint16_t>, _fn_<uint32_t>, _fn_<uint64_t>, _fn_<CC::Helper>

// Add 25 tests, (5 types x 5 TQs)
#define ADD_TESTS_T_TQ1(_fn_)\
    _fn_<uint8_t,  TestQuantity::None>, _fn_<uint8_t,  TestQuantity::VeryLow>, _fn_<uint8_t,  TestQuantity::Low>, _fn_<uint8_t,  TestQuantity::Mid>, _fn_<uint8_t,  TestQuantity::High>,\
    _fn_<uint16_t, TestQuantity::None>, _fn_<uint16_t, TestQuantity::VeryLow>, _fn_<uint16_t, TestQuantity::Low>, _fn_<uint16_t, TestQuantity::Mid>, _fn_<uint16_t, TestQuantity::High>,\
    _fn_<uint32_t, TestQuantity::None>, _fn_<uint32_t, TestQuantity::VeryLow>, _fn_<uint32_t, TestQuantity::Low>, _fn_<uint32_t, TestQuantity::Mid>, _fn_<uint32_t, TestQuantity::High>,\
    _fn_<uint64_t, TestQuantity::None>, _fn_<uint64_t, TestQuantity::VeryLow>, _fn_<uint64_t, TestQuantity::Low>, _fn_<uint64_t, TestQuantity::Mid>, _fn_<uint64_t, TestQuantity::High>,\
    _fn_<CC::Helper,   TestQuantity::None>, _fn_<CC::Helper,   TestQuantity::VeryLow>, _fn_<CC::Helper,   TestQuantity::Low>, _fn_<CC::Helper,   TestQuantity::Mid>, _fn_<CC::Helper,   TestQuantity::High>

// Add 50 tests, (5 types x 5 TQs x T/F)
#define ADD_TESTS_T_TQ1_B(_fn_)\
    _fn_<uint8_t,  TestQuantity::None, false>, _fn_<uint8_t,  TestQuantity::None, true>, _fn_<uint8_t,  TestQuantity::VeryLow, false>, _fn_<uint8_t,  TestQuantity::VeryLow, true>, _fn_<uint8_t,  TestQuantity::Low, false>, _fn_<uint8_t,  TestQuantity::Low, true>, _fn_<uint8_t,  TestQuantity::Mid, false>, _fn_<uint8_t,  TestQuantity::Mid, true>, _fn_<uint8_t,  TestQuantity::High, false>, _fn_<uint8_t,  TestQuantity::High, true>,\
    _fn_<uint16_t, TestQuantity::None, false>, _fn_<uint16_t, TestQuantity::None, true>, _fn_<uint16_t, TestQuantity::VeryLow, false>, _fn_<uint16_t, TestQuantity::VeryLow, true>, _fn_<uint16_t, TestQuantity::Low, false>, _fn_<uint16_t, TestQuantity::Low, true>, _fn_<uint16_t, TestQuantity::Mid, false>, _fn_<uint16_t, TestQuantity::Mid, true>, _fn_<uint16_t, TestQuantity::High, false>, _fn_<uint16_t, TestQuantity::High, true>,\
    _fn_<uint32_t, TestQuantity::None, false>, _fn_<uint32_t, TestQuantity::None, true>, _fn_<uint32_t, TestQuantity::VeryLow, false>, _fn_<uint32_t, TestQuantity::VeryLow, true>, _fn_<uint32_t, TestQuantity::Low, false>, _fn_<uint32_t, TestQuantity::Low, true>, _fn_<uint32_t, TestQuantity::Mid, false>, _fn_<uint32_t, TestQuantity::Mid, true>, _fn_<uint32_t, TestQuantity::High, false>, _fn_<uint32_t, TestQuantity::High, true>,\
    _fn_<uint64_t, TestQuantity::None, false>, _fn_<uint64_t, TestQuantity::None, true>, _fn_<uint64_t, TestQuantity::VeryLow, false>, _fn_<uint64_t, TestQuantity::VeryLow, true>, _fn_<uint64_t, TestQuantity::Low, false>, _fn_<uint64_t, TestQuantity::Low, true>, _fn_<uint64_t, TestQuantity::Mid, false>, _fn_<uint64_t, TestQuantity::Mid, true>, _fn_<uint64_t, TestQuantity::High, false>, _fn_<uint64_t, TestQuantity::High, true>,\
    _fn_<CC::Helper,   TestQuantity::None, false>, _fn_<CC::Helper,   TestQuantity::None, true>, _fn_<CC::Helper,   TestQuantity::VeryLow, false>, _fn_<CC::Helper,   TestQuantity::VeryLow, true>, _fn_<CC::Helper,   TestQuantity::Low, false>, _fn_<CC::Helper,   TestQuantity::Low, true>, _fn_<CC::Helper,   TestQuantity::Mid, false>, _fn_<CC::Helper,   TestQuantity::Mid, true>, _fn_<CC::Helper,   TestQuantity::High, false>, _fn_<CC::Helper,   TestQuantity::High, true>

// Add 125 tests, (5 types X 5 TQs X 5 TQs)
#define ADD_TESTS_T_TQ2(_fn_)\
    _fn_<uint8_t,  TestQuantity::None,    TestQuantity::None>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::VeryLow>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::Low>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::Mid>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::High>,\
    _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::None>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::VeryLow>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::Low>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::Mid>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::High>,\
    _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::None>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::VeryLow>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::Low>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::Mid>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::High>,\
    _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::None>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::VeryLow>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::Low>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::Mid>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::High>,\
    _fn_<uint8_t,  TestQuantity::High,    TestQuantity::None>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::VeryLow>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::Low>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::Mid>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::High>,\
    _fn_<uint16_t, TestQuantity::None,    TestQuantity::None>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::VeryLow>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::Low>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::Mid>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::High>,\
    _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::None>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::VeryLow>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::Low>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::Mid>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::High>,\
    _fn_<uint16_t, TestQuantity::Low,     TestQuantity::None>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::VeryLow>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::Low>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::Mid>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::High>,\
    _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::None>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::VeryLow>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::Low>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::Mid>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::High>,\
    _fn_<uint16_t, TestQuantity::High,    TestQuantity::None>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::VeryLow>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::Low>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::Mid>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::High>,\
    _fn_<uint32_t, TestQuantity::None,    TestQuantity::None>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::VeryLow>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::Low>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::Mid>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::High>,\
    _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::None>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::VeryLow>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::Low>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::Mid>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::High>,\
    _fn_<uint32_t, TestQuantity::Low,     TestQuantity::None>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::VeryLow>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::Low>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::Mid>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::High>,\
    _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::None>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::VeryLow>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::Low>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::Mid>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::High>,\
    _fn_<uint32_t, TestQuantity::High,    TestQuantity::None>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::VeryLow>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::Low>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::Mid>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::High>,\
    _fn_<uint64_t, TestQuantity::None,    TestQuantity::None>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::VeryLow>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::Low>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::Mid>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::High>,\
    _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::None>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::VeryLow>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::Low>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::Mid>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::High>,\
    _fn_<uint64_t, TestQuantity::Low,     TestQuantity::None>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::VeryLow>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::Low>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::Mid>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::High>,\
    _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::None>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::VeryLow>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::Low>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::Mid>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::High>,\
    _fn_<uint64_t, TestQuantity::High,    TestQuantity::None>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::VeryLow>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::Low>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::Mid>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::High>,\
    _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::None>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::VeryLow>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::Low>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::Mid>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::High>,\
    _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::None>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::VeryLow>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::Low>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::Mid>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::High>,\
    _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::None>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::VeryLow>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::Low>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::Mid>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::High>,\
    _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::None>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::VeryLow>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::Low>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::Mid>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::High>,\
    _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::None>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::VeryLow>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::Low>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::Mid>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::High>

// Add 250 tests, (5 types X 5 TQs X 5 TQs X T/F)
#define ADD_TESTS_T_TQ2_B(_fn_)\
    _fn_<uint8_t,  TestQuantity::None,    TestQuantity::None, false>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::None, true>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::VeryLow, false>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::VeryLow, true>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::Low, false>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::Low, true>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::Mid, false>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::Mid, true>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::High, false>, _fn_<uint8_t,  TestQuantity::None,    TestQuantity::High, true>,\
    _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::None, false>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::None, true>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::VeryLow, false>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::VeryLow, true>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::Low, false>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::Low, true>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::Mid, false>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::Mid, true>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::High, false>, _fn_<uint8_t,  TestQuantity::VeryLow, TestQuantity::High, true>,\
    _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::None, false>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::None, true>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::VeryLow, false>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::VeryLow, true>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::Low, false>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::Low, true>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::Mid, false>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::Mid, true>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::High, false>, _fn_<uint8_t,  TestQuantity::Low,     TestQuantity::High, true>,\
    _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::None, false>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::None, true>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::VeryLow, false>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::VeryLow, true>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::Low, false>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::Low, true>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::Mid, false>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::Mid, true>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::High, false>, _fn_<uint8_t,  TestQuantity::Mid,     TestQuantity::High, true>,\
    _fn_<uint8_t,  TestQuantity::High,    TestQuantity::None, false>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::None, true>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::VeryLow, false>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::VeryLow, true>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::Low, false>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::Low, true>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::Mid, false>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::Mid, true>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::High, false>, _fn_<uint8_t,  TestQuantity::High,    TestQuantity::High, true>,\
    _fn_<uint16_t, TestQuantity::None,    TestQuantity::None, false>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::None, true>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::VeryLow, false>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::VeryLow, true>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::Low, false>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::Low, true>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::Mid, false>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::Mid, true>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::High, false>, _fn_<uint16_t, TestQuantity::None,    TestQuantity::High, true>,\
    _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::None, false>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::None, true>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::VeryLow, false>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::VeryLow, true>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::Low, false>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::Low, true>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::Mid, false>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::Mid, true>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::High, false>, _fn_<uint16_t, TestQuantity::VeryLow, TestQuantity::High, true>,\
    _fn_<uint16_t, TestQuantity::Low,     TestQuantity::None, false>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::None, true>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::VeryLow, false>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::VeryLow, true>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::Low, false>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::Low, true>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::Mid, false>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::Mid, true>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::High, false>, _fn_<uint16_t, TestQuantity::Low,     TestQuantity::High, true>,\
    _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::None, false>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::None, true>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::VeryLow, false>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::VeryLow, true>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::Low, false>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::Low, true>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::Mid, false>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::Mid, true>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::High, false>, _fn_<uint16_t, TestQuantity::Mid,     TestQuantity::High, true>,\
    _fn_<uint16_t, TestQuantity::High,    TestQuantity::None, false>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::None, true>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::VeryLow, false>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::VeryLow, true>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::Low, false>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::Low, true>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::Mid, false>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::Mid, true>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::High, false>, _fn_<uint16_t, TestQuantity::High,    TestQuantity::High, true>,\
    _fn_<uint32_t, TestQuantity::None,    TestQuantity::None, false>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::None, true>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::VeryLow, false>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::VeryLow, true>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::Low, false>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::Low, true>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::Mid, false>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::Mid, true>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::High, false>, _fn_<uint32_t, TestQuantity::None,    TestQuantity::High, true>,\
    _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::None, false>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::None, true>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::VeryLow, false>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::VeryLow, true>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::Low, false>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::Low, true>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::Mid, false>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::Mid, true>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::High, false>, _fn_<uint32_t, TestQuantity::VeryLow, TestQuantity::High, true>,\
    _fn_<uint32_t, TestQuantity::Low,     TestQuantity::None, false>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::None, true>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::VeryLow, false>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::VeryLow, true>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::Low, false>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::Low, true>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::Mid, false>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::Mid, true>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::High, false>, _fn_<uint32_t, TestQuantity::Low,     TestQuantity::High, true>,\
    _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::None, false>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::None, true>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::VeryLow, false>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::VeryLow, true>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::Low, false>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::Low, true>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::Mid, false>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::Mid, true>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::High, false>, _fn_<uint32_t, TestQuantity::Mid,     TestQuantity::High, true>,\
    _fn_<uint32_t, TestQuantity::High,    TestQuantity::None, false>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::None, true>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::VeryLow, false>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::VeryLow, true>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::Low, false>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::Low, true>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::Mid, false>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::Mid, true>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::High, false>, _fn_<uint32_t, TestQuantity::High,    TestQuantity::High, true>,\
    _fn_<uint64_t, TestQuantity::None,    TestQuantity::None, false>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::None, true>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::VeryLow, false>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::VeryLow, true>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::Low, false>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::Low, true>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::Mid, false>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::Mid, true>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::High, false>, _fn_<uint64_t, TestQuantity::None,    TestQuantity::High, true>,\
    _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::None, false>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::None, true>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::VeryLow, false>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::VeryLow, true>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::Low, false>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::Low, true>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::Mid, false>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::Mid, true>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::High, false>, _fn_<uint64_t, TestQuantity::VeryLow, TestQuantity::High, true>,\
    _fn_<uint64_t, TestQuantity::Low,     TestQuantity::None, false>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::None, true>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::VeryLow, false>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::VeryLow, true>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::Low, false>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::Low, true>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::Mid, false>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::Mid, true>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::High, false>, _fn_<uint64_t, TestQuantity::Low,     TestQuantity::High, true>,\
    _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::None, false>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::None, true>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::VeryLow, false>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::VeryLow, true>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::Low, false>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::Low, true>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::Mid, false>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::Mid, true>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::High, false>, _fn_<uint64_t, TestQuantity::Mid,     TestQuantity::High, true>,\
    _fn_<uint64_t, TestQuantity::High,    TestQuantity::None, false>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::None, true>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::VeryLow, false>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::VeryLow, true>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::Low, false>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::Low, true>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::Mid, false>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::Mid, true>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::High, false>, _fn_<uint64_t, TestQuantity::High,    TestQuantity::High, true>,\
    _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::None, false>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::None, true>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::VeryLow, false>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::VeryLow, true>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::Low, false>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::Low, true>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::Mid, false>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::Mid, true>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::High, false>, _fn_<CC::Helper,   TestQuantity::None,    TestQuantity::High, true>,\
    _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::None, false>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::None, true>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::VeryLow, false>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::VeryLow, true>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::Low, false>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::Low, true>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::Mid, false>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::Mid, true>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::High, false>, _fn_<CC::Helper,   TestQuantity::VeryLow, TestQuantity::High, true>,\
    _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::None, false>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::None, true>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::VeryLow, false>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::VeryLow, true>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::Low, false>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::Low, true>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::Mid, false>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::Mid, true>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::High, false>, _fn_<CC::Helper,   TestQuantity::Low,     TestQuantity::High, true>,\
    _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::None, false>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::None, true>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::VeryLow, false>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::VeryLow, true>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::Low, false>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::Low, true>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::Mid, false>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::Mid, true>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::High, false>, _fn_<CC::Helper,   TestQuantity::Mid,     TestQuantity::High, true>,\
    _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::None, false>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::None, true>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::VeryLow, false>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::VeryLow, true>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::Low, false>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::Low, true>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::Mid, false>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::Mid, true>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::High, false>, _fn_<CC::Helper,   TestQuantity::High,    TestQuantity::High, true>
