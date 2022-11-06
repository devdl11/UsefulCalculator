#ifndef ASCII_H
#define ASCII_H

#include <string>

namespace ASCII
{
    enum CharType {
        Letter,
        Digit,
        MathSymbol,
        Comma,
        Dot,
        BoolSymbol,
        SimpleParenthesis,
        Other
    };

    enum ParenthesisType {
        Open,
        Close
    };

    CharType getCharType(const char & c);
    ParenthesisType getParenthesisType(const char & c);

    char toUpper(const char & c);
    char toLower(const char & c);

    // ! We suppose that the input is valid because of the TokenType checking !
    int toInt(std::string s);
    double toDouble(std::string s);

} // namespace ASCII


#endif
