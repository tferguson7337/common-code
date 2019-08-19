#pragma once

// CC
#include "CCIPointer.h"
#include "CCMacros.h"

// STL
#include <stdexcept>


namespace CC
{
    template <typename T>
    class PointerHelper
    {
        friend class PointerHelperTests;

    public:

        /// Public Default Constructor, Virtual Destructor \\\

        PointerHelper() noexcept = delete;
        PointerHelper(const PointerHelper&) noexcept = delete;
        PointerHelper(PointerHelper&&) noexcept = delete;
        ~PointerHelper() noexcept = delete;
        PointerHelper& operator=(const PointerHelper&) noexcept = delete;
        PointerHelper& operator=(PointerHelper&&) noexcept = delete;

        /// Common Static Public Throwing Validator Methods \\\

        // Throw std::logic_error if p is nullptr.
        inline static void ValidateDereferenceT(_In_z_ const char* const f, _In_opt_ const T* const p)
        {
            if (!p)
            {
                static const std::string msg = ": Attempted to dereference nullptr.";
                throw std::logic_error(f + msg);
            }
        }

        /// Common Public Static Helper Methods \\\

        // Copies len element from src into dst.
        static void CopyToRawPointer(_Out_writes_opt_(len) T* dst, _In_reads_opt_(len) const T* src, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            if (!dst || !src)
            {
                return;
            }

            if constexpr (!std::is_class_v<T> && !std::is_union_v<T>)
            {
                memcpy(dst, src, sizeof(T) * len);
            }
            else
            {
                for (size_t i = 0; i < len; i++)
                {
                    dst[i] = src[i];
                }
            }
        }

        // Moves len element from src into dst.
        static void MoveToRawPointer(_Out_writes_opt_(len) T* dst, _Inout_opt_count_(len) T* src, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            if (!dst || !src)
            {
                return;
            }

            if constexpr (!std::is_class_v<T> && !std::is_union_v<T>)
            {
                memcpy(dst, src, sizeof(T) * len);
            }
            else
            {
                for (size_t i = 0; i < len; i++)
                {
                    dst[i] = std::move(src[i]);
                }
            }
        }

        // Allocates and returns address to single T element, forwards any arguments to constructor.
        template <typename... Args>
        [[nodiscard]] _Ret_maybenull_ static T* Allocate(_In_ Args&& ... args) noexcept(CC_IS_NOTHROW_CTOR_A(T, Args))
        {
            return new (std::nothrow) T(std::forward<Args>(args)...);
        }

        // Allocates and returns address to len T elements.
        [[nodiscard]] _Ret_maybenull_ static T* AllocateLen(_In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T))
        {
            if (len == 0)
            {
                return nullptr;
            }
            else if (len == 1)
            {
                return new (std::nothrow) T;
            }
            else
            {
                return new (std::nothrow) T[len];
            }
        }

        // Allocates pointer to len T elements, copies contents of ptr to the new memory block.
        [[nodiscard]] _Ret_writes_maybenull_(len) static T* AllocateFromRawPointer(_In_reads_opt_(len) const T* src, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T))
        {
            T* p = nullptr;
            if (!!src && len > 0)
            {
                p = AllocateLen(len);
                if (!!p)
                {
                    CopyToRawPointer(p, src, len);
                }
            }

            return p;
        }

        // Allocates pointer to len T elements, copies contents of raw pointer to the new memory block.
        [[nodiscard]] _Ret_maybenull_ static T* AllocateFromIPointerObj(_In_ const IPointer<T>& src) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T))
        {
            return AllocateFromRawPointer(src.Get(), src.Length());
        }
    };
}