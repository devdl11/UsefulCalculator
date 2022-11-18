#ifndef COMMANDS_POW_H
#define COMMANDS_POW_H

#include "../Parser.h"
#include <math.h>

namespace Commands {
    class Pow : public Command {
        public:
            Pow() : Command("pow", 2, {TokenType::DOUBLE_TYPE, TokenType::INT_TYPE}) {}

            virtual Token handler(std::vector<Token *> & args) override;
        
        private:
            double power(double number, int power);
    };
}

#endif