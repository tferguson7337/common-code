#pragma once

#include <stdexcept>

namespace CC
{
    template <typename T>
    class Pointer
    {
        // Testing class.
        friend class PointerTests;

    protected:

        /// Type Aliases \\\

        using FreeFunc = void(*)(T*);

        /// Protected Member Functions \\\

        T* m_pPtr;
        size_t m_Len;
        FreeFunc m_FreeFunc;

        /// Static Protected Throwing Validator Methods \\\

        // Throw std::logic_error if p is nullptr.
        inline static void ValidateDereferenceT(_In_ const char* const f, _In_opt_ const T* const p)
        {
            if ( !p )
            {
                static const std::string msg = ": Attempted to dereference nullptr.";
                throw std::logic_error(f + msg);
            }
        }

        /// Static Protected Helper Methods \\\

        // Cleanup function - used for single-element raw pointers.
        static void FreeSingle(_In_opt_ _Post_ptr_invalid_ T* p) noexcept
        {
            delete p;
        }

        // Cleanup function - used for multi-element raw pointers.
        static void FreeArray(_In_opt_ _Post_ptr_invalid_ T* p) noexcept
        {
            delete[ ] p;
        }

        // Allocates pointer to len T elements.
        // Note: Will return nullptr if allocation fails.
        _Ret_maybenull_ static T* Allocate(_In_ const size_t& len) noexcept
        {
            if ( len == 0 )
            {
                return nullptr;
            }
            else if ( len == 1 )
            {
                return new (std::nothrow) T;
            }
            else
            {
                return new (std::nothrow) T[len];
            }
        }

        // Copies len element from src into dst.
        static void CopyToRawPointer(_Out_writes_opt_(len) T* dst, _In_reads_opt_(len) const T* src, _In_ const size_t& len) noexcept(std::is_scalar_v<T>)
        {
            if ( !dst || !src )
            {
                return;
            }

            if constexpr ( std::is_scalar_v<T> )
            {
                memcpy(dst, src, sizeof(T) * len);
            }
            else
            {
                for ( size_t i = 0; i < len; i++ )
                {
                    dst[i] = src[i];
                }
            }
        }

        // Moves len element from src into dst.
        static void MoveToRawPointer(_Out_writes_opt_(len) T* dst, _Inout_opt_count_(len) T* src, _In_ const size_t& len) noexcept
        {
            if ( !dst || !src )
            {
                return;
            }

            if constexpr ( std::is_scalar_v<T> )
            {
                memcpy(dst, src, sizeof(T) * len);
            }
            else
            {
                for ( size_t i = 0; i < len; i++ )
                {
                    dst[i] = std::move(src[i]);
                }
            }
        }

        // Allocates pointer to len T elements, copies contents of ptr to the new memory block.
        _Ret_writes_maybenull_(len) static T* AllocateFromRawPointer(_In_reads_opt_(len) const T* src, _In_ const size_t& len) noexcept(std::is_scalar_v<T>)
        {
            T* p = nullptr;
            if ( !!src && len > 0 )
            {
                p = Allocate(len);
                CopyToRawPointer(p, src, len);
            }

            return p;
        }

        // Allocates pointer to len T elements, copies contents of raw pointer to the new memory block.
        _Ret_maybenull_ static T* AllocateFromPointerObj(_In_ const Pointer<T>& src) noexcept(std::is_scalar_v<T>)
        {
            T* p = Allocate(src.m_Len);
            CopyToRawPointer(p, src.m_pPtr, src.m_Len);
            return p;
        }

        // Copies all Pointer data members from source Pointer object.
        // Note: The pointer data member is shallow copied.
        static void CopyAllDataMembers(_Out_ Pointer<T>& dst, _In_ Pointer<T>& src) noexcept
        {
            dst.m_pPtr = src.m_pPtr;
            CopyNonRawPointerDataMembers(dst, src);
        }

        // Copies all Pointer data members from source Pointer object, except for the internal pointer.
        static void CopyNonRawPointerDataMembers(_Out_ Pointer<T>& dst, _In_ const Pointer<T>& src) noexcept
        {
            dst.m_Len = src.m_Len;
            dst.m_FreeFunc = dst.GetFreeFunction( );
        }

        // Performs deep copy of src pointer, frees dst pointer, then assigned new pointer to dst.
        // Also copies all other Pointer data members of src to dst.
        // Note: If src pointer is nullptr, then dst will free its pointer and replace it with nullptr.
        static void CopyPointerObj(_Inout_ Pointer<T>& dst, _In_ const Pointer<T>& src) noexcept(std::is_scalar_v<T>)
        {
            T* p = AllocateFromPointerObj(src);
            dst.InvokeFreeFunction( );

            dst.m_pPtr = p;
            CopyNonRawPointerDataMembers(dst, src);
        }

        // Performs shallow copy of src Pointer data members to dst, then resets src.
        static void TransferPointerObj(_Inout_ Pointer<T>& dst, _Inout_ Pointer<T>& src) noexcept
        {
            dst.InvokeFreeFunction( );
            CopyAllDataMembers(dst, src);
            src.Reset( );
        }

        /// Protected Helper Methods \\\

        // Returns function pointer to appropriate cleanup function.
        // nullptr if m_Len == 0, FreeSingle if m_Len == 1, and FreeArray otherwise.
        _Ret_maybenull_ FreeFunc GetFreeFunction( ) noexcept
        {
            if ( m_Len == 0 )
            {
                return nullptr;
            }
            else if ( m_Len == 1 )
            {
                return FreeSingle;
            }
            else
            {
                return FreeArray;
            }
        }

        // Calls the appropriate cleanup function, if one has been assigned.
        void InvokeFreeFunction( ) noexcept
        {
            if ( m_FreeFunc )
            {
                m_FreeFunc(m_pPtr);
            }
        }

    public:

        /// Public Constructors \\\

        // Default constructor
        constexpr Pointer( ) noexcept :
            m_pPtr(nullptr),
            m_Len(0),
            m_FreeFunc(nullptr)
        { }

        // Pointer length constructor
        explicit Pointer(_In_ const size_t& len) noexcept :
            m_pPtr(Allocate(len)),
            m_Len((!!m_pPtr) ? len : 0),
            m_FreeFunc(GetFreeFunction( ))
        { }

        // Raw pointer copy constructor
        Pointer(_In_reads_opt_(len) const T* p, _In_ const size_t& len) noexcept(std::is_scalar_v<T>) :
            m_pPtr(AllocateFromRawPointer(p, len)),
            m_Len((!!m_pPtr) ? len : 0),
            m_FreeFunc(GetFreeFunction( ))
        { }

        // Raw pointer steal constructor
        Pointer(_Inout_opt_ T*&p, _In_ const size_t& len) noexcept :
            m_pPtr(p),
            m_Len((!!m_pPtr) ? len : 0),
            m_FreeFunc(GetFreeFunction( ))
        {
            p = nullptr;
        }

        // Copy constructor
        Pointer(_In_ const Pointer<T>& src) noexcept(std::is_scalar_v<T>) :
            m_pPtr(AllocateFromPointerObj(src)),
            m_Len(src.m_Len),
            m_FreeFunc(GetFreeFunction( ))
        { }

        // Move constructor
        Pointer(_Inout_ Pointer<T>&& src) noexcept :
            m_pPtr(src.m_pPtr),
            m_Len(src.m_Len),
            m_FreeFunc(GetFreeFunction( ))
        {
            src.Reset( );
        }

        /// Public Destructor \\\

        // Destructor
        ~Pointer( ) noexcept
        {
            InvokeFreeFunction( );
        }

        /// Operator Overloads \\\

        // Copy assignment
        Pointer<T>& operator=(_In_ const Pointer<T>& src) noexcept(std::is_scalar_v<T>)
        {
            if ( this != &src )
            {
                CopyPointerObj(*this, src);
            }

            return *this;
        }

        // Move assignment
        Pointer<T>& operator=(_Inout_ Pointer<T>&& src) noexcept
        {
            if ( this != &src )
            {
                TransferPointerObj(*this, src);
            }

            return *this;
        }

        // bool overload - returns true if m_pPtr != nullptr, false otherwise.
        virtual explicit operator bool( ) const noexcept
        {
            return m_pPtr != nullptr;
        }

        // T* overload - returns pointer to mutable internal pointer.
        virtual explicit operator T*() noexcept
        {
            return m_pPtr;
        }

        // const T* overload - returns pointer to immutable internal pointer.
        virtual explicit operator const T*() const noexcept
        {
            return m_pPtr;
        }

        // Dereference overload - returns reference to first element from internal pointer (mutable).
        // Note: Will throw std::logic_error if m_pPtr == nullptr.
        virtual T& operator*( )
        {
            ValidateDereferenceT(__FUNCSIG__, m_pPtr);
            return *m_pPtr;
        }

        // Dereference overload - returns reference to first element from internal pointer (immutable).
        // Note: Will throw std::logic_error if m_pPtr == nullptr.
        virtual const T& operator*( ) const
        {
            ValidateDereferenceT(__FUNCSIG__, m_pPtr);
            return *m_pPtr;
        }

        // Pointer-member-access overload - returns reference to internal pointer (mutable).
        // Note: Will throw std::logic_error if m_pPtr == nullptr.
        virtual T* operator->( )
        {
            // Technically not dereferencing here, but the intention is likely to access a data member.
            ValidateDereferenceT(__FUNCSIG__, m_pPtr);
            return m_pPtr;
        }

        // Pointer-member-access overload - returns reference to internal pointer (immutable).
        // Note: Will throw std::logic_error if m_pPtr == nullptr.
        virtual const T* operator->( ) const
        {
            // Technically not dereferencing here, but the intention is likely to access a data member.
            ValidateDereferenceT(__FUNCSIG__, m_pPtr);
            return m_pPtr;
        }

        /// Getters \\\

        // Returns pointer to internal pointer (mutable).
        virtual T* Ptr( ) noexcept
        {
            return m_pPtr;
        }

        // Returns pointer to internal pointer (immutable).
        virtual const T* Ptr( ) const noexcept
        {
            return m_pPtr;
        }

        // Returns length of internal pointer.
        virtual const size_t& Length( ) const noexcept
        {
            return m_Len;
        }

        // Returns size in bytes of internal pointer.
        virtual const size_t Size( ) const noexcept
        {
            return sizeof(T) * m_Len;
        }

        /// Public Methods \\\

        // Resets internal data members to default values.
        // Note: This will not free any internal resources - use with caution.
        virtual void Reset( ) noexcept
        {
            m_pPtr = nullptr;
            m_Len = 0;
            m_FreeFunc = nullptr;
        }

        // Causes the pointer to free any resources, resetting data members to their default values.
        virtual void Free( ) noexcept
        {
            InvokeFreeFunction( );
            Reset( );
        }
    };
}