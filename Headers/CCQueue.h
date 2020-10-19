#pragma once

#include <CCMacros.h>
#include <CCForwardList.h>

namespace CC
{
#define CC_IS_ELEMENT(_T, _U)\
        (std::is_same_v<std::decay_t<_U>, _T> ||\
        std::is_convertible_v<std::decay_t<_U>, _T>)

#define CC_IS_QUEUE(_T, _U)\
        (std::is_same_v<std::decay_t<_U>, Queue<_T>>)

#define CC_ENABLE_IF_ELEMENT(_T, _U)\
        typename = typename std::enable_if_t<CC_IS_ELEMENT(_T, _U)>

#define CC_ENABLE_IF_QUEUE(_T, _U)\
        typename = typename std::enable_if_t<CC_IS_QUEUE(_T, _U)>

#define CC_ENABLE_IF_QUEUE_OR_ELEMENT(_T, _U)    \
        typename = typename std::enable_if_t<CC_IS_QUEUE(_T, _U) || CC_IS_ELEMENT(_T, _U)>

    template <typename T, CC_ENABLE_IF_NOT_POINTER_TYPE(T)>
    class [[nodiscard]] Queue
    {
        // Testing class.
        friend class QueueTests;

    private:

        // Private Data Members //

        ForwardList<T> m_FwdList;

    public:

        // Constructors //

        // Default constructor
        Queue() noexcept = default;

        // Copy constructor
        Queue(_In_ const Queue<T>& src) noexcept(CC_IS_NOTHROW_CTOR_COPY(T)) :
            Queue<T>()
        {
            *this = src;
        }

        // Move constructor
        Queue(_Inout_ Queue<T>&& src) noexcept(CC_IS_NOTHROW_DTOR(T)) :
            Queue<T>()
        {
            *this = std::move(src);
        }

        // Forwarding constructor
        // Can handle single element copy/move.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        Queue(_In_ U&& src) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T)) :
            Queue<T>()
        {
            *this = std::forward<U>(src);
        }

        // Destructor //

        ~Queue() noexcept(CC_IS_NOTHROW_DTOR(T)) { }

        // Assignment Overload //

        // Copy assignment
        Queue<T>& operator=(_In_ const Queue<T>& src) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            m_FwdList = src.m_FwdList;
            return *this;
        }

        // Move assignment
        Queue<T>& operator=(_Inout_ Queue<T>&& src) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            m_FwdList = std::move(src.m_FwdList);
            return *this;
        }

        // Targeted Universal Forwarder to Assign
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        Queue<T>& operator=(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            m_FwdList = std::forward<U>(obj);
            return *this;
        }

        // Operator Overloads //

        // Returns true if queue is not empty, false otherwise.
        [[nodiscard]] explicit operator bool() const noexcept
        {
            return !!m_FwdList;
        }

        // Compares this queue's elements against rhs' queue elements.
        // Returns true if queues are the same length and all elements match, false otherwise.
        [[nodiscard]] bool operator==(_In_ const Queue<T>& rhs) const noexcept
        {
            return Compare(rhs);
        }

        // Getters //

        // Returns length of queue (number of elements).
        [[nodiscard]] inline size_t Length() const noexcept
        {
            return m_FwdList.Length();
        }

        // Returns mutable reference to last element in the queue.
        // Note: If queue is empty, this throws std::logic_error.
        [[nodiscard]] inline T& Back()
        {
            return m_FwdList.Back();
        }

        // Returns immutable reference to last element in the queue.
        // Note: If queue is empty, this throws std::logic_error.
        [[nodiscard]] inline const T& Back() const
        {
            return m_FwdList.Back();
        }

        // Returns mutable reference to first element in the queue.
        // Note: If queue is empty, this throws std::logic_error.
        [[nodiscard]] inline T& Front()
        {
            return m_FwdList.Front();
        }

        // Returns mutable reference to first element in the queue.
        // Note: If queue is empty, this throws std::logic_error.
        [[nodiscard]] inline const T& Front() const
        {
            return m_FwdList.Front();
        }

        // Public Methods //

        // Clears previous queue contents and assigns specified element or queue object to this queue.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_QUEUE_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Assign(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if constexpr (CC_IS_QUEUE(T, U))
            {
                if constexpr (std::is_rvalue_reference_v<decltype(std::forward<U>(obj))>)
                {
                    return m_FwdList.Assign(std::move(obj.m_FwdList));
                }
                else
                {
                    return m_FwdList.Assign(obj.m_FwdList);
                }
            }
            else // if constexpr (CC_IS_ELEMENT(T, U))
            {
                return m_FwdList.Assign(std::forward<U>(obj));
            }
        }

        // Destroys queue, freeing all resources and reseting queue.
        void Clear() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            m_FwdList.Clear();
        }

        // Compares this queue's elements against rhs' queue elements.
        // Returns true if queues are the same length and all elements match, false otherwise.
        [[nodiscard]] bool Compare(_In_ const Queue<T>& rhs) const noexcept
        {
            return m_FwdList.Compare(rhs.m_FwdList);
        }

        // Returns true if the queue is empty, false otherwise.
        [[nodiscard]] bool IsEmpty() const noexcept
        {
            return m_FwdList.IsEmpty();
        }

        // Removes element from the front of the queue.
        // Returns true if element is removed, false otherwise (e.g., queue is empty).
        _Success_(return) bool Dequeue() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            return m_FwdList.PopFront();
        }

        // Removes element from the front of the queue and moves element to the out parameter.
        // Returns true if element is removed, false otherwise (e.g., queue is empty).
        [[nodiscard]] _Success_(return) bool Dequeue(_Out_ T& obj) noexcept(CC_IS_NOTHROW_MOVE(T) && CC_IS_NOTHROW_DTOR(T))
        {
            return m_FwdList.PopFront(obj);
        }

        // Enqueues new element onto the queue.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Enqueue(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            return m_FwdList.Append(std::forward<U>(obj));
        }
    };

    // Undefine local macro helpers so we don't add even more pollution to global namespace.
#undef CC_ENABLE_IF_QUEUE_OR_ELEMENT
#undef CC_ENABLE_IF_QUEUE
#undef CC_ENABLE_IF_ELEMENT
#undef CC_IS_QUEUE
#undef CC_IS_ELEMENT

}
