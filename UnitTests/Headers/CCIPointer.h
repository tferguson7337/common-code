#pragma once


namespace CC
{
    template <typename T>
    class [[nodiscard]] IPointer
    {
    protected:

        /// Protected Constructors \\\

        IPointer( ) noexcept = default;
        IPointer(const IPointer<T>&) noexcept = default;
        IPointer(IPointer<T>&&) noexcept = default;

        /// Protected Virtual Destructor \\\

        virtual ~IPointer( ) noexcept = default;

        /// Protected Assignment Overloads \\\

        virtual IPointer& operator=(const IPointer<T>&) noexcept = default;
        virtual IPointer& operator=(IPointer<T>&&) noexcept = default;

    public:

        /// Public Operator Overloads \\\

        // bool overload - returns true if pointer refers to valid object, false otherwise.
        [[nodiscard]] virtual explicit operator bool( ) const noexcept = 0;

        // T* overload - returns pointer to mutable internal pointer.
        [[nodiscard]] _Ret_maybenull_ virtual explicit operator T*() noexcept = 0;

        // const T* overload - returns pointer to immutable internal pointer.
        [[nodiscard]] _Ret_maybenull_ virtual explicit operator const T*() const noexcept = 0;

        // Dereference overload - returns mutable reference to first element from internal pointer.
        // Note: Excepted to throw exception if internal pointer is nullptr.
        [[nodiscard]] virtual T& operator*( ) = 0;

        // Dereference overload - returns immutable reference to first element from internal pointer.
        // Note: Excepted to throw exception if internal pointer is nullptr.
        [[nodiscard]] virtual const T& operator*( ) const = 0;

        // Pointer-member-access overload - returns reference to mutable internal pointer.
        // Note: Excepted to throw exception if internal pointer is nullptr.
        [[nodiscard]] virtual T* operator->( ) = 0;

        // Pointer-member-access overload - returns reference to immutable internal pointer.
        // Note: Excepted to throw exception if internal pointer is nullptr.
        [[nodiscard]] virtual const T* operator->( ) const = 0;

        /// Getters \\\

        // Returns pointer to mutable internal pointer.
        [[nodiscard]] _Ret_maybenull_ virtual T* Get( ) noexcept = 0;

        // Returns pointer to immutable internal pointer.
        [[nodiscard]] _Ret_maybenull_ virtual const T* Get( ) const noexcept = 0;

        // Returns length of elements pointed to by internal pointer.
        [[nodiscard]] virtual const size_t& Length( ) const noexcept = 0;

        // Returns size in bytes of data element(s) pointed to by internal pointer.
        [[nodiscard]] virtual const size_t Size( ) const noexcept = 0;

        /// Public Methods \\\

        // Resets internal data members to default values.
        // Note: This will not free any internal resources - use with caution.
        virtual void Reset( ) noexcept = 0;

        // Frees resources and resets data members to default values.
        virtual void Free( ) noexcept = 0;
    };
}