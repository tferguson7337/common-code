#pragma once

#include <iterator>
#include <stdexcept>


namespace CC
{
    // Helper throw macros
#define CC_THROW_LOGIC_ERROR_IF(_pred, _str) if (_pred) throw std::logic_error(_str)
#define CC_THROW_OUT_OF_RANGE_IF(_pred, _str) if (_pred) throw std::out_of_range(_str)

    // Helper SFINAE macros
#define CC_ENABLE_IF_MUTABLE_ITERATOR typename = typename std::enable_if_t<!std::is_const_v<reference>>

    template <typename Class, typename Val, typename Diff, typename Ptr, typename Ref>
    class [[nodiscard]] ArrayIterator
    {
    public:

        /// Standard Iterator Type Aliases \\\

        using iterator_category = std::random_access_iterator_tag;
        using value_type = Val;
        using difference_type = Diff;
        using pointer = Ptr;
        using reference = Ref;

    protected:

        /// Protected Data Members \\\

        Class* m_pBackingContainer;
        Ptr m_pElem;


        /// Protected Validation Helper Methods \\\

        [[nodiscard]] constexpr ArrayIterator GetBackingBegin() const noexcept
        {
            return m_pBackingContainer->begin();
        }

        [[nodiscard]] constexpr ArrayIterator GetBackingEnd() const noexcept
        {
            return m_pBackingContainer->end();
        }

        [[nodiscard]] constexpr bool IsValid() const noexcept
        {
            return (!!m_pBackingContainer && !!m_pElem);
        }

        [[nodiscard]] constexpr bool IsBegin() const noexcept
        {
            return (*this == GetBackingBegin());
        }

        [[nodiscard]] constexpr bool IsEnd() const noexcept
        {
            return (*this == GetBackingEnd());
        }

        [[nodiscard]] constexpr bool SameBackingContainer(_In_ const ArrayIterator& other) const noexcept
        {
            return (m_pBackingContainer == other.m_pBackingContainer);
        }

        [[nodiscard]] constexpr bool SameElement(_In_ const ArrayIterator& other) const noexcept
        {
            return (m_pElem == other.m_pElem);
        }

        [[nodiscard]] constexpr bool IsOutOfRange() const noexcept
        {
            return (*this < GetBackingBegin()) || (*this > GetBackingEnd());
        }

        [[nodiscard]] constexpr bool IsOffsetOutOfRange(_In_ const Diff d) const noexcept
        {
            return ((m_pElem + d) < GetBackingBegin().m_pElem) || ((m_pElem + d) > GetBackingEnd().m_pElem);
        }

    public:

        /// Constructors \\\

        constexpr ArrayIterator() noexcept :
            m_pBackingContainer(nullptr),
            m_pElem(nullptr)
        { }

        constexpr ArrayIterator(_In_ Class* pBackingContainer, _In_ Ptr pElem) :
            m_pBackingContainer(pBackingContainer),
            m_pElem(pElem)
        {
            CC_THROW_LOGIC_ERROR_IF(!IsValid(), __FUNCSIG__ ": Attempted to create invalid iterator.");
        }

        constexpr ArrayIterator(_In_ const ArrayIterator& src) noexcept :
            ArrayIterator()
        {
            *this = src;
        }

        constexpr ArrayIterator(_Inout_ ArrayIterator&& src) noexcept :
            ArrayIterator()
        {
            *this = std::move(src);
        }


        /// Copy/Move Assignment \\\

        constexpr ArrayIterator& operator=(_In_ const ArrayIterator& src) noexcept
        {
            if (this != &src)
            {
                m_pBackingContainer = src.m_pBackingContainer;
                m_pElem = src.m_pElem;
            }

            return *this;
        }

        constexpr ArrayIterator& operator=(_Inout_ ArrayIterator&& src) noexcept
        {
            if (this != &src)
            {
                m_pBackingContainer = src.m_pBackingContainer;
                m_pElem = src.m_pElem;

                src.m_pBackingContainer = nullptr;
                src.m_pElem = nullptr;
            }

            return *this;
        }


        /// Standard Iterator Operators \\\

        // Accessor Operators

        [[nodiscard]] constexpr Ref operator*()
        {
            return *m_pElem;
        }

        [[nodiscard]] constexpr Ptr operator->()
        {
            return m_pElem;
        }

        [[nodiscard]] constexpr Val& operator[](_In_ const Diff d)
        {
            CC_THROW_OUT_OF_RANGE_IF(IsOffsetOutOfRange(d), __FUNCSIG__ ": Attempted to access out-of-range iterator.");
            return m_pElem[d];
        }


        // Traversal Operators

        constexpr ArrayIterator& operator++()
        {
            CC_THROW_OUT_OF_RANGE_IF(IsEnd(), __FUNCSIG__ ": Attempted to increment end iterator.");
            ++m_pElem;
            return *this;
        }

        constexpr ArrayIterator operator++(int)
        {
            constexpr ArrayIterator old(*this);
            ++(*this);
            return old;
        }


        constexpr ArrayIterator& operator--()
        {
            CC_THROW_OUT_OF_RANGE_IF(IsBegin(), __FUNCSIG__ ": Attempted to decrement beginning iterator.");
            --m_pElem;
            return *this;
        }

        constexpr ArrayIterator operator--(int)
        {
            constexpr ArrayIterator old(*this);
            --(*this);
            return old;
        }


        constexpr ArrayIterator& operator+=(_In_ const Diff d)
        {
            constexpr ArrayIterator itr(*this + d);
            CC_THROW_OUT_OF_RANGE_IF(itr.IsOutOfRange(), __FUNCSIG__ ": Attempted to create out-of-range iterator.");
            *this = itr;
            return *this;
        }

        constexpr ArrayIterator& operator-=(_In_ const Diff d)
        {
            constexpr ArrayIterator itr(*this - d);
            CC_THROW_OUT_OF_RANGE_IF(itr.IsOutOfRange(), __FUNCSIG__ ": Attempted to create out-of-range iterator.");
            *this = itr;
            return *this;
        }


        [[nodiscard]] constexpr ArrayIterator operator+(_In_ const Diff d) const
        {
            constexpr ArrayIterator itr(m_pBackingContainer, m_pElem + d);
            CC_THROW_OUT_OF_RANGE_IF(itr.IsOutOfRange(), __FUNCSIG__ ": Attempted to create out-of-range iterator.");
            return itr;
        }

        [[nodiscard]] constexpr friend ArrayIterator operator+(_In_ const Diff d, _In_ const ArrayIterator& itr)
        {
            return itr + d;
        }


        [[nodiscard]] constexpr ArrayIterator operator-(_In_ const Diff d) const
        {
            constexpr ArrayIterator itr(m_pBackingContainer, m_pElem - d);
            CC_THROW_OUT_OF_RANGE_IF(itr.IsOutOfRange(), __FUNCSIG__ ": Attempted to create out-of-range iterator.");
            return itr;
        }

        [[nodiscard]] constexpr Diff operator-(_In_ const ArrayIterator& rhs) const
        {
            CC_THROW_LOGIC_ERROR_IF(!SameBackingContainer(rhs), __FUNCSIG__ ": Attempted to find distance between iterators from differing container objects.");
            return (static_cast<Diff>(m_pElem) - static_cast<Diff>(rhs.m_pElem)) / sizeof(Val);
        }


        // Comparison Operators

        [[nodiscard]] constexpr bool operator==(_In_ const ArrayIterator& rhs) const
        {
            CC_THROW_LOGIC_ERROR_IF(!SameBackingContainer(rhs), __FUNCSIG__ ": Attempted to compare iterators from differing container objects.");
            return SameElement(rhs);
        }

        [[nodiscard]] constexpr bool operator!=(_In_ const ArrayIterator& rhs) const
        {
            return !(*this == rhs);
        }

        [[nodiscard]] constexpr bool operator<(_In_ const ArrayIterator& rhs) const
        {
            CC_THROW_LOGIC_ERROR_IF(!SameBackingContainer(rhs), __FUNCSIG__ ": Attempted to compare iterators from differing container objects.");
            return m_pElem < rhs.m_pElem;
        }

        [[nodiscard]] constexpr bool operator>(_In_ const ArrayIterator& rhs) const
        {
            CC_THROW_LOGIC_ERROR_IF(!SameBackingContainer(rhs), __FUNCSIG__ ": Attempted to compare iterators from differing container objects.");
            return m_pElem > rhs.m_pElem;
        }

        [[nodiscard]] constexpr bool operator<=(_In_ const ArrayIterator& rhs) const
        {
            return !(*this > rhs);
        }

        [[nodiscard]] constexpr bool operator>=(_In_ const ArrayIterator& rhs) const
        {
            return !(*this < rhs);
        }
    };


    /// TODO: Make common iterator base class that can support node-based iteration.
    /// class [[nodiscard]] NodeIterator { ... };


#undef CC_ENABLE_IF_MUTABLE_ITERATOR
#undef CC_THROW_OUT_OF_RANGE_IF
#undef CC_THROW_LOGIC_ERROR_IF
}
