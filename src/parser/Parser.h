#ifndef PARSER_H
#define PARSER_H

#include <stdexcept>
#include <map>

#include "Lexer.h"
#include "../utility/Tools.h"

enum OperationType {
    ADD,
    SUB,
    MUL,
    DIV  
};

namespace {
    // i Here we define our commands
    bool hasPriority(const char & c);
    OperationType getOperationType(const char & c);
    double executeMathOperation(double a, double b, OperationType t);
    std::string executeStringOperation(std::string a, std::string b, OperationType t);
}

class Command {
    public:
        Command(std::string name, int argc, std::vector<TokenType> types) : m_name(name), m_argc(argc), m_argTypes(types), m_counter(0), m_canExecute(false) {}

        const std::string & getName() { return m_name; }
        int getArgc() { return m_argc; }

        bool checkArgs(std::vector<Token *> args);
        
        template<typename T>
        void convertArg(T & dest, Token * arg) {
            // i We have to define our function here because it is a template
            if (m_counter >= m_argc) {
                throw std::runtime_error("[Parser/Command/convertArg] counter is out of range !");
            } else {
                privateConvertArg(dest, arg);
            }
        }
        
        void resetCounter() { m_counter = 0; }

        virtual Token handler(std::vector<Token *> & args);

    private:
        TokenType getCurrentType() { return m_argTypes.at(m_counter++); }

        template<typename T>
        void privateConvertArg(T & dest, Token * arg) {
            throw std::runtime_error("[Parser/Command/convertArg] argument type is not supported !");
        }

        void privateConvertArg(std::string & dest, Token * arg);
        void privateConvertArg(int & dest, Token * arg);
        void privateConvertArg(double & dest, Token * arg);


        const std::string m_name;
        const int m_argc;
        const std::vector<TokenType> m_argTypes;
        size_t m_counter;

    protected:
        bool m_canExecute;
};

class Parser {
    public:
        Parser() {};

        ~Parser();

        void addCommand(Command * cmd);
        void parseCommands(std::vector<Token *> & tokens);
        void parseExpression(Expression & exp);
        
    private:
        bool isCommand(std::string name, size_t * index = nullptr);
        void scanForCommands(std::vector<Token *> & tokens);
        void resetScannedCommands(std::vector<Token *> & tokens);

        std::vector<Command *> m_commands;
        std::vector<const std::string *> m_commandsNames;

};


#endif