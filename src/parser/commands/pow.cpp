#include "pow.h"

double Commands::Pow::power(double number, int power) {
    return pow(number, power);
}

Token Commands::Pow::handler(std::vector<Token *> & args) {
    Command::handler(args);
    if (!m_canExecute) {
        return Token();
    }
    double number = 0;
    int powr = 0;
    convertArg<double>(number, args.at(0));
    convertArg<int>(powr, args.at(1));
    return Token(std::to_string(power(number, powr)), -2);
}
