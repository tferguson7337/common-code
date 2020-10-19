#include <CCStringUtilTests.h>


// Add-String-Util-Test Helper Macros //

// Add 2 Tests (2 Char types)
#define ADD_STR_UTIL_TESTS_C(_fn_)\
    _fn_<char>, _fn_<wchar_t>

// Add 6 Tests (3 Return Types x 2 Char Types)
#define ADD_STR_UTIL_TESTS_RT_C(_fn_)\
    _fn_<CC::ReturnType::CCBuffer,  char>, _fn_<CC::ReturnType::CCBuffer,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  char>, _fn_<CC::ReturnType::CCString,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, char>, _fn_<CC::ReturnType::CppString, wchar_t>

// Add 12 Tests (3 Return Types x 2 Char Types x 2 Char Types)
#define ADD_STR_UTIL_TESTS_RT_C2(_fn_)\
    _fn_<CC::ReturnType::CCBuffer,  char,    char>, _fn_<CC::ReturnType::CCBuffer,  char,    wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  wchar_t, char>, _fn_<CC::ReturnType::CCBuffer,  wchar_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  char,    char>, _fn_<CC::ReturnType::CCString,  char,    wchar_t>,\
    _fn_<CC::ReturnType::CCString,  wchar_t, char>, _fn_<CC::ReturnType::CCString,  wchar_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, char,    char>, _fn_<CC::ReturnType::CppString, char,    wchar_t>,\
    _fn_<CC::ReturnType::CppString, wchar_t, char>, _fn_<CC::ReturnType::CppString, wchar_t, wchar_t>

// Add 216 Tests (3 Returns Types x 4 Bases x 9 Types X 2 Char Types)
#define ADD_STR_UTIL_TESTS_RT_BT_T_C(_fn_)\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       int8_t,   char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       uint8_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       int16_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       uint16_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       int32_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       uint32_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       int64_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       uint64_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       void*,    char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Binary,       void*,    wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        int8_t,   char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        uint8_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        int16_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        uint16_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        int32_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        uint32_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        int64_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        uint64_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        void*,    char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Octal,        void*,    wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      int8_t,   char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      uint8_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      int16_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      uint16_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      int32_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      uint32_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      int64_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      uint64_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      void*,    char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Decimal,      void*,    wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  int8_t,   char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  uint8_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  int16_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  uint16_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  int32_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  uint32_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  int64_t,  char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  uint64_t, char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  void*,    char>, _fn_<CC::ReturnType::CCBuffer,  CC::Base::Hexadecimal,  void*,    wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       int8_t,   char>, _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       uint8_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       int16_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       uint16_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       int32_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       uint32_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       int64_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       uint64_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       void*,    char>, _fn_<CC::ReturnType::CCString,  CC::Base::Binary,       void*,    wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        int8_t,   char>, _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        uint8_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        int16_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        uint16_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        int32_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        uint32_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        int64_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        uint64_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        void*,    char>, _fn_<CC::ReturnType::CCString,  CC::Base::Octal,        void*,    wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      int8_t,   char>, _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      uint8_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      int16_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      uint16_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      int32_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      uint32_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      int64_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      uint64_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      void*,    char>, _fn_<CC::ReturnType::CCString,  CC::Base::Decimal,      void*,    wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  int8_t,   char>, _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  uint8_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  int16_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  uint16_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  int32_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  uint32_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  int64_t,  char>, _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  uint64_t, char>, _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  void*,    char>, _fn_<CC::ReturnType::CCString,  CC::Base::Hexadecimal,  void*,    wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Binary,       int8_t,   char>, _fn_<CC::ReturnType::CppString, CC::Base::Binary,       int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Binary,       uint8_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Binary,       uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Binary,       int16_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Binary,       int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Binary,       uint16_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Binary,       uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Binary,       int32_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Binary,       int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Binary,       uint32_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Binary,       uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Binary,       int64_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Binary,       int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Binary,       uint64_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Binary,       uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Binary,       void*,    char>, _fn_<CC::ReturnType::CppString, CC::Base::Binary,       void*,    wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Octal,        int8_t,   char>, _fn_<CC::ReturnType::CppString, CC::Base::Octal,        int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Octal,        uint8_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Octal,        uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Octal,        int16_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Octal,        int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Octal,        uint16_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Octal,        uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Octal,        int32_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Octal,        int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Octal,        uint32_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Octal,        uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Octal,        int64_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Octal,        int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Octal,        uint64_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Octal,        uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Octal,        void*,    char>, _fn_<CC::ReturnType::CppString, CC::Base::Octal,        void*,    wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      int8_t,   char>, _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      uint8_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      int16_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      uint16_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      int32_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      uint32_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      int64_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      uint64_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      void*,    char>, _fn_<CC::ReturnType::CppString, CC::Base::Decimal,      void*,    wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  int8_t,   char>, _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  int8_t,   wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  uint8_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  uint8_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  int16_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  int16_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  uint16_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  uint16_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  int32_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  int32_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  uint32_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  uint32_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  int64_t,  char>, _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  int64_t,  wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  uint64_t, char>, _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  uint64_t, wchar_t>,\
    _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  void*,    char>, _fn_<CC::ReturnType::CppString, CC::Base::Hexadecimal,  void*,    wchar_t>


std::list<CC::StringUtilTests::UTFunc> CC::StringUtilTests::GetTests()
{
#pragma warning(suppress : 6262) // Suppress high stack memory usage warning.
    static const std::list<std::function<UTR(void)>> tests
    {
        // Enum Validator Tests
        EnumValidatorTests::InvalidOperationType,
        EnumValidatorTests::ValidOperationType,
        EnumValidatorTests::InvalidEarlyExitResult,
        EnumValidatorTests::ValidEarlyExitResult,
        EnumValidatorTests::InvalidReturnType,
        EnumValidatorTests::ValidReturnType,
        EnumValidatorTests::InvalidBaseType,
        EnumValidatorTests::ValidBaseType,

        // Build Buffer Tests
        ADD_STR_UTIL_TESTS_RT_C(BuildBufferTests::ZeroLength),
        ADD_STR_UTIL_TESTS_RT_C(BuildBufferTests::OneLength),
        ADD_STR_UTIL_TESTS_RT_C(BuildBufferTests::ManyLength),

        // Get Length Tests
        ADD_STR_UTIL_TESTS_C(GetLengthTests::NullptrArg),
        ADD_STR_UTIL_TESTS_C(GetLengthTests::ZeroLength),
        ADD_STR_UTIL_TESTS_C(GetLengthTests::OneLength),
        ADD_STR_UTIL_TESTS_C(GetLengthTests::ManyLength),

        // Comparison Early-Exit Tests
        ADD_STR_UTIL_TESTS_C(ComparisonEarlyExitTests::SameLengthDifferentStrs),
        ADD_STR_UTIL_TESTS_C(ComparisonEarlyExitTests::SameStr),
        ADD_STR_UTIL_TESTS_C(ComparisonEarlyExitTests::LengthMismatch),
        ADD_STR_UTIL_TESTS_C(ComparisonEarlyExitTests::BothZeroLength),
        ADD_STR_UTIL_TESTS_C(ComparisonEarlyExitTests::OneNullptr),
        ADD_STR_UTIL_TESTS_C(ComparisonEarlyExitTests::BothNullptr),
        ADD_STR_UTIL_TESTS_C(ComparisonEarlyExitTests::SamePtr),

        // Comparison Tests
        ADD_STR_UTIL_TESTS_C(ComparisonTests::LengthMismatch),
        ADD_STR_UTIL_TESTS_C(ComparisonTests::BothZeroLength),
        ADD_STR_UTIL_TESTS_C(ComparisonTests::OneNullptr),
        ADD_STR_UTIL_TESTS_C(ComparisonTests::BothNullptr),
        ADD_STR_UTIL_TESTS_C(ComparisonTests::SamePtr),
        ADD_STR_UTIL_TESTS_C(ComparisonTests::CaseInsensitiveNoMatch),
        ADD_STR_UTIL_TESTS_C(ComparisonTests::CaseInsensitiveDifferentCasing),
        ADD_STR_UTIL_TESTS_C(ComparisonTests::CaseInsensitiveMatch),
        ADD_STR_UTIL_TESTS_C(ComparisonTests::CaseSensitiveNoMatch),
        ADD_STR_UTIL_TESTS_C(ComparisonTests::CaseSensitiveDifferentCasing),
        ADD_STR_UTIL_TESTS_C(ComparisonTests::CaseSensitiveMatch),

        // Copy Early-Exit Tests
        ADD_STR_UTIL_TESTS_RT_C(CopyEarlyExitTests::Nullptr),
        ADD_STR_UTIL_TESTS_RT_C(CopyEarlyExitTests::ZeroLength),
        ADD_STR_UTIL_TESTS_RT_C(CopyEarlyExitTests::OneLength),
        ADD_STR_UTIL_TESTS_RT_C(CopyEarlyExitTests::ManyLength),

        // Copy Tests
        ADD_STR_UTIL_TESTS_RT_C(CopyTests::Nullptr),
        ADD_STR_UTIL_TESTS_RT_C(CopyTests::ZeroLength),
        ADD_STR_UTIL_TESTS_RT_C(CopyTests::OneLength),
        ADD_STR_UTIL_TESTS_RT_C(CopyTests::ManyLength),

        // UTF Conversion Early-Exit Tests
        ADD_STR_UTIL_TESTS_RT_C(UTFConversionEarlyExitTests::Nullptr),
        ADD_STR_UTIL_TESTS_RT_C(UTFConversionEarlyExitTests::ZeroLength),
        ADD_STR_UTIL_TESTS_RT_C(UTFConversionEarlyExitTests::OneLength),
        ADD_STR_UTIL_TESTS_RT_C(UTFConversionEarlyExitTests::ManyLength),

        // UTF Conversion Tests
        ADD_STR_UTIL_TESTS_RT_C2(UTFConversionTests::Nullptr),
        ADD_STR_UTIL_TESTS_RT_C2(UTFConversionTests::ZeroLength),
        ADD_STR_UTIL_TESTS_RT_C2(UTFConversionTests::OneLength),
        ADD_STR_UTIL_TESTS_RT_C2(UTFConversionTests::ManyLength),

        // Number Conversion Tests
        ADD_STR_UTIL_TESTS_RT_BT_T_C(NumberConversionTests::NumberType)
    };

    return tests;
}
