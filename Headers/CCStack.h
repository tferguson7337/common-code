#pragma once

#include <CCMacros.h>
#include <CCForwardList.h>

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

        /// Private Data Members \\\

        ForwardList<T> m_FwdList;

    public:

        /// Constructors \\\

        // Default constructor
        Stack() noexcept = default;

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

        ~Stack() noexcept(CC_IS_NOTHROW_DTOR(T)) { };

        /// Assignment Overload \\\

        // Copy assignment
        Stack<T>& operator=(_In_ const Stack<T>& src) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            m_FwdList.operator=(src.m_FwdList);
            return *this;
        }

        // Move assignment
        Stack<T>& operator=(_Inout_ Stack<T>&& src) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            m_FwdList.operator=(std::move(src.m_FwdList));
            return *this;
        }

        // Targeted Universal Forwarder to Assign
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        Stack<T>& operator=(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            m_FwdList.operator=(std::forward<U>(obj));
            return *this;
        }

        /// Operator Overloads \\\

        // Returns true if stack is not empty, false otherwise.
        [[nodiscard]] explicit operator bool() const noexcept
        {
            return !!m_FwdList;
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
            return m_FwdList.Length();
        }

        // Returns mutable reference to first element in the stack.
        // Note: If stack is empty, this throws std::logic_error.
        [[nodiscard]] inline T& Top()
        {
            return m_FwdList.Front();
        }

        // Returns mutable reference to first element in the stack.
        // Note: If stack is empty, this throws std::logic_error.
        [[nodiscard]] inline const T& Top() const
        {
            return m_FwdList.Front();
        }

        /// Public Methods \\\

        // Clears previous stack contents and assigns specified element or stack object to this stack.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_STACK_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Assign(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if constexpr (CC_IS_STACK(T, U))
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
            else if constexpr (CC_IS_ELEMENT(T, U))
            {
                return m_FwdList.Assign(std::forward<U>(obj));
            }
            else
            {
                static_assert(false, __FUNCSIG__ ": Unsupported assign type.");
            }
        }

        // Destroys stack, freeing all resources and reseting stack.
        void Clear() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            m_FwdList.Clear();
        }

        // Compares this stack's elements against rhs' stack elements.
        // Returns true if stacks are the same length and all elements match, false otherwise.
        [[nodiscard]] bool Compare(_In_ const Stack<T>& rhs) const noexcept
        {
            return m_FwdList.Compare(rhs.m_FwdList);
        }

        // Returns true if the stack is empty, false otherwise.
        [[nodiscard]] bool IsEmpty() const noexcept
        {
            return m_FwdList.IsEmpty();
        }

        // Pops element from the top of the stack.
        // Returns true if element is removed, false otherwise (e.g., stack is empty).
        bool Pop() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            return m_FwdList.PopFront();
        }

        // Pops element from the top of the stack and moves element to the out parameter.
        // Returns true if element is removed, false otherwise (e.g., stack is empty).
        [[nodiscard]] _Success_(return) bool Pop(_Out_ T& obj) noexcept(CC_IS_NOTHROW_MOVE(T) && CC_IS_NOTHROW_DTOR(T))
        {
            return m_FwdList.PopFront(obj);
        }

        // Pushes new element onto the stack.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Push(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            return m_FwdList.Prepend(std::forward<U>(obj));
        }
    };

    // Undefine local macro helpers so we don't add even more pollution to global namespace.
#undef CC_ENABLE_IF_STACK_OR_ELEMENT
#undef CC_ENABLE_IF_STACK
#undef CC_ENABLE_IF_ELEMENT
#undef CC_IS_STACK
#undef CC_IS_ELEMENT

}
