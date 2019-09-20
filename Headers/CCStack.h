#pragma once

#include <CCMacros.h>
#include <CCPointerHelper.h>

namespace CC
{
#define CC_IS_ELEMENT(_T, _U)\
        (std::is_same_v<std::decay_t<_U>, _T> ||\
        std::is_convertible_v<std::decay_t<_U>, _T>)

#define CC_IS_STACK(_T, _U)\
        (std::is_same_v<std::decay_t<_U>, Stack<_T>>)

#define CC_ENABLE_IF_ELEMENT(_T, _U)\
        typename = typename std::enable_if_t<CC_IS_ELEMENT(_T, _U)>

#define CC_ENABLE_IF_STACK(_T, _U)\
        typename = typename std::enable_if_t<CC_IS_STACK(_T, _U)>

#define CC_ENABLE_IF_STACK_OR_ELEMENT(_T, _U)    \
        typename = typename std::enable_if_t<CC_IS_STACK(_T, _U) || CC_IS_ELEMENT(_T, _U)>

    template <typename T, CC_ENABLE_IF_NOT_POINTER_TYPE(T)>
    class [[nodiscard]] Stack
    {
        // Testing class.
        friend class StackTests;

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
            Node(_In_ U&& obj, _In_opt_ Node<T>* pN = nullptr) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T)) :
                data(std::forward<U>(obj)),
                pNext(pN)
            { }

            ~Node() noexcept(CC_IS_NOTHROW_DTOR(T)) = default;
        };

        // Type aliases
        using PH = PointerHelper<Node<T>>;

        /// Private Data Members \\\

        Node<T>* m_pHead;
        size_t m_Len;

        /// Private Throwing Validators \\\

        // Throws std::logic_error if this stack is empty.
        inline void ValidateDereferenceT(_In_z_ const char* const f)
        {
            if (IsEmpty())
            {
                std::string msg(": Attempted invalid access on empty stack.");
                throw std::logic_error(f + msg);
            }
        }

        /// Private Helper Methods \\\

        // Deallocates all nodes in the stack.
        // Note: Data members are not reset.
        inline void DestroyStack()
        {
            while (!!m_pHead)
            {
                Node<T>* p = m_pHead;
                m_pHead = m_pHead->pNext;
                delete p;
            }
        }

        // If specified, frees all stack nodes. Always resets all data members to default values.
        template <bool bDestroy>
        inline void ResetStack() noexcept
        {
            if constexpr (bDestroy)
            {
                DestroyStack();
            }

            m_pHead = nullptr;
            m_Len = 0;
        }

        // Clears this stack and assigns single element to this stack via copy or move.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool AssignElementObj(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            // Allocate new stack element.
            Node<T>* p = PH::Allocate(std::forward<U>(obj));
            if (!p)
            {
                return false;
            }

            // Destroy current stack.
            DestroyStack();

            // Assign element to the stack, adjust length.
            m_pHead = p;
            m_Len = 1;

            return true;
        }

        // Clears this stack and copies src stack to this stack.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool AssignStackObj(_In_ const Stack<T>& src) noexcept(CC_IS_NOTHROW_CTOR_COPY(T))
        {
            Node<T>* pSrc = src.m_pHead;
            Node<T>* pDst = nullptr;
            Stack<T> tmp;

            if (src.IsEmpty())
            {
                // Assigns empty stack - clear out this stack.
                ResetStack<true>();
                return true;
            }

            // Copy first element to tmp stack.
            tmp.m_pHead = PH::Allocate(pSrc->data);
            if (!tmp.m_pHead)
            {
                return false;
            }

            // Copy the rest of the elements.
            pSrc = pSrc->pNext;
            pDst = tmp.m_pHead;
            while (!!pSrc)
            {
                pDst->pNext = PH::Allocate(pSrc->data);
                if (!pDst->pNext)
                {
                    // Allocation failed - bail out.
                    return false;
                }

                pSrc = pSrc->pNext;
                pDst = pDst->pNext;
            }

            // Copy the length.
            tmp.m_Len = src.m_Len;

            // Move tmp stack to this stack.
            return AssignStackObj(std::move(tmp));
        }

        // Clears this stack and transfers src stack contents to this stack.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool AssignStackObj(_Inout_ Stack<T>&& src) noexcept
        {
            // Destroy current stack.
            DestroyStack();

            // Transfer elements to this stack, copy length.
            m_pHead = src.m_pHead;
            m_Len = src.m_Len;

            // src doesn't own this stack anymore - reset it.
            src.ResetStack<false>();

            return true;
        }

        // Adds element to the top of the stack.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        [[nodiscard]] bool PushElementObj(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            Node<T>* p = PH::Allocate(std::forward<U>(obj), m_pHead);
            if (!p)
            {
                // Allocation failed - bail out.
                return false;
            }

            m_pHead = p;
            m_Len++;

            return true;
        }

    public:

        /// Constructors \\\

        // Default constructor
        Stack() noexcept :
            m_pHead(nullptr),
            m_Len(0)
        { }

        // Copy constructor
        Stack(_In_ const Stack<T>& src) noexcept(CC_IS_NOTHROW_CTOR_COPY(T)) :
            Stack<T>()
        {
            *this = src;
        }

        // Move constructor
        Stack(_Inout_ Stack<T>&& src) noexcept(CC_IS_NOTHROW_DTOR(T)) :
            Stack<T>()
        {
            *this = std::move(src);
        }

        // Forwarding constructor
        // Can handle single element copy/move.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        Stack(_In_ U&& src) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T)) :
            Stack<T>()
        {
            *this = std::forward<U>(src);
        }

        /// Destructor \\\

        ~Stack() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            DestroyStack();
        }

        /// Assignment Overload \\\

        // Copy assignment
        Stack<T>& operator=(_In_ const Stack<T>& src) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            if (!Assign(src))
            {
                ResetStack<true>();
            }

            return *this;
        }

        // Move assignment
        Stack<T>& operator=(_Inout_ Stack<T>&& src) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            if (!Assign(std::move(src)))
            {
                ResetStack<true>();
            }

            return *this;
        }

        // Targeted Universal Forwarder to Assign
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        Stack<T>& operator=(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if (!Assign(std::forward<U>(obj)))
            {
                ResetStack<true>();
            }

            return *this;
        }

        /// Operator Overloads \\\

        // Returns true if stack is not empty, false otherwise.
        [[nodiscard]] explicit operator bool() const noexcept
        {
            return !IsEmpty();
        }

        // Compares this stack's elements against rhs' stack elements.
        // Returns true if stacks are the same length and all elements match, false otherwise.
        [[nodiscard]] bool operator==(_In_ const Stack<T>& rhs) const noexcept
        {
            return Compare(rhs);
        }

        /// Getters \\\

        // Returns length of stack (number of elements).
        [[nodiscard]] inline size_t Length() const noexcept
        {
            return m_Len;
        }

        // Returns mutable reference to first element in the stack.
        // Note: If stack is empty, this throw std::logic_error.
        [[nodiscard]] inline T& Top()
        {
            ValidateDereferenceT(__FUNCSIG__);
            return m_pHead->data;
        }

        // Returns mutable reference to first element in the stack.
        // Note: If stack is empty, this throw std::logic_error.
        [[nodiscard]] inline const T& Top() const
        {
            ValidateDereferenceT(__FUNCSIG__);
            return m_pHead->data;
        }

        /// Public Methods \\\

        // Clears previous stack contents and assigns specified element or stack object to this stack.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_STACK_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Assign(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if constexpr (CC_IS_STACK(T, U))
            {
                return (this == &obj) || AssignStackObj(std::forward<U>(obj));
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

        // Destroys stack, freeing all resources and reseting stack.
        void Clear() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            ResetStack<true>();
        }

        // Compares this stack's elements against rhs' stack elements.
        // Returns true if stacks are the same length and all elements match, false otherwise.
        [[nodiscard]] bool Compare(_In_ const Stack<T>& rhs) const noexcept
        {
            Node<T>* pL = m_pHead;
            Node<T>* pR = rhs.m_pHead;

            if (pL == pR)
            {
                // Comparing same stack - equal.
                return true;
            }

            if (m_Len != rhs.m_Len)
            {
                // Length mismatch - not equal.
                return false;
            }

            // Compare while we haven't exhausted either stack.
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

        // Returns true if the stack is empty, false otherwise.
        [[nodiscard]] bool IsEmpty() const noexcept
        {
            return (m_Len == 0);
        }

        // Pops element from the top of the stack.
        // Returns true if element is removed, false otherwise (e.g., stack is empty).
        _Success_(return) bool Pop() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            if (IsEmpty())
            {
                return false;
            }

            Node<T>* p = m_pHead;
            m_pHead = m_pHead->pNext;
            delete p;
            m_Len--;

            return true;
        }

        // Pops element from the top of the stack and moves element to the out parameter.
        // Returns true if element is removed, false otherwise (e.g., stack is empty).
        [[nodiscard]] _Success_(return) bool Pop(_Out_ T& obj) noexcept(CC_IS_NOTHROW_MOVE(T) && CC_IS_NOTHROW_DTOR(T))
        {
            if (IsEmpty())
            {
                return false;
            }

            obj = std::move(m_pHead->data);
            return Pop();
        }

        // Pushes new element onto the stack.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Push(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if (IsEmpty())
            {
                // Push to empty stack is same as assign.
                return AssignElementObj(std::forward<U>(obj));
            }

            return PushElementObj(std::forward<U>(obj));
        }
    };

    // Undefine local macro helpers so we don't add even more pollution to global namespace.
#undef CC_ENABLE_IF_STACK_OR_ELEMENT
#undef CC_ENABLE_IF_STACK
#undef CC_ENABLE_IF_ELEMENT
#undef CC_IS_STACK
#undef CC_IS_ELEMENT

}
