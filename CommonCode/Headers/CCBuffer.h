#ifndef _CC_BUFFER_H_
#define _CC_BUFFER_H_

#include "CCTypes.h"

#include <sal.h>

#include <stdexcept>
#include <sstream>
#include <utility>

#include <CCIBuffer.h>

namespace CC
{
    template <typename T>
    class Buffer : public IBuffer<T>
    {
        // Testing class.
        friend class BufferTests;

    protected:

        /// Type Aliases \\\

        using FreeFunc = void(*)(T*);

        /// Protected Member Functions \\\

        T* m_pBuf;
        size_t m_Len;
        size_t m_WritePos;
        FreeFunc m_FreeFunc;

        /// Static Protected Validator Methods \\\

        // Throws std::invalid_argument if p is null and len is not zero, or if p is not null and len is zero.
        inline static void ValidateRawPointerArgs(_In_ const char* const f, _In_opt_ const T* p, _In_ const size_t& len)
        {
            if ( !p && len != 0 )
            {
                static const std::string msg = ": Attempted to use nullptr w/ non-zero length.";
                throw std::invalid_argument(f + msg);
            }
            else if ( p && len == 0 )
            {
                static const std::string msg = ": Attempted to use ptr w/ zero length.";
                throw std::invalid_argument(f + msg);
            }
        }

        // Throws std::invalid_argument if len is zero.
        inline static void ValidateAllocationLength(_In_ const char* const f, _In_ const size_t& len)
        {
            if ( len == 0 )
            {
                static const std::string msg = ": Cannot allocate zero-length buffer.";
                throw std::invalid_argument(f + msg);
            }
        }

        // Throw std::logic_error if p is nullptr.
        inline static void ValidateDereference(_In_ const char* const f, _In_opt_ const T* p)
        {
            if ( !p )
            {
                static const std::string msg = ": Attempted to dereference nullptr.";
                throw std::logic_error(f + msg);
            }
        }

        /// Protected Validator Methods \\\

        // Throws std::out_of_range if idx >= m_Len.
        inline void ValidateIndex(_In_ const char* const f, _In_ const size_t& idx) const
        {
            if ( idx >= m_Len )
            {
                static const std::string msg1 = ": Index[";
                static const std::string msg2 = "] exceeds length of buffer[";
                static const std::string msg3 = "].";
                const std::string idxStr = std::to_string(idx);
                const std::string endStr = std::to_string(m_Len);

                throw std::out_of_range(f + msg1 + idxStr + msg2 + endStr + msg3);
            }
        }

        // Throws std::out_of_range if pos >= m_Len.
        inline void ValidateWritePosition(_In_ const char* const f, _In_ const size_t& pos) const
        {
            if ( pos > m_Len )
            {
                static const std::string msg1 = ": Position[";
                static const std::string msg2 = "] exceeds length of buffer[";
                static const std::string msg3 = "].";
                const std::string idxStr = std::to_string(pos);
                const std::string endStr = std::to_string(m_Len);

                throw std::out_of_range(f + msg1 + idxStr + msg2 + endStr + msg3);
            }
        }

        // Throws std::logic_error if p is nullptr.
        // Throws std::logic_error if m_pBuf is nullptr.
        // Throws std::out_of_range if (m_WritePos + writeLen) >= m_Len.
        inline void ValidateWriteRequest(_In_ const char* const f, _In_opt_ const T* p, _In_ const size_t& writeLen) const
        {
            ValidateDereference(f, p);
            ValidateDereference(f, m_pBuf);
            if ( (m_WritePos + writeLen) > m_Len )
            {
                static const std::string msg1 = ": Write of length[";
                static const std::string msg2 = "] at buffer at position[";
                static const std::string msg3 = "] would exceed length of buffer[";
                static const std::string msg4 = "].";
                const std::string writeLenStr = std::to_string(writeLen);
                const std::string writePosStr = std::to_string(m_WritePos);
                const std::string bufLenStr = std::to_string(m_Len);

                throw std::out_of_range(f + msg1 + writeLenStr + msg2 + writePosStr + msg3 + bufLenStr + msg4);
            }
        }

        /// Static Protected Helper Methods \\\

        // Cleanup function - used for single-element buffers.
        static void FreeSingle(_In_opt_ T* p) noexcept
        {
            delete p;
        }

        // Cleanup function - used for multi-element buffers.
        static void FreeArray(_In_opt_ T* p) noexcept
        {
            delete[ ] p;
        }

        static FreeFunc GetFreeFunction(_In_ const size_t& len) noexcept
        {
            return len == 0 ? nullptr : len == 1 ? FreeSingle : FreeArray;
        }

        // Allocates pointer to len T elements.
        // Note: Throws std::bad_alloc if allocation fails.
        static T* Allocate(_In_ const size_t& len)
        {
            return (len == 0) ? nullptr : (len == 1) ? new T : new T[len];
        }

        // Allocates pointer to len T elements, copies contents of ptr to the new memory block.
        // Note: Throws std::invalid_argument if p is null and len is not zero, or if p is not null and len is zero.
        static T* AllocateFromPointer(_In_opt_ const T* ptr, _In_ const size_t& len)
        {
            ValidateRawPointerArgs(__FUNCSIG__, ptr, len);
            T* p = nullptr;
            if ( ptr )
            {
                p = Allocate(len);
                memcpy(p, ptr, sizeof(T) * len);
            }
            
            return p;
        }

        // Copies all Buffer data members from source IBuffer object.
        // Note: The pointer data member is shallow copied.
        static void CopyAllBufferDataMembers(_In_ Buffer<T>& dst, _In_ IBuffer<T>& src) noexcept
        {
            dst.m_pBuf = src.Ptr( );
            CopyNonPointerBufferDataMembers(dst, src);
        }

        // Copies all Buffer data members from source IBuffer object, except for the internal buffer pointer.
        static void CopyNonPointerBufferDataMembers(_In_ Buffer<T>& dst, _In_ const IBuffer<T>& src) noexcept
        {
            dst.m_Len = src.Length( );
            dst.m_WritePos = src.WritePosition( );
            dst.m_FreeFunc = GetFreeFunction(dst.m_Len);
        }

        // Performs deep copy of src buffer, frees dst buffer, then assigned new buffer to dst.
        // Also copies all other Buffer data members of src to dst.
        // Note: If src buffer is nullptr, then dst will free buffer and replace it with nullptr.
        static void CopyBuffer(_In_ Buffer<T>& dst, _In_ const IBuffer<T>& src)
        {
            T* p = AllocateFromPointer(src.Ptr( ), src.Length( ));
            dst.InvokeFreeFunction( );

            dst.m_pBuf = p;
            CopyNonPointerBufferDataMembers(dst, src);
        }

        // Performs copy of src Buffer data members to dst, then resets src.
        static void TransferBuffer(_In_ Buffer<T>& dst, _In_ IBuffer<T>& src) noexcept
        {
            dst.InvokeFreeFunction( );
            CopyAllBufferDataMembers(dst, src);
            src.Claim( );
        }

        /// Protected Helper Methods \\\

        // Calls the appropriate cleanup function, if one has been assigned.
        virtual inline void InvokeFreeFunction( ) noexcept
        {
            if ( m_FreeFunc )
            {
                m_FreeFunc(m_pBuf);
            }
        }

        // Resets all data members to default values.
        virtual inline void Reset( ) noexcept
        {
            m_pBuf = nullptr;
            m_Len = 0;
            m_WritePos = 0;
            m_FreeFunc = nullptr;
        }

    public:

        /// Protected Constructors \\\

        // Default constructor
        constexpr Buffer( ) noexcept :
            m_pBuf(nullptr),
            m_Len(0),
            m_WritePos(0),
            m_FreeFunc(nullptr)
        { }

        // Buffer length constructor
        explicit Buffer(_In_ const size_t& len) :
            m_pBuf(Allocate(len)),
            m_Len(len),
            m_WritePos(0),
            m_FreeFunc(GetFreeFunction(len))
        { }

        // Raw pointer copy constructor
        Buffer(_In_opt_ const T* p, _In_ const size_t& len) :
            m_pBuf(AllocateFromPointer(p, len)),
            m_Len(len),
            m_WritePos(0),
            m_FreeFunc(GetFreeFunction(len))
        { }

        // Raw pointer move constructor
        // Note: Throws std::invalid_argument if p is null and len is not zero, or if p is not null and len is zero.
        Buffer(_In_opt_ T*& p, _In_ const size_t& len) :
            m_pBuf(p),
            m_Len(len),
            m_WritePos(0),
            m_FreeFunc(GetFreeFunction(len))
        {
            try
            {
                ValidateRawPointerArgs(__FUNCSIG__, p, len);
            }
            catch ( const std::invalid_argument& )
            {
                Reset( );
                throw;
            }
            
            p = nullptr;
        }

        // Copy constructor
        Buffer(_In_ const Buffer<T>& src) :
            m_pBuf(AllocateFromPointer(src.Ptr( ), src.Length( ))),
            m_Len(src.Length( )),
            m_WritePos(src.WritePosition( )),
            m_FreeFunc(GetFreeFunction(src.Length( )))
        { }

        // Move constructor
        Buffer(_In_ Buffer<T>&& src) noexcept :
            m_pBuf(src.Ptr( )),
            m_Len(src.Length( )),
            m_WritePos(src.WritePosition( )),
            m_FreeFunc(GetFreeFunction(src.Length( )))
        {
            src.Reset( );
        }

        // Interface Copy constructor
        Buffer(_In_ const IBuffer<T>& src) :
            m_pBuf(AllocateFromPointer(src.Ptr( ), src.Length( ))),
            m_Len(src.Length( )),
            m_WritePos(src.WritePosition( )),
            m_FreeFunc(GetFreeFunction(src.Length( )))
        { }

        // Interface Move constructor
        Buffer(_In_ IBuffer<T>&& src) noexcept :
            m_pBuf(src.Ptr( )),
            m_Len(src.Length( )),
            m_WritePos(src.WritePosition( )),
            m_FreeFunc(GetFreeFunction(src.Length( )))
        {
            src.Reset( );
        }

        /// Protected Destructor \\\

        // Destructor
        virtual ~Buffer( ) noexcept
        {
            InvokeFreeFunction( );
        }

        /// Operator Overloads \\\

        // Copy assignment
        Buffer<T>& operator=(_In_ const Buffer<T>& src)
        {
            if ( this != &src )
            {
                CopyBuffer(*this, src);
            }

            return *this;
        }

        // Move assignment
        Buffer<T>& operator=(_In_ Buffer<T>&& src) noexcept
        {
            if ( this != &src )
            {
                TransferBuffer(*this, src);
            }

            return *this;
        }

        // Interface copy assignment
        Buffer<T>& operator=(_In_ const IBuffer<T>& src)
        {
            if ( this != &src )
            {
                CopyBuffer(*this, src);
            }

            return *this;
        }

        // Interface move assignment
        Buffer<T>& operator=(_In_ IBuffer<T>&& src) noexcept
        {
            if ( this != &src )
            {
                TransferBuffer(*this, src);
            }

            return *this;
        }

        // bool overload - returns true if m_pBuf != nullptr, false otherwise.
        explicit operator bool( ) const noexcept
        {
            return m_pBuf != nullptr;
        }

        // T* overload - returns pointer to mutable internal buffer.
        explicit operator T*() noexcept
        {
            return m_pBuf;
        }

        // const T* overload - returns pointer to immutable internal buffer.
        explicit operator const T*() const noexcept
        {
            return m_pBuf;
        }

        // Subscript overload - returns reference to mutable element from internal buffer via index.
        // Note: Will throw std::logic_error if m_pBuf == nullptr.
        // Note: Will throw std::out_of_range if i >= m_Len.
        virtual T& operator[](_In_ const size_t& i)
        {
            ValidateDereference(__FUNCSIG__, m_pBuf);
            ValidateIndex(__FUNCSIG__, i);
            return m_pBuf[i];
        }

        // Subscript overload - returns reference to immutable element from internal buffer via index.
        // Note: Will throw std::logic_error if m_pBuf == nullptr.
        // Note: Will throw std::out_of_range if i >= m_Len.
        virtual const T& operator[](_In_ const size_t& i) const
        {
            return this->operator[](i);
        }

        // Dereference overload - returns reference to first element from internal buffer (mutable).
        // Note: Will throw std::logic_error if m_pBuf == nullptr.
        virtual T& operator*( )
        {
            ValidateDereference(__FUNCSIG__, m_pBuf);
            return *m_pBuf;
        }

        // Dereference overload - returns reference to first element from internal buffer (immutable).
        // Note: Will throw std::logic_error if m_pBuf == nullptr.
        virtual const T& operator*( ) const
        {
            return this->operator*( );
        }

        // Comparison Equal-To overload - returns true if contents of specified buffer match m_pBuf, false otherwise.
        virtual const bool operator==(_In_ const IBuffer<T>& rhs) const noexcept
        {
            return Compare(rhs.Ptr( ), rhs.Length( ));
        }

        // Comparison Not-Equal-To overload - returns true if contents of specified buffer do not match m_pBuf, false otherwise.
        virtual const bool operator!=(_In_ const IBuffer<T>& rhs) const noexcept
        {
            return !Compare(rhs.Ptr( ), rhs.Length( ));
        }

        // Insertion overload - writes specified T object to internal buffer at m_WritePos, then returns reference to this object.
        // Note: Will throw std::logic_error if m_pBuf == nullptr.
        // Note: Will throw std::out_of_range if write would go beyond end of buffer.
        virtual Buffer<T>& operator<<(_In_ const T& t)
        {
            Write(t);
            return *this;
        }

        // Insertion overload - writes source buffer content to internal buffer at m_WritePos, then returns reference to this object.
        // Note: Will only write up to src.WritePosition( ).
        // Note: Will throw std::logic_error if m_pBuf == nullptr.
        // Note: Will throw std::logic_error if src.Ptr( ) == nullptr.
        // Note: Will throw std::out_of_range if write would go beyond end of buffer.
        virtual Buffer<T>& operator<<(_In_ const IBuffer<T>& src)
        {
            Write(src);
            return *this;
        }

        /// Getters \\\

        // Returns pointer to internal buffer (mutable).
        virtual constexpr T* Ptr( ) noexcept
        {
            return m_pBuf;
        }

        // Returns pointer to internal buffer (immutable).
        virtual constexpr const T* Ptr( ) const noexcept
        {
            return m_pBuf;
        }

        // Returns length of internal buffer.
        virtual constexpr const size_t& Length( ) const noexcept
        {
            return m_Len;
        }

        // Returns size in bytes of internal buffer.
        virtual constexpr const size_t Size( ) const noexcept
        {
            return sizeof(T) * m_Len;
        }

        // Returns current write position.
        virtual constexpr const size_t& WritePosition( ) const noexcept
        {
            return m_WritePos;
        }

        /// Setters \\\

        // Sets write position to specified value.
        // Note: Will throw std::out_of_range if pos >= m_Len
        virtual void SetWritePosition(_In_ const size_t& pos)
        {
            ValidateWritePosition(__FUNCSIG__, pos);
            m_WritePos = pos;
        }

        // Resets write position to beginning of buffer (0).
        virtual void ResetWritePosition( ) noexcept
        {
            m_WritePos = 0;
        }

        /// Public Methods \\\

        // Returns pointer to internal buffer, resets data members to default values.
        // Note: Caller now owns the buffer memory - they must ensure it is freed as some point.
        // Note: Will return nullptr if the buffer doesn't currently manage any resources.
        virtual T* Claim( ) noexcept
        {
            T* p = m_pBuf;
            Reset( );
            return p;
        }

        // Causes the buffer to free any resources, resetting data members to their default values.
        virtual void Free( ) noexcept
        {
            InvokeFreeFunction( );
            Reset( );
        }

        // Sets all byte values of the internal buffer to zero.
        // Note: This will throw std::logic_error if the internal buffer is nullptr.
        virtual void ZeroBuffer( )
        {
            ValidateDereference(__FUNCSIG__, m_pBuf);
            memset(m_pBuf, 0, sizeof(T) * m_Len);
        }

        // Returns true if contents of m_pBuf match contents of p (up to len), false otherwise.
        // Note: Throws std::invalid_argument if p is null and len is not zero, or if p is not null and len is zero.
        // Note: Throws std::out_of_range if len > m_Len
        const bool Compare(_In_opt_ const T* p, _In_ const size_t& len) const 
        {
            ValidateRawPointerArgs(__FUNCSIG__, p, len);

            if ( m_pBuf == p )
            {
                return true;
            }

            if ( !m_pBuf || !p || len > m_Len )
            {
                return false;
            }

            for ( size_t i = 0; i < len; i++ )
            {
                if ( m_pBuf[i] != p[i] )
                {
                    return false;
                }
            }

            return true;
        }

        // Returns true if contents of m_pBuf match contents of buffer.Ptr( ), false otherwise.
        const bool Compare(_In_ const IBuffer<T>& buffer) const
        {
            return Compare(buffer.Ptr( ), buffer.Length( ));
        }

        // Writes specified T object to internal buffer at m_WritePos.
        // Note: Will throw std::logic_error if m_pBuf == nullptr.
        // Note: Will throw std::out_of_range if write would go beyond end of buffer.
        virtual void Write(_In_ const T& t)
        {
            ValidateWriteRequest(__FUNCSIG__, &t, 1);
            m_pBuf[m_WritePos++] = t;
        }

        // Writes raw pointer content to internal buffer at m_WritePos.
        // Note: Will throw std::logic_error if m_pBuf == nullptr.
        // Note: Will throw std::logic_error if p == nullptr.
        // Note: Will throw std::out_of_range if write would go beyond end of buffer.
        virtual void Write(_In_ const T* const p, _In_ const size_t& len)
        {
            ValidateWriteRequest(__FUNCSIG__, p, len);
            memcpy(m_pBuf + m_WritePos, p, sizeof(T) * len);
            m_WritePos += len;
        }

        // Writes source buffer content to internal buffer at m_WritePos.
        // Note: If bCopyUpToSrcWritePos == true, will write up to src.WritePosition( ), otherwise up to src.Length( ).
        // Note: Will throw std::logic_error if m_pBuf == nullptr.
        // Note: Will throw std::logic_error if src.Ptr( ) == nullptr.
        // Note: Will throw std::out_of_range if write would go beyond end of buffer.
        virtual void Write(_In_ const IBuffer<T>& src, _In_ const bool& bCopyUpToSrcWritePos = true)
        {
            Write(src.Ptr( ), bCopyUpToSrcWritePos ? src.WritePosition( ) : src.Length( ));
        }
    };
}

#endif // _CC_BUFFER_H_