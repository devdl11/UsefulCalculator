#include "upper.h"

#include "../../utility/ASCII.h"

std::string Commands::Upper::upper(std::string str) {
    std::string result = "";
    for (std::string::iterator current = str.begin(); current < str.end(); current++) {
        result += ASCII::toUpper(*current);
    }
    return result;
}

Token Commands::Upper::handler(std::vector<Token *> & args) {
    Command::handler(args);
    if (!m_canExecute) {
        return Token();
    }
    std::string str = "";
    convertArg<std::string>(str, args.at(0));
    return Token(upper(str), -2);
}
