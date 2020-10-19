#pragma once

// CC
#include "CCAPIAnnotations.h"
#include "CCMacros.h"
#include "CCIPointer.h"
#include "CCPointerHelper.h"

// STL
#include <atomic>
#include <stdexcept>


namespace CC
{
    template <typename T>
    class [[nodiscard]] SharedPointer : public IPointer<T>
    {
        // Test class.
        friend class SharedPointerTests;

        // Type aliases.
        using PH = PointerHelper<T>;
        using IBase = IPointer<T>;
        using RCIntegral = std::make_signed_t<size_t>;
        using RefCounter = std::atomic<RCIntegral>;

    protected:

        // Protected Data Members //

        T* m_pPtr;
        RefCounter* m_pRefCount;
        size_t m_Len;

        // Static Protected Helper Methods //

        [[nodiscard]] _Ret_maybenull_ static RefCounter* AllocateRefCounter() noexcept
        {
            return new (std::nothrow) RefCounter(1);
        }

        // Protected Helper Methods //

        // Calls the appropriate cleanup function.
        void InvokeFreeFunction() noexcept
        {
            if (m_Len == 1)
            {
                delete m_pPtr;
            }
            else
            {
                delete[] m_pPtr;
            }

            delete m_pRefCount;

            Reset();
        }

        // Calls Free if m_pPtr is not nullptr and m_pRefCount is nullptr.
        // Will return true if Free is called, false if no action is taken.
        void FreeIfAnyCtorOrCopyAllocsFailed() noexcept
        {
            if (!!m_pPtr && !m_pRefCount)
            {
                Free();
            }
        }

        // Increments reference counter if pointer is not nullptr.
        // Note: Returns "old" value (i.e., post-fix increment) if m_pRefCount is not null, 0 otherwise.
        RCIntegral IncrementRefCounter() noexcept
        {
            return (!!m_pRefCount) ? (*m_pRefCount)++ : 0;
        }

        // Decrements reference counter if pointer is not nullptr.
        // Note: Returns "old" value (i.e., post-fix decrement) if m_pRefCount is not null, 0 otherwise.
        [[nodiscard]] RCIntegral DecrementRefCounter() noexcept
        {
            return (!!m_pRefCount) ? (*m_pRefCount)-- : 0;
        }

    public:

        // Public Constructors //

        // Default constructor
        constexpr SharedPointer() noexcept :
            m_pPtr(nullptr),
            m_pRefCount(nullptr),
            m_Len(0)
        { }

        // SharedPointer length constructor
        explicit SharedPointer(_In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T)) :
            m_pPtr(PH::AllocateLen(len)),
            m_pRefCount((!!m_pPtr) ? AllocateRefCounter() : nullptr),
            m_Len((!!m_pPtr) ? len : 0)
        {
            FreeIfAnyCtorOrCopyAllocsFailed();
        }

        // Raw pointer copy constructor
        SharedPointer(_In_reads_opt_(len) const T* p, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T)) :
            m_pPtr(PH::AllocateFromRawPointer(p, len)),
            m_pRefCount((!!m_pPtr) ? AllocateRefCounter() : nullptr),
            m_Len((!!m_pPtr) ? len : 0)
        {
            FreeIfAnyCtorOrCopyAllocsFailed();
        }

        // Raw pointer steal constructor
        SharedPointer(_Inout_opt_ T*& p, _In_ const size_t& len) noexcept :
            m_pPtr(p),
            m_pRefCount((!!m_pPtr) ? AllocateRefCounter() : nullptr),
            m_Len((!!m_pPtr) ? len : 0)
        {
            FreeIfAnyCtorOrCopyAllocsFailed();
            p = nullptr;
        }

        // Copy constructor
        SharedPointer(_In_ const SharedPointer<T>& src) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T)) :
            m_pPtr(src.m_pPtr),
            m_pRefCount(src.m_pRefCount),
            m_Len((!!m_pPtr) ? src.m_Len : 0)
        {
            IncrementRefCounter();
        }

        // Move constructor
        SharedPointer(_Inout_ SharedPointer<T>&& src) noexcept :
            m_pPtr(src.m_pPtr),
            m_pRefCount(src.m_pRefCount),
            m_Len((!!m_pPtr) ? src.m_Len : 0)
        {
            src.Reset();
        }

        // Public Virtual Destructor //

        virtual ~SharedPointer() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            Free();
        }

        // Public Assignment Overloads //

        SharedPointer<T>& operator=(_In_ const SharedPointer<T>& src) noexcept
        {
            if (this != &src)
            {
                Free();
                m_pPtr = src.m_pPtr;
                m_pRefCount = src.m_pRefCount;
                m_Len = src.m_Len;
                IncrementRefCounter();
            }

            return *this;
        }

        SharedPointer<T>& operator=(_Inout_ SharedPointer<T>&& src) noexcept
        {
            if (this != &src)
            {
                Free();
                m_pPtr = src.m_pPtr;
                m_pRefCount = src.m_pRefCount;
                m_Len = src.m_Len;
                src.Reset();
            }

            return *this;
        }

        // Public Operator Overloads //

        [[nodiscard]] virtual explicit operator bool() const noexcept
        {
            return !!m_pPtr;
        }

        [[nodiscard]] virtual explicit operator T* () noexcept
        {
            return m_pPtr;
        }

        [[nodiscard]] virtual explicit operator const T* () const noexcept
        {
            return m_pPtr;
        }

        [[nodiscard]] virtual T& operator*()
        {
            PH::ValidateDereferenceT(__PRETTY_FUNCTION__, m_pPtr);
            return *m_pPtr;
        }

        [[nodiscard]] virtual const T& operator*() const
        {
            PH::ValidateDereferenceT(__PRETTY_FUNCTION__, m_pPtr);
            return *m_pPtr;
        }

        [[nodiscard]] virtual T* operator->()
        {
            PH::ValidateDereferenceT(__PRETTY_FUNCTION__, m_pPtr);
            return m_pPtr;
        }

        [[nodiscard]] virtual const T* operator->() const
        {
            PH::ValidateDereferenceT(__PRETTY_FUNCTION__, m_pPtr);
            return m_pPtr;
        }

        // Getters //

        // Returns pointer to mutable internal pointer.
        [[nodiscard]] virtual T* Get() noexcept
        {
            return m_pPtr;
        }

        // Returns pointer to immutable internal pointer.
        [[nodiscard]] virtual const T* Get() const noexcept
        {
            return m_pPtr;
        }

        // Returns length of elements pointed to by internal pointer.
        [[nodiscard]] virtual const size_t& Length() const noexcept
        {
            return m_Len;
        }

        // Returns size in bytes of data element(s) pointed to by internal pointer.
        [[nodiscard]] virtual size_t Size() const noexcept
        {
            return sizeof(T) * m_Len;
        }

        // Public Methods //

        // Resets internal data members to default values.
        // Note: This will not free any internal resources - use with caution.
        virtual void Reset() noexcept
        {
            m_pPtr = nullptr;
            m_pRefCount = nullptr;
            m_Len = 0;
        }

        // Frees resources and resets data members to default values.
        virtual void Free() noexcept
        {
            if (DecrementRefCounter() == 1)
            {
                InvokeFreeFunction();
            }
            else
            {
                Reset();
            }
        }
    };
}