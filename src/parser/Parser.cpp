#include "Parser.h"
#include <iostream>
#include <math.h>

#include "../utility/ASCII.h"
#include "../static/text.h"

namespace Parser {
    namespace {
        void upper(std::string s) {
            std::string res;
            for(char c : s) {
                res.push_back(ASCII::toUpper(c));
            }
            std::cout << res << std::endl;
        }

        void lower(std::string s) {
            std::string res;
            for(char c : s) {
                res.push_back(ASCII::toLower(c));
            }
            std::cout << res << std::endl;
        }

        void int_power(int n, int p) {
            std::cout << pow(n,p) << std::endl;
        }

        void double_power(double n, int p) {
            std::cout << pow(n, p) << std::endl;
        }

        void double_sqrt(double n) {
            std::cout << sqrt(n) << std::endl;
        }

        bool executeCommand(Expression & ex) {
            const std::vector<Token *> tokens = ex.getTokens();

            std::string command = tokens.front()->getString();
            if (command == "upper") {
                if (tokens.size() < 2) {
                    std::cout << TEXT::ERR_FEW_ARGUMENTS << std::endl;
                    return true;
                }
                Token * argument = tokens[1];
                if (argument->getType() == TokenType::STR_TYPE) {
                    upper(argument->getString());
                    return true;
                }
            } else if (command == "lower") {
                if (tokens.size() < 2) {
                    std::cout << TEXT::ERR_FEW_ARGUMENTS << std::endl;
                    return true;
                }
                Token * argument = tokens[1];
                if (argument->getType() == TokenType::STR_TYPE) {
                    lower(argument->getString());
                    return true;
                }
            } else if (command == "pow") {
                if (tokens.size() < 3) {
                    std::cout << TEXT::ERR_FEW_ARGUMENTS << std::endl;
                    return true;
                }
                Token * number = tokens[1];
                Token * power = tokens[2];
                if (power->getType() != TokenType::INT_TYPE) {
                    std::cout << "! Invalid power type !" << std::endl;
                    return true;
                } else if (number->getType() != TokenType::INT_TYPE and number->getType() != TokenType::DOUBLE_TYPE) {
                    return false;
                } else if (number->getType() == TokenType::INT_TYPE) {
                    int conv_power = ASCII::toInt(power->getString());
                    int conv_number = ASCII::toInt(number->getString());
                    int_power(conv_number, conv_power);
                    return true;
                } else {
                    int conv_power = ASCII::toInt(power->getString());
                    double conv_number = ASCII::toDouble(number->getString());
                    double_power(conv_number, conv_power);
                    return true;
                }
            } else if (command == "sqrt") {
                if (tokens.size() < 2) {
                    std::cout << TEXT::ERR_FEW_ARGUMENTS << std::endl;
                    return true;
                }
                Token * number = tokens[1];
                if (number->getType() != TokenType::INT_TYPE and number->getType() != TokenType::DOUBLE_TYPE) {
                    return false;
                }
                double conv_number = ASCII::toDouble(number->getString());
                if (conv_number < 0) {
                    std::cout << " ! Can't square a negative number !";
                    return true;
                }
                double_sqrt(conv_number);
                return true;
            }
            return false;
        }

        OperationType getOperationType(const char & c) {
            switch (c)
            {
            case '+':
                return OperationType::ADD;
            case '-':
                return OperationType::SUB;
            case '/':
                return OperationType::DIV;
            default:
                return OperationType::MUL;
            }
        }

        double executeOperation(double a, double b, OperationType t) {
            switch (t)
            {
            case OperationType::ADD:
                return a + b;
            case OperationType::SUB:
                return a - b;
            case OperationType::MUL:
                return a * b;
            case OperationType::DIV:
                return a / b;
            default:
                return 0;
            }
        }

        bool hasPriority(const char & c) {
            if (c == '*' or c == '/') {
                return true;
            } else {
                return false;
            }
        }
    }

    void parseExpression(Expression & exp) {
        // We first check if the expression contains a command
        if(executeCommand(exp)) {
            return;
        }
        
        std::vector<Token *> tokens = std::vector<Token *>(exp.getTokens());

        // Let's remove extra math character
        if (tokens.front()->getType() == TokenType::MATH_TYPE) {
            tokens.erase(tokens.begin());
        }
        if (tokens.back()->getType() == TokenType::MATH_TYPE) {
            tokens.pop_back();
        }

        int lastIndex = tokens.back()->getIndex() + 1;
        
        tokens.insert(tokens.begin(), new AdvancedToken("(", lastIndex++));
        tokens.front()->analyze();
        static_cast<AdvancedToken *>(tokens.front())->addMeta(TokenMetaType::DEPTH, 0);

        tokens.push_back(new AdvancedToken(")", lastIndex++));
        tokens.back()->analyze();
        static_cast<AdvancedToken *>(tokens.back())->addMeta(TokenMetaType::DEPTH, 0);

        // Now, we will use the parenthesis to detect "sub-expressions"

        std::vector<size_t> paraStack = std::vector<size_t>();

        double result = 0;
        size_t index = 0;

        while(tokens.size() > 1) {
            if (tokens[index]->getType() == TokenType::PARENTHESES_TYPE && ASCII::getParenthesisType(tokens[index]->getString()[0]) == ASCII::ParenthesisType::Open) {
                paraStack.push_back(index);
            } else if (tokens[index]->getType() == TokenType::PARENTHESES_TYPE && ASCII::getParenthesisType(tokens[index]->getString()[0]) == ASCII::ParenthesisType::Close) {
                size_t openIndex = paraStack.back();
                paraStack.pop_back();
                std::vector<Token *> subTokens = std::vector<Token *>(tokens.begin() + openIndex + 1, tokens.begin() + index);
                tokens.erase(tokens.begin() + openIndex, tokens.begin() + index + 1);
                

                int priorityCount = 0;

                // Let's check now if it's a correct math input
                bool mustBeMath = false;
                std::cout << "Subtokens: " << std::endl;

                for(Token * t : subTokens) {
                    std::cout << t->getString() << std::endl;
                    // i We can combine all the if in a unique expression, but it will be too ugly for our eyes x)
                    if (mustBeMath && t->getType() != TokenType::MATH_TYPE) {
                        std::cout << TEXT::ERR_INVALID_MATH << std::endl;
                        return;
                    } else if (!mustBeMath && t->getType() != TokenType::DOUBLE_TYPE && t->getType() != TokenType::INT_TYPE) {
                        std::cout << TEXT::ERR_INVALID_MATH << std::endl;
                        return;
                    } else if (t->getType() == TokenType::MATH_TYPE && hasPriority(t->getString()[0])) {
                        priorityCount ++;
                    }
                    mustBeMath = !mustBeMath;
                }

                int subindex = -1;
                while (subTokens.size() > 1) {
                    std::cout << "Solving sub-expression : ";
                    for (Token * t : subTokens) {
                        std::cout << t->getString() << " ";
                    }
                    std::cout << std::endl;

                    for (Token * t : subTokens) {
                        subindex ++;
                        if (t->getType() != TokenType::MATH_TYPE) continue;
                        if (priorityCount > 0) {
                            if (!hasPriority(t->getString()[0])) continue;
                            priorityCount --;
                        }
                        Token * first = subTokens[subindex - 1];
                        Token * second = subTokens[subindex + 1];
                        OperationType op = getOperationType(subTokens[subindex]->getString()[0]);
                        result = executeOperation(ASCII::toDouble(first->getString()), ASCII::toDouble(second->getString()), op);

                        for (char i = 0; i < 3; i ++) {
                            subTokens.erase(subTokens.begin() + subindex - 1);
                        }

                        Token * resToken = new Token(std::to_string(result), lastIndex++);
                        resToken->analyze();
                        subTokens.insert(subTokens.begin() + subindex - 1, resToken);
                        break;
                    }
                    subindex = -1;
                }
                std::cout << "Result : " << result << std::endl;

                tokens.insert(tokens.begin() + openIndex, subTokens.back());
                if (openIndex > 0 and tokens.size() > 1 and (tokens[openIndex]->getType() == TokenType::INT_TYPE or tokens[openIndex]->getType() == TokenType::DOUBLE_TYPE) and (tokens[openIndex-1]->getType() == TokenType::INT_TYPE or tokens[openIndex-1]->getType() == TokenType::DOUBLE_TYPE)) {
                    tokens.insert(tokens.begin() + openIndex, new Token("*", lastIndex++));
                    tokens[openIndex]->analyze();
                }
                index = openIndex;
            }
            index ++;
        }

        std::cout << result << std::endl;
    }
}


