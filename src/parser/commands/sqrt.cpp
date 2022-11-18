#include "sqrt.h"

#include <math.h>

double Commands::Sqrt::sqrt(double x) {
    return std::sqrt(x);
}

Token Commands::Sqrt::handler(std::vector<Token *> & args) {
    Command::handler(args);
    if (!m_canExecute) {
        return Token();
    }
    double number = 0;
    convertArg<double>(number, args.at(0));
    return Token(std::to_string(sqrt(number)), -2);
}
