#ifndef PARSER_H
#define PARSER_H

#include <map>

#include "Lexer.h"

namespace Parser {
    enum OperationType {
        ADD,
        SUB,
        MUL,
        DIV  
    };

    namespace {
        // i Here we define our commands

        void upper(std::string s);
        void lower(std::string s);
        void int_power(int number, int power);
        void double_power(double number, int power);
        void double_sqrt(double number);
        
        bool executeCommand(Expression & exp);
        bool hasPriority(const char & c);
        OperationType getOperationType(const char & c);
        double executeOperation(double a, double b, OperationType t);
    }

    void parseExpression(Expression & exp);
}



#endif