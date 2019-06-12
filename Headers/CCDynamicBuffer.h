#pragma once

#include "CCBuffer.h"

namespace CC
{
    template <typename T>
    class DynamicBuffer : public Buffer<T>
    {
        // Testing class.
        friend class DynamicBufferTests;

    protected:

        // Calculates new length for growing buffer.
        // If geometric increase is sufficent, then new length will be ((m_Len * 3) >> 1) (i.e., +50%)
        // Otherwise, the new length will be m_Len + minInc.
        static const size_t CalculateNewLength(_In_ const size_t& oldLen, _In_ const size_t& minInc) noexcept
        {
            const size_t geoNewLen = (oldLen * 3) >> 1;
            const size_t minIncLen = oldLen + minInc;
            const size_t newLen = (geoNewLen < minIncLen) ? minIncLen : geoNewLen;
            return (newLen > 16) ? newLen : 16;
        }

        // Grows buffer to new length, if required.
        bool GrowBufferIfRequired(_In_ const size_t& minInc) noexcept
        {
            if ( (this->WritePosition( ) + minInc) > this->Length( ) )
            {
                DynamicBuffer<T> newBuf(CalculateNewLength(this->Length( ), minInc));
                if ( !newBuf )
                {
                    return false;
                }

                Pointer<T>::MoveToRawPointer(newBuf.Ptr( ), this->Ptr( ), this->WritePosition( ));
                newBuf.m_WritePos = this->WritePosition( );
                Buffer<T>::TransferBuffer(*this, newBuf);
            }

            return true;
        }

        // Common wrapper for copy assignment.
        DynamicBuffer<T>& CopyAssignmentCommon(_In_ const IBuffer<T>& src) noexcept
        {
            if ( this != &src )
            {
                Buffer<T>::operator=(src);
            }

            return *this;
        }

        // Common wrapper for move assignment.
        DynamicBuffer<T>& MoveAssignmentCommon(_Inout_ IBuffer<T>&& src) noexcept
        {
            if ( this != &src )
            {
                Buffer<T>::operator=(std::move(src));
            }

            return *this;
        }

    public:

        /// Public Constructors \\\

        // Default constructor
        constexpr DynamicBuffer( ) noexcept :
            Buffer<T>( )
        { }

        // Buffer length constructor
        explicit DynamicBuffer(_In_ const size_t& len) noexcept :
            Buffer<T>(len)
        { }

        // Raw pointer copy constructor
        DynamicBuffer(_In_reads_opt_(len) const T* p, _In_ const size_t& len) noexcept(std::is_scalar_v<T>) :
            Buffer<T>(p, len)
        { }

        // Raw pointer steal constructor
        DynamicBuffer(_Inout_opt_ T*& p, _In_ const size_t& len) noexcept :
            Buffer<T>(p, len)
        { }

        // Copy constructor
        DynamicBuffer(_In_ const DynamicBuffer<T>& src) noexcept(std::is_scalar_v<T>) :
            Buffer<T>(src)
        { }

        // Move constructor
        DynamicBuffer(_Inout_ DynamicBuffer<T>&& src) noexcept :
            Buffer<T>(std::move(src))
        { }

        // Interface copy constructor
        DynamicBuffer(_In_ const IBuffer<T>& src) noexcept(std::is_scalar_v<T>) :
            Buffer<T>(src)
        { }

        // Interface move constructor
        DynamicBuffer(_Inout_ IBuffer<T>&& src) noexcept :
            Buffer<T>(std::move(src))
        { }

        /// Public Destructor \\\

        // Destructor
        virtual ~DynamicBuffer( ) noexcept = default;

        /// Operator Overloads \\\

        // Copy Assignment
        DynamicBuffer<T>& operator=(_In_ const DynamicBuffer<T>& src) noexcept(std::is_scalar_v<T>)
        {
            return CopyAssignmentCommon(src);
        }

        // Move Assignment
        DynamicBuffer<T>& operator=(_Inout_ DynamicBuffer<T>&& src) noexcept
        { 
            return MoveAssignmentCommon(std::move(src));
        }

        // Interface Copy Assignment
        DynamicBuffer<T>& operator=(_In_ const IBuffer<T>& src) noexcept(std::is_scalar_v<T>)
        {
            return CopyAssignmentCommon(src);
        }

        // Interface Move Assignment
        DynamicBuffer<T>& operator=(_Inout_ IBuffer<T>&& src) noexcept
        {
            return MoveAssignmentCommon(std::move(src));
        }

        /// Public Methods \\\

        // Writes specified T object to internal buffer at m_WritePos.
        // Note: Will grow the internal buffer to hold the new element if the buffer is full.
        virtual bool Write(_In_ const T& t) noexcept(std::is_scalar_v<T>)
        {
            GrowBufferIfRequired(1);
            return Buffer<T>::Write(t);
        }

        // Writes specified T object to internal buffer at m_WritePos.
        // Note: Will grow the internal buffer to hold the new element if the buffer is full.
        virtual bool Write(_Inout_ T&& t) noexcept
        {
            GrowBufferIfRequired(1);
            return Buffer<T>::Write(std::move(t));
        }

        // Null-pointer write.
        // Returns false.
        virtual bool Write(_In_ const std::nullptr_t&, _In_ const size_t&) noexcept
        {
            return false;
        }

        // Copies raw pointer content to internal buffer at m_WritePos, increments m_WritePos by len on successful write.
        // Note: Will grow the internal buffer to hold the new element if the buffer is full.
        virtual bool Write(_In_reads_opt_(len) const T* const src, _In_ const size_t& len) noexcept(std::is_scalar_v<T>)
        {
            GrowBufferIfRequired(len);
            return Buffer<T>::Write(src, len);
        }

        // Moves raw pointer content to internal buffer at m_WritePos, increments m_WritePos by len on successful write.
        // Note: Will grow the internal buffer to hold the new element if the buffer is full.
        virtual bool Write(_Inout_opt_count_(len) T*&& src, _In_ const size_t& len) noexcept
        {
            GrowBufferIfRequired(len);
            return Buffer<T>::Write(std::move(src), len);
        }

        // Copies source buffer content to internal buffer at m_WritePos.
        // Note: If bCopyUpToSrcWritePos == true, will write up to src.WritePosition( ), otherwise up to src.Length( ).
        // Note: Will grow the internal buffer to hold the new element if the buffer is full.
        virtual bool Write(_In_ const IBuffer<T>& src, _In_ const bool& bCopyUpToSrcWritePos = true) noexcept(std::is_scalar_v<T>)
        {
            return Write(src.Ptr( ), bCopyUpToSrcWritePos ? src.WritePosition( ) : src.Length( ));
        }

        // Moves source buffer content to internal buffer at m_WritePos.
        // Note: If bCopyUpToSrcWritePos == true, will write up to src.WritePosition( ), otherwise up to src.Length( ).
        // Note: Will grow the internal buffer to hold the new element if the buffer is full.
        virtual bool Write(_In_ IBuffer<T>&& src, _In_ const bool& bCopyUpToSrcWritePos = true) noexcept
        {
            return Write(std::move(src.Ptr( )), bCopyUpToSrcWritePos ? src.WritePosition( ) : src.Length( ));
        }
    };
}