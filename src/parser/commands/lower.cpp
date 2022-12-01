#include "lower.h"

#include "../../utility/ASCII.h"

std::string Commands::Lower::lower(std::string str) {
    std::string result = "";
    for (std::string::iterator current = str.begin(); current < str.end(); current++) {
        result += ASCII::toLower(*current);
    }
    return result;
}

Token Commands::Lower::handler(std::vector<Token *> & args) {
    Command::handler(args);
    if (!m_canExecute) {
        return Token();
    }
    std::string str = "";
    convertArg<std::string>(str, args.at(0));
    return Token(lower(str), -2);
}
