#pragma once

#include <CCMacros.h>
#include <CCPointerHelper.h>

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

        /// Helper Struct \\\

        template <typename T>
        struct Node
        {
            T data;
            Node<T>* pNext;

            // Default constructor.
            Node() noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T)) :
                data(T()),
                pNext(nullptr)
            { }

            // Constructor, targeted universal forwarder.
            template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
            Node(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T)) :
                data(std::forward<U>(obj)),
                pNext(nullptr)
            { }

            ~Node() noexcept(CC_IS_NOTHROW_DTOR(T)) = default;
        };

        // Type aliases
        using PH = PointerHelper<Node<T>>;

        /// Private Data Members \\\

        Node<T>* m_pHead;
        Node<T>* m_pTail;
        size_t m_Len;

        /// Private Throwing Validators \\\

        // Throws std::logic_error if this queue is empty.
        inline void ValidateDereferenceT(_In_z_ const char* const f)
        {
            if (IsEmpty())
            {
                std::string msg(": Attempted invalid access on empty queue.");
                throw std::logic_error(f + msg);
            }
        }

        /// Private Helper Methods \\\

        // Deallocates all nodes in the queue.
        // Note: Data members are not reset.
        inline void DestroyQueue()
        {
            while (!!m_pHead)
            {
                Node<T>* p = m_pHead;
                m_pHead = m_pHead->pNext;
                delete p;
            }
        }

        // If specified, frees all queue nodes. Always resets all data members to default values.
        template <bool bDestroy>
        inline void ResetQueue() noexcept
        {
            if constexpr (bDestroy)
            {
                DestroyQueue();
            }

            m_pHead = nullptr;
            m_pTail = nullptr;
            m_Len = 0;
        }

        // Clears this queue and assigns single element to this queue via copy or move.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool AssignElementObj(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            // Allocate new queue element.
            Node<T>* p = PH::Allocate(std::forward<U>(obj));
            if (!p)
            {
                return false;
            }

            // Destroy current queue.
            DestroyQueue();

            // Assign element to the queue, adjust length.
            m_pHead = m_pTail = p;
            m_Len = 1;

            return true;
        }

        // Clears this queue and copies src queue to this queue.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool AssignQueueObj(_In_ const Queue<T>& src) noexcept(CC_IS_NOTHROW_CTOR_COPY(T))
        {
            Node<T>* pSrc = src.m_pHead;
            Queue<T> tmp;

            if (src.IsEmpty())
            {
                // Assigns empty queue - clear out this queue.
                ResetQueue<true>();
                return true;
            }

            // Copy first element to tmp queue.
            tmp.m_pHead = tmp.m_pTail = PH::Allocate(pSrc->data);
            if (!tmp.m_pHead)
            {
                return false;
            }

            // Copy the rest of the elements.
            pSrc = pSrc->pNext;
            while (!!pSrc)
            {
                tmp.m_pTail->pNext = PH::Allocate(pSrc->data);
                if (!tmp.m_pTail->pNext)
                {
                    // Allocation failed - bail out.
                    return false;
                }

                pSrc = pSrc->pNext;
                tmp.m_pTail = tmp.m_pTail->pNext;
            }

            // Copy the length.
            tmp.m_Len = src.m_Len;

            // Move tmp queue to this queue.
            return AssignQueueObj(std::move(tmp));
        }

        // Clears this queue and transfers src queue contents to this queue.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool AssignQueueObj(_Inout_ Queue<T>&& src) noexcept
        {
            // Destroy current queue.
            DestroyQueue();

            // Transfer elements to this queue, copy length.
            m_pHead = src.m_pHead;
            m_pTail = src.m_pTail;
            m_Len = src.m_Len;

            // src doesn't own this queue anymore - reset it.
            src.ResetQueue<false>();

            return true;
        }

        // Adds element to the back of the queue.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        [[nodiscard]] bool EnqueueElementObj(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            Node<T>* p = PH::Allocate(std::forward<U>(obj));
            if (!p)
            {
                // Allocation failed - bail out.
                return false;
            }

            if (IsEmpty())
            {
                m_pHead = m_pTail = p;
            }
            else
            {
                m_pTail = m_pTail->pNext = p;
            }

            m_Len++;

            return true;
        }

    public:

        /// Constructors \\\

        // Default constructor
        Queue() noexcept :
            m_pHead(nullptr),
            m_Len(0)
        { }

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

        /// Destructor \\\

        ~Queue() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            DestroyQueue();
        }

        /// Assignment Overload \\\

        // Copy assignment
        Queue<T>& operator=(_In_ const Queue<T>& src) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            if (!Assign(src))
            {
                ResetQueue<true>();
            }

            return *this;
        }

        // Move assignment
        Queue<T>& operator=(_Inout_ Queue<T>&& src) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            if (!Assign(std::move(src)))
            {
                ResetQueue<true>();
            }

            return *this;
        }

        // Targeted Universal Forwarder to Assign
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        Queue<T>& operator=(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if (!Assign(std::forward<U>(obj)))
            {
                ResetQueue<true>();
            }

            return *this;
        }

        /// Operator Overloads \\\

        // Returns true if queue is not empty, false otherwise.
        [[nodiscard]] explicit operator bool() const noexcept
        {
            return !IsEmpty();
        }

        // Compares this queue's elements against rhs' queue elements.
        // Returns true if queues are the same length and all elements match, false otherwise.
        [[nodiscard]] bool operator==(_In_ const Queue<T>& rhs) const noexcept
        {
            return Compare(rhs);
        }

        /// Getters \\\

        // Returns length of queue (number of elements).
        [[nodiscard]] inline size_t Length() const noexcept
        {
            return m_Len;
        }

        // Returns mutable reference to last element in the queue.
        // Note: If queue is empty, this throws std::logic_error.
        [[nodiscard]] inline T& Back()
        {
            ValidateDereferenceT(__FUNCSIG__);
            return m_pTail->data;
        }

        // Returns immutable reference to last element in the queue.
        // Note: If queue is empty, this throws std::logic_error.
        [[nodiscard]] inline const T& Back() const
        {
            ValidateDereferenceT(__FUNCSIG__);
            return m_pTail->data;
        }

        // Returns mutable reference to first element in the queue.
        // Note: If queue is empty, this throws std::logic_error.
        [[nodiscard]] inline T& Front()
        {
            ValidateDereferenceT(__FUNCSIG__);
            return m_pHead->data;
        }

        // Returns mutable reference to first element in the queue.
        // Note: If queue is empty, this throws std::logic_error.
        [[nodiscard]] inline const T& Front() const
        {
            ValidateDereferenceT(__FUNCSIG__);
            return m_pHead->data;
        }

        /// Public Methods \\\

        // Clears previous queue contents and assigns specified element or queue object to this queue.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_QUEUE_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Assign(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if constexpr (CC_IS_QUEUE(T, U))
            {
                return (this == &obj) || AssignQueueObj(std::forward<U>(obj));
            }
            else if constexpr (CC_IS_ELEMENT(T, U))
            {
                return AssignElementObj(std::forward<U>(obj));
            }
            else
            {
                static_assert(false, __FUNCSIG__ ": Unsuported assign type.");
            }
        }

        // Destroys queue, freeing all resources and reseting queue.
        void Clear() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            ResetQueue<true>();
        }

        // Compares this queue's elements against rhs' queue elements.
        // Returns true if queues are the same length and all elements match, false otherwise.
        [[nodiscard]] bool Compare(_In_ const Queue<T>& rhs) const noexcept
        {
            Node<T>* pL = m_pHead;
            Node<T>* pR = rhs.m_pHead;

            if (pL == pR)
            {
                // Comparing same queue - equal.
                return true;
            }

            if (m_Len != rhs.m_Len)
            {
                // Length mismatch - not equal.
                return false;
            }

            // Compare while we haven't exhausted either queue.
            while (!!pL)
            {
                if (pL->data != pR->data)
                {
                    // Mismatch - not equal.
                    return false;
                }

                // Move to next elements.
                pL = pL->pNext;
                pR = pR->pNext;
            }

            return true;
        }

        // Returns true if the queue is empty, false otherwise.
        [[nodiscard]] bool IsEmpty() const noexcept
        {
            return (m_Len == 0);
        }

        // Removes element from the front of the queue.
        // Returns true if element is removed, false otherwise (e.g., queue is empty).
        _Success_(return) bool Dequeue() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            if (IsEmpty())
            {
                return false;
            }

            Node<T>* p = m_pHead;
            m_pHead = m_pHead->pNext;
            delete p;
            m_Len--;

            if (IsEmpty())
            {
                m_pTail = nullptr;
            }

            return true;
        }

        // Removes element from the front of the queue and moves element to the out parameter.
        // Returns true if element is removed, false otherwise (e.g., queue is empty).
        [[nodiscard]] _Success_(return) bool Dequeue(_Out_ T& obj) noexcept(CC_IS_NOTHROW_MOVE(T) && CC_IS_NOTHROW_DTOR(T))
        {
            if (IsEmpty())
            {
                return false;
            }

            obj = std::move(m_pHead->data);
            return Dequeue();
        }

        // Enqueues new element onto the queue.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Enqueue(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if (IsEmpty())
            {
                // Enqueue to empty queue is same as assign.
                return AssignElementObj(std::forward<U>(obj));
            }

            return EnqueueElementObj(std::forward<U>(obj));
        }
    };

    // Undefine local macro helpers so we don't add even more pollution to global namespace.
#undef CC_ENABLE_IF_QUEUE_OR_ELEMENT
#undef CC_ENABLE_IF_QUEUE
#undef CC_ENABLE_IF_ELEMENT
#undef CC_IS_QUEUE
#undef CC_IS_ELEMENT

}
