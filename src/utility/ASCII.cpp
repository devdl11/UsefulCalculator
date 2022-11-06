#include "ASCII.h"
#include <math.h>
#include <cstring>

namespace ASCII {

    CharType getCharType(const char & c) {
        // i We just use the ascii table
        if (c <= '\'') {
            return CharType::Other;
        } else if (c <= ')') {
            return CharType::SimpleParenthesis; 
        } else if (c == ',') {
            return CharType::Comma;
        } else if (c == '.') {
            return CharType::Dot;
        } else if (c <= '/') {
            return CharType::MathSymbol;
        } else if (c <= '9') {
            return CharType::Digit;
        } else if (c <= ';') {
            return CharType::Other;
        } else if (c <= '>') {
            return CharType::BoolSymbol;
        } else if (c <= '@') {
            return CharType::Other;
        } else if (c <= 'Z') {
            return CharType::Letter;
        } else if (c < 'a') {
            return CharType::Other;
        } else if (c <= 'z') {
            return CharType::Letter;
        } else {
            return CharType::Other;
        }
    }

    ParenthesisType getParenthesisType(const char & c) {
        // ! We suppose that the input is a simple parenthesis !
        if (c == ')') {
            return ParenthesisType::Close;
        } else {
            return ParenthesisType::Open;
        }
    }

    char toLower(const char & c) {
        if (c > 'Z') {
            return c;
        } else {
            return c - 'A' + 'a';
        }
    }

    char toUpper(const char & c) {
        if (c < 'a') {
            return c;
        } else {
            return c - 'a' + 'A';
        }
    }

    int toInt(std::string s) {
        CharType first_char = getCharType(s[0]);
        bool isPositive = true;
        int result = 0;
        if (first_char == CharType::MathSymbol && s[0] == '-') {
            isPositive = false;
            s.erase(s.begin());
        } else if (first_char != CharType::Digit) {
            s.erase(s.begin());
        }

        // Normally this function isn't safe, but we already took care of that with the lexer
        size_t max = strlen(s.c_str());
        for (size_t index = 0; index < max; index ++) {
            result += (s[index] - '0') * pow(10, max - index - 1);
        }

        return isPositive ? result : -result;
    }

    double toDouble(std::string s) {
        CharType first_char = getCharType(s[0]);
        bool isPositive = true;
        double result = 0;
        if (first_char == CharType::MathSymbol && s[0] == '-') {
            isPositive = false;
            s.erase(s.begin());
        } else if (first_char != CharType::Digit) {
            s.erase(s.begin());
        }

        double max = strlen(s.c_str());
        double sepIndex = s.find('.');
        bool hasSep = true;

        if (sepIndex == std::string::npos) {
            sepIndex = s.find(',');
        }

        if (sepIndex == std::string::npos && s[0] != '.' && s[0] != ',') {
            hasSep = false;
        }

        for (double index = 0; index < max; index ++) {
            if (hasSep) {
                if (index == sepIndex) {
                    result *= 0.1;
                    continue;
                }
                result += (s[index] - '0') * pow(10, sepIndex - index);
            } else {
                result += (s[index] - '0') * pow(10, max - index - 1);
            }
        }

        return isPositive ? result : -result;
    }

}
