#pragma once

#include <CCMacros.h>
#include <CCPointerHelper.h>


namespace CC
{
    // Local macro helpers for targeted universal forwarders.
#define CC_IS_ELEMENT(_T, _U)\
        (std::is_same_v<std::decay_t<_U>, _T> ||\
        std::is_convertible_v<std::decay_t<_U>, _T>)

#define CC_IS_FWD_LIST(_T, _U)\
        (std::is_same_v<std::decay_t<_U>, ForwardList<_T>>)

#define CC_ENABLE_IF_ELEMENT(_T, _U)\
        typename = typename std::enable_if_t<CC_IS_ELEMENT(_T, _U)>

#define CC_ENABLE_IF_FWD_LIST(_T, _U)\
        typename = typename std::enable_if_t<CC_IS_FWD_LIST(_T, _U)>

#define CC_ENABLE_IF_FWD_LIST_OR_ELEMENT(_T, _U)    \
        typename = typename std::enable_if_t<CC_IS_FWD_LIST(_T, _U) || CC_IS_ELEMENT(_T, _U)>

    template <typename T, CC_ENABLE_IF_NOT_POINTER_TYPE(T)>
    class [[nodiscard]] ForwardList
    {
        // Testing classes.
        friend class ForwardListTests;
        friend class StackTests;
        friend class QueueTests;

    private:

        // Helper Struct - Singly-Linked List Node //

        struct Node
        {
            T data;
            Node* pNext;

            // Default constructor.
            Node() noexcept(CC_IS_NOTHROW_CTOR_DEFAULT(T)) :
                data(T()),
                pNext(nullptr)
            { }

            // Constructor, targeted universal forwarder.
            template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
            Node(_In_ U&& obj, _In_opt_ Node* pN = nullptr) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T)) :
                data(std::forward<U>(obj)),
                pNext(pN)
            { }

            ~Node() noexcept(CC_IS_NOTHROW_DTOR(T)) = default;
        };

        using PH = PointerHelper<Node>;


        // Private Data Members //

        Node* m_pHead;
        Node* m_pTail;
        size_t m_Len;

        // Private Throwing Validators //

        // Throws std::logic_error if this list is empty.
        inline void ValidateDereferenceT(_In_z_ const char* const f)
        {
            if (IsEmpty())
            {
                std::string msg(": Attempted invalid access on empty list.");
                throw std::logic_error(f + msg);
            }
        }

        // Throws std::logic_error if this list is empty.
        // Throws std::out_of_range if pos does not refer to a valid position in the list.
        inline void ValidateAccessAtPositionT(_In_z_ const char* const f, _In_ const size_t pos)
        {
            ValidateDereferenceT(f);
            if (pos >= m_Len)
            {
                std::string msg1(": Attempted to access out-of-range list position[");
                std::string dat1(std::to_string(pos));
                std::string msg2("]  Valid Range[0, ");
                std::string dat2(std::to_string(m_Len - 1));
                std::string msg3("].");
                throw std::out_of_range(f + msg1 + dat1 + msg2 + dat2 + msg3);
            }
        }

        // Private Helper Methods //

        // Frees all list nodes.
        // Note: Does not reset all data members to default values.
        inline void DestroyList() noexcept
        {
            while (!!m_pHead)
            {
                Node* p = m_pHead;
                m_pHead = m_pHead->pNext;
                delete p;
            }
        }

        // Returns node located at the specified position.
        // Note: Caller is expected to verify that specified position is valid.
        [[nodiscard]] _Ret_notnull_ Node* GetNodeAtPosition(_In_ const size_t pos) noexcept
        {
            Node* p = m_pHead;
            for (size_t i = 0; i < pos; i++)
            {
                p = p->pNext;
            }

            return p;
        }

        // Appends copy of src list to this list.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool AppendListObj(_In_ const ForwardList<T>& src) noexcept(CC_IS_NOTHROW_CTOR_COPY(T))
        {
            // Copy list.
            ForwardList<T> tmp(src);

            if (src.IsEmpty())
            {
                // Nothing to append if src list is empty.
                return true;
            }

            // Ensure copy was successful, move-append tmp to this list.
            return !!tmp && AppendListObj(std::move(tmp));
        }

        // Transfers src list elements to this list via node links and reseting src's data members.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool AppendListObj(_Inout_ ForwardList<T>&& src) noexcept
        {
            if (IsEmpty())
            {
                // Append to empty list is same as assign.
                return AssignListObj(std::move(src));
            }

            // Adjust tail and length.
            m_pTail->pNext = src.m_pHead;
            m_pTail = src.m_pTail;
            m_Len += src.m_Len;

            // this list now owns the resources, revoke src's ownership.
            src.m_pHead = nullptr;
            src.m_pTail = nullptr;
            src.m_Len = 0;

            return true;
        }

        // Append-element forwarder.
        // Appends an element to the end of this list via copy or move.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool AppendElementObj(_In_ U&& elem) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            Node* pNewElem = nullptr;

            if (IsEmpty())
            {
                // Append to empty list is same as assign.
                return AssignElementObj(std::forward<U>(elem));
            }

            pNewElem = PH::Allocate(std::forward<U>(elem));
            if (!pNewElem)
            {
                // Allocation failed.
                return false;
            }

            // Update tail and length.
            m_pTail->pNext = pNewElem;
            m_pTail = pNewElem;
            m_Len++;

            return true;
        }

        // Copies src list and assigns the copy to this list.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool AssignListObj(_In_ const ForwardList<T>& src) noexcept(CC_IS_NOTHROW_CTOR_COPY(T))
        {
            Node* pSrc = src.m_pHead;
            ForwardList<T> tmp;

            if (src.IsEmpty())
            {
                // Assigning empty list - clear out this list.
                Clear();
                return true;
            }

            // Copy first element to tmp list.
            tmp.m_pHead = tmp.m_pTail = PH::Allocate(pSrc->data);
            if (!tmp.m_pHead)
            {
                return false;
            }

            // Append the rest of the elements to tmp list.
            pSrc = pSrc->pNext;
            while (!!pSrc)
            {
                Node* pNewElem = PH::Allocate(pSrc->data);
                if (!pNewElem)
                {
                    return false;
                }

                // Update tail.
                tmp.m_pTail->pNext = pNewElem;
                tmp.m_pTail = pNewElem;

                pSrc = pSrc->pNext;
            }

            // Copy the length.
            tmp.m_Len = src.m_Len;

            // Move tmp list to this list.
            return AssignListObj(std::move(tmp));
        }

        // Clears this list and transfers src list contents to this list.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool AssignListObj(_Inout_ ForwardList<T>&& src) noexcept
        {
            // Destroy current list.
            DestroyList();

            // Transfer elements to this list, copy length.
            m_pHead = src.m_pHead;
            m_pTail = src.m_pTail;
            m_Len = src.m_Len;

            // src doesn't own this list anymore - reset it.
            src.m_pHead = nullptr;
            src.m_pTail = nullptr;
            src.m_Len = 0;

            return true;
        }

        // Clears this list and assigns single element to this list via copy or move.
        // Returns true if operation succeeds, false otherwise.
        template <typename U>
        [[nodiscard]] _Success_(return) bool AssignElementObj(_In_ U&& elem) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            // Allocate new list element.
            Node* p = PH::Allocate(std::forward<U>(elem));
            if (!p)
            {
                return false;
            }

            // Destroy current list.
            DestroyList();

            // Assign element to list, adjust length.
            m_pHead = m_pTail = p;
            m_Len = 1;

            return true;
        }

        // Inserts copy of src list into this list at specified position.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool InsertListObj(_In_ const ForwardList<T>& src, _In_ const size_t pos) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            // Copy list.
            ForwardList<T> tmp(src);

            if (src.IsEmpty())
            {
                // Nothing to insert if src list is empty.
                return true;
            }

            // Ensure copy was successful, move-insert tmp to this list.
            return !!tmp && InsertListObj(std::move(tmp), pos);
        }

        // Transfers src list into this list at specified position.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool InsertListObj(_Inout_ ForwardList<T>&& src, _In_ const size_t pos) noexcept
        {
            Node* p = nullptr;

            if (IsEmpty())
            {
                // Insert to empty list is same as assign.
                return AssignListObj(std::move(src));
            }

            if (pos == 0)
            {
                // Insert at front of list is same as prepend.
                return PrependListObj(std::move(src));
            }

            if (pos >= (m_Len - 1))
            {
                // Insert at back of list is same as append.
                return AppendListObj(std::move(src));
            }

            // Get target node.
            p = GetNodeAtPosition(pos - 1);

            // Link with src's head and tail, adjust length.
            src.m_pTail->pNext = p->pNext;
            p->pNext = src.m_pHead;
            m_Len += src.m_Len;

            // Pointers transfered, reset src.
            src.m_pHead = nullptr;
            src.m_pTail = nullptr;
            src.m_Len = 0;

            return true;
        }

        // Inserts element into this list at specified position via copy or move.
        // Returns true if operation succeeds, false otherwise.
        template <typename U>
        [[nodiscard]] _Success_(return) bool InsertElementObj(_In_ U&& elem, _In_ const size_t pos) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            Node* pNewElem = nullptr;
            Node* p = nullptr;

            if (IsEmpty())
            {
                // Insert to empty list is same as assign.
                return AssignElementObj(std::forward<U>(elem));
            }

            if (pos == 0)
            {
                // Insert at front of list is same as prepend.
                return PrependElementObj(std::forward<U>(elem));
            }

            if (pos >= m_Len)
            {
                // Insert at back of list is same as append.
                return AppendElementObj(std::forward<U>(elem));
            }

            p = GetNodeAtPosition(pos - 1);
            pNewElem = PH::Allocate(std::forward<U>(elem));
            if (!!pNewElem)
            {
                // Link new element node into list.
                pNewElem->pNext = p->pNext;
                p->pNext = pNewElem;
                m_Len++;

                return true;
            }

            return false;
        }

        // Adds copy of src list to the front of this list.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool PrependListObj(_In_ const ForwardList<T>& src) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            // Copy list.
            ForwardList<T> tmp(src);

            if (src.IsEmpty())
            {
                // Nothing to prepend if src list is empty.
                return true;
            }

            return !!tmp && PrependListObj(std::move(tmp));
        }

        // Transfers src list to the front of this list.
        // Returns true if operation succeeds, false otherwise.
        [[nodiscard]] _Success_(return) bool PrependListObj(_Inout_ ForwardList<T>&& src) noexcept
        {
            if (IsEmpty())
            {
                // Prepend on empty list is same as assign.
                return AssignListObj(std::move(src));
            }

            // Link nodes, adjust length.
            src.m_pTail->pNext = m_pHead;
            m_pHead = src.m_pHead;
            m_Len += src.m_Len;

            src.m_pHead = nullptr;
            src.m_pTail = nullptr;
            src.m_Len = 0;

            return true;
        }

        // Adds new element to the front of the list via copy or move.
        template <typename U>
        [[nodiscard]] _Success_(return) bool PrependElementObj(_In_ U&& elem)  noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            Node* pNewElem = nullptr;

            if (IsEmpty())
            {
                // Prepend to empty list is same as assign.
                return AssignElementObj(std::forward<U>(elem));
            }

            pNewElem = PH::Allocate(std::forward<U>(elem));
            if (!pNewElem)
            {
                return false;
            }

            // Link new element to head, update head, and increment length.
            pNewElem->pNext = m_pHead;
            m_pHead = pNewElem;
            m_Len++;

            return true;
        }

    public:

        // Constructors //

        // Default constructor
        ForwardList() noexcept :
            m_pHead(nullptr),
            m_pTail(nullptr),
            m_Len(0)
        { }

        // Copy constructor
        ForwardList(_In_ const ForwardList<T>& src) noexcept(CC_IS_NOTHROW_CTOR(T)) :
            ForwardList()
        {
            *this = src;
        }

        // Move constructor
        ForwardList(_Inout_ ForwardList<T>&& src) noexcept :
            ForwardList()
        {
            *this = std::move(src);
        }

        // Forwarding constructor
        // Can handle single element copy/move.
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        ForwardList(_In_ U&& src) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T)) :
            ForwardList<T>()
        {
            *this = std::forward<U>(src);
        }


        // Destructor //

        ~ForwardList() noexcept(CC_IS_NOTHROW_DTOR(T))
        {
            DestroyList();
        }


        // Assignment Overloads //

        // Copy assignment
        ForwardList<T>& operator=(_In_ const ForwardList<T>& src) noexcept(CC_IS_NOTHROW_COPY(T))
        {
            if (!Assign(src))
            {
                Clear();
            }

            return *this;
        }

        // Move assignment
        ForwardList<T>& operator=(_Inout_ ForwardList<T>&& src) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            if (!Assign(std::move(src)))
            {
                Clear();
            }

            return *this;
        }

        // Targeted Universal Forwarder to Assign
        template <typename U, CC_ENABLE_IF_ELEMENT(T, U)>
        ForwardList<T>& operator=(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if (!Assign(std::forward<U>(obj)))
            {
                Clear();
            }

            return *this;
        }


        // Operator Overloads //

        // Returns true if list is not empty, false otherwise.
        [[nodiscard]] explicit operator bool() const noexcept
        {
            return !IsEmpty();
        }

        // Compares this list's elements against rhs' list elements.
        // Returns true if lists are same length all elements match, false otherwise.
        [[nodiscard]] bool operator==(_In_ const ForwardList<T>& rhs) const noexcept
        {
            return Compare(rhs);
        }

        // Targeted Universal Forwarder to Append
        template <typename U, CC_ENABLE_IF_FWD_LIST_OR_ELEMENT(T, U)>
        ForwardList<T>& operator+=(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if (!Append(std::forward<U>(obj)))
            {
                Clear();
            }

            return *this;
        }

        // Getters //

        // Returns length of list (number of elements).
        [[nodiscard]] inline size_t Length() const noexcept
        {
            return m_Len;
        }

        // Returns mutable reference to first element in the list.
        // Note: If list is empty, this throws std::logic_error
        [[nodiscard]] inline T& Front()
        {
            ValidateDereferenceT(__PRETTY_FUNCTION__);
            return m_pHead->data;
        }

        // Returns immutable reference to first element in the list.
        // Note: If list is empty, this throws std::logic_error
        [[nodiscard]] inline const T& Front() const
        {
            ValidateDereferenceT(__PRETTY_FUNCTION__);
            return m_pHead->data;
        }

        // Returns mutable reference to first element in the list.
        // Note: If list is empty, this throws std::logic_error
        [[nodiscard]] inline T& Back()
        {
            ValidateDereferenceT(__PRETTY_FUNCTION__);
            return m_pTail->data;
        }

        // Returns immutable reference to first element in the list.
        // Note: If list is empty, this throws std::logic_error
        [[nodiscard]] inline const T& Back() const
        {
            ValidateDereferenceT(__PRETTY_FUNCTION__);
            return m_pTail->data;
        }

        // Returns mutable element at specified position.
        // Note: Throws std::logic_error if the list is empty.
        // Note: Throws std::out_of_range if pos refers to a non-existant element.
        [[nodiscard]] inline T& At(_In_ const size_t pos)
        {
            ValidateAccessAtPositionT(__PRETTY_FUNCTION__, pos);
            return GetNodeAtPosition(pos)->data;
        }

        // Returns immutable element at specified position.
        // Note: Throws std::logic_error if the list is empty.
        // Note: Throws std::out_of_range if pos refers to a non-existant element.
        [[nodiscard]] inline const T& At(_In_ const size_t pos) const
        {
            ValidateAccessAtPositionT(__PRETTY_FUNCTION__, pos);
            return GetNodeAtPosition(pos)->data;
        }


        // Public Methods //

        // Adds specified element or list to the end of this list via copy/move.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_FWD_LIST_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Append(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if (IsEmpty())
            {
                // Append on empty list is same as assign.
                return Assign(std::forward<U>(obj));
            }

            if constexpr (CC_IS_FWD_LIST(T, U))
            {
                return (this == std::addressof(obj)) ? AppendListObj(obj) : AppendListObj(std::forward<U>(obj));
            }
            else // if constexpr (CC_IS_ELEMENT(T, U))
            {
                return AppendElementObj(std::forward<U>(obj));
            }
        }

        // Clears previous list contents and assigns specified element or list object to this list.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_FWD_LIST_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Assign(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if constexpr (CC_IS_FWD_LIST(T, U))
            {
                return (this == std::addressof(obj)) || AssignListObj(std::forward<U>(obj));
            }
            else // if constexpr (CC_IS_ELEMENT(T, U))
            {
                return AssignElementObj(std::forward<U>(obj));
            }
        }

        // Destroys all list elements, setting this list back to default state.
        inline void Clear() noexcept
        {
            DestroyList();
            m_pHead = nullptr;
            m_pTail = nullptr;
            m_Len = 0;
        }

        // Compares this list's elements against src's list elements.
        // Returns true if lists are same length all elements match, false otherwise.
        [[nodiscard]] bool Compare(_In_ const ForwardList<T>& rhs) const noexcept
        {
            Node* pL = m_pHead;
            Node* pR = rhs.m_pHead;

            if (pL == pR)
            {
                // Comparing same list - equal.
                return true;
            }

            if (m_Len != rhs.m_Len)
            {
                // Length mismatch - not equal.
                return false;
            }

            // Compare while we haven't exhausted either list.
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

        // Inserts specified element/list into this list via copy/move at the specified position.
        // Returns true if operation succeeds, false otherwise.
        // Note: If pos exceeds list length, then element is appended to the end of the list.
        template <typename U, CC_ENABLE_IF_FWD_LIST_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool InsertAt(_In_ U&& obj, _In_ const size_t pos) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if (IsEmpty())
            {
                // Insert on empty list is same as assign.
                return Assign(std::forward<U>(obj));
            }

            if (pos == 0)
            {
                // Insert at first position is same as prepend.
                return Prepend(std::forward<U>(obj));
            }

            if (pos >= m_Len)
            {
                // Insert at last position (or beyond) is same as append.
                return Append(std::forward<U>(obj));
            }

            if constexpr (CC_IS_FWD_LIST(T, U))
            {
                return (this == std::addressof(obj)) ? InsertListObj(obj, pos) : InsertListObj(std::forward<U>(obj), pos);
            }
            else // if constexpr (CC_IS_ELEMENT(T, U))
            {
                return InsertElementObj(std::forward<U>(obj), pos);
            }
        }

        // Returns true if the list is empty, false otherwise.
        [[nodiscard]] inline bool IsEmpty() const noexcept
        {
            return (m_Len == 0);
        }

        // Adds element to the front of the list.
        // Returns true if operation succeeds, false otherwise.
        template <typename U, CC_ENABLE_IF_FWD_LIST_OR_ELEMENT(T, U)>
        [[nodiscard]] _Success_(return) bool Prepend(_In_ U&& obj) noexcept(CC_IS_NOTHROW_CTOR_COPY(T) && CC_IS_NOTHROW_CTOR_MOVE(T))
        {
            if (IsEmpty())
            {
                // Prepend on empty list is same as assign.
                return Assign(std::forward<U>(obj));
            }

            if constexpr (CC_IS_FWD_LIST(T, U))
            {
                return (this == std::addressof(obj)) ? PrependListObj(obj) : PrependListObj(std::forward<U>(obj));
            }
            else // if constexpr (CC_IS_ELEMENT(T, U))
            {
                return PrependElementObj(std::forward<U>(obj));
            }
        }

        // Removes last element from list.
        // Returns true if element is removed, false otherwise (e.g., list is empty).
        bool PopBack() noexcept
        {
            Node* p = nullptr;
            if (IsEmpty())
            {
                // List is empty - exit early.
                return false;
            }

            if (m_pHead == m_pTail)
            {
                // One element left - clean up.
                Clear();
                return true;
            }

            // Get second to last element.
            p = GetNodeAtPosition(m_Len - 2);

            // Destroy tail, adjust to previous element, remove dangling pointer.
            delete m_pTail;
            m_pTail = p;
            m_pTail->pNext = nullptr;

            // Adjust length.
            m_Len--;

            return true;
        }

        // Removes last element from list, moving element to argument.
        // Returns true if element is removed, false otherwise (e.g., list is empty).
        [[nodiscard]] _Success_(return) bool PopBack(_Out_ T& obj) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            if (IsEmpty())
            {
                // List is empty - exit early.
                return false;
            }

            obj = std::move(m_pTail->data);
            return PopBack();
        }

        // Removes first element from list.
        // Returns true if element is removed, false otherwise (e.g., list is empty).
        bool PopFront() noexcept
        {
            Node* p = m_pHead;
            if (!p)
            {
                // List must be empty - exit early.
                return false;
            }

            // Adjust pointers.
            m_pHead = m_pHead->pNext;
            if (!m_pHead)
            {
                // That was the last element - reset tail.
                m_pTail = nullptr;
            }

            // Adjust length.
            m_Len--;

            // Free memory.
            delete p;

            return true;
        }

        // Removes first element from list, moving element to argument.
        // Returns true if element is removed, false otherwise (e.g., list is empty).
        [[nodiscard]] _Success_(return) bool PopFront(_Out_ T& obj) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            if (IsEmpty())
            {
                // List is empty - exit early.
                return false;
            }

            obj = std::move(m_pHead->data);
            return PopFront();
        }

        // Removes specified element from list.
        // Note: If the list empty, then this does nothing.
        // Note: If the list is not empty and pos exceeds list length, then the last element is removed.
        bool RemoveAt(_In_ const size_t pos) noexcept
        {
            if (IsEmpty())
            {
                return false;
            }

            if (pos == 0)
            {
                return PopFront();
            }
            else if (pos >= (m_Len - 1))
            {
                return PopBack();
            }
            else
            {
                // Get node prior to delete.
                Node* p = GetNodeAtPosition(pos - 1);
                Node* pDel = p->pNext;

                // Adjust pointer and length.
                p->pNext = pDel->pNext;
                m_Len--;

                // Free memory.
                delete pDel;

                return true;
            }
        }

        // Removes specified element from list, moving the data to the obj argument.
        // Note: If the list empty, then this does nothing.
        // Note: If the list is not empty and pos exceeds list length, then the last element is removed.
        [[nodiscard]] _Success_(return) bool RemoveAt(_In_ const size_t pos, _Out_ T& obj) noexcept(CC_IS_NOTHROW_MOVE(T))
        {
            if (IsEmpty())
            {
                return false;
            }

            if (pos == 0)
            {
                return PopFront(obj);
            }
            else if (pos >= (m_Len - 1))
            {
                return PopBack(obj);
            }
            else
            {
                // Get node prior to delete.
                Node* p = GetNodeAtPosition(pos - 1);
                Node* pDel = p->pNext;

                // Adjust pointer and length.
                p->pNext = pDel->pNext;
                m_Len--;

                // Move data.
                obj = std::move(pDel->data);

                // Free memory.
                delete pDel;

                return true;
            }
        }
    };

    // Undefine local macro helpers so we don't add even more pollution to global namespace.
#undef CC_ENABLE_IF_FWD_LIST_OR_ELEMENT
#undef CC_ENABLE_IF_FWD_LIST
#undef CC_ENABLE_IF_ELEMENT
#undef CC_IS_FWD_LIST
#undef CC_IS_ELEMENT
}