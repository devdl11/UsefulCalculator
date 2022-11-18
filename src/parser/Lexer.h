#ifndef LEXER_H
#define LEXER_H

#include <cstddef>
#include <vector>
#include <string>

#include "../utility/ASCII.h"

/*
    In this file we define the Lexer class, which is responsible for parsing the user's input in something 
    more comprehensible for the rest of the program.

    How it works:
    UserInput : [command] [operation]
    
    if the command isn't supplied, we treat the input as a simple math expression, and we try to evaluate it.
*/

enum TokenType {
    INT_TYPE,
    DOUBLE_TYPE,
    STR_TYPE,
    MATH_TYPE,
    PARENTHESES_TYPE,
    INVALID_TYPE
};

enum TokenMetaType {
    NONE,
    DEPTH,
};

enum TokenLevel {
    BASIC,
    ADVANCED,
};

class Token {
    public:
        Token() : Token("", -1) {};
        Token(const std::string s, int i) : m_index(i), m_str(s), m_type(TokenType::INVALID_TYPE) {}

        // i Here we analyze our token string and set our properties
        void analyze();
        virtual const TokenLevel getLevel() const { return TokenLevel::BASIC; }
        
        const std::string getString() const { return m_str; };
        TokenType getType() { return m_type; }
        int getIndex() { return m_index; }

    private:
        int m_index;
        std::string m_str;
        std::vector<ASCII::CharType> m_chars;
        TokenType m_type;
};

struct TokenMeta {
    TokenMetaType type;
    int value;
};

class AdvancedToken : public Token {
    public:
        AdvancedToken() : AdvancedToken("", -1) {};
        AdvancedToken(const std::string s, int i) : Token(s, i) {};

        virtual const TokenLevel getLevel() const override { return TokenLevel::ADVANCED; }

        void addMeta(TokenMetaType t, int v);
        bool hasMeta(TokenMetaType t);
        int getMeta(TokenMetaType t);

    private:
        std::vector<TokenMeta> m_meta;
};

struct PosiToken {
    std::string::iterator begin;
    std::string::iterator end;
};

class Expression {
    public:
        Expression() : m_isValid(false) {};
        Expression(std::vector<Token *> token_list, bool hasParen) : m_isValid(true), m_hasParentheses(hasParen), m_tokens(token_list) {}

        ~Expression();

        bool isValid() { return m_isValid; } 
        const std::vector<Token *> getTokens() { return m_tokens; }; 

    private:
        bool m_isValid;
        bool m_hasParentheses;
        std::vector<Token *> m_tokens;

};

class Lexer {
    public:
        Lexer(const char * prompt) : m_prompt(prompt) {}

        Expression getInput();
    
    private:
        std::vector<PosiToken> getTokensPosition(std::string & input);
        
        const size_t MAX_LEN = 64;
        const char * m_prompt;
};

#endif