#include "Parser.h"
#include <iostream>
#include <math.h>
#include <string.h>

#include "../utility/ASCII.h"
#include "../static/text.h"

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

    bool executeCommand(Expression & ex) {
        const std::vector<Token *> tokens = ex.getTokens();

        std::string command = tokens.front()->getString();
        if (command == "upper") {
            if (tokens.size() < 2) {
                std::cout << TEXT::ERR_FEW_ARGUMENTS << std::endl;
                return true;
            }
            Token * argument = tokens.at(1);
            if (argument->getType() == TokenType::STR_TYPE) {
                upper(argument->getString());
                return true;
            }
        } else if (command == "lower") {
            if (tokens.size() < 2) {
                std::cout << TEXT::ERR_FEW_ARGUMENTS << std::endl;
                return true;
            }
            Token * argument = tokens.at(1);
            if (argument->getType() == TokenType::STR_TYPE) {
                lower(argument->getString());
                return true;
            }
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

    double executeMathOperation(double a, double b, OperationType t) {
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

    std::string executeStringOperation(std::string a, std::string b, OperationType t) {
        std::string res = "";
        switch (t)
        {
        case OperationType::ADD:
            return a + b;
        case OperationType::SUB:
            if (a.rfind(b) != std::string::npos) {
                return a.substr(0, a.rfind(b)) + a.substr(a.rfind(b) + b.size());
            } else {
                return a;
            }
        case OperationType::MUL:
            for (char c : a) {
                res += c + b;
            }
            return res;
        case OperationType::DIV:
            while (a.rfind(b) != std::string::npos) {
                a = a.substr(0, a.rfind(b)) + a.substr(a.rfind(b) + b.size());
            }
            return a;
        default:
            return "";
        }
    }

    bool hasPriority(const char & c) {
        return c == '*' or c == '/';
    }
}

bool Command::checkArgs(std::vector<Token *> args) {
    if (args.size() != m_argc) {
        std::cout << TEXT::ERR_FEW_ARGUMENTS << std::endl;
        return false;
    }
    for (size_t i = 0; i < m_argc; i++) {
        TokenType type = args.at(i)->getType();
        TokenType expected = m_argTypes.at(i);

        if (expected >= type and expected < 3) continue;

        std::cout << TEXT::ERR_INVALID_ARGUMENT << std::endl;
        return false;
    }
    return true;
}

Token Command::handler(std::vector<Token *> & args) {
    if (not checkArgs(args)) {
        m_canExecute = false;
        return Token();
    }
    resetCounter();
    m_canExecute = true;
    return Token();
}

void Command::privateConvertArg(std::string & dest, Token * token) {
    m_counter++;
    dest = token->getString();
}

void Command::privateConvertArg(int & dest, Token * token) {
    m_counter++;
    dest = ASCII::toInt(token->getString());
}

void Command::privateConvertArg(double & dest, Token * token) {
    m_counter++;
    dest = ASCII::toDouble(token->getString());
}


Parser::~Parser() {
    for (Command * c : m_commands) {
        delete c;
    }
    m_commands.clear();
    m_commandsNames.clear();
}

void Parser::addCommand(Command * c) {
    m_commands.push_back(c);
    m_commandsNames.push_back(&c->getName());
}

bool Parser::isCommand(std::string name, size_t * index) {
    for (std::vector<const std::string *>::iterator it = m_commandsNames.begin(); it != m_commandsNames.end(); it++) {
        if (strncmp((**it).c_str(), name.c_str(), (**it).size()) == 0) {
            if (index != nullptr) {
                *index = it - m_commandsNames.begin();   
            }
            return true;
        }
    }
    if (index != nullptr) {
        *index = -1;
    }
    return false;
}

void Parser::scanForCommands(std::vector<Token *> & tokens) {
    size_t index = -1;
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens.at(i)->getType() == TokenType::STR_TYPE && isCommand(tokens.at(i)->getString(), &index)) {
            AdvancedToken * at = new AdvancedToken(tokens.at(i)->getString(), tokens.at(i)->getIndex());
            at->analyze();
            at->setType(TokenType::COMMAND_TYPE);
            at->addMeta(TokenMetaType::COMMAND, index);
            tokens.erase(tokens.begin() + i);
            tokens.insert(tokens.begin() + i, at);
        }
    }
}

void Parser::resetScannedCommands(std::vector<Token *> & tokens) {
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens.at(i)->getType() == TokenType::COMMAND_TYPE) {
            tokens.at(i)->setType(TokenType::STR_TYPE);
        }
    }
}

void Parser::parseCommands(std::vector<Token *> & tokens) {
    size_t end = tokens.size();
    size_t i = 0;

    while (i < end) {
        Token * token = tokens.at(i);
        if (token->getType() != TokenType::COMMAND_TYPE) {
            i++;
            continue;
        }
        AdvancedToken * at = (AdvancedToken *)token;
        Command * command = m_commands.at(at->getMeta(TokenMetaType::COMMAND));

        if (end - i >= command->getArgc()) {
            std::vector<Token *> args;
            for (size_t j = 0; j < command->getArgc(); j++) {
                args.push_back(tokens.at(i + j + 1));
            }
            Token result = command->handler(args);
            if (result.getIndex() == -1) {
                i++;
                continue;
            }
            tokens.erase(tokens.begin() + i, tokens.begin() + i + command->getArgc() + 1);
            end -= command->getArgc();
            tokens.insert(tokens.begin() + i, new Token(result));
            tokens.at(i)->analyze();
            if (i > 0 and tokens.at(i - 1)->getType() != TokenType::MATH_TYPE) {
                tokens.insert(tokens.begin() + i - 1, new Token("*", -2));
                tokens.at(i-1)->analyze();
                end++;
            }
        }
        
        i++;
    }

}


void Parser::parseExpression(Expression & exp) {
    // We first check if the expression contains a command
    std::vector<Token *> tokens = std::vector<Token *>(exp.getTokens());
    scanForCommands(tokens);

    if(executeCommand(exp)) {
        return;
    }

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

    bool useStringOp = false;
    std::string result = "";
    size_t index = 0;

    while(tokens.size() > 1) {
        if (tokens.at(index)->getType() == TokenType::PARENTHESES_TYPE && ASCII::getParenthesisType(tokens.at(index)->getString().at(0)) == ASCII::ParenthesisType::Open) {
            paraStack.push_back(index);
        } else if (tokens.at(index)->getType() == TokenType::PARENTHESES_TYPE && ASCII::getParenthesisType(tokens.at(index)->getString().at(0)) == ASCII::ParenthesisType::Close) {
            size_t openIndex = paraStack.back();
            paraStack.pop_back();
            std::vector<Token *> subTokens = std::vector<Token *>(tokens.begin() + openIndex + 1, tokens.begin() + index);
            tokens.erase(tokens.begin() + openIndex, tokens.begin() + index + 1);

            parseCommands(subTokens);
            resetScannedCommands(subTokens);

            size_t end = subTokens.size();
            for (size_t j = 0; j < end - 1; j++) {
                if (subTokens.at(j)->getType() == TokenType::STR_TYPE and subTokens.at(j+1)->getType() == TokenType::STR_TYPE) {
                    subTokens.insert(subTokens.begin() + j + 1, new Token("+", -2));
                    subTokens.at(j + 1)->analyze();
                    end++;
                    j +=1;
                }
            }

            // Let's remove extra math character
            if (subTokens.front()->getType() == TokenType::MATH_TYPE) {
                subTokens.erase(subTokens.begin());
            }
            if (subTokens.back()->getType() == TokenType::MATH_TYPE) {
                subTokens.pop_back();
            }
            

            int priorityCount = 0;

            // Let's check now if it's a correct math input
            bool mustBeMath = false;

            if (!useStringOp) {
                for(Token * t : subTokens) {
                    // i We can combine all the if in a unique expression, but it will be too ugly for our eyes x)
                    if (mustBeMath && t->getType() != TokenType::MATH_TYPE) {
                        useStringOp = true;
                        break;
                    } else if (!mustBeMath && t->getType() != TokenType::DOUBLE_TYPE && t->getType() != TokenType::INT_TYPE) {
                        useStringOp = true;
                        break;
                    } else if (t->getType() == TokenType::MATH_TYPE && hasPriority(t->getString().at(0))) {
                        priorityCount ++;
                    }
                    mustBeMath = !mustBeMath;
                }
            }

            
            result = subTokens.back()->getString();

            int subindex = -1;
            while (subTokens.size() > 1) {
                for (Token * t : subTokens) {
                    subindex ++;
                    if (t->getType() != TokenType::MATH_TYPE) continue;
                    if (priorityCount > 0) {
                        if (!hasPriority(t->getString().at(0))) continue;
                        priorityCount --;
                    }
                    Token * first = subTokens.at(subindex - 1);
                    Token * second = subTokens.at(subindex + 1);
                    OperationType op = getOperationType(subTokens.at(subindex)->getString().at(0));
                    result = !useStringOp ? std::to_string(executeMathOperation(ASCII::toDouble(first->getString()), ASCII::toDouble(second->getString()), op)) : executeStringOperation(first->getString(), second->getString(), op);

                    subTokens.erase(subTokens.begin() + subindex - 1, subTokens.begin() + subindex + 2);

                    Token * resToken = new Token(result, lastIndex++);
                    resToken->analyze();
                    subTokens.insert(subTokens.begin() + subindex - 1, resToken);
                    break;
                }
                subindex = -1;
            }
            tokens.insert(tokens.begin() + openIndex, subTokens.back());
            if (openIndex > 0 and tokens.size() > 1 
                and (not useStringOp ? tokens.at(openIndex)->getType() == TokenType::INT_TYPE or tokens.at(openIndex)->getType() == TokenType::DOUBLE_TYPE : tokens.at(openIndex)->getType() == TokenType::STR_TYPE)
                and (not useStringOp ? tokens.at(openIndex-1)->getType() == TokenType::INT_TYPE or tokens.at(openIndex-1)->getType() == TokenType::DOUBLE_TYPE : tokens.at(openIndex-1)->getType() == TokenType::STR_TYPE)) {
                tokens.insert(tokens.begin() + openIndex, new Token("*", lastIndex++));
                tokens.at(openIndex)->analyze();
            }
            index = openIndex;
        }
        index ++;
    }

    std::cout << result << std::endl;
}
