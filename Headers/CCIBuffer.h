#pragma once

#include <sal.h>

namespace CC
{
    template <typename T>
    class IBuffer
    {
    protected:
        IBuffer( ) noexcept = default;
        IBuffer(const IBuffer<T>&) = default;
        IBuffer(IBuffer<T>&&) noexcept = default;
        virtual ~IBuffer( ) noexcept = default;
        IBuffer<T>& operator=(const IBuffer<T>&) = default;
        IBuffer<T>& operator=(IBuffer<T>&&) noexcept = default;

    public:
        /// Operator Overloads \\\

        // Return true for non-null buffers - false otherwise.
        virtual explicit operator bool( ) const noexcept = 0;

        // Return pointer to internal buffer (mutable).
        virtual explicit operator T* () noexcept = 0;

        // Return pointer to internal buffer (immutable).
        virtual explicit operator const T* () const noexcept = 0;

        // Access internal buffer elements via index, returns a reference to the indexed element (mutable).
        // Note: This will throw std::logic_error if the internal buffer is nullptr.
        // Note: This will throw std::out_of_range if the specified index is >= the internal buffer's length.
        virtual T& operator[](_In_ const size_t&) = 0;

        // Access internal buffer elements via index, returns a reference to the indexed element (immutable).
        // Note: This will throw std::logic_error if the internal buffer is nullptr.
        // Note: This will throw std::out_of_range if the specified index is >= the internal buffer's length.
        virtual const T& operator[](_In_ const size_t&) const = 0;

        // Dereference internal buffer pointer, returning a reference to the first element (mutable).
        // Note: This will throw std::logic_error if the internal buffer is nullptr.
        virtual T& operator*( ) = 0;

        // Dereference internal buffer pointer, returning a reference to the first element (immutable).
        // Note: This will throw std::logic_error if the internal buffer is nullptr.
        virtual const T& operator*( ) const = 0;

        // Compares this buffer to RHS buffer, returning true if buffer contents match, false otherwise.
        virtual const bool operator==(_In_ const IBuffer<T>&) const = 0;

        // Compares this buffer to RHS buffer, returning true if buffer contents do not match, false otherwise.
        virtual const bool operator!=(_In_ const IBuffer<T>&) const = 0;

        /// Getters \\\

        // Return a pointer to the internal buffer (mutable).
        virtual T* Ptr( ) noexcept = 0;

        // Return a pointer to the internal buffer (immutable).
        virtual const T* Ptr( ) const noexcept = 0;

        // Return length of internal buffer.
        virtual const size_t& Length( ) const noexcept = 0;

        // Return size in bytes of internal buffer.
        virtual const size_t Size( ) const noexcept = 0;

        // Return where a subsequent write operation would take place on the internal buffer.
        virtual const size_t& WritePosition( ) const noexcept = 0;

        /// Setters \\\

        // Explicitly set the write position to a specified value.
        // Note: This will throw std::out_of_range if the specified write position is beyond the range of the buffer.     
        virtual void SetWritePosition(_In_ const size_t&) = 0;

        // Explicitly resets the write position to the beginning of the buffer (0).
        virtual void ResetWritePosition( ) noexcept = 0;

        /// Public Methods \\\

        // Resets internal data members to default values.
        // Note: This will not free any internal resources - use with caution.
        virtual void Reset( ) noexcept = 0;

        // Causes the object to free the memory of the internal buffer.
        // Note: This does nothing if the buffer does not currently own any resources.
        virtual void Free( ) noexcept = 0;

        // Compares contents of internal buffer to p (up to len).  Returns true if contents match, false otherwise.
        // Note: This will throw std::invalid_argument if p == nullptr && len != 0 or if p != nullptr && len == 0.
        // Note: This will throw std::out_of_range if len is larger than the internal buffer's length.
        virtual const bool Compare(_In_opt_ const T* p, _In_ const size_t& len) const = 0;

        // Compares contents of this buffer to the specified buffer.  Returns true if contents match, false otherwise.
        virtual const bool Compare(_In_ const IBuffer<T>& buffer) const = 0;

        // Writes a single object T to the current write position of the internal buffer - increments the write position.
        // Note: This will throw std::out_of_range if the write would go beyond the end of the buffer.
        virtual void Write(_In_ const T& t) = 0;

        // Writes a single object T to the current write position of the internal buffer - increments the write position.
        // Note: This will throw std::out_of_range if the write would go beyond the end of the buffer.
        virtual void Write(_In_ T&& t) = 0;

        // Writes len elements from p to the internal buffer - increments the write position by len.
        // Note: This will throw std::logic_error internal buffer or p are nullptr.
        // Note: This will throw std::out_of_range if the write would go beyond the end of the buffer.
        virtual void Write(_In_ const T* const p, _In_ const size_t& len) = 0;

        // Writes src.WritePosition( ) elements from src's internal buffer if bCopyUpToSrcWritePos is true.
        // Writes src.Length( ) elements from src's internal buffer if bCopyUpToSrcWritePos is false.
        // Increments the write position by appropriate amount.
        // Note: This will throw std::out_of_range if the write would go beyond the end of the buffer.
        virtual void Write(_In_ const IBuffer<T>& src, _In_ const bool& bCopyUpToSrcWritePos) = 0;
    };
}
