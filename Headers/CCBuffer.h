#pragma once

#include "CCIBuffer.h"
#include "CCPointer.h"
#include "CCTypes.h"

#include <stdexcept>

namespace CC
{
    template <typename T>
    class Buffer : public IBuffer<T>, public Pointer<T>
    {
        // Testing class.
        friend class BufferTests;

    protected:

        /// Protected Data Members \\\

        size_t m_WritePos;

        /// Protected Validator Methods \\\ 

        // Returns false if pos >= this->m_Len - returns true otherwise.
        inline bool ValidateNewWritePosition(_In_ const size_t& pos) const noexcept
        {
            return pos <= Length( );
        }

        // Returns false if m_WritePos >= this->m_Len.
        // Returns true otherwise.
        inline bool ValidateSingleElementWriteRequest( ) const noexcept
        {
            return m_WritePos < Length( );
        }

        // Returns false if !p || !this->m_pPtr || writeLen == 0 || ((m_WritePos + writeLen) <= this->m_Len)
        // Returns true otherwise.
        inline bool ValidateWriteRequest(_In_opt_ const T* const p, _In_ const size_t& writeLen) const noexcept
        {
            return !!p && !!Ptr( ) && writeLen > 0 && ((m_WritePos + writeLen) <= Length( ));
        }

        /// Protected Throwing Validator Methods \\\

        // Returns false if idx >= this->m_Len - returns true otherwise.
        inline void ValidateAccessorIndexT(_In_ const char* const f, _In_ const size_t& idx) const
        {
            Pointer<T>::ValidateDereferenceT(f, Ptr( ));
            if ( idx >= Length( ) )
            {
                static const std::string msg1 = ": Index[";
                static const std::string msg2 = "] exceeds length of buffer[";
                static const std::string msg3 = "].";
                const std::string idxStr = std::to_string(idx);
                const std::string endStr = std::to_string(Length( ));

                throw std::out_of_range(f + msg1 + idxStr + msg2 + endStr + msg3);
            }
        }

        /// Static Protected Helper Methods \\\

        // Copies all Buffer data members from source IBuffer object.
        // Note: The pointer data member is shallow copied.
        static void CopyAllBufferDataMembers(_Out_ Buffer<T>& dst, _In_ IBuffer<T>& src) noexcept
        {
            dst.m_pPtr = src.Ptr( );
            CopyNonPointerBufferDataMembers(dst, src);
        }

        // Copies all Buffer data members from source IBuffer object, except for the internal buffer pointer.
        static void CopyNonPointerBufferDataMembers(_Out_ Buffer<T>& dst, _In_ const IBuffer<T>& src) noexcept
        {
            dst.m_Len = src.Length( );
            dst.m_FreeFunc = dst.GetFreeFunction( );
            dst.m_WritePos = src.WritePosition( );
        }

        // Performs deep copy of src buffer, frees dst buffer, then assigned new buffer to dst.
        // Also copies all other Buffer data members of src to dst.
        // Note: If src buffer is nullptr, then dst will free buffer and replace it with nullptr.
        static void CopyBuffer(_Inout_ Buffer<T>& dst, _In_ const IBuffer<T>& src) noexcept(std::is_scalar_v<T>)
        {
            T* p = Pointer<T>::AllocateFromRawPointer(src.Ptr( ), src.Length( ));
            dst.InvokeFreeFunction( );

            dst.m_pPtr = p;
            CopyNonPointerBufferDataMembers(dst, src);
        }

        // Performs shallow copy of src Buffer data members to dst, then resets src.
        static void TransferBuffer(_Inout_ Buffer<T>& dst, _Inout_ IBuffer<T>& src) noexcept
        {
            dst.InvokeFreeFunction( );
            CopyAllBufferDataMembers(dst, src);
            src.Reset( );
        }

        /// Protected Helper Methods \\\

        // Calls the appropriate cleanup function, if one has been assigned.
        void InvokeFreeFunction( ) noexcept
        {
            if ( this->m_FreeFunc )
            {
                this->m_FreeFunc(Ptr( ));
            }
        }

        // Writes elements via copy to internal buffer, updates write position.
        inline bool WriteInternal(_In_ const T* p, _In_ const size_t& len) noexcept(std::is_scalar_v<T>)
        {
            Pointer<T>::CopyToRawPointer(Ptr( ) + m_WritePos, p, len);
            WriteInternalCommon(len);
            return true;
        }

        // Writes elements via move semantics to internal buffer, updates write position.
        inline bool WriteInternal(_In_ T* p, _In_ const size_t& len) noexcept
        {
            Pointer<T>::MoveToRawPointer(Ptr( ) + m_WritePos, p, len);
            WriteInternalCommon(len);
            return true;
        }

        // Common operation for all writes: updates write position.
        inline void WriteInternalCommon(_In_ const size_t& len) noexcept
        {
            m_WritePos += len;
        }

    public:

        /// Public Constructors \\\

        // Default constructor
        constexpr Buffer( ) noexcept :
            Pointer<T>( ),
            m_WritePos(0)
        { }

        // Buffer length constructor
        explicit Buffer(_In_ const size_t& len) noexcept :
            Pointer<T>(len),
            m_WritePos(0)
        { }

        // Raw pointer copy constructor
        Buffer(_In_reads_opt_(len) const T* p, _In_ const size_t& len) noexcept(std::is_scalar_v<T>) :
            Pointer<T>(p, len),
            m_WritePos(0)
        { }

        // Raw pointer steal constructor
        Buffer(_Inout_opt_ T*&p, _In_ const size_t& len) noexcept :
            Pointer<T>(p, len),
            m_WritePos(0)
        { }

        // Copy constructor
        Buffer(_In_ const Buffer<T>& src) noexcept(std::is_scalar_v<T>) :
            Pointer<T>(src),
            m_WritePos(src.WritePosition( ))
        { }

        // Move constructor
        Buffer(_Inout_ Buffer<T>&& src) noexcept :
            Pointer<T>(std::move(src)),
            m_WritePos(src.WritePosition( ))
        {
            src.Reset( );
        }

        // Interface copy constructor
        Buffer(_In_ const IBuffer<T>& src) noexcept(std::is_scalar_v<T>) :
            Pointer<T>(src.Ptr( ), src.Length( )),
            m_WritePos(src.WritePosition( ))
        { }

        // Interface move constructor
        Buffer(_Inout_ IBuffer<T>&& src) noexcept :
            Pointer<T>(src.Ptr( ), src.Length( )),
            m_WritePos(src.WritePosition( ))
        {
            src.Reset( );
        }

        /// Public Destructor \\\

        // Destructor
        virtual ~Buffer( ) noexcept = default;

        /// Operator Overloads \\\

        // Copy assignment
        Buffer<T>& operator=(_In_ const Buffer<T>& src) noexcept(std::is_scalar_v<T>)
        {
            if ( this != &src )
            {
                CopyBuffer(*this, src);
            }

            return *this;
        }

        // Move assignment
        Buffer<T>& operator=(_Inout_ Buffer<T>&& src) noexcept
        {
            if ( this != &src )
            {
                TransferBuffer(*this, src);
            }

            return *this;
        }

        // Interface copy assignment
        IBuffer<T>& operator=(_In_ const IBuffer<T>& src) noexcept(std::is_scalar_v<T>)
        {
            if ( this != &src )
            {
                CopyBuffer(*this, src);
            }

            return *this;
        }

        // Interface move assignment
        IBuffer<T>& operator=(_Inout_ IBuffer<T>&& src) noexcept
        {
            if ( this != &src )
            {
                TransferBuffer(*this, src);
            }

            return *this;
        }

        // Return true for non-null buffer - false otherwise.
        virtual explicit operator bool( ) const noexcept
        {
            return !!Ptr( );
        }

        // Return pointer to internal buffer (mutable).
        virtual explicit operator T*() noexcept
        {
            return Ptr( );
        }

        // Return pointer to internal buffer (immutable).
        virtual explicit operator const T*() const noexcept
        {
            return Ptr( );
        }

        // Subscript overload - returns reference to mutable element from internal buffer via index.
        // Note: Will throw std::logic_error if this->m_pPtr == nullptr.
        // Note: Will throw std::out_of_range if i >=this->m_Len.
        virtual T& operator[](_In_ const size_t& i)
        {
            ValidateAccessorIndexT(__FUNCSIG__, i);
            return Ptr( )[i];
        }

        // Subscript overload - returns reference to immutable element from internal buffer via index.
        // Note: Will throw std::logic_error if this->m_pPtr == nullptr.
        // Note: Will throw std::out_of_range if i >=this->m_Len.
        virtual const T& operator[](_In_ const size_t& i) const
        {
            ValidateAccessorIndexT(__FUNCSIG__, i);
            return Ptr( )[i];
        }

        // Comparison Equal-To overload - returns true if contents of specified buffer match this->m_pPtr, false otherwise.
        virtual const bool operator==(_In_ const IBuffer<T>& rhs) const noexcept
        {
            return Compare(rhs.Ptr( ), rhs.Length( ));
        }

        // Comparison Not-Equal-To overload - returns true if contents of specified buffer do not match this->m_pPtr, false otherwise.
        virtual const bool operator!=(_In_ const IBuffer<T>& rhs) const noexcept
        {
            return !Compare(rhs.Ptr( ), rhs.Length( ));
        }

        /// Getters \\\

        // Returns pointer to internal buffer (mutable).
        virtual T* Ptr( ) noexcept
        {
            return Pointer<T>::Ptr( );
        }

        // Returns pointer to internal buffer (immutable).
        virtual const T* Ptr( ) const noexcept
        {
            return Pointer<T>::Ptr( );
        }

        // Returns length of internal buffer.
        virtual const size_t& Length( ) const noexcept
        {
            return Pointer<T>::Length( );
        }

        // Returns size in bytes of internal buffer.
        virtual const size_t Size( ) const noexcept
        {
            return Pointer<T>::Size( );
        }

        // Returns current write position.
        virtual const size_t& WritePosition( ) const noexcept
        {
            return m_WritePos;
        }

        /// Setters \\\

        // Sets write position to specified value.
        // Note: Will throw std::out_of_range if pos >= this->m_Len
        virtual bool SetWritePosition(_In_ const size_t& pos) noexcept
        {
            if ( !ValidateNewWritePosition(pos) )
            {
                return false;
            }

            m_WritePos = pos;
            return true;
        }

        // Resets write position to beginning of buffer (0).
        virtual void ResetWritePosition( ) noexcept
        {
            m_WritePos = 0;
        }

        /// Public Methods \\\

        // Resets internal data members to default values.
        // Note: This will not free any internal resources - use with caution.
        virtual void Reset( ) noexcept
        {
            Pointer<T>::Reset( );
            m_WritePos = 0;
        }

        // Causes the buffer to free any resources, resetting data members to their default values.
        virtual void Free( ) noexcept
        {
            Pointer<T>::InvokeFreeFunction( );
        }

        // Returns true if contents of this->m_pPtr match contents of p (up to len), false otherwise.
        virtual const bool Compare(_In_reads_opt_(len) const T* p, _In_ const size_t& len) const noexcept
        {
            bool bRet = true;

            // Trivial case(t): both pointers point to same addr.
            if ( Ptr( ) == p )
            {
                return true;
            }

            // Trivial cases(f): Either pointer is null or requested comparison-length exceeds length of buffer.
            if ( !Ptr( ) || !p || len > Length( ) )
            {
                return false;
            }

            // Use memcmp if possible (scalar values only).
            if constexpr ( std::is_scalar_v<T> )
            {
                bRet = (memcmp(Ptr( ), p, len * sizeof(T)) == 0);
            }
            else
            {
                for ( size_t i = 0; i < len; i++ )
                {
                    if ( Ptr( )[i] != p[i] )
                    {
                        return false;
                    }
                }
            }

            return bRet;
        }

        // Returns true if contents of this->m_pPtr match contents of buffer.Ptr( ), false otherwise.
        virtual const bool Compare(_In_ const IBuffer<T>& buffer) const noexcept
        {
            return Compare(buffer.Ptr( ), buffer.Length( ));
        }

        // Writes specified T object to internal buffer at m_WritePos.
        // Returns false if write would exceed length of buffer, true otherwise.
        virtual bool Write(_In_ const T& t) noexcept(std::is_scalar_v<T>)
        {
            return ValidateSingleElementWriteRequest( ) && WriteInternal(&t, 1);
        }

        // Writes specified T object to internal buffer at m_WritePos.
        // Returns false if write would exceed length of buffer, true otherwise.
        virtual bool Write(_Inout_ T&& t) noexcept
        {
            return ValidateSingleElementWriteRequest( ) && WriteInternal(&t, 1);
        }

        // Null-pointer write.
        // Returns false.
        virtual bool Write(_In_ const std::nullptr_t&, _In_ const size_t&) noexcept
        {
            return false;
        }

        // Copies raw pointer content to internal buffer at m_WritePos, increments m_WritePos by len on successful write.
        virtual bool Write(_In_reads_opt_(len) const T* const src, _In_ const size_t& len) noexcept(std::is_scalar_v<T>)
        {
            return ValidateWriteRequest(src, len) && WriteInternal(src, len);
        }

        // Moves raw pointer content to internal buffer at m_WritePos, increments m_WritePos by len on successful write.
        virtual bool Write(_Inout_opt_count_(len) T*&& src, _In_ const size_t& len) noexcept
        {
            return ValidateWriteRequest(src, len) && WriteInternal(src, len);
        }

        // Copies source buffer content to internal buffer at m_WritePos.
        // Note: If bCopyUpToSrcWritePos == true, will write up to src.WritePosition( ), otherwise up to src.Length( ).
        virtual bool Write(_In_ const IBuffer<T>& src, _In_ const bool& bCopyUpToSrcWritePos = true) noexcept(std::is_scalar_v<T>)
        {
            return Write(src.Ptr( ), bCopyUpToSrcWritePos ? src.WritePosition( ) : src.Length( ));
        }

        // Moves source buffer content to internal buffer at m_WritePos.
        // Note: If bCopyUpToSrcWritePos == true, will write up to src.WritePosition( ), otherwise up to src.Length( ).
        virtual bool Write(_In_ IBuffer<T>&& src, _In_ const bool& bCopyUpToSrcWritePos = true) noexcept
        {
            return Write(std::move(src.Ptr( )), bCopyUpToSrcWritePos ? src.WritePosition( ) : src.Length( ));
        }
    };
}
