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
    bool hasParen = false;

    int mathCount = 0;
    int sepCount = 0;
    int parenCount = 0;

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
        } else if (t == ASCII::CharType::SimpleParenthesis) {
            hasParen = true;
            parenCount ++;
        }

        m_chars.push_back(t);
    }

    if (hasParen && parenCount == 1 && m_chars.size() == 1) {
        m_type = TokenType::PARENTHESES_TYPE;
    } else if (hasLetters && !hasMaths && !hasOther && !hasSep && !hasBool && !hasParen && (!hasDigits || m_chars[0] == ASCII::CharType::Letter)) {
        m_type = TokenType::STR_TYPE;
    } else if (hasDigits && !hasLetters && !hasOther && !hasSep && !hasBool && !hasParen && (!hasMaths || (m_chars[0] == ASCII::CharType::MathSymbol && mathCount == 1))) {
        m_type = TokenType::INT_TYPE;
    } else if (hasDigits && !hasLetters && !hasOther && hasSep && sepCount == 1 && !hasBool && !hasParen && (!hasMaths || (m_chars[0] == ASCII::CharType::MathSymbol && mathCount == 1))) {
        m_type = TokenType::DOUBLE_TYPE;
    } else if (hasMaths && !hasLetters && !hasDigits && !hasOther && !hasSep && !hasBool && !hasParen && m_chars.size() == 1) {
        m_type = TokenType::MATH_TYPE;
    } else {
        m_type = TokenType::INVALID_TYPE;
    }
}

bool AdvancedToken::hasMeta(TokenMetaType t) {
    for (TokenMeta m : m_meta) {
        if (m.type == t) {
            return true;
        }
    }
    return false;
}

void AdvancedToken::addMeta(TokenMetaType t, int v) {
    for (std::vector<TokenMeta>::iterator it = m_meta.begin(); it != m_meta.end(); it++) {
        if (it->type == t) {
            it->value = v;
            return;
        }
    }
    m_meta.push_back({t, v});
}

int AdvancedToken::getMeta(TokenMetaType t) {
    for (TokenMeta m : m_meta) {
        if (m.type == t) {
            return m.value;
        }
    }
    return -1;
}

// We need to create a destructor in order to delete the tokens we created
Expression::~Expression() {
    size_t size = m_tokens.size();
    for (size_t s = 0; s < size; s++) {
        Token * t = m_tokens.back();
        m_tokens.pop_back();
        delete t;
    }
    // ! Once we deleted the tokens, we must not use the expression object anymore !!
    // ? Maybe add an integrity check to avoid this ?
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
    size_t index = 0;
    size_t end = input.size();

    while (index < end) {
        std::string::iterator my_iter = input.begin() + index;
        if (*my_iter == ' ' and *(my_iter + 1) == ' ') {
            input.erase(my_iter);
        } else if (ASCII::getCharType(*my_iter) == ASCII::CharType::SimpleParenthesis) {
            // We need to treat the parenthesis as a single token
            if (*(my_iter - 1) != ' ') {
                input.insert(my_iter, ' ');
                index++;
                end++;
                my_iter = input.begin() + index;
            }
            if (*(my_iter + 1) != ' ') {
                input.insert(my_iter + 1, ' ');
                end ++;
                index ++;
            }
            index ++;
        } else {
            index ++;
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

    std::cout << input << std::endl;

    std::vector<PosiToken> token_pos = getTokensPosition(input);
    std::vector<Token *> tokens;

    bool hasParenthesis = false;
    index = 0;
    for(PosiToken token : token_pos) {
        Token * t = new Token(std::string(token.begin, token.end), index++);
        t->analyze();
        if (t->getType() == TokenType::INVALID_TYPE) {
            return Expression();
        } else if (t->getType() == TokenType::PARENTHESES_TYPE) {
            delete t;
            AdvancedToken * at = new AdvancedToken(std::string(token.begin, token.end), index - 1);
            tokens.push_back(at);
            hasParenthesis = true;
            tokens.back()->analyze();
        } else {
            tokens.push_back(t);
        }
    }

    if (hasParenthesis) {
        // We need to check if the parenthesis are balanced
        int parenthesisCount = 0;
        int depth = 0;
        for (Token * t : tokens) {
            if (t->getType() != TokenType::PARENTHESES_TYPE) continue;
            if (ASCII::getParenthesisType(t->getString()[0]) == ASCII::ParenthesisType::Open) {
                parenthesisCount ++;
                static_cast<AdvancedToken *>(t)->addMeta(TokenMetaType::DEPTH, depth);
                depth ++;
            } else {
                parenthesisCount --;
                static_cast<AdvancedToken *>(t)->addMeta(TokenMetaType::DEPTH, depth);
                depth --;
            }
        }

        if (parenthesisCount != 0) {
            return Expression();
        }
    }

    return Expression(tokens, hasParenthesis);
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
