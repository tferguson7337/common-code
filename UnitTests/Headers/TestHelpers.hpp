#pragma once

// SUTL
#include <TestQuantity.h>

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

        static_assert(IsValidTestQuantity<_TQ>(), __FUNCSIG__": Invalid TestQuantity Template Argument.");

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
            static_assert(false, __FUNCSIG__": Unknown TestQuantity Template Argument.");
        }
    }

    template <typename T, TestQuantity TQ>
    static std::vector<T> GetTestData()
    {
        static_assert(IsValidTestType<T>(), __FUNCSIG__": Invalid Template Type Argument");
        static_assert(IsValidTestQuantity<TQ>(), __FUNCSIG__": Invalid TestQuantity Template Argument.");

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
