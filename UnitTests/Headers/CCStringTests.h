#pragma once

// SUTL
#include <UnitTestResult.h>

// STL
#include <functional>
#include <list>
#include <vector>

// Target Class
#include <CCString.h>

// Test Helper Utils
#include <CCPointerCommonHelpersTests.h>


namespace CC
{
    class StringTests
    {
        StringTests( ) = delete;
        StringTests(const StringTests&) = delete;
        StringTests(StringTests&&) = delete;
        ~StringTests( ) = delete;
        StringTests& operator=(const StringTests&) = delete;
        StringTests& operator=(StringTests&&) = delete;

    private:

        // Type aliases
        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;
        using UTList = std::list<UTFunc>;

        using TestQuantity = PointerCommonHelpersTests::TestQuantity;

        static const std::vector<SupportedStringTuple> ms_TestStrings;

        template <TestQuantity TQ>
        [[nodiscard]] static constexpr size_t GetTQNum( ) noexcept
        {
            if constexpr ( TQ == TestQuantity::Zero )
            {
                return 0;
            }
            else if constexpr ( TQ == TestQuantity::One )
            {
                return 1;
            }
            else if constexpr ( TQ == TestQuantity::Many )
            {
                return 64;
            }
            else
            {
                static_assert(false, __FUNCSIG__": Unknown TestQuantity value.");
            }
        }

        template <TestQuantity TQ>
        [[nodiscard]] static constexpr size_t TQToTestCStrIdx( ) noexcept
        {
            if constexpr ( TQ == TestQuantity::Zero )
            {
                return 0;
            }
            else if constexpr ( TQ == TestQuantity::One )
            {
                return 1;
            }
            else if constexpr ( TQ == TestQuantity::Many )
            {
                return 2;
            }
            else
            {
                static_assert(false, __FUNCSIG__": Unknown TestQuantity value.");
            }
        }

        template <typename T, TestQuantity TQ, CC_ENABLE_IF_SUPPORTED_CHARACTER_TYPE(T)>
        [[nodiscard]] static const T* GetTestCStr( ) noexcept
        {
            constexpr size_t idx = TQToTestCStrIdx<TQ>( );
            return std::get<std::basic_string<T>>(ms_TestStrings[idx]).c_str( );
        }

        /// Test Subclasses \\\

        // Tests constructor behavior.
        class ConstructorTests;

        // Tests overloaded assignment operator behavior.
        class AssignmentOperatorTests;

        // Tests overloaded add-assignment operator behavior.
        class AddAssignmentOperatorTests;

        // Tests overloaded subscript operator behavior.
        class SubscriptOperatorTests;

        // Tests overloaded equivalency operator behavior.
        class EquivalencyOperatorTests;

        // Tests behavior of the various Getter methods.
        class GetterTests;

        // Tests Assign behavior.
        class Assign;

        // Tests Append behavior.
        class Append;

        // Tests comparison behavior between strings.
        class CompareTests;

        // Tests Clear behavior.
        class ClearTests;

        // Tests IsEmpty behavior.
        class IsEmptyTests;

    public:

        // Returns list of String unit tests.
        [[nodiscard]] static UTList GetTests( );
    };


    // Tests constructor behavior.
    class StringTests::ConstructorTests
    {
    public:

        template <typename T>
        [[nodiscard]] static UTR DefaultCtor( )
        {
            String<T> str;

            SUTL_TEST_ASSERT(str.m_SSOPos == 0);
            SUTL_TEST_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
            SUTL_TEST_ASSERT(!str.m_DynBuf);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR LenAndCharCtor( )
        {
            constexpr size_t len = GetTQNum<TQ>( );
            String<T> str(len, static_cast<T>('x'));

            if constexpr ( TQ != TestQuantity::Many )
            {
                SUTL_TEST_ASSERT(str.m_SSOPos == len);
                SUTL_TEST_ASSERT((str.m_SSOArr[0] == static_cast<T>('x')) == (TQ == TestQuantity::One));
                SUTL_TEST_ASSERT(str.m_SSOArr[len] == String<T>::ms_NullTerminator);
                SUTL_TEST_ASSERT(!str.m_DynBuf);
            }
            else
            {
                SUTL_TEST_ASSERT(str.m_SSOPos == 0);
                SUTL_TEST_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_TEST_ASSERT(!!str.m_DynBuf);
                SUTL_TEST_ASSERT(str.m_DynBuf.Length( ) == (len + 1));
                SUTL_TEST_ASSERT(str.m_DynBuf.WritePosition( ) == len);
                for ( size_t i = 0; i < len; i++ )
                {
                    SUTL_TEST_ASSERT(str.m_DynBuf.Get( )[i] == T('x'));
                }

                SUTL_TEST_ASSERT(str.m_DynBuf.Get( )[len] == String<T>::ms_NullTerminator);
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T>
        [[nodiscard]] static UTR CStrCtorNullptr( )
        {
            String<T> str(nullptr);

            SUTL_TEST_ASSERT(str.m_SSOPos == 0);
            SUTL_TEST_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
            SUTL_TEST_ASSERT(!str.m_DynBuf);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CStrCtor( )
        {
            const T* const testCStr = GetTestCStr<T, TQ>( );
            constexpr size_t len = GetTQNum<TQ>( );
            String<T> str(testCStr);

            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(str.m_SSOPos == 0);
                SUTL_TEST_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_TEST_ASSERT(!str.m_DynBuf);
            }
            else if constexpr ( TQ == TestQuantity::One )
            {
                SUTL_TEST_ASSERT(str.m_SSOPos == 1);
                SUTL_TEST_ASSERT(memcmp(str.m_SSOArr, testCStr, sizeof(T) * len) == 0);
                SUTL_TEST_ASSERT(!str.m_DynBuf);
            }
            else
            {
                SUTL_TEST_ASSERT(str.m_SSOPos == 0);
                SUTL_TEST_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_TEST_ASSERT(!!str.m_DynBuf);
                SUTL_TEST_ASSERT(str.m_DynBuf.Length( ) == (len + 1));
                SUTL_TEST_ASSERT(str.m_DynBuf.WritePosition( ) == len);
                SUTL_TEST_ASSERT(memcmp(str.m_DynBuf.Get( ), testCStr, sizeof(T) * (len + 1)) == 0);
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR PtrAndLenCtorNullptr( )
        {
            constexpr size_t len = GetTQNum<TQ>( );
            String<T> str(nullptr, len);

            SUTL_TEST_ASSERT(str.m_SSOPos == 0);
            SUTL_TEST_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
            SUTL_TEST_ASSERT(!str.m_DynBuf);

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR PtrAndLenCtor( )
        {
            const T* const testCStr = GetTestCStr<T, TQ>( );
            constexpr size_t len = GetTQNum<TQ>( ) >> ((TQ == TestQuantity::Many) ? 1 : 0);
            String<T> str(testCStr, len);

            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(str.m_SSOPos == 0);
                SUTL_TEST_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_TEST_ASSERT(!str.m_DynBuf);
            }
            else if constexpr ( TQ == TestQuantity::One )
            {
                SUTL_TEST_ASSERT(str.m_SSOPos == 1);
                SUTL_TEST_ASSERT(memcmp(str.m_SSOArr, testCStr, sizeof(T) * len) == 0);
                SUTL_TEST_ASSERT(!str.m_DynBuf);
            }
            else
            {
                SUTL_TEST_ASSERT(str.m_SSOPos == 0);
                SUTL_TEST_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_TEST_ASSERT(!!str.m_DynBuf);
                SUTL_TEST_ASSERT(str.m_DynBuf.Length( ) == (len + 1));
                SUTL_TEST_ASSERT(str.m_DynBuf.WritePosition( ) == len);
                SUTL_TEST_ASSERT(memcmp(str.m_DynBuf.Get( ), testCStr, sizeof(T) * len) == 0);
                SUTL_TEST_ASSERT(memcmp(str.m_DynBuf.Get( ), testCStr, sizeof(T) * (len + 1)) != 0);
                SUTL_TEST_ASSERT(str.m_DynBuf.Get( )[len] == String<T>::ms_NullTerminator);
            }

            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CopyCtor( )
        {
            const T* const testCStr = GetTestCStr<T, TQ>( );
            constexpr size_t len = GetTQNum<TQ>( ) >> ((TQ == TestQuantity::Many) ? 1 : 0);
            String<T> str(testCStr, len);

            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_SETUP_ASSERT(str.m_SSOPos == 0);
                SUTL_SETUP_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_SETUP_ASSERT(!str.m_DynBuf);
            }
            else if constexpr ( TQ == TestQuantity::One )
            {
                SUTL_SETUP_ASSERT(str.m_SSOPos == 1);
                SUTL_SETUP_ASSERT(memcmp(str.m_SSOArr, testCStr, sizeof(T) * len) == 0);
                SUTL_SETUP_ASSERT(!str.m_DynBuf);
            }
            else
            {
                SUTL_SETUP_ASSERT(str.m_SSOPos == 0);
                SUTL_SETUP_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_SETUP_ASSERT(!!str.m_DynBuf);
                SUTL_SETUP_ASSERT(str.m_DynBuf.Length( ) == (len + 1));
                SUTL_SETUP_ASSERT(str.m_DynBuf.WritePosition( ) == len);
                SUTL_SETUP_ASSERT(memcmp(str.m_DynBuf.Get( ), testCStr, sizeof(T) * len) == 0);
                SUTL_SETUP_ASSERT(memcmp(str.m_DynBuf.Get( ), testCStr, sizeof(T) * (len + 1)) != 0);
                SUTL_SETUP_ASSERT(str.m_DynBuf.Get( )[len] == String<T>::ms_NullTerminator);
            }

            String<T> cpyStr(str);

            // Compare with test string.
            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(cpyStr.m_SSOPos == 0);
                SUTL_TEST_ASSERT(cpyStr.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_TEST_ASSERT(!cpyStr.m_DynBuf);
            }
            else if constexpr ( TQ == TestQuantity::One )
            {
                SUTL_TEST_ASSERT(cpyStr.m_SSOPos == 1);
                SUTL_TEST_ASSERT(memcmp(cpyStr.m_SSOArr, testCStr, sizeof(T) * len) == 0);
                SUTL_TEST_ASSERT(!cpyStr.m_DynBuf);
            }
            else
            {
                SUTL_TEST_ASSERT(cpyStr.m_SSOPos == 0);
                SUTL_TEST_ASSERT(cpyStr.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_TEST_ASSERT(!!cpyStr.m_DynBuf);
                SUTL_TEST_ASSERT(cpyStr.m_DynBuf.Length( ) == (len + 1));
                SUTL_TEST_ASSERT(cpyStr.m_DynBuf.WritePosition( ) == len);
                SUTL_TEST_ASSERT(memcmp(cpyStr.m_DynBuf.Get( ), testCStr, sizeof(T) * len) == 0);
                SUTL_TEST_ASSERT(memcmp(cpyStr.m_DynBuf.Get( ), testCStr, sizeof(T) * (len + 1)) != 0);
                SUTL_TEST_ASSERT(cpyStr.m_DynBuf.Get( )[len] == String<T>::ms_NullTerminator);
            }

            // Compare with original String obj.
            SUTL_TEST_ASSERT(cpyStr.m_SSOPos == str.m_SSOPos);
            if constexpr ( TQ != TestQuantity::Many )
            {
                SUTL_TEST_ASSERT(memcmp(cpyStr.m_SSOArr, str.m_SSOArr, sizeof(T) * (len + 1)) == 0);
                SUTL_TEST_ASSERT(!cpyStr.m_DynBuf);
            }
            else
            {
                SUTL_TEST_ASSERT(cpyStr.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_TEST_ASSERT(memcmp(cpyStr.m_DynBuf.Get( ), str.m_DynBuf.Get( ), sizeof(T) * (len + 1)) == 0);
            }


            SUTL_TEST_SUCCESS( );
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR MoveCtor( )
        {
            const T* const testCStr = GetTestCStr<T, TQ>( );
            constexpr size_t len = GetTQNum<TQ>( ) >> ((TQ == TestQuantity::Many) ? 1 : 0);
            String<T> str(testCStr, len);

            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_SETUP_ASSERT(str.m_SSOPos == 0);
                SUTL_SETUP_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_SETUP_ASSERT(!str.m_DynBuf);
            }
            else if constexpr ( TQ == TestQuantity::One )
            {
                SUTL_SETUP_ASSERT(str.m_SSOPos == 1);
                SUTL_SETUP_ASSERT(memcmp(str.m_SSOArr, testCStr, sizeof(T) * len) == 0);
                SUTL_SETUP_ASSERT(!str.m_DynBuf);
            }
            else
            {
                SUTL_SETUP_ASSERT(str.m_SSOPos == 0);
                SUTL_SETUP_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_SETUP_ASSERT(!!str.m_DynBuf);
                SUTL_SETUP_ASSERT(str.m_DynBuf.Length( ) == (len + 1));
                SUTL_SETUP_ASSERT(str.m_DynBuf.WritePosition( ) == len);
                SUTL_SETUP_ASSERT(memcmp(str.m_DynBuf.Get( ), testCStr, sizeof(T) * len) == 0);
                SUTL_SETUP_ASSERT(memcmp(str.m_DynBuf.Get( ), testCStr, sizeof(T) * (len + 1)) != 0);
                SUTL_SETUP_ASSERT(str.m_DynBuf.Get( )[len] == String<T>::ms_NullTerminator);
            }

            String<T> cpyStr(std::move(str));

            // Compare with test string.
            if constexpr ( TQ == TestQuantity::Zero )
            {
                SUTL_TEST_ASSERT(cpyStr.m_SSOPos == 0);
                SUTL_TEST_ASSERT(cpyStr.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_TEST_ASSERT(!cpyStr.m_DynBuf);
            }
            else if constexpr ( TQ == TestQuantity::One )
            {
                SUTL_TEST_ASSERT(cpyStr.m_SSOPos == 1);
                SUTL_TEST_ASSERT(memcmp(cpyStr.m_SSOArr, testCStr, sizeof(T) * len) == 0);
                SUTL_TEST_ASSERT(!cpyStr.m_DynBuf);
            }
            else
            {
                SUTL_TEST_ASSERT(cpyStr.m_SSOPos == 0);
                SUTL_TEST_ASSERT(cpyStr.m_SSOArr[0] == String<T>::ms_NullTerminator);
                SUTL_TEST_ASSERT(!!cpyStr.m_DynBuf);
                SUTL_TEST_ASSERT(cpyStr.m_DynBuf.Length( ) == (len + 1));
                SUTL_TEST_ASSERT(cpyStr.m_DynBuf.WritePosition( ) == len);
                SUTL_TEST_ASSERT(memcmp(cpyStr.m_DynBuf.Get( ), testCStr, sizeof(T) * len) == 0);
                SUTL_TEST_ASSERT(memcmp(cpyStr.m_DynBuf.Get( ), testCStr, sizeof(T) * (len + 1)) != 0);
                SUTL_TEST_ASSERT(cpyStr.m_DynBuf.Get( )[len] == String<T>::ms_NullTerminator);
            }

            // Ensure original string was cleared.
            SUTL_TEST_ASSERT(str.m_SSOPos == 0);
            SUTL_TEST_ASSERT(str.m_SSOArr[0] == String<T>::ms_NullTerminator);
            SUTL_TEST_ASSERT(!str.m_DynBuf);

            SUTL_TEST_SUCCESS( );
        }
    };


    // Tests overloaded assignment operator behavior.
    class StringTests::AssignmentOperatorTests
    {
    public:

        template <typename T>
        [[nodiscard]] static UTR CStrAssignOpNullptr( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CStrAssignOp( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CopyAssignOp( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR MoveAssignOp( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }
    };


    // Tests overloaded add-assignment operator behavior.
    class StringTests::AddAssignmentOperatorTests
    {
    public:

        template <typename T>
        [[nodiscard]] static UTR CStrAddAssignOpNullptr( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CStrAddAssignOp( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR StrObjAddAssignOp( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }
    };


    // Tests overloaded subscript operator behavior.
    class StringTests::SubscriptOperatorTests
    {
    public:

        template <typename T, TestQuantity TQLen, TestQuantity TQPos>
        [[nodiscard]] static UTR SubscriptOp( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }
    };


    // Tests overloaded equivalency operator behavior.
    class StringTests::EquivalencyOperatorTests
    {
    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CStrEquivOp( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR StrObjEquivOp( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }
    };


    // Tests behavior of the various Getter methods.
    class StringTests::GetterTests
    {
    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CStr( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR Length( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR Capacity( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR Front( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR Back( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }
    };


    // Tests Assign behavior.
    class StringTests::Assign
    {
    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR ChAssign( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CStrAssign( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR PtrAndLenAssign( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CopyAssign( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR MoveAssign( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }
    };


    // Tests Append behavior.
    class StringTests::Append
    {
    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR ChAppend( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR CStrAppend( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR PtrAndLenAppend( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR StrObjAppend( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }
    };


    // Tests comparison behavior between strings.
    class StringTests::CompareTests
    {
    public:

        template <typename T, TestQuantity TQ1, TestQuantity TQ2, bool bCaseInsensitive>
        [[nodiscard]] static UTR CStrCompare( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2, bool bCaseInsensitive>
        [[nodiscard]] static UTR StrAndLenCompare( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }

        template <typename T, TestQuantity TQ1, TestQuantity TQ2, bool bCaseInsensitive>
        [[nodiscard]] static UTR StrObjCompare( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }
    };


    // Tests Clear behavior.
    class StringTests::ClearTests
    {
    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR Clear( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }
    };


    // Tests IsEmpty behavior.
    class StringTests::IsEmptyTests
    {
    public:

        template <typename T, TestQuantity TQ>
        [[nodiscard]] static UTR IsEmpty( )
        {


            SUTL_SKIP_TEST("Impl pending...");
        }
    };
}