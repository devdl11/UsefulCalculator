#include "Lexer.h"

#include <iostream>
#include <string>
#include <cstring>

#include "../static/text.h"
#include "../utility/ASCII.h"

void Token::analyze() {
    m_chars.clear();

    // We define some booleans to avoid another fetch over m_chars

    bool hasLetters = false;
    bool hasDigits = false;
    bool hasMaths = false;
    bool hasOther = false;
    bool hasSep = false;
    bool hasBool = false;

    int mathCount = 0;
    int sepCount = 0;

    for (std::string::const_iterator current = m_str.begin(); current < m_str.end(); current++) {
        ASCII::CharType t = ASCII::getCharType(*current);
        if (t == ASCII::CharType::Digit) {
            hasDigits = true;
        } else if (t == ASCII::CharType::Letter) {
            hasLetters = true;
        } else if (t == ASCII::CharType::Other) {
            hasOther = true;
        } else if (t == ASCII::CharType::BoolSymbol) {
            hasBool = true;
        } else if (t == ASCII::CharType::MathSymbol) {
            hasMaths = true;
            mathCount ++;
        } else if (t == ASCII::CharType::Comma || t == ASCII::CharType::Dot) {
            hasSep = true;
            sepCount ++;
        } 

        m_chars.push_back(t);
    }

    if (hasLetters && !hasMaths && !hasOther && !hasSep && !hasBool && (!hasDigits || m_chars[0] == ASCII::CharType::Letter)) {
        m_type = TokenType::STR_TYPE;
    } else if (hasDigits && !hasLetters && !hasOther && !hasSep && !hasBool && (!hasMaths || (m_chars[0] == ASCII::CharType::MathSymbol && mathCount == 1))) {
        m_type = TokenType::INT_TYPE;
    } else if (hasDigits && !hasLetters && !hasOther && hasSep && sepCount == 1 && !hasBool && (!hasMaths || (m_chars[0] == ASCII::CharType::MathSymbol && mathCount == 1))) {
        m_type = TokenType::DOUBLE_TYPE;
    } else if (hasMaths && !hasLetters && !hasDigits && !hasOther && !hasSep && !hasBool && m_chars.size() == 1) {
        m_type = TokenType::MATH_TYPE;
    } else {
        m_type = TokenType::INVALID_TYPE;
    }
}

Expression Lexer::getInput() {
    std::string input;

    do {
        if (strnlen(input.c_str(), MAX_LEN) > 0) {
            std::cout << TEXT::ERR_INPUT_LEN << std::endl;
        }

        std::cout << m_prompt;

        //* We need to use getline() instead of cin, because cin will only read the first word of the input.
        std::getline(std::cin, input);

    } while (strnlen(input.c_str(), MAX_LEN) >= MAX_LEN);

    // Let's filter our input by removing extra whitespaces
    std::string::iterator my_iter = input.begin();

    while (my_iter < input.end()) {
        if (*my_iter == ' ' and *(my_iter + 1) == ' ') {
            input.erase(my_iter);
        } else {
            my_iter ++;
        }
    }

    // If our input begins with a whitespace, we remove it
    if (*input.begin() == ' ') {
        input.erase(input.begin());
    }

    // Same if it ends with it.
    if (*(input.end() - 1) == ' ') {
        input.erase(input.end() - 1);
    }

    std::vector<PosiToken> token_pos = getTokensPosition(input);
    std::vector<Token> tokens;

    int index = 0;
    for(PosiToken token : token_pos) {
        tokens.push_back(Token(std::string(token.begin, token.end), index++));
        tokens.back().analyze();
        if (tokens.back().getType() == TokenType::INVALID_TYPE) {
            return Expression();
        }
    }

    return Expression(tokens);
}

std::vector<PosiToken> Lexer::getTokensPosition(std::string & input) {
    std::vector<PosiToken> result;

    std::string::iterator current_pos = input.begin();
    std::string::iterator begin = current_pos;

    // ~ Can be optimized

    while (current_pos < input.end()) {
        if (*current_pos == ' ') {
            result.push_back({begin, current_pos++});
            begin = current_pos;
        }
        current_pos++;
    }
    result.push_back({begin, current_pos});
    return result;
}
