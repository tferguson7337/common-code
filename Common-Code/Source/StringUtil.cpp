#include "../Headers/StringUtil.h"

/// Static Data Member Initialization \\\

const std::vector<SupportedStringTuple> StringUtil::NumberConversion::s_BasePrefixes
{
    MAKE_STR_TUPLE("0b"),   // binary
    MAKE_STR_TUPLE("0"),    // octal
    MAKE_STR_TUPLE("0n"),   // decimal
    MAKE_STR_TUPLE("0x"),   // hexadecimal
};


const std::vector<SupportedCharacterTuple> StringUtil::NumberConversion::s_NumberChars
{
    MAKE_CHAR_TUPLE('0'), MAKE_CHAR_TUPLE('1'), MAKE_CHAR_TUPLE('2'), MAKE_CHAR_TUPLE('3'),
    MAKE_CHAR_TUPLE('4'), MAKE_CHAR_TUPLE('5'), MAKE_CHAR_TUPLE('6'), MAKE_CHAR_TUPLE('7'),
    MAKE_CHAR_TUPLE('8'), MAKE_CHAR_TUPLE('9'), MAKE_CHAR_TUPLE('A'), MAKE_CHAR_TUPLE('B'),
    MAKE_CHAR_TUPLE('C'), MAKE_CHAR_TUPLE('D'), MAKE_CHAR_TUPLE('E'), MAKE_CHAR_TUPLE('F')
};
