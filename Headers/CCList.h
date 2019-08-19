#pragma once

#include "CCPointerHelper.h"

namespace CC
{
    // Local macro helpers for restricted universal forwarders.
#define CC_IS_ELEMENT(_T, _U)\
        std::is_same_v<std::decay_t<_U>, _T>

#define CC_IS_LIST(_T, _U)\
        std::is_same_v<std::decay_t<_U>, List<_T>>

#define CC_ENABLE_IF_ELEMENT(_T, _U)\
        typename = typename std::enable_if_t<CC_IS_ELEMENT(_T, _U)>

#define CC_ENABLE_IF_LIST(_T, _U)\
        typename = typename std::enable_if_t<CC_IS_LIST(_T, _U)>

#define CC_ENABLE_IF_LIST_OR_ELEMENT(_T, _U)    \
        typename = typename std::enable_if_t<CC_IS_LIST(_T, _U) || CC_IS_ELEMENT(_T, _U)>

    template <typename T, CC_ENABLE_IF_NOT_POINTER_TYPE(T)>
    class [[nodiscard]] List
    {
        // Helper class forward decl.
        template <typename T>
        struct DNode;

        // Type alias.
        using PH = PointerHelper<DNode<T>>;

    private:

        // Helper Class - Doubly-linked DNode.
        template <typename T>
        struct DNode
        {
            T data;
            DNode<T>* pPrev;
            DNode<T>* pNext;

            DNode() noexcept :
                data(T()),
                pPrev(nullptr),
                pNext(nullptr)
            { }

            DNode(_In_ const T& t, _In_opt_ DNode<T>* pP = nullptr, _In_opt_ DNode<T>* pN = nullptr) noexcept :
                data(t),
                pPrev(pP),
                pNext(pN)
            { }

            DNode(_Inout_ T&& t, _In_opt_ DNode<T>* pP = nullptr, _In_opt_ DNode<T>* pN = nullptr) noexcept :
                data(std::move(t)),
                pPrev(pP),
                pNext(pN)
            { }

            ~DNode() noexcept = default;

            [[nodiscard]] _Success_(return) bool Prepend(_In_ T&& t) noexcept
            {
                DNode<T>* pNew = PointerHelper<DNode<T>>::Allocate(std::forward<T>(t), pPrev, pNext);
                if (!pNew)
                {
                    return false;
                }

                if (!!pPrev)
                {
                    pPrev->pNext = pNew;
                }

                if (!!pNext)
                {
                    pNext->pPrev = pNew;
                }

                return true;
            }

            [[nodiscard]] _Success_(return) bool Append(_In_ T&& t) noexcept
            {
                DNode<T>* const& pP = pNext;
                DNode<T>* const& pN = (!!pNext) ? pNext->pNext : nullptr;
                DNode<T>* pNew = PointerHelper<DNode<T>>::Allocate(std::forward<T>(t), pP, pN);
                if (!pNew)
                {
                    return false;
                }

                if (!!pP)
                {
                    pP->pNext = pNew;
                }

                if (!!pN)
                {
                    pN->pPrev = pNew;
                }

                return true;
            }
        };


        /// Private Data Members \\\

        DNode<T>* m_pHead;
        DNode<T>* m_pTail;
        size_t m_Len;

        /// Private Throwing Validators \\\

        inline void ValidateDereferenceT(_In_z_ const char* const f)
        {
            if (IsEmpty())
            {
                std::string msg(": Attempted to access empty list.");
                throw std::logic_error(f + msg1);
            }
        }

        inline void ValidateAccessAtPositionT(_In_z_ const char* const f, _In_ const size_t& pos)
        {
            ValidateDereferenceT(f);
            if (pos >= (m_Len - 1))
            {
                std::string msg1(": Attempted to access out-of-range list position[");
                std::string dat1(std::to_string(pos));
                std::string msg2("]  Valid Range[0, ");
                std::string dat2(std::to_string(m_Len - 1));
                std::string msg3("].");
                throw std::out_of_range(f + msg1 + dat1 + msg2 + dat2 + msg3);
            }
        }

        /// Private Helper Methods \\\

        //@todo: Implement...

    public:

        /// Constructors \\\

        List() noexcept;

        template <typename U, CC_ENABLE_IF_LIST_OR_ELEMENT(T, U)>
        List(_In_ U&& src) noexcept;


        /// Destructor \\\

        ~List() noexcept;


        /// Assignment Overloads \\\

        // Targetted Universal Forwarder to Assign
        template <typename U, CC_ENABLE_IF_LIST_OR_ELEMENT(T, U)>
        List<T>& operator=(_In_ U&& obj) noexcept;


        /// Operator Overloads \\\

        [[nodiscard]] explicit operator bool() const noexcept;

        // Targetted Universal Forwarder to Append
        template <typename U, CC_ENABLE_IF_LIST_OR_ELEMENT(T, U)>
        List<T>& operator+=(_In_ U&& t) noexcept;

        /// Getters \\\

        [[nodiscard]] const size_t& Length() const noexcept;

        [[nodiscard]] T& Front();
        [[nodiscard]] const T& Front() const;

        [[nodiscard]] T& Back();
        [[nodiscard]] const T& Back() const;

        // Retrieves mutable element at specified position.
        // Note: Throws std::out_of_range if pos refers to a non-existant element.
        [[nodiscard]] T& At(_In_ const size_t& pos);

        // Retrieves immutable element at specified position.
        // Note: Throws std::out_of_range if pos refers to a non-existant element.
        [[nodiscard]] const T& At(_In_ const size_t& pos) const;


        /// Public Methods \\\

        // Universal Assign Forwarder.
        // Clears previous list contents and assigns specified element or list object to this list.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_LIST_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Assign(_In_ U&& obj) noexcept;

        // Adds specified element or list to the end of this list via copy/move.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_LIST_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Append(_In_ U&& obj) noexcept;

        // Destroys all list elements, setting this list back to default state.
        void Clear() noexcept;

        // Inserts specified element/list into this list via copy/move.
        // Returns true if operation succeeds, false otherwise.
        // Note: If pos exceeds list length, then element is appending to the end of the list.
        template <typename U, CC_ENABLE_IF_LIST_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Insert(_In_ const size_t& pos, _In_ U&& t) noexcept;

        // Returns true if the list is empty, false otherwise.
        [[nodiscard]] bool IsEmpty() const noexcept;

        // Adds element to the front of the list.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_LIST_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Prepend(_In_ U&& t) noexcept;

        // Removes last element from list.
        // Note: Does nothing if list is currently empty.
        void PopBack() noexcept;

        // Removes first element from list.
        // Note: Does nothing if list is currently empty.
        void PopFront() noexcept;

        // Removes specified element from list.
        // Note: If the list empty, then this does nothing.
        // Note: If the list is not empty and pos exceeds list length, then the last element is removed.
        void RemoveAt(_In_ const size_t& pos) noexcept;
    };

    // Undefine local macro helpers so we don't add even more pollution to global namespace.
#undef CC_ENABLE_IF_LIST_OR_ELEMENT
#undef CC_ENABLE_IF_LIST
#undef CC_ENABLE_IF_ELEMENT
#undef CC_IS_LIST
#undef CC_IS_ELEMENT
}