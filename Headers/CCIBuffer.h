#pragma once

// CC
#include "CCMacros.h"

// SAL Macros
#include <sal.h>


namespace CC
{
    template <typename T>
    class [[nodiscard]] IBuffer
    {
    protected:
        constexpr IBuffer() noexcept = default;
        IBuffer(const IBuffer<T>&) noexcept = default;
        IBuffer(IBuffer<T>&&) noexcept = default;
        virtual ~IBuffer() noexcept = default;
        IBuffer<T>& operator=(const IBuffer<T>&) noexcept = default;
        IBuffer<T>& operator=(IBuffer<T>&&) noexcept = default;

    public:
        /// Operator Overloads \\\

        // Return true for non-null buffers - false otherwise.
        [[nodiscard]] virtual explicit operator bool() const noexcept = 0;

        // Return pointer to internal buffer (mutable).
        [[nodiscard]] virtual explicit operator T* () noexcept = 0;

        // Return pointer to internal buffer (immutable).
        [[nodiscard]] virtual explicit operator const T* () const noexcept = 0;

        // Access internal buffer elements via index, returns a reference to the indexed element (mutable).
        // Note: This will throw std::logic_error if the internal buffer is nullptr.
        // Note: This will throw std::out_of_range if the specified index is >= the internal buffer's length.
        [[nodiscard]] virtual T& operator[](_In_ const size_t&) = 0;

        // Access internal buffer elements via index, returns a reference to the indexed element (immutable).
        // Note: This will throw std::logic_error if the internal buffer is nullptr.
        // Note: This will throw std::out_of_range if the specified index is >= the internal buffer's length.
        [[nodiscard]] virtual const T& operator[](_In_ const size_t&) const = 0;

        // Compares this buffer to RHS buffer, returning true if buffer contents match, false otherwise.
        [[nodiscard]] virtual bool operator==(_In_ const IBuffer<T>&) const noexcept = 0;

        // Compares this buffer to RHS buffer, returning true if buffer contents do not match, false otherwise.
        [[nodiscard]] virtual bool operator!=(_In_ const IBuffer<T>&) const noexcept = 0;

        /// Getters \\\

        // Return a pointer to the internal buffer (mutable).
        [[nodiscard]] virtual T* Get() noexcept = 0;

        // Return a pointer to the internal buffer (immutable).
        [[nodiscard]] virtual const T* Get() const noexcept = 0;

        // Return length of internal buffer.
        [[nodiscard]] virtual const size_t& Length() const noexcept = 0;

        // Return size in bytes of internal buffer.
        [[nodiscard]] virtual const size_t Size() const noexcept = 0;

        // Return where a subsequent write operation would take place on the internal buffer.
        [[nodiscard]] virtual const size_t& WritePosition() const noexcept = 0;

        /// Setters \\\

        // Explicitly set the write position to a specified value.
        // Note: Returns false if desired write position exceeds buffer length.  Returns true otherwise.
        [[nodiscard]] _Success_(return) virtual bool SetWritePosition(_In_ const size_t&) noexcept = 0;

        // Explicitly resets the write position to the beginning of the buffer (0).
        virtual void ResetWritePosition() noexcept = 0;

        /// Public Methods \\\

        // Resets internal data members to default values.
        // Note: This will not free any internal resources - use with caution.
        virtual void Reset() noexcept = 0;

        // Causes the object to free the memory of the internal buffer.
        // Note: This does nothing if the buffer does not currently own any resources.
        virtual void Free() noexcept = 0;

        // Compares contents of internal buffer to p (up to len).  Returns true if contents match, false otherwise.
        [[nodiscard]] _Success_(return) virtual bool Compare(_In_reads_opt_(len) const T* p, _In_ const size_t& len) const noexcept = 0;

        // Compares contents of this buffer to the specified buffer.  Returns true if contents match, false otherwise.
        [[nodiscard]] _Success_(return) virtual bool Compare(_In_ const IBuffer<T>& buffer) const noexcept = 0;

        // Copies a single object T to the current write position of the internal buffer - increments the write position.
        // Returns false if the write fails or otherwise could not be completed - returns true otherwise.
        // Note: Modifies buffer's write position if the write is successful.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_ const T& t) noexcept(CC_IS_NOTHROW_COPY(T)) = 0;

        // Moves a single object T to the current write position of the internal buffer - increments the write position.
        // Returns false if the write fails or otherwise could not be completed - returns true otherwise.
        // Note: Modifies buffer's write position if the write is successful.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_ T&& t) noexcept(CC_IS_NOTHROW_MOVE(T)) = 0;

        // Null-pointer write.
        // Returns false.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_opt_ const std::nullptr_t& pNull, _In_ const size_t& len) noexcept = 0;

        // Raw pointer copy write.
        // Writes len elements from p to the internal buffer - increments the write position by len if successful.
        // Returns false if the write fails or otherwise could not be completed - returns true otherwise.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_reads_opt_(len) const T* p, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_COPY(T)) = 0;

        // Raw pointer move write.
        // Writes len elements from p to the internal buffer - increments the write position by len if successful.
        // Returns false if the write fails or otherwise could not be completed - returns true otherwise.
        [[nodiscard]] _Success_(return) virtual bool Write(_Inout_updates_opt_(len) T*&& p, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_MOVE(T)) = 0;

        // Buffer copy write.
        // Writes src.WritePosition() elements from src's internal buffer if bCopyUpToSrcWritePos is true.
        // Returns false if the write fails or otherwise could not be completed - returns true otherwise.
        // Note: Modifies buffer's write position if the write is successful.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_ const IBuffer<T>& src, _In_ const bool& bCopyUpToSrcWritePos) noexcept(CC_IS_NOTHROW_COPY(T)) = 0;

        // Buffer copy move.
        // Writes src.WritePosition() elements from src's internal buffer if bCopyUpToSrcWritePos is true.
        // Returns false if the write fails or otherwise could not be completed - returns true otherwise.
        // Note: Modifies buffer's write position if the write is successful.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_ IBuffer<T>&& src, _In_ const bool& bCopyUpToSrcWritePos) noexcept(CC_IS_NOTHROW_MOVE(T)) = 0;
    };
}
