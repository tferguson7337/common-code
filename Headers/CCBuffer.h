#pragma once

// CC
#include "CCIBuffer.h"
#include "CCPointer.h"

// STL
#include <stdexcept>


namespace CC
{
    template <typename T>
    class [[nodiscard]] Buffer : public IBuffer<T>, protected Pointer<T>
    {
        // Testing class.
        friend class BufferTests;

        // Type aliases.
        using PH = PointerHelper<T>;
        using Base = Pointer<T>;
        using IBase = IBuffer<T>;

    protected:

        /// Protected Data Members \\\

        size_t m_WritePos;

        /// Protected Validator Methods \\\ 

        // Returns false if pos >= this->m_Len - returns true otherwise.
        [[nodiscard]] _Success_(return) inline bool ValidateNewWritePosition(_In_ const size_t& pos) const noexcept
        {
            return pos <= Length();
        }

        // Returns false if m_WritePos >= this->m_Len.
        // Returns true otherwise.
        [[nodiscard]] _Success_(return) inline bool ValidateSingleElementWriteRequest() const noexcept
        {
            return m_WritePos < Length();
        }

        // Returns false if !p || !this->m_pPtr || writeLen == 0 || ((m_WritePos + writeLen) <= this->m_Len)
        // Returns true otherwise.
        [[nodiscard]] _Success_(return) inline bool ValidateWriteRequest(_In_opt_count_(writeLen) const T* const p, _In_ const size_t& writeLen) const noexcept
        {
            return !!p && !!this->m_pPtr && writeLen > 0 && ((m_WritePos + writeLen) <= Length());
        }

        /// Protected Throwing Validator Methods \\\

        // Returns false if idx >= this->m_Len - returns true otherwise.
        inline void ValidateAccessorIndexT(_In_z_ const char* const f, _In_ const size_t& idx) const
        {
            PH::ValidateDereferenceT(f, this->m_pPtr);
            if (idx >= Length())
            {
                static const std::string msg1 = ": Index[";
                static const std::string msg2 = "] exceeds length of buffer[";
                static const std::string msg3 = "].";
                const std::string idxStr = std::to_string(idx);
                const std::string endStr = std::to_string(Length());

                throw std::out_of_range(f + msg1 + idxStr + msg2 + endStr + msg3);
            }
        }

        /// Static Protected Helper Methods \\\

        // Copies all Buffer data members from source IBuffer object.
        // Note: The pointer data member is shallow copied.
        static void CopyAllBufferDataMembers(_Out_ Buffer<T>& dst, _In_ IBase& src) noexcept
        {
            dst.m_pPtr = src.Get();
            CopyNonPointerBufferDataMembers(dst, src);
        }

        // Copies all Buffer data members from source IBuffer object, except for the internal buffer pointer.
        static void CopyNonPointerBufferDataMembers(_Out_ Buffer<T>& dst, _In_ const IBase& src) noexcept
        {
            dst.m_Len = src.Length();
            dst.m_WritePos = src.WritePosition();
        }

        // Performs deep copy of src buffer, frees dst buffer, then assigned new buffer to dst.
        // Also copies all other Buffer data members of src to dst.
        // Note: If src buffer is nullptr, then dst will free buffer and replace it with nullptr.
        static void CopyBuffer(_Inout_ Buffer<T>& dst, _In_ const IBase& src) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            if (dst.Length() < src.Length())
            {
                dst.InvokeFreeFunction();
                dst.m_pPtr = PH::AllocateFromRawPointer(src.Get(), src.Length());
                CopyNonPointerBufferDataMembers(dst, src);
            }
            else
            {
                PH::CopyToRawPointer(dst.m_pPtr, src.Get(), src.Length());
                dst.m_WritePos = src.Length();
            }
        }

        // Performs shallow copy of src Buffer data members to dst, then resets src.
        static void TransferBuffer(_Inout_ Buffer<T>& dst, _Inout_ IBase& src) noexcept
        {
            dst.InvokeFreeFunction();
            CopyAllBufferDataMembers(dst, src);
            src.Reset();
        }

        /// Protected Helper Methods \\\

        // Writes elements via copy to internal buffer, updates write position.
        [[nodiscard]] _Success_(return) bool WriteInternal(_In_reads_(len) const T* p, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            PH::CopyToRawPointer(this->m_pPtr + m_WritePos, p, len);
            m_WritePos += len;
            return true;
        }

        // Writes elements via move semantics to internal buffer, updates write position.
        [[nodiscard]] _Success_(return) bool WriteInternal(_Inout_updates_all_(len) T* p, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            PH::MoveToRawPointer(this->m_pPtr + m_WritePos, p, len);
            m_WritePos += len;
            return true;
        }

    public:

        /// Public Constructors \\\

        // Default constructor
        constexpr Buffer() noexcept :
            Base(),
            m_WritePos(0)
        { }

        // Buffer length constructor
        explicit Buffer(_In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T)) :
            Base(len),
            m_WritePos(0)
        { }

        // Raw pointer copy constructor
        Buffer(_In_reads_opt_(len) const T* p, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T)) :
            Base(p, len),
            m_WritePos(0)
        { }

        // Raw pointer steal constructor
        Buffer(_Inout_updates_all_opt_(len) T*& p, _In_ const size_t& len) noexcept :
            Base(p, len),
            m_WritePos(0)
        { }

        // Copy constructor
        Buffer(_In_ const Buffer<T>& src) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T)) :
            Base(src),
            m_WritePos(src.WritePosition())
        { }

        // Move constructor
        Buffer(_Inout_ Buffer<T>&& src) noexcept :
            Base(std::move(src)),
            m_WritePos(src.WritePosition())
        {
            src.Reset();
        }

        // Interface copy constructor
        Buffer(_In_ const IBase& src) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T)) :
            Base(src.Get(), src.Length()),
            m_WritePos(src.WritePosition())
        { }

        // Interface move constructor
        Buffer(_Inout_ IBase&& src) noexcept :
            Base(src.Get(), src.Length()),
            m_WritePos(src.WritePosition())
        {
            src.Reset();
        }

        /// Public Destructor \\\

        // Destructor
        virtual ~Buffer() noexcept = default;

        /// Operator Overloads \\\

        // Copy assignment
        Buffer<T>& operator=(_In_ const Buffer<T>& src) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T))
        {
            if (this != &src)
            {
                CopyBuffer(*this, src);
            }

            return *this;
        }

        // Move assignment
        Buffer<T>& operator=(_Inout_ Buffer<T>&& src) noexcept
        {
            if (this != &src)
            {
                TransferBuffer(*this, src);
            }

            return *this;
        }

        // Interface copy assignment
        IBase& operator=(_In_ const IBase& src) noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T) && CC_IS_NOTHROW_COPY(T))
        {
            if (this != &src)
            {
                CopyBuffer(*this, src);
            }

            return *this;
        }

        // Interface move assignment
        IBase& operator=(_Inout_ IBase&& src) noexcept
        {
            if (this != &src)
            {
                TransferBuffer(*this, src);
            }

            return *this;
        }

        // Return true for non-null buffer - false otherwise.
        [[nodiscard]] virtual explicit operator bool() const noexcept
        {
            return !!this->m_pPtr;
        }

        // Return pointer to internal buffer (mutable).
        [[nodiscard]] virtual explicit operator T* () noexcept
        {
            return this->m_pPtr;
        }

        // Return pointer to internal buffer (immutable).
        [[nodiscard]] virtual explicit operator const T* () const noexcept
        {
            return this->m_pPtr;
        }

        // Subscript overload - returns reference to mutable element from internal buffer via index.
        // Note: Will throw std::logic_error if this->m_pPtr == nullptr.
        // Note: Will throw std::out_of_range if i >=this->m_Len.
        [[nodiscard]] virtual T& operator[](_In_ const size_t& i)
        {
            ValidateAccessorIndexT(__FUNCSIG__, i);
            return this->m_pPtr[i];
        }

        // Subscript overload - returns reference to immutable element from internal buffer via index.
        // Note: Will throw std::logic_error if this->m_pPtr == nullptr.
        // Note: Will throw std::out_of_range if i >=this->m_Len.
        [[nodiscard]] virtual const T& operator[](_In_ const size_t& i) const
        {
            ValidateAccessorIndexT(__FUNCSIG__, i);
            return this->m_pPtr[i];
        }

        // Comparison Equal-To overload - returns true if contents of specified buffer match this->m_pPtr, false otherwise.
        [[nodiscard]] virtual bool operator==(_In_ const IBase& rhs) const noexcept
        {
            return Compare(rhs.Get(), rhs.Length());
        }

        // Comparison Not-Equal-To overload - returns true if contents of specified buffer do not match this->m_pPtr, false otherwise.
        [[nodiscard]] virtual bool operator!=(_In_ const IBase& rhs) const noexcept
        {
            return !Compare(rhs.Get(), rhs.Length());
        }

        /// Getters \\\

        // Returns pointer to internal buffer (mutable).
        [[nodiscard]] inline virtual T* Get() noexcept
        {
            return Base::Get();
        }

        // Returns pointer to internal buffer (immutable).
        [[nodiscard]] inline virtual const T* Get() const noexcept
        {
            return Base::Get();
        }

        // Returns length of internal buffer.
        [[nodiscard]] inline virtual const size_t& Length() const noexcept
        {
            return Base::Length();
        }

        // Returns size in bytes of internal buffer.
        [[nodiscard]] inline virtual const size_t Size() const noexcept
        {
            return Base::Size();
        }

        // Returns current write position.
        [[nodiscard]] inline virtual const size_t& WritePosition() const noexcept
        {
            return m_WritePos;
        }

        /// Setters \\\

        // Sets write position to specified value.
        // Note: Will throw std::out_of_range if pos >= this->m_Len
        [[nodiscard]] _Success_(return) inline virtual bool SetWritePosition(_In_ const size_t& pos) noexcept
        {
            if (!ValidateNewWritePosition(pos))
            {
                return false;
            }

            m_WritePos = pos;
            return true;
        }

        // Resets write position to beginning of buffer (0).
        inline virtual void ResetWritePosition() noexcept
        {
            m_WritePos = 0;
        }

        /// Public Methods \\\

        // Resets internal data members to default values.
        // Note: This will not free any internal resources - use with caution.
        inline virtual void Reset() noexcept
        {
            Base::Reset();
            m_WritePos = 0;
        }

        // Causes the buffer to free any resources, resetting data members to their default values.
        inline virtual void Free() noexcept
        {
            Base::InvokeFreeFunction();
        }

        // Returns true if contents of this->m_pPtr match contents of p (up to len), false otherwise.
        [[nodiscard]] _Success_(return) virtual bool Compare(_In_reads_opt_(len) const T* p, _In_ const size_t& len) const noexcept
        {
            bool bRet = true;

            // Trivial case(t): both pointers point to same addr.
            if (this->m_pPtr == p)
            {
                return true;
            }

            // Trivial cases(f): Either pointer is null or requested comparison-length exceeds length of buffer.
            if (!this->m_pPtr || !p || len > Length())
            {
                return false;
            }

            // Use memcmp if possible (scalar values only).
            if constexpr (std::is_scalar_v<T>)
            {
                bRet = (memcmp(this->m_pPtr, p, len * sizeof(T)) == 0);
            }
            else
            {
                for (size_t i = 0; i < len; i++)
                {
                    if (this->m_pPtr[i] != p[i])
                    {
                        return false;
                    }
                }
            }

            return bRet;
        }

        // Returns true if contents of this->m_pPtr match contents of buffer.Get( ), false otherwise.
        [[nodiscard]] _Success_(return) inline virtual bool Compare(_In_ const IBase& buffer) const noexcept
        {
            return Compare(buffer.Get(), buffer.Length());
        }

        // Writes specified T object to internal buffer at m_WritePos.
        // Returns false if write would exceed length of buffer, true otherwise.
        [[nodiscard]] _Success_(return) inline virtual bool Write(_In_ const T& t) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            if (!ValidateSingleElementWriteRequest())
            {
                return false;
            }

            this->m_pPtr[m_WritePos++] = t;
            return true;
        }

        // Writes specified T object to internal buffer at m_WritePos.
        // Returns false if write would exceed length of buffer, true otherwise.
        [[nodiscard]] _Success_(return) inline virtual bool Write(_Inout_ T&& t) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            if (!ValidateSingleElementWriteRequest())
            {
                return false;
            }

            this->m_pPtr[m_WritePos++] = std::move(t);
            return true;
        }

        // Null-pointer write.
        // Returns false.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_ const std::nullptr_t&, _In_ const size_t&) noexcept
        {
            return false;
        }

        // Copies raw pointer content to internal buffer at m_WritePos, increments m_WritePos by len on successful write.
        [[nodiscard]] _Success_(return) virtual bool Write(_In_reads_opt_(len) const T* src, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            return ValidateWriteRequest(src, len) && WriteInternal(src, len);
        }

        // Moves raw pointer content to internal buffer at m_WritePos, increments m_WritePos by len on successful write.
        [[nodiscard]] _Success_(return) virtual bool Write(_Inout_updates_opt_(len) T*&& src, _In_ const size_t& len) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            return ValidateWriteRequest(src, len) && WriteInternal(src, len);
        }

        // Copies source buffer content to internal buffer at m_WritePos.
        // Note: If bCopyUpToSrcWritePos == true, will write up to src.WritePosition( ), otherwise up to src.Length( ).
        [[nodiscard]] _Success_(return) virtual bool Write(_In_ const IBase& src, _In_ const bool& bCopyUpToSrcWritePos = true) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            return Write(src.Get(), bCopyUpToSrcWritePos ? src.WritePosition() : src.Length());
        }

        // Moves source buffer content to internal buffer at m_WritePos.
        // Note: If bCopyUpToSrcWritePos == true, will write up to src.WritePosition( ), otherwise up to src.Length( ).
        [[nodiscard]] _Success_(return) virtual bool Write(_In_ IBase&& src, _In_ const bool& bCopyUpToSrcWritePos = true) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            return Write(std::move(src.Get()), bCopyUpToSrcWritePos ? src.WritePosition() : src.Length());
        }
    };
}
