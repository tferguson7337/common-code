#pragma once

// CC
#include "CCBuffer.h"


namespace CC
{
    template <typename T>
    class [[nodiscard]] DynamicBuffer : public Buffer<T>
    {
        // Testing class.
        friend class DynamicBufferTests;

        // Type aliases.
        using PH = PointerHelper<T>;
        using Base = Buffer<T>;
        using IBase = IBuffer<T>;

    protected:

        /// Protected Validator Methods \\\

        // Returns false if !p || writeLen == 0
        // Returns true otherwise.
        [[nodiscard]] _Success_(return) inline static bool ValidateWriteRequest(_In_opt_ const T* const p, _In_ const size_t& writeLen) noexcept
        {
            return !!p && writeLen > 0;
        }

        /// Protected Helper Methods \\\

        // Calculates new length for growing buffer.
        // If geometric increase is sufficent, then new length will be (m_Len + (m_Len >> 1)) (i.e., +50%)
        // Otherwise, the new length will be std::max(16, m_Len + minInc).
        [[nodiscard]] static size_t CalculateNewLength(_In_ const size_t& oldLen, _In_ const size_t& minInc) noexcept
        {
            static constexpr size_t minNewLen = 16;
            return std::max(minNewLen, std::max(oldLen + (oldLen >> 1), oldLen + minInc));
        }

        // Grows buffer by creating a new buffer of greater size and moving original buffer contents over to new buffer.
        [[nodiscard]] _Success_(return) bool GrowBuffer(_In_ const size_t& minInc) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_MOVE(T))
        {
            DynamicBuffer<T> newBuf(CalculateNewLength(this->Length(), minInc));
            if (!newBuf)
            {
                return false;
            }

            PH::MoveToRawPointer(newBuf.Get(), this->Get(), this->WritePosition());
            newBuf.m_WritePos = this->WritePosition();
            Base::TransferBuffer(*this, newBuf);
            return true;
        }

        [[nodiscard]] _Success_(return) bool WriteInternalGrowReq(_In_reads_(len) const T* src, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T) && CC_IS_NOTHROW_MOVE(T))
        {
            bool bRet = false;

            DynamicBuffer<T> newBuf(CalculateNewLength(this->Length(), len));
            if (!!newBuf)
            {
                if (newBuf.WriteInternal(this->Get(), this->WritePosition()) && newBuf.WriteInternal(src, len))
                {
                    *this = std::move(newBuf);
                    bRet = true;
                }
            }

            return bRet;
        }

        [[nodiscard]] _Success_(return) bool WriteInternalGrowReq(_Inout_updates_(len) T* src, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T) && CC_IS_NOTHROW_MOVE(T))
        {
            bool bRet = false;

            DynamicBuffer<T> newBuf(CalculateNewLength(this->Length(), len));
            if (!!newBuf)
            {
                if (newBuf.WriteInternal(this->Get(), this->WritePosition()) && newBuf.WriteInternal(src, len))
                {
                    *this = std::move(newBuf);
                    bRet = true;
                }
            }

            return bRet;
        }

        [[nodiscard]] _Success_(return) bool WriteInternalGrowIfReq(_In_reads_(len) const T* src, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T) && CC_IS_NOTHROW_MOVE(T))
        {
            return ((this->WritePosition() + len) > this->Length()) ? WriteInternalGrowReq(src, len) : Base::WriteInternal(src, len);
        }

        [[nodiscard]] _Success_(return) bool WriteInternalGrowIfReq(_Inout_updates_(len) T* src, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T) && CC_IS_NOTHROW_MOVE(T))
        {
            return ((this->WritePosition() + len) > this->Length()) ? WriteInternalGrowReq(src, len) : Base::WriteInternal(src, len);
        }

        // Common wrapper for copy assignment.
        DynamicBuffer<T>& CopyAssignmentCommon(_In_ const IBase& src) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            if (this != &src)
            {
                Base::operator=(src);
            }

            return *this;
        }

        // Common wrapper for move assignment.
        DynamicBuffer<T>& MoveAssignmentCommon(_Inout_ IBase&& src) noexcept
        {
            if (this != &src)
            {
                Base::operator=(std::move(src));
            }

            return *this;
        }

    public:

        /// Public Constructors \\\

        // Default constructor
        constexpr DynamicBuffer() noexcept :
            Base()
        { }

        // Buffer length constructor
        explicit DynamicBuffer(_In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T)) :
            Base(len)
        { }

        // Raw pointer copy constructor
        DynamicBuffer(_In_reads_opt_(len) const T* p, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T)) :
            Base(p, len)
        { }

        // Raw pointer steal constructor
        DynamicBuffer(_Inout_opt_ T*& p, _In_ const size_t& len) noexcept :
            Base(p, len)
        { }

        // Copy constructor
        DynamicBuffer(_In_ const DynamicBuffer<T>& src) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T)) :
            Base(src)
        { }

        // Move constructor
        DynamicBuffer(_Inout_ DynamicBuffer<T>&& src) noexcept :
            Base(std::move(src))
        { }

        // Interface copy constructor
        DynamicBuffer(_In_ const IBase& src) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T)) :
            Base(src)
        { }

        // Interface move constructor
        DynamicBuffer(_Inout_ IBase&& src) noexcept :
            Base(std::move(src))
        { }

        /// Public Destructor \\\

        // Destructor
        virtual ~DynamicBuffer() noexcept = default;

        /// Operator Overloads \\\

        // Copy Assignment
        DynamicBuffer<T>& operator=(_In_ const DynamicBuffer<T>& src) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T))
        {
            return CopyAssignmentCommon(src);
        }

        // Move Assignment
        DynamicBuffer<T>& operator=(_Inout_ DynamicBuffer<T>&& src) noexcept
        {
            return MoveAssignmentCommon(std::move(src));
        }

        // Interface Copy Assignment
        IBase& operator=(_In_ const IBase& src) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T))
        {
            return CopyAssignmentCommon(src);
        }

        // Interface Move Assignment
        IBase& operator=(_Inout_ IBase&& src) noexcept
        {
            return MoveAssignmentCommon(std::move(src));
        }

        /// Public Methods \\\

        // Writes specified T object to internal buffer at m_WritePos.
        // Note: Will allocate/grow the internal buffer to hold the new element if the buffer is null/full.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_ const T& t) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T) && CC_IS_NOTHROW_MOVE(T))
        {
            if ((this->m_WritePos >= this->m_Len) && !GrowBuffer(1))
            {
                return false;
            }

            this->m_pPtr[this->m_WritePos++] = t;
            return true;
        }

        // Writes specified T object to internal buffer at m_WritePos.
        // Note: Will allocate/grow the internal buffer to hold the new element if the buffer is null/full.
        [[nodiscard]] _Success_(return) virtual bool Write(_Inout_ T&& t) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T) && CC_IS_NOTHROW_MOVE(T))
        {
            if ((this->m_WritePos >= this->m_Len) && !GrowBuffer(1))
            {
                return false;
            }

            this->m_pPtr[this->m_WritePos++] = std::move(t);
            return true;
        }

        // Null-pointer write.
        // Returns false.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_opt_ const std::nullptr_t&, _In_ const size_t&) noexcept
        {
            return false;
        }

        // Copies raw pointer content to internal buffer at m_WritePos, increments m_WritePos by len on successful write.
        // Note: Will allocate/grow the internal buffer to hold the new element(s) if the buffer is null/full.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_reads_opt_(len) const T* src, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T) && CC_IS_NOTHROW_MOVE(T))
        {
            return ValidateWriteRequest(src, len) && WriteInternalGrowIfReq(src, len);
        }

        // Moves raw pointer content to internal buffer at m_WritePos, increments m_WritePos by len on successful write.
        // Note: Will allocate/grow the internal buffer to hold the new element(s) if the buffer is null/full.
        [[nodiscard]] _Success_(return) virtual bool Write(_Inout_updates_opt_(len) T*&& src, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T) && CC_IS_NOTHROW_MOVE(T))
        {
            return ValidateWriteRequest(src, len) && WriteInternalGrowIfReq(src, len);
        }

        // Copies source buffer content to internal buffer at m_WritePos.
        // Note: If bCopyUpToSrcWritePos == true, will write up to src.WritePosition( ), otherwise up to src.Length( ).
        // Note: Will allocate/grow the internal buffer to hold the new element(s) if the buffer is null/full.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_ const IBase& src, _In_ const bool& bCopyUpToSrcWritePos = true) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T) && CC_IS_NOTHROW_MOVE(T))
        {
            return Write(src.Get(), bCopyUpToSrcWritePos ? src.WritePosition() : src.Length());
        }

        // Moves source buffer content to internal buffer at m_WritePos.
        // Note: If bCopyUpToSrcWritePos == true, will write up to src.WritePosition( ), otherwise up to src.Length( ).
        // Note: Will allocate/grow the internal buffer to hold the new element(s) if the buffer is null/full.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_ IBase&& src, _In_ const bool& bCopyUpToSrcWritePos = true) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T) && CC_IS_NOTHROW_MOVE(T))
        {
            return Write(std::move(src.Get()), bCopyUpToSrcWritePos ? src.WritePosition() : src.Length());
        }
    };
}
