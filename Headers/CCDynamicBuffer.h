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

        // Moves len elements from src into dst.
        static void MoveToRawBuffer(_Out_writes_opt_(len) T* dst, _Inout_opt_count_(len) T* src, _In_ const size_t& len)
        {
            if ( dst && src )
            {
                for ( size_t i = 0; i < len; i++ )
                {
                    dst[i] = std::move(src[i]);
                }
            }
        }

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
        void GrowBufferIfRequired(_In_ const size_t& minInc)
        {
            if ( (this->m_WritePos + minInc) > this->m_Len )
            {
                DynamicBuffer<T> newBuf(CalculateNewLength(this->m_Len, minInc));
                MoveToRawBuffer(newBuf.m_pBuf, this->m_pBuf, this->m_WritePos);
                newBuf.m_WritePos = this->m_WritePos;
                Buffer<T>::TransferBuffer(*this, newBuf);
            }
        }

        // Common wrapper for copy assignment.
        DynamicBuffer<T>& CopyAssignmentCommon(_In_ const IBuffer<T>& src)
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
        explicit DynamicBuffer(_In_ const size_t& len) :
            Buffer<T>(len)
        { }

        // Raw pointer copy constructor
        DynamicBuffer(_In_reads_opt_(len) const T* p, _In_ const size_t& len) :
            Buffer<T>(p, len)
        { }

        // Raw pointer steal constructor
        DynamicBuffer(_Inout_opt_ T*& p, _In_ const size_t& len) :
            Buffer<T>(p, len)
        { }

        // Copy constructor
        DynamicBuffer(_In_ const DynamicBuffer<T>& src) :
            Buffer<T>(src)
        { }

        // Move constructor
        DynamicBuffer(_Inout_ DynamicBuffer<T>&& src) noexcept :
            Buffer<T>(std::move(src))
        { }

        // Interface copy constructor
        DynamicBuffer(_In_ const IBuffer<T>& src) :
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
        DynamicBuffer<T>& operator=(_In_ const DynamicBuffer<T>& src)
        {
            return CopyAssignmentCommon(src);
        }

        // Move Assignment
        DynamicBuffer<T>& operator=(_Inout_ DynamicBuffer<T>&& src) noexcept
        { 
            return MoveAssignmentCommon(std::move(src));
        }

        // Interface Copy Assignment
        DynamicBuffer<T>& operator=(_In_ const IBuffer<T>& src)
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
        virtual void Write(_In_ const T& t)
        {
            GrowBufferIfRequired(1);
            this->m_pBuf[this->m_WritePos++] = t;
        }

        // Writes specified T object to internal buffer at m_WritePos.
        // Note: Will grow the internal buffer to hold the new element if the buffer is full.
        virtual void Write(_Inout_ T&& t)
        {
            GrowBufferIfRequired(1);
            this->m_pBuf[this->m_WritePos++] = std::move(t);
        }

        // Writes raw pointer content to internal buffer at m_WritePos.
        // Note: Will grow the internal buffer to hold the new elements if buffer doesn't have enough room.
        virtual void Write(_In_reads_opt_(len) const T* const src, _In_ const size_t& len)
        {
            if ( len > 0 )
            {
                Buffer<T>::ValidateDereference(__FUNCSIG__, src);
                GrowBufferIfRequired(len);
                Buffer<T>::CopyToRawBuffer(this->m_pBuf + this->m_WritePos, src, len);
                this->m_WritePos += len;
            }
        }

        // Writes buffer content to internal buffer at m_WritePos.
        // Note: Will grow the internal buffer to hold the new elements if buffer doesn't have enough room.
        virtual void Write(_In_ const IBuffer<T>& src, _In_ const bool& bCopyUpToSrcWritePos = true)
        {
            Write(src.Ptr( ), bCopyUpToSrcWritePos ? src.WritePosition( ) : src.Length( ));
        }
    };
}