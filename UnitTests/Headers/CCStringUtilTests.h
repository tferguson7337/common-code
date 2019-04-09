#pragma once

// SUTL
#include <UnitTestResult.h>

// STL
#include <array>
#include <functional>
#include <list>

#include <CCStringUtil.h>

#define _ASSIGN_TEST_STR_(_Var, _Type, _Str)    \
    if constexpr (std::is_same_v<_Type, char>)  \
        _Var = _Str;                            \
    else                                        \
        _Var = L ## _Str;


namespace CC
{
    class StringUtilTests
    {
        StringUtilTests( ) = delete;
        StringUtilTests(const StringUtilTests&) = delete;
        StringUtilTests(StringUtilTests&&) = delete;
        ~StringUtilTests( ) = delete;
        StringUtilTests& operator=(const StringUtilTests&) = delete;
        StringUtilTests& operator=(StringUtilTests&&) = delete;

        using UTR = UnitTestResult;
        using UTFunc = std::function<UTR(void)>;

        using OT = CC::StringUtil::OperationType;
        using OT_T = std::underlying_type_t<OT>;

        using EER = CC::StringUtil::EarlyExitResult;
        using EER_T = std::underlying_type_t<EER>;

        using RT = CC::StringUtil::ReturnType;
        using RT_T = std::underlying_type_t<RT>;

        using BT = CC::StringUtil::NumberConversion::Base;
        using BT_T = std::underlying_type_t<BT>;

        /// Test Helpers \\\

        template <RT RT, typename C>
        static const C* GetStringPointer(const void* obj)
        {
            if ( !obj )
            {
                throw std::invalid_argument(__FUNCTION__": Received nullptr string object argument.");
            }

            if constexpr ( RT == CC::StringUtil::ReturnType::SmartCString )
            {
                return reinterpret_cast<const std::unique_ptr<C[ ]>*>(obj)->get( );
            }
            else if constexpr ( RT == CC::StringUtil::ReturnType::StringObj )
            {
                return reinterpret_cast<const std::basic_string<C>*>(obj)->c_str( );
            }
            else
            {
                throw std::logic_error(
                    __FUNCTION__": Unknown return type[" + std::to_string(static_cast<size_t>(RT)) + "]."
                );
            }
        }

        /// Test Data Getters \\\

        template <typename C>
        using TestStr = std::basic_string<C>;

        template <typename C>
        using TestSetArray = std::array<TestStr<C>, 2>;

        template <typename C>
        static const TestStr<C>& GetZeroLengthStr( )
        {
            if constexpr ( std::is_same_v<C, char> )
            {
                static const std::basic_string<C> str("");
                return str;
            }
            else if constexpr ( std::is_same_v<C, wchar_t> )
            {
                static const std::basic_string<C> str(L"");
                return str;
            }
            else
            {
                throw std::logic_error(
                    __FUNCTION__": Unknown character type[" + std::string(typeid(C).name( )) + "]."
                );
            }
        }

        template <typename C>
        static const TestStr<C>& GetOneLengthStr( )
        {
            if constexpr ( std::is_same_v<C, char> )
            {
                static const std::basic_string<C> str("1");
                return str;
            }
            else if constexpr ( std::is_same_v<C, wchar_t> )
            {
                static const std::basic_string<C> str(L"1");
                return str;
            }
            else
            {
                throw std::logic_error(
                    __FUNCTION__": Unknown character type[" + std::string(typeid(C).name( )) + "]."
                );
            }
        }

        template <typename C>
        static const TestStr<C>& GetManyLengthStr( )
        {
            if constexpr ( std::is_same_v<C, char> )
            {
                static const std::basic_string<C> str("Test String");
                return str;
            }
            else if constexpr ( std::is_same_v<C, wchar_t> )
            {
                static const std::basic_string<C> str(L"Test String");
                return str;
            }
            else
            {
                throw std::logic_error(
                    __FUNCTION__": Unknown character type[" + std::string(typeid(C).name( )) + "]."
                );
            }
        }

        template <typename C>
        static const TestSetArray<C>& GetEmptyStrs( )
        {
            if constexpr ( std::is_same_v<C, char> )
            {
                static const TestSetArray<C> arr = { "", "" };
                return arr;
            }
            else if constexpr ( std::is_same_v<C, wchar_t> )
            {
                static const TestSetArray<C> arr = { L"", L"" };
                return arr;
            }
            else
            {
                throw std::logic_error(
                    __FUNCTION__": Unknown character type[" + std::string(typeid(C).name( )) + "]."
                );
            }
        }

        template <typename C>
        static const TestSetArray<C>& GetSameLengthStrs( )
        {
            if constexpr ( std::is_same_v<C, char> )
            {
                static const TestSetArray<C> arr = { "TestString1", "TestString2" };
                return arr;
            }
            else if constexpr ( std::is_same_v<C, wchar_t> )
            {
                static const TestSetArray<C> arr = { L"TestString1", L"TestString2" };
                return arr;
            }
            else
            {
                throw std::logic_error(
                    __FUNCTION__": Unknown character type[" + std::string(typeid(C).name( )) + "]."
                );
            }
        }

        template <typename C>
        static const TestSetArray<C>& GetSameStrs( )
        {
            if constexpr ( std::is_same_v<C, char> )
            {
                static const TestSetArray<C> arr = { "TestString", "TestString" };
                return arr;
            }
            else if constexpr ( std::is_same_v<C, wchar_t> )
            {
                static const TestSetArray<C> arr = { L"TestString", L"TestString" };
                return arr;
            }
            else
            {
                throw std::logic_error(
                    __FUNCTION__": Unknown character type[" + std::string(typeid(C).name( )) + "]."
                );
            }
        }

        template <typename C>
        static const TestSetArray<C>& GetDifferentLengthStrs( )
        {
            if constexpr ( std::is_same_v<C, char> )
            {
                static const TestSetArray<C> arr = { "TestString1", "Test String 2" };
                return arr;
            }
            else if constexpr ( std::is_same_v<C, wchar_t> )
            {
                static const TestSetArray<C> arr = { L"TestString1", L"Test String 2" };
                return arr;
            }
            else
            {
                throw std::logic_error(
                    __FUNCTION__": Unknown character type[" + std::string(typeid(C).name( )) + "]."
                );
            }
        }

        template <typename C>
        static const TestSetArray<C>& GetDifferentCaseStrs( )
        {
            if constexpr ( std::is_same_v<C, char> )
            {
                static const TestSetArray<C> arr = { "TestString", "TeStStRiNg" };
                return arr;
            }
            else if constexpr ( std::is_same_v<C, wchar_t> )
            {
                static const TestSetArray<C> arr = { L"TestString", L"TeStStRiNg" };
                return arr;
            }
            else
            {
                throw std::logic_error(
                    __FUNCTION__": Unknown character type[" + std::string(typeid(C).name( )) + "]."
                );
            }
        }

    public:
        // Return list of all StringUtil unit tests functions.
        static std::list<UTFunc> GetTests( );

        class EnumValidatorTests;
        class BuildStringTests;

        class GetLengthTests;

        class ComparisonEarlyExitTests;
        class ComparisonTests;

        class CopyEarlyExitTests;
        class CopyTests;

        class UTFConversionEarlyExitTests;
        class UTFConversionTests;

        class NumberConversionTests;
    };


    class StringUtilTests::EnumValidatorTests
    {
    public:
        static UTR InvalidOperationType( )
        {
            constexpr OT invalidOT1 = static_cast<OT>(static_cast<OT_T>(OT::Begin) - 1);
            constexpr OT invalidOT2 = OT::End;

            SUTL_TEST_ASSERT(!CC::StringUtil::IsValidOperationType<invalidOT1>( ));
            SUTL_TEST_ASSERT(!CC::StringUtil::IsValidOperationType<invalidOT2>( ));

            SUTL_TEST_SUCCESS( );
        }

        static UTR ValidOperationType( )
        {
            SUTL_TEST_ASSERT(CC::StringUtil::IsValidOperationType<OT::Comparison>( ));
            SUTL_TEST_ASSERT(CC::StringUtil::IsValidOperationType<OT::Copy>( ));
            SUTL_TEST_ASSERT(CC::StringUtil::IsValidOperationType<OT::UTFConversion>( ));
            SUTL_TEST_ASSERT(CC::StringUtil::IsValidOperationType<OT::NumberConversion>( ));

            SUTL_TEST_SUCCESS( );
        }

        static UTR InvalidEarlyExitResult( )
        {
            constexpr EER invalidEER1 = static_cast<EER>(static_cast<EER_T>(EER::Begin) - 1);
            constexpr EER invalidEER2 = EER::End;

            SUTL_TEST_ASSERT(!CC::StringUtil::IsValidEarlyExitResult<invalidEER1>( ));
            SUTL_TEST_ASSERT(!CC::StringUtil::IsValidEarlyExitResult<invalidEER2>( ));

            SUTL_TEST_SUCCESS( );
        }

        static UTR ValidEarlyExitResult( )
        {
            SUTL_TEST_ASSERT(CC::StringUtil::IsValidEarlyExitResult<EER::NoExit>( ));
            SUTL_TEST_ASSERT(CC::StringUtil::IsValidEarlyExitResult<EER::True>( ));
            SUTL_TEST_ASSERT(CC::StringUtil::IsValidEarlyExitResult<EER::False>( ));
            SUTL_TEST_ASSERT(CC::StringUtil::IsValidEarlyExitResult<EER::EmptyString>( ));
            SUTL_TEST_ASSERT(CC::StringUtil::IsValidEarlyExitResult<EER::ZeroedBuffer>( ));

            SUTL_TEST_SUCCESS( );
        }

        static UTR InvalidReturnType( )
        {
            constexpr RT invalidRT1 = static_cast<RT>(static_cast<RT_T>(RT::_Begin) - 1);
            constexpr RT invalidRT2 = RT::_End;

            SUTL_TEST_ASSERT(!CC::StringUtil::IsValidReturnType<invalidRT1>( ));
            SUTL_TEST_ASSERT(!CC::StringUtil::IsValidReturnType<invalidRT2>( ));

            SUTL_TEST_SUCCESS( );
        }

        static UTR ValidReturnType( )
        {
            SUTL_TEST_ASSERT(CC::StringUtil::IsValidReturnType<RT::SmartCString>( ));
            SUTL_TEST_ASSERT(CC::StringUtil::IsValidReturnType<RT::StringObj>( ));

            SUTL_TEST_SUCCESS( );
        }

        static UTR InvalidBaseType( )
        {
            constexpr BT invalidBT1 = static_cast<BT>(static_cast<BT_T>(BT::_Begin) - 1);
            constexpr BT invalidBT2 = BT::_End;

            SUTL_TEST_ASSERT(!CC::StringUtil::NumberConversion::IsValidBaseType<invalidBT1>( ));
            SUTL_TEST_ASSERT(!CC::StringUtil::NumberConversion::IsValidBaseType<invalidBT2>( ));

            SUTL_TEST_SUCCESS( );
        }

        static UTR ValidBaseType( )
        {
            SUTL_TEST_ASSERT(CC::StringUtil::NumberConversion::IsValidBaseType<BT::Binary>( ));
            SUTL_TEST_ASSERT(CC::StringUtil::NumberConversion::IsValidBaseType<BT::Octal>( ));
            SUTL_TEST_ASSERT(CC::StringUtil::NumberConversion::IsValidBaseType<BT::Decimal>( ));
            SUTL_TEST_ASSERT(CC::StringUtil::NumberConversion::IsValidBaseType<BT::Hexadecimal>( ));

            SUTL_TEST_SUCCESS( );
        }
    };


    class StringUtilTests::BuildStringTests
    {
    public:
        template <RT RT, typename C>
        static UTR ZeroLength( )
        {
            const auto str = CC::StringUtil::BuildString<RT, C>(0);
            const C* p = GetStringPointer<RT, C>(&str);

            if constexpr ( RT == RT::StringObj )
            {
                SUTL_TEST_ASSERT(str.empty( ));
            }

            SUTL_TEST_ASSERT(p);
            SUTL_TEST_ASSERT(p[0] == C('\0'));

            SUTL_TEST_SUCCESS( );
        }


        template <CC::StringUtil::ReturnType RT, typename C>
        static UTR OneLength( )
        {
            const auto str = CC::StringUtil::BuildString<RT, C>(1);
            const C* p = GetStringPointer<RT, C>(&str);

            if constexpr ( RT == RT::StringObj )
            {
                SUTL_TEST_ASSERT(str.length( ) == 1);
            }

            SUTL_TEST_ASSERT(p);
            SUTL_TEST_ASSERT(p[0] == C('\0'));
            SUTL_TEST_ASSERT(p[1] == C('\0'));

            SUTL_TEST_SUCCESS( );
        }


        template <CC::StringUtil::ReturnType RT, typename C>
        static UTR ManyLength( )
        {
            const size_t len = 64;
            const C cmpArr[len + 1] = { C('\0') };
            const auto str = CC::StringUtil::BuildString<RT, C>(len);
            const C* p = GetStringPointer<RT, C>(&str);

            if constexpr ( RT == RT::StringObj )
            {
                SUTL_TEST_ASSERT(str.length( ) == len);
            }

            SUTL_TEST_ASSERT(p);
            SUTL_TEST_ASSERT(memcmp(p, cmpArr, (len + 1) * sizeof(C)) == 0);

            SUTL_TEST_SUCCESS( );
        }
    };


    class StringUtilTests::GetLengthTests
    {
    public:
        template <typename C>
        static UTR NullptrArg( )
        {
            SUTL_TEST_ASSERT(CC::StringUtil::GetLength<C>(nullptr) == 0);
            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR ZeroLength( )
        {
            const TestStr<C>& str = GetZeroLengthStr<C>( );
            SUTL_TEST_ASSERT(CC::StringUtil::GetLength<C>(str.c_str( )) == str.length( ));
            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR OneLength( )
        {
            const TestStr<C>& str = GetOneLengthStr<C>( );
            SUTL_TEST_ASSERT(CC::StringUtil::GetLength<C>(str.c_str( )) == str.length( ));
            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR ManyLength( )
        {
            const TestStr<C>& str = GetManyLengthStr<C>( );
            SUTL_TEST_ASSERT(CC::StringUtil::GetLength<C>(str.c_str( )) == str.length( ));
            SUTL_TEST_SUCCESS( );
        }
    };


    class StringUtilTests::ComparisonEarlyExitTests
    {
    public:
        template <typename C>
        static UTR SameLengthDifferentStrs( )
        {
            const TestSetArray<C>& arr = GetSameLengthStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            const EER eer = CC::StringUtil::CheckForComparisonEarlyExit<C>(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ));

            SUTL_TEST_ASSERT(eer == EER::NoExit);

            SUTL_TEST_SUCCESS( );
        }

        template <typename C>
        static UTR SameStr( )
        {
            const TestSetArray<C>& arr = GetSameStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            const EER eer = CC::StringUtil::CheckForComparisonEarlyExit<C>(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ));

            SUTL_TEST_ASSERT(eer == EER::NoExit);

            SUTL_TEST_SUCCESS( );
        }

        template <typename C>
        static UTR LengthMismatch( )
        {
            const TestSetArray<C>& arr = GetDifferentLengthStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            const EER eer = CC::StringUtil::CheckForComparisonEarlyExit<C>(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ));

            SUTL_TEST_ASSERT(eer == EER::False);

            SUTL_TEST_SUCCESS( );
        }

        template <typename C>
        static UTR BothZeroLength( )
        {
            const TestSetArray<C>& arr = GetEmptyStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            const EER eer = CC::StringUtil::CheckForComparisonEarlyExit<C>(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ));

            SUTL_TEST_ASSERT(eer == EER::True);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR OneNullptr( )
        {
            const TestSetArray<C>& arr = GetSameLengthStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            const EER eer = CC::StringUtil::CheckForComparisonEarlyExit<C>(nullptr, str1.length( ), str2.c_str( ), str2.length( ));

            SUTL_TEST_ASSERT(eer == EER::False);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR BothNullptr( )
        {
            const TestSetArray<C>& arr = GetSameLengthStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            const EER eer = CC::StringUtil::CheckForComparisonEarlyExit<C>(nullptr, str1.length( ), nullptr, str2.length( ));

            SUTL_TEST_ASSERT(eer == EER::True);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR SamePtr( )
        {
            const TestSetArray<C>& arr = GetSameStrs<C>( );
            const TestStr<C>& str = arr[0];

            const EER eer = CC::StringUtil::CheckForComparisonEarlyExit<C>(str.c_str( ), str.length( ), str.c_str( ), str.length( ));

            SUTL_TEST_ASSERT(eer == EER::True);

            SUTL_TEST_SUCCESS( );
        }
    };


    class StringUtilTests::ComparisonTests
    {
    public:
        template <typename C>
        static UTR LengthMismatch( )
        {
            const TestSetArray<C>& arr = GetDifferentLengthStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ), false) == false);
            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ), true) == false);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR BothZeroLength( )
        {
            const TestSetArray<C>& arr = GetSameLengthStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str1.c_str( ), 0, str2.c_str( ), 0, false) == true);
            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str1.c_str( ), 0, str2.c_str( ), 0, true) == true);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR OneNullptr( )
        {
            const TestSetArray<C>& arr = GetSameLengthStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            SUTL_TEST_ASSERT(CC::StringUtil::Compare<C>(nullptr, str1.length( ), str2.c_str( ), str2.length( ), false) == false);
            SUTL_TEST_ASSERT(CC::StringUtil::Compare<C>(nullptr, str1.length( ), str2.c_str( ), str2.length( ), true) == false);
            SUTL_TEST_ASSERT(CC::StringUtil::Compare<C>(str1.c_str( ), str1.length( ), nullptr, str2.length( ), false) == false);
            SUTL_TEST_ASSERT(CC::StringUtil::Compare<C>(str1.c_str( ), str1.length( ), nullptr, str2.length( ), true) == false);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR BothNullptr( )
        {
            const TestSetArray<C>& arr = GetSameLengthStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            SUTL_TEST_ASSERT(CC::StringUtil::Compare<C>(nullptr, str1.length( ), nullptr, str2.length( ), false) == true);
            SUTL_TEST_ASSERT(CC::StringUtil::Compare<C>(nullptr, str1.length( ), nullptr, str2.length( ), true) == true);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR SamePtr( )
        {
            const TestSetArray<C>& arr = GetSameStrs<C>( );
            const TestStr<C>& str = arr[0];

            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str.c_str( ), str.length( ), str.c_str( ), str.length( ), false) == true);
            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str.c_str( ), str.length( ), str.c_str( ), str.length( ), true) == true);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR CaseInsensitiveNoMatch( )
        {
            const TestSetArray<C>& arr = GetSameLengthStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ), false) == false);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR CaseInsensitiveDifferentCasing( )
        {
            const TestSetArray<C>& arr = GetDifferentCaseStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ), false) == true);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR CaseInsensitiveMatch( )
        {
            const TestSetArray<C>& arr = GetSameStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ), false) == true);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR CaseSensitiveNoMatch( )
        {
            const TestSetArray<C>& arr = GetSameLengthStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ), true) == false);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR CaseSensitiveDifferentCasing( )
        {
            const TestSetArray<C>& arr = GetDifferentCaseStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ), true) == false);

            SUTL_TEST_SUCCESS( );
        }


        template <typename C>
        static UTR CaseSensitiveMatch( )
        {
            const TestSetArray<C>& arr = GetSameStrs<C>( );
            const TestStr<C>& str1 = arr[0];
            const TestStr<C>& str2 = arr[1];

            SUTL_TEST_ASSERT(CC::StringUtil::Compare(str1.c_str( ), str1.length( ), str2.c_str( ), str2.length( ), true) == true);

            SUTL_TEST_SUCCESS( );
        }
    };


    class StringUtilTests::CopyEarlyExitTests
    {
    public:
        template <RT RT, typename C>
        static UTR Nullptr( )
        {
            const TestStr<C>& str = GetManyLengthStr<C>( );

            const EER eer = CC::StringUtil::CheckForCopyEarlyExit<RT, C>(nullptr, str.length( ));

            SUTL_TEST_ASSERT(eer == ((RT == RT::StringObj) ? EER::EmptyString : EER::ZeroedBuffer));

            SUTL_TEST_SUCCESS( );
        }


        template <RT RT, typename C>
        static UTR ZeroLength( )
        {
            const TestStr<C>& str = GetZeroLengthStr<C>( );

            const EER eer = CC::StringUtil::CheckForCopyEarlyExit<RT, C>(str.c_str( ), str.length( ));

            SUTL_TEST_ASSERT(eer == ((RT == RT::StringObj) ? EER::EmptyString : EER::ZeroedBuffer));

            SUTL_TEST_SUCCESS( );
        }


        template <RT RT, typename C>
        static UTR OneLength( )
        {
            const TestStr<C>& str = GetOneLengthStr<C>( );

            const EER eer = CC::StringUtil::CheckForCopyEarlyExit<RT, C>(str.c_str( ), str.length( ));

            SUTL_TEST_ASSERT(eer == EER::NoExit);

            SUTL_TEST_SUCCESS( );
        }


        template <RT RT, typename C>
        static UTR ManyLength( )
        {
            const TestStr<C>& str = GetManyLengthStr<C>( );

            const EER eer = CC::StringUtil::CheckForCopyEarlyExit<RT, C>(str.c_str( ), str.length( ));

            SUTL_TEST_ASSERT(eer == EER::NoExit);

            SUTL_TEST_SUCCESS( );
        }
    };


    class StringUtilTests::CopyTests
    {
    public:
        template <CC::StringUtil::ReturnType RT, typename C>
        static UTR Nullptr( )
        {
            const TestStr<C>& str = GetManyLengthStr<C>( );
            const auto copy = CC::StringUtil::Copy<RT, C>(nullptr, str.length( ));

            if constexpr ( RT == RT::StringObj )
            {
                SUTL_TEST_ASSERT(copy.empty( ));
            }
            else if constexpr ( RT == RT::SmartCString )
            {
                const C* ptr = GetStringPointer<RT, C>(&copy);
                SUTL_TEST_ASSERT(ptr);

                for ( size_t i = 0; i <= str.length( ); i++ )
                {
                    SUTL_TEST_ASSERT(ptr[i] == C('\0'));
                }
            }

            SUTL_TEST_SUCCESS( );
        }


        template <CC::StringUtil::ReturnType RT, typename C>
        static UTR ZeroLength( )
        {
            const TestStr<C>& str = GetZeroLengthStr<C>( );
            const auto copy = CC::StringUtil::Copy<RT, C>(str.c_str( ), str.length( ));
            const C* ptr = GetStringPointer<RT, C>(&copy);

            SUTL_TEST_ASSERT(ptr);

            if constexpr ( RT == RT::StringObj )
            {
                SUTL_TEST_ASSERT(copy.empty( ));
            }

            SUTL_TEST_ASSERT(memcmp(ptr, str.c_str( ), (str.length( ) + 1) * sizeof(C)) == 0);

            SUTL_TEST_SUCCESS( );
        }


        template <CC::StringUtil::ReturnType RT, typename C>
        static UTR OneLength( )
        {
            const TestStr<C>& str = GetOneLengthStr<C>( );
            const auto copy = CC::StringUtil::Copy<RT, C>(str.c_str( ), str.length( ));
            const C* ptr = GetStringPointer<RT, C>(&copy);

            SUTL_TEST_ASSERT(ptr);

            if constexpr ( RT == RT::StringObj )
            {
                SUTL_TEST_ASSERT(!copy.empty( ));
            }

            SUTL_TEST_ASSERT(memcmp(ptr, str.c_str( ), (str.length( ) + 1) * sizeof(C)) == 0);

            SUTL_TEST_SUCCESS( );
        }


        template <CC::StringUtil::ReturnType RT, typename C>
        static UTR ManyLength( )
        {
            const TestStr<C>& str = GetManyLengthStr<C>( );
            const auto copy = CC::StringUtil::Copy<RT, C>(str.c_str( ), str.length( ));
            const C* ptr = GetStringPointer<RT, C>(&copy);

            SUTL_TEST_ASSERT(ptr);

            if constexpr ( RT == RT::StringObj )
            {
                SUTL_TEST_ASSERT(!copy.empty( ));
            }

            SUTL_TEST_ASSERT(memcmp(ptr, str.c_str( ), (str.length( ) + 1) * sizeof(C)) == 0);

            SUTL_TEST_SUCCESS( );
        }
    };


    class StringUtilTests::UTFConversionEarlyExitTests
    {
    public:
        template <RT RT, typename C>
        static UTR Nullptr( )
        {
            const TestStr<C>& str = GetManyLengthStr<C>( );

            const EER eer = CC::StringUtil::CheckForUTFConversionEarlyExit<RT, C>(nullptr, str.length( ));

            SUTL_TEST_ASSERT(eer == ((RT == RT::StringObj) ? EER::EmptyString : EER::ZeroedBuffer));

            SUTL_TEST_SUCCESS( );
        }


        template <RT RT, typename C>
        static UTR ZeroLength( )
        {
            const TestStr<C>& str = GetZeroLengthStr<C>( );

            const EER eer = CC::StringUtil::CheckForUTFConversionEarlyExit<RT, C>(str.c_str( ), str.length( ));

            SUTL_TEST_ASSERT(eer == ((RT == RT::StringObj) ? EER::EmptyString : EER::ZeroedBuffer));

            SUTL_TEST_SUCCESS( );
        }


        template <RT RT, typename C>
        static UTR OneLength( )
        {
            const TestStr<C>& str = GetOneLengthStr<C>( );

            const EER eer = CC::StringUtil::CheckForUTFConversionEarlyExit<RT, C>(str.c_str( ), str.length( ));

            SUTL_TEST_ASSERT(eer == EER::NoExit);

            SUTL_TEST_SUCCESS( );
        }


        template <RT RT, typename C>
        static UTR ManyLength( )
        {
            const TestStr<C>& str = GetManyLengthStr<C>( );

            const EER eer = CC::StringUtil::CheckForUTFConversionEarlyExit<RT, C>(str.c_str( ), str.length( ));

            SUTL_TEST_ASSERT(eer == EER::NoExit);

            SUTL_TEST_SUCCESS( );
        }
    };


    class StringUtilTests::UTFConversionTests
    {
    public:
        template <CC::StringUtil::ReturnType RT, typename CDst, typename CSrc>
        static UTR Nullptr( )
        {
            const TestStr<CSrc>& str = GetManyLengthStr<CSrc>( );
            const auto conv = CC::StringUtil::UTFConversion<RT, CDst, CSrc>(nullptr, str.length( ));

            if constexpr ( RT == RT::StringObj )
            {
                SUTL_TEST_ASSERT(conv.empty( ));
            }
            else if constexpr ( RT == RT::SmartCString )
            {
                const CDst* ptr = GetStringPointer<RT, CDst>(&conv);
                SUTL_TEST_ASSERT(ptr);

                for ( size_t i = 0; i <= str.length( ); i++ )
                {
                    SUTL_TEST_ASSERT(ptr[i] == CDst('\0'));
                }
            }

            SUTL_TEST_SUCCESS( );
        }


        template <CC::StringUtil::ReturnType RT, typename CDst, typename CSrc>
        static UTR ZeroLength( )
        {
            const TestStr<CSrc>& str = GetZeroLengthStr<CSrc>( );
            const auto conv = CC::StringUtil::UTFConversion<RT, CDst, CSrc>(str.c_str( ), str.length( ));
            const CSrc* srcPtr = str.c_str( );
            const CDst* ptr = GetStringPointer<RT, CDst>(&conv);

            SUTL_TEST_ASSERT(ptr);

            if constexpr ( RT == RT::StringObj )
            {
                SUTL_TEST_ASSERT(conv.length( ) == str.length( ));
            }

            for ( size_t i = 0; i <= str.length( ); i++ )
            {
                SUTL_TEST_ASSERT(ptr[i] == static_cast<CDst>(srcPtr[i]));
            }

            SUTL_TEST_SUCCESS( );
        }


        template <CC::StringUtil::ReturnType RT, typename CDst, typename CSrc>
        static UTR OneLength( )
        {
            const TestStr<CSrc>& str = GetOneLengthStr<CSrc>( );
            const auto conv = CC::StringUtil::UTFConversion<RT, CDst, CSrc>(str.c_str( ), str.length( ));
            const CSrc* srcPtr = str.c_str( );
            const CDst* ptr = GetStringPointer<RT, CDst>(&conv);

            SUTL_TEST_ASSERT(ptr);

            if constexpr ( RT == RT::StringObj )
            {
                SUTL_TEST_ASSERT(conv.length( ) == str.length( ));
            }

            for ( size_t i = 0; i <= str.length( ); i++ )
            {
                SUTL_TEST_ASSERT(ptr[i] == static_cast<CDst>(srcPtr[i]));
            }

            SUTL_TEST_SUCCESS( );
        }


        template <CC::StringUtil::ReturnType RT, typename CDst, typename CSrc>
        static UTR ManyLength( )
        {
            const TestStr<CSrc>& str = GetManyLengthStr<CSrc>( );
            const auto conv = CC::StringUtil::UTFConversion<RT, CDst, CSrc>(str.c_str( ), str.length( ));
            const CSrc* srcPtr = str.c_str( );
            const CDst* ptr = GetStringPointer<RT, CDst>(&conv);

            SUTL_TEST_ASSERT(ptr);

            if constexpr ( RT == RT::StringObj )
            {
                SUTL_TEST_ASSERT(conv.length( ) == str.length( ));
            }

            for ( size_t i = 0; i <= str.length( ); i++ )
            {
                SUTL_TEST_ASSERT(ptr[i] == static_cast<CDst>(srcPtr[i]));
            }

            SUTL_TEST_SUCCESS( );
        }
    };


    class StringUtilTests::NumberConversionTests
    {
    private:
        template <typename N, typename C>
        using NCTestPair = std::pair<N, TestStr<C>>;

        template <typename N, typename C>
        using NCTestArray = std::array<NCTestPair<N, C>, 7>;

        template <BT BT, typename C>
        static NCTestArray<int8_t, C> GetSigned8BitTestArray( )
        {
            NCTestArray<int8_t, C> arr;

            arr[0].first = std::numeric_limits<int8_t>::min( );
            arr[1].first = -50;
            arr[2].first = -1;
            arr[3].first = 0;
            arr[4].first = 1;
            arr[5].first = 105;
            arr[6].first = std::numeric_limits<int8_t>::max( );

            if constexpr ( BT == BT::Binary )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0b1000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0b1100`1110");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0b1111`1111");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0b0000`0000");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0b0000`0001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0b0110`1001");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0b0111`1111");
            }
            else if constexpr ( BT == BT::Octal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0200");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0316");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0377");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0000");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0151");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0177");
            }
            else if constexpr ( BT == BT::Decimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0n-128");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0n-50");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0n-1");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0n0");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0n1");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0n105");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0n127");
            }
            else if constexpr ( BT == BT::Hexadecimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0x80");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0xCE");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0xFF");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0x00");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0x01");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0x69");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0x7F");
            }

            return arr;
        }

        template <BT BT, typename C>
        static NCTestArray<uint8_t, C> GetUnsigned8BitTestArray( )
        {
            NCTestArray<uint8_t, C> arr;

            arr[0].first = 0;
            arr[1].first = 1;
            arr[2].first = 37;
            arr[3].first = 54;
            arr[4].first = 105;
            arr[5].first = 180;
            arr[6].first = std::numeric_limits<uint8_t>::max( );

            if constexpr ( BT == BT::Binary )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0b0000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0b0000`0001");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0b0010`0101");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0b0011`0110");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0b0110`1001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0b1011`0100");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0b1111`1111");
            }
            else if constexpr ( BT == BT::Octal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0001");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0045");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0066");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0151");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0264");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0377");
            }
            else if constexpr ( BT == BT::Decimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0n0");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0n1");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0n37");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0n54");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0n105");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0n180");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0n255");
            }
            else if constexpr ( BT == BT::Hexadecimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0x00");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0x01");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0x25");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0x36");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0x69");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0xB4");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0xFF");
            }

            return arr;
        }

        template <BT BT, typename C>
        static NCTestArray<int16_t, C> GetSigned16BitTestArray( )
        {
            NCTestArray<int16_t, C> arr;

            arr[0].first = std::numeric_limits<int16_t>::min( );
            arr[1].first = -23881;
            arr[2].first = -1;
            arr[3].first = 0;
            arr[4].first = 1;
            arr[5].first = 11663;
            arr[6].first = std::numeric_limits<int16_t>::max( );

            if constexpr ( BT == BT::Binary )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0b1000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0b1010`0010`1011`0111");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0b1111`1111`1111`1111");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0b0000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0b0000`0000`0000`0001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0b0010`1101`1000`1111");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0b0111`1111`1111`1111");
            }
            else if constexpr ( BT == BT::Octal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0100`000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0121`267");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0177`777");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0000`000");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0000`001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0026`617");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0077`777");
            }
            else if constexpr ( BT == BT::Decimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0n-32,768");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0n-23,881");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0n-1");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0n0");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0n1");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0n11,663");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0n32,767");
            }
            else if constexpr ( BT == BT::Hexadecimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0x8000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0xA2B7");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0xFFFF");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0x0000");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0x0001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0x2D8F");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0x7FFF");
            }

            return arr;
        }

        template <BT BT, typename C>
        static NCTestArray<uint16_t, C> GetUnsigned16BitTestArray( )
        {
            NCTestArray<uint16_t, C> arr;

            arr[0].first = 0;
            arr[1].first = 1;
            arr[2].first = 16295;
            arr[3].first = 32738;
            arr[4].first = 40194;
            arr[5].first = 56814;
            arr[6].first = std::numeric_limits<uint16_t>::max( );

            if constexpr ( BT == BT::Binary )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0b0000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0b0000`0000`0000`0001");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0b0011`1111`1010`0111");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0b0111`1111`1110`0010");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0b1001`1101`0000`0010");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0b1101`1101`1110`1110");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0b1111`1111`1111`1111");
            }
            else if constexpr ( BT == BT::Octal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0000`000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0000`001");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0037`647");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0077`742");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0116`402");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0156`756");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0177`777");
            }
            else if constexpr ( BT == BT::Decimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0n0");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0n1");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0n16,295");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0n32,738");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0n40,194");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0n56,814");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0n65,535");
            }
            else if constexpr ( BT == BT::Hexadecimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0x0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0x0001");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0x3FA7");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0x7FE2");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0x9D02");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0xDDEE");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0xFFFF");
            }

            return arr;
        }

        template <BT BT, typename C>
        static NCTestArray<int32_t, C> GetSigned32BitTestArray( )
        {
            NCTestArray<int32_t, C> arr;

            arr[0].first = std::numeric_limits<int32_t>::min( );
            arr[1].first = -625183928;
            arr[2].first = -1;
            arr[3].first = 0;
            arr[4].first = 1;
            arr[5].first = 1873574107;
            arr[6].first = std::numeric_limits<int32_t>::max( );

            if constexpr ( BT == BT::Binary )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0b1000`0000`0000`0000`0000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0b1101`1010`1011`1100`0111`0011`0100`1000");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0b1111`1111`1111`1111`1111`1111`1111`1111");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0b0110`1111`1010`1100`0111`1000`1101`1011");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0b0111`1111`1111`1111`1111`1111`1111`1111");
            }
            else if constexpr ( BT == BT::Octal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0020`000`000`000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0033`257`071`510");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0037`777`777`777");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0000`000`000`000");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0000`000`000`001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0015`753`074`333");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0017`777`777`777");
            }
            else if constexpr ( BT == BT::Decimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0n-2,147,483,648");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0n-625,183,928");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0n-1");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0n0");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0n1");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0n1,873,574,107");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0n2,147,483,647");
            }
            else if constexpr ( BT == BT::Hexadecimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0x8000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0xDABC`7348");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0xFFFF`FFFF");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0x0000`0000");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0x0000`0001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0x6FAC`78DB");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0x7FFF`FFFF");
            }

            return arr;
        }

        template <BT BT, typename C>
        static NCTestArray<uint32_t, C> GetUnsigned32BitTestArray( )
        {
            NCTestArray<uint32_t, C> arr;

            arr[0].first = 0;
            arr[1].first = 1;
            arr[2].first = 1341882450;
            arr[3].first = 1879067553;
            arr[4].first = 2863092254;
            arr[5].first = 3434837845;
            arr[6].first = std::numeric_limits<uint32_t>::max( );

            if constexpr ( BT == BT::Binary )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0001");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0b0100`1111`1111`1011`1000`0000`0101`0010");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0b0111`0000`0000`0000`0100`1011`1010`0001");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0b1010`1010`1010`0111`0101`0010`0001`1110");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0b1100`1100`1011`1011`0111`0111`0101`0101");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0b1111`1111`1111`1111`1111`1111`1111`1111");
            }
            else if constexpr ( BT == BT::Octal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0000`000`000`000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0000`000`000`001");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0011`776`700`122");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0016`000`045`641");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0025`251`651`036");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0031`456`673`525");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0037`777`777`777");
            }
            else if constexpr ( BT == BT::Decimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0n0");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0n1");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0n1,341,882,450");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0n1,879,067,553");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0n2,863,092,254");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0n3,434,837,845");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0n4,294,967,295");
            }
            else if constexpr ( BT == BT::Hexadecimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0x0000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0x0000`0001");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0x4FFB`8052");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0x7000`4BA1");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0xAAA7`521E");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0xCCBB`7755");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0xFFFF`FFFF");
            }

            return arr;
        }

        template <BT BT, typename C>
        static NCTestArray<int64_t, C> GetSigned64BitTestArray( )
        {
            NCTestArray<int64_t, C> arr;

            arr[0].first = std::numeric_limits<int64_t>::min( );
            arr[1].first = -3786109484092722936;
            arr[2].first = -1;
            arr[3].first = 0;
            arr[4].first = 1;
            arr[5].first = 485820086583038111;
            arr[6].first = std::numeric_limits<int64_t>::max( );

            if constexpr ( BT == BT::Binary )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0b1000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0b1100`1011`0111`0101`0000`1001`0111`1011`1101`1111`1110`1010`0111`0001`0000`1000");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0b1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0b0000`0110`1011`1101`1111`1010`1100`1011`0111`0101`1000`1000`0001`0000`1001`1111");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0b0111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111");
            }
            else if constexpr ( BT == BT::Octal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0001`000`000`000`000`000`000`000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0001`455`650`227`573`772`470`410");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0001`777`777`777`777`777`777`777");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0000`000`000`000`000`000`000`000");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0000`000`000`000`000`000`000`001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0000`032`757`654`556`542`010`237");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0000`777`777`777`777`777`777`777");
            }
            else if constexpr ( BT == BT::Decimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0n-9,223,372,036,854,775,808");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0n-3,786,109,484,092,722,936");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0n-1");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0n0");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0n1");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0n485,820,086,583,038,111");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0n9,223,372,036,854,775,807");
            }
            else if constexpr ( BT == BT::Hexadecimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0x8000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0xCB75`097B`DFEA`7108");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0xFFFF`FFFF`FFFF`FFFF");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0x0000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0x0000`0000`0000`0001");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0x06BD`FACB`7588`109F");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0x7FFF`FFFF`FFFF`FFFF");
            }

            return arr;
        }

        template <BT BT, typename C>
        static NCTestArray<uint64_t, C> GetUnsigned64BitTestArray( )
        {
            NCTestArray<uint64_t, C> arr;

            arr[0].first = 0;
            arr[1].first = 1;
            arr[2].first = 2852707957240492308;
            arr[3].first = 6625083791587863114;
            arr[4].first = 9221709922292305978;
            arr[5].first = 14524601349428068613;
            arr[6].first = std::numeric_limits<uint64_t>::max( );

            if constexpr ( BT == BT::Binary )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0001");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0b0010`0111`1001`0110`1101`1010`1100`0101`1101`0111`0011`1111`1111`0001`0001`0100");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0b0101`1011`1111`0001`0000`0110`1001`1011`0111`0100`1100`1010`1110`1010`0100`1010");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0b0111`1111`1111`1010`0001`1000`0101`0000`1100`1011`1110`1101`1001`0000`0011`1010");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0b1100`1001`1001`0001`1011`1111`1111`1001`0000`0000`1101`0001`1100`0001`0000`0101");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0b1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111");
            }
            else if constexpr ( BT == BT::Octal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0000`000`000`000`000`000`000`000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0000`000`000`000`000`000`000`001");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0000`236`266`654`272`717`770`424");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0000`557`610`151`556`462`565`112");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0000`777`720`605`031`373`310`072");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0001`446`215`777`440`064`340`405");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0001`777`777`777`777`777`777`777");
            }
            else if constexpr ( BT == BT::Decimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0n0");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0n1");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0n2,852,707,957,240,492,308");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0n6,625,083,791,587,863,114");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0n9,221,709,922,292,305,978");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0n14,524,601,349,428,068,613");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0n18,446,744,073,709,551,615");
            }
            else if constexpr ( BT == BT::Hexadecimal )
            {
                _ASSIGN_TEST_STR_(arr[0].second, C, "0x0000`0000`0000`0000");
                _ASSIGN_TEST_STR_(arr[1].second, C, "0x0000`0000`0000`0001");
                _ASSIGN_TEST_STR_(arr[2].second, C, "0x2796`DAC5`D73F`F114");
                _ASSIGN_TEST_STR_(arr[3].second, C, "0x5BF1`069B`74CA`EA4A");
                _ASSIGN_TEST_STR_(arr[4].second, C, "0x7FFA`1850`CBED`903A");
                _ASSIGN_TEST_STR_(arr[5].second, C, "0xC991`BFF9`00D1`C105");
                _ASSIGN_TEST_STR_(arr[6].second, C, "0xFFFF`FFFF`FFFF`FFFF");
            }

            return arr;
        }

        template <BT BT, typename C>
        static NCTestArray<void*, C> GetPointerTestArray( )
        {
            NCTestArray<void*, C> arr;

            if constexpr ( sizeof(void*) == 4 )
            {
                arr[0].first = reinterpret_cast<void*>(0);
                arr[1].first = reinterpret_cast<void*>(1);
                arr[2].first = reinterpret_cast<void*>(1341882450);
                arr[3].first = reinterpret_cast<void*>(1879067553);
                arr[4].first = reinterpret_cast<void*>(2863092254);
                arr[5].first = reinterpret_cast<void*>(3434837845);
                arr[6].first = reinterpret_cast<void*>(std::numeric_limits<uint32_t>::max( ));

                if constexpr ( BT == BT::Binary )
                {
                    _ASSIGN_TEST_STR_(arr[0].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0000");
                    _ASSIGN_TEST_STR_(arr[1].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0001");
                    _ASSIGN_TEST_STR_(arr[2].second, C, "0b0100`1111`1111`1011`1000`0000`0101`0010");
                    _ASSIGN_TEST_STR_(arr[3].second, C, "0b0111`0000`0000`0000`0100`1011`1010`0001");
                    _ASSIGN_TEST_STR_(arr[4].second, C, "0b1010`1010`1010`0111`0101`0010`0001`1110");
                    _ASSIGN_TEST_STR_(arr[5].second, C, "0b1100`1100`1011`1011`0111`0111`0101`0101");
                    _ASSIGN_TEST_STR_(arr[6].second, C, "0b1111`1111`1111`1111`1111`1111`1111`1111");
                }
                else if constexpr ( BT == BT::Octal )
                {
                    _ASSIGN_TEST_STR_(arr[0].second, C, "0000`000`000`000");
                    _ASSIGN_TEST_STR_(arr[1].second, C, "0000`000`000`001");
                    _ASSIGN_TEST_STR_(arr[2].second, C, "0011`776`700`122");
                    _ASSIGN_TEST_STR_(arr[3].second, C, "0016`000`045`641");
                    _ASSIGN_TEST_STR_(arr[4].second, C, "0025`251`651`036");
                    _ASSIGN_TEST_STR_(arr[5].second, C, "0031`456`673`525");
                    _ASSIGN_TEST_STR_(arr[6].second, C, "0037`777`777`777");
                }
                else if constexpr ( BT == BT::Decimal )
                {
                    _ASSIGN_TEST_STR_(arr[0].second, C, "0n0");
                    _ASSIGN_TEST_STR_(arr[1].second, C, "0n1");
                    _ASSIGN_TEST_STR_(arr[2].second, C, "0n1,341,882,450");
                    _ASSIGN_TEST_STR_(arr[3].second, C, "0n1,879,067,553");
                    _ASSIGN_TEST_STR_(arr[4].second, C, "0n2,863,092,254");
                    _ASSIGN_TEST_STR_(arr[5].second, C, "0n3,434,837,845");
                    _ASSIGN_TEST_STR_(arr[6].second, C, "0n4,294,967,295");
                }
                else if constexpr ( BT == BT::Hexadecimal )
                {
                    _ASSIGN_TEST_STR_(arr[0].second, C, "0x0000`0000");
                    _ASSIGN_TEST_STR_(arr[1].second, C, "0x0000`0001");
                    _ASSIGN_TEST_STR_(arr[2].second, C, "0x4FFB`8052");
                    _ASSIGN_TEST_STR_(arr[3].second, C, "0x7000`4BA1");
                    _ASSIGN_TEST_STR_(arr[4].second, C, "0xAAA7`521E");
                    _ASSIGN_TEST_STR_(arr[5].second, C, "0xCCBB`7755");
                    _ASSIGN_TEST_STR_(arr[6].second, C, "0xFFFF`FFFF");
                }
            }
            else if constexpr ( sizeof(void*) == 8 )
            {
                arr[0].first = reinterpret_cast<void*>(0);
                arr[1].first = reinterpret_cast<void*>(1);
                arr[2].first = reinterpret_cast<void*>(2852707957240492308);
                arr[3].first = reinterpret_cast<void*>(6625083791587863114);
                arr[4].first = reinterpret_cast<void*>(9221709922292305978);
                arr[5].first = reinterpret_cast<void*>(14524601349428068613);
                arr[6].first = reinterpret_cast<void*>(std::numeric_limits<uint64_t>::max( ));

                if constexpr ( BT == BT::Binary )
                {
                    _ASSIGN_TEST_STR_(arr[0].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000");
                    _ASSIGN_TEST_STR_(arr[1].second, C, "0b0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0000`0001");
                    _ASSIGN_TEST_STR_(arr[2].second, C, "0b0010`0111`1001`0110`1101`1010`1100`0101`1101`0111`0011`1111`1111`0001`0001`0100");
                    _ASSIGN_TEST_STR_(arr[3].second, C, "0b0101`1011`1111`0001`0000`0110`1001`1011`0111`0100`1100`1010`1110`1010`0100`1010");
                    _ASSIGN_TEST_STR_(arr[4].second, C, "0b0111`1111`1111`1010`0001`1000`0101`0000`1100`1011`1110`1101`1001`0000`0011`1010");
                    _ASSIGN_TEST_STR_(arr[5].second, C, "0b1100`1001`1001`0001`1011`1111`1111`1001`0000`0000`1101`0001`1100`0001`0000`0101");
                    _ASSIGN_TEST_STR_(arr[6].second, C, "0b1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111`1111");
                }
                else if constexpr ( BT == BT::Octal )
                {
                    _ASSIGN_TEST_STR_(arr[0].second, C, "0000`000`000`000`000`000`000`000");
                    _ASSIGN_TEST_STR_(arr[1].second, C, "0000`000`000`000`000`000`000`001");
                    _ASSIGN_TEST_STR_(arr[2].second, C, "0000`236`266`654`272`717`770`424");
                    _ASSIGN_TEST_STR_(arr[3].second, C, "0000`557`610`151`556`462`565`112");
                    _ASSIGN_TEST_STR_(arr[4].second, C, "0000`777`720`605`031`373`310`072");
                    _ASSIGN_TEST_STR_(arr[5].second, C, "0001`446`215`777`440`064`340`405");
                    _ASSIGN_TEST_STR_(arr[6].second, C, "0001`777`777`777`777`777`777`777");
                }
                else if constexpr ( BT == BT::Decimal )
                {
                    _ASSIGN_TEST_STR_(arr[0].second, C, "0n0");
                    _ASSIGN_TEST_STR_(arr[1].second, C, "0n1");
                    _ASSIGN_TEST_STR_(arr[2].second, C, "0n2,852,707,957,240,492,308");
                    _ASSIGN_TEST_STR_(arr[3].second, C, "0n6,625,083,791,587,863,114");
                    _ASSIGN_TEST_STR_(arr[4].second, C, "0n9,221,709,922,292,305,978");
                    _ASSIGN_TEST_STR_(arr[5].second, C, "0n14,524,601,349,428,068,613");
                    _ASSIGN_TEST_STR_(arr[6].second, C, "0n18,446,744,073,709,551,615");
                }
                else if constexpr ( BT == BT::Hexadecimal )
                {
                    _ASSIGN_TEST_STR_(arr[0].second, C, "0x0000`0000`0000`0000");
                    _ASSIGN_TEST_STR_(arr[1].second, C, "0x0000`0000`0000`0001");
                    _ASSIGN_TEST_STR_(arr[2].second, C, "0x2796`DAC5`D73F`F114");
                    _ASSIGN_TEST_STR_(arr[3].second, C, "0x5BF1`069B`74CA`EA4A");
                    _ASSIGN_TEST_STR_(arr[4].second, C, "0x7FFA`1850`CBED`903A");
                    _ASSIGN_TEST_STR_(arr[5].second, C, "0xC991`BFF9`00D1`C105");
                    _ASSIGN_TEST_STR_(arr[6].second, C, "0xFFFF`FFFF`FFFF`FFFF");
                }
            }

            return arr;
        }

        template <BT BT, typename N, typename C>
        static NCTestArray<N, C> GetTestArray( )
        {
            if constexpr ( std::is_same_v<N, int8_t> )
            {
                return GetSigned8BitTestArray<BT, C>( );
            }
            else if constexpr ( std::is_same_v<N, uint8_t> )
            {
                return GetUnsigned8BitTestArray<BT, C>( );
            }
            else if constexpr ( std::is_same_v<N, int16_t> )
            {
                return GetSigned16BitTestArray<BT, C>( );
            }
            else if constexpr ( std::is_same_v<N, uint16_t> )
            {
                return GetUnsigned16BitTestArray<BT, C>( );
            }
            else if constexpr ( std::is_same_v<N, int32_t> )
            {
                return GetSigned32BitTestArray<BT, C>( );
            }
            else if constexpr ( std::is_same_v<N, uint32_t> )
            {
                return GetUnsigned32BitTestArray<BT, C>( );
            }
            else if constexpr ( std::is_same_v<N, int64_t> )
            {
                return GetSigned64BitTestArray<BT, C>( );
            }
            else if constexpr ( std::is_same_v<N, uint64_t> )
            {
                return GetUnsigned64BitTestArray<BT, C>( );
            }
            else if constexpr ( std::is_same_v<N, void*> )
            {
                return GetPointerTestArray<BT, C>( );
            }
            else
            {
                throw std::logic_error(__FUNCTION__": Unknown integral type[" + std::string(typeid(N).name( )) + "].");
            }
        }

    public:

        template <RT RT, BT BT, typename N, typename C>
        static UTR NumberType( )
        {
            NCTestArray<N, C> arr = GetTestArray<BT, N, C>( );

            for ( size_t i = 0; i < arr.size( ); i++ )
            {
                const N& num = arr[i].first;
                const std::basic_string<C>& expected = arr[i].second;

                auto str = CC::StringUtil::NumberConversion::Convert<RT, BT, C, N>(num);
                const C* ptr = GetStringPointer<RT, C>(&str);

                SUTL_TEST_ASSERT(ptr);
                SUTL_TEST_ASSERT(memcmp(ptr, expected.c_str( ), (expected.length( ) + 1) * sizeof(C)) == 0);
            }

            SUTL_TEST_SUCCESS( );
        }
    };
}

inline std::list<CC::StringUtilTests::UTFunc> CC::StringUtilTests::GetTests( )
{
    static const std::list<std::function<UTR(void)>> tests
    {
        EnumValidatorTests::InvalidOperationType,
        EnumValidatorTests::ValidOperationType,
        EnumValidatorTests::InvalidEarlyExitResult,
        EnumValidatorTests::ValidEarlyExitResult,
        EnumValidatorTests::InvalidReturnType,
        EnumValidatorTests::ValidReturnType,
        EnumValidatorTests::InvalidBaseType,
        EnumValidatorTests::ValidBaseType,

        BuildStringTests::ZeroLength<RT::StringObj, char>,
        BuildStringTests::ZeroLength<RT::StringObj, wchar_t>,
        BuildStringTests::ZeroLength<RT::SmartCString, char>,
        BuildStringTests::ZeroLength<RT::SmartCString, wchar_t>,
        BuildStringTests::OneLength<RT::StringObj, char>,
        BuildStringTests::OneLength<RT::StringObj, wchar_t>,
        BuildStringTests::OneLength<RT::SmartCString, char>,
        BuildStringTests::OneLength<RT::SmartCString, wchar_t>,
        BuildStringTests::ManyLength<RT::StringObj, char>,
        BuildStringTests::ManyLength<RT::StringObj, wchar_t>,
        BuildStringTests::ManyLength<RT::SmartCString, char>,
        BuildStringTests::ManyLength<RT::SmartCString, wchar_t>,

        GetLengthTests::NullptrArg<char>,
        GetLengthTests::NullptrArg<wchar_t>,
        GetLengthTests::ZeroLength<char>,
        GetLengthTests::ZeroLength<wchar_t>,
        GetLengthTests::OneLength<char>,
        GetLengthTests::OneLength<wchar_t>,
        GetLengthTests::ManyLength<char>,
        GetLengthTests::ManyLength<wchar_t>,

        ComparisonEarlyExitTests::SameLengthDifferentStrs<char>,
        ComparisonEarlyExitTests::SameLengthDifferentStrs<wchar_t>,
        ComparisonEarlyExitTests::SameStr<char>,
        ComparisonEarlyExitTests::SameStr<wchar_t>,
        ComparisonEarlyExitTests::LengthMismatch<char>,
        ComparisonEarlyExitTests::LengthMismatch<wchar_t>,
        ComparisonEarlyExitTests::BothZeroLength<char>,
        ComparisonEarlyExitTests::BothZeroLength<wchar_t>,
        ComparisonEarlyExitTests::OneNullptr<char>,
        ComparisonEarlyExitTests::OneNullptr<wchar_t>,
        ComparisonEarlyExitTests::BothNullptr<char>,
        ComparisonEarlyExitTests::BothNullptr<wchar_t>,
        ComparisonEarlyExitTests::SamePtr<char>,
        ComparisonEarlyExitTests::SamePtr<wchar_t>,

        ComparisonTests::LengthMismatch<char>,
        ComparisonTests::LengthMismatch<wchar_t>,
        ComparisonTests::BothZeroLength<char>,
        ComparisonTests::BothZeroLength<wchar_t>,
        ComparisonTests::OneNullptr<char>,
        ComparisonTests::OneNullptr<wchar_t>,
        ComparisonTests::BothNullptr<char>,
        ComparisonTests::BothNullptr<wchar_t>,
        ComparisonTests::SamePtr<char>,
        ComparisonTests::SamePtr<wchar_t>,
        ComparisonTests::CaseInsensitiveNoMatch<char>,
        ComparisonTests::CaseInsensitiveNoMatch<wchar_t>,
        ComparisonTests::CaseInsensitiveDifferentCasing<char>,
        ComparisonTests::CaseInsensitiveDifferentCasing<wchar_t>,
        ComparisonTests::CaseInsensitiveMatch<char>,
        ComparisonTests::CaseInsensitiveMatch<wchar_t>,
        ComparisonTests::CaseSensitiveNoMatch<char>,
        ComparisonTests::CaseSensitiveNoMatch<wchar_t>,
        ComparisonTests::CaseSensitiveDifferentCasing<char>,
        ComparisonTests::CaseSensitiveDifferentCasing<wchar_t>,
        ComparisonTests::CaseSensitiveMatch<char>,
        ComparisonTests::CaseSensitiveMatch<wchar_t>,

        CopyEarlyExitTests::Nullptr<RT::StringObj, char>,
        CopyEarlyExitTests::Nullptr<RT::StringObj, wchar_t>,
        CopyEarlyExitTests::Nullptr<RT::SmartCString, char>,
        CopyEarlyExitTests::Nullptr<RT::SmartCString, wchar_t>,
        CopyEarlyExitTests::ZeroLength<RT::StringObj, char>,
        CopyEarlyExitTests::ZeroLength<RT::StringObj, wchar_t>,
        CopyEarlyExitTests::ZeroLength<RT::SmartCString, char>,
        CopyEarlyExitTests::ZeroLength<RT::SmartCString, wchar_t>,
        CopyEarlyExitTests::OneLength<RT::StringObj, char>,
        CopyEarlyExitTests::OneLength<RT::StringObj, wchar_t>,
        CopyEarlyExitTests::OneLength<RT::SmartCString, char>,
        CopyEarlyExitTests::OneLength<RT::SmartCString, wchar_t>,
        CopyEarlyExitTests::ManyLength<RT::StringObj, char>,
        CopyEarlyExitTests::ManyLength<RT::StringObj, wchar_t>,
        CopyEarlyExitTests::ManyLength<RT::SmartCString, char>,
        CopyEarlyExitTests::ManyLength<RT::SmartCString, wchar_t>,

        CopyTests::Nullptr<RT::StringObj, char>,
        CopyTests::Nullptr<RT::StringObj, wchar_t>,
        CopyTests::Nullptr<RT::SmartCString, char>,
        CopyTests::Nullptr<RT::SmartCString, wchar_t>,
        CopyTests::ZeroLength<RT::StringObj, char>,
        CopyTests::ZeroLength<RT::StringObj, wchar_t>,
        CopyTests::ZeroLength<RT::SmartCString, char>,
        CopyTests::ZeroLength<RT::SmartCString, wchar_t>,
        CopyTests::OneLength<RT::StringObj, char>,
        CopyTests::OneLength<RT::StringObj, wchar_t>,
        CopyTests::OneLength<RT::SmartCString, char>,
        CopyTests::OneLength<RT::SmartCString, wchar_t>,
        CopyTests::ManyLength<RT::StringObj, char>,
        CopyTests::ManyLength<RT::StringObj, wchar_t>,
        CopyTests::ManyLength<RT::SmartCString, char>,
        CopyTests::ManyLength<RT::SmartCString, wchar_t>,

        UTFConversionEarlyExitTests::Nullptr<RT::StringObj, char>,
        UTFConversionEarlyExitTests::Nullptr<RT::StringObj, wchar_t>,
        UTFConversionEarlyExitTests::Nullptr<RT::SmartCString, char>,
        UTFConversionEarlyExitTests::Nullptr<RT::SmartCString, wchar_t>,
        UTFConversionEarlyExitTests::ZeroLength<RT::StringObj, char>,
        UTFConversionEarlyExitTests::ZeroLength<RT::StringObj, wchar_t>,
        UTFConversionEarlyExitTests::ZeroLength<RT::SmartCString, char>,
        UTFConversionEarlyExitTests::ZeroLength<RT::SmartCString, wchar_t>,
        UTFConversionEarlyExitTests::OneLength<RT::StringObj, char>,
        UTFConversionEarlyExitTests::OneLength<RT::StringObj, wchar_t>,
        UTFConversionEarlyExitTests::OneLength<RT::SmartCString, char>,
        UTFConversionEarlyExitTests::OneLength<RT::SmartCString, wchar_t>,
        UTFConversionEarlyExitTests::ManyLength<RT::StringObj, char>,
        UTFConversionEarlyExitTests::ManyLength<RT::StringObj, wchar_t>,
        UTFConversionEarlyExitTests::ManyLength<RT::SmartCString, char>,
        UTFConversionEarlyExitTests::ManyLength<RT::SmartCString, wchar_t>,

        UTFConversionTests::Nullptr<RT::StringObj, char, char>,
        UTFConversionTests::Nullptr<RT::StringObj, char, wchar_t>,
        UTFConversionTests::Nullptr<RT::StringObj, wchar_t, char>,
        UTFConversionTests::Nullptr<RT::StringObj, wchar_t, wchar_t>,
        UTFConversionTests::Nullptr<RT::SmartCString, char, char>,
        UTFConversionTests::Nullptr<RT::SmartCString, char, wchar_t>,
        UTFConversionTests::Nullptr<RT::SmartCString, wchar_t, char>,
        UTFConversionTests::Nullptr<RT::SmartCString, wchar_t, wchar_t>,
        UTFConversionTests::ZeroLength<RT::StringObj, char, char>,
        UTFConversionTests::ZeroLength<RT::StringObj, char, wchar_t>,
        UTFConversionTests::ZeroLength<RT::StringObj, wchar_t, char>,
        UTFConversionTests::ZeroLength<RT::StringObj, wchar_t, wchar_t>,
        UTFConversionTests::ZeroLength<RT::SmartCString, char, char>,
        UTFConversionTests::ZeroLength<RT::SmartCString, char, wchar_t>,
        UTFConversionTests::ZeroLength<RT::SmartCString, wchar_t, char>,
        UTFConversionTests::ZeroLength<RT::SmartCString, wchar_t, wchar_t>,
        UTFConversionTests::OneLength<RT::StringObj, char, char>,
        UTFConversionTests::OneLength<RT::StringObj, char, wchar_t>,
        UTFConversionTests::OneLength<RT::StringObj, wchar_t, char>,
        UTFConversionTests::OneLength<RT::StringObj, wchar_t, wchar_t>,
        UTFConversionTests::OneLength<RT::SmartCString, char, char>,
        UTFConversionTests::OneLength<RT::SmartCString, char, wchar_t>,
        UTFConversionTests::OneLength<RT::SmartCString, wchar_t, char>,
        UTFConversionTests::OneLength<RT::SmartCString, wchar_t, wchar_t>,
        UTFConversionTests::ManyLength<RT::StringObj, char, char>,
        UTFConversionTests::ManyLength<RT::StringObj, char, wchar_t>,
        UTFConversionTests::ManyLength<RT::StringObj, wchar_t, char>,
        UTFConversionTests::ManyLength<RT::StringObj, wchar_t, wchar_t>,
        UTFConversionTests::ManyLength<RT::SmartCString, char, char>,
        UTFConversionTests::ManyLength<RT::SmartCString, char, wchar_t>,
        UTFConversionTests::ManyLength<RT::SmartCString, wchar_t, char>,
        UTFConversionTests::ManyLength<RT::SmartCString, wchar_t, wchar_t>,

        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, int8_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, int8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, int8_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, int8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, int8_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, int8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, int8_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, int8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, uint8_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, uint8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, uint8_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, uint8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, uint8_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, uint8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, uint8_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, uint8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, int16_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, int16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, int16_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, int16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, int16_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, int16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, int16_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, int16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, uint16_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, uint16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, uint16_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, uint16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, uint16_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, uint16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, uint16_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, uint16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, int32_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, int32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, int32_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, int32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, int32_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, int32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, int32_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, int32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, uint32_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, uint32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, uint32_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, uint32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, uint32_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, uint32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, uint32_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, uint32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, int64_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, int64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, int64_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, int64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, int64_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, int64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, int64_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, int64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, uint64_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, uint64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, uint64_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, uint64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, uint64_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, uint64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, uint64_t, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, uint64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, void*, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Binary, void*, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, void*, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Octal, void*, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, void*, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Decimal, void*, wchar_t>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, void*, char>,
        NumberConversionTests::NumberType<RT::StringObj, BT::Hexadecimal, void*, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, int8_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, int8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, int8_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, int8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, int8_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, int8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, int8_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, int8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, uint8_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, uint8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, uint8_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, uint8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, uint8_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, uint8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, uint8_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, uint8_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, int16_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, int16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, int16_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, int16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, int16_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, int16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, int16_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, int16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, uint16_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, uint16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, uint16_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, uint16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, uint16_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, uint16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, uint16_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, uint16_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, int32_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, int32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, int32_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, int32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, int32_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, int32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, int32_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, int32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, uint32_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, uint32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, uint32_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, uint32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, uint32_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, uint32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, uint32_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, uint32_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, int64_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, int64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, int64_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, int64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, int64_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, int64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, int64_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, int64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, uint64_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, uint64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, uint64_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, uint64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, uint64_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, uint64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, uint64_t, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, uint64_t, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, void*, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Binary, void*, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, void*, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Octal, void*, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, void*, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Decimal, void*, wchar_t>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, void*, char>,
        NumberConversionTests::NumberType<RT::SmartCString, BT::Hexadecimal, void*, wchar_t>
    };

    return tests;
}

#ifdef _ASSIGN_TEST_STR_
#undef _ASSIGN_TEST_STR_
#endif
