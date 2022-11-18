#ifndef COMMANDS_SQRT_H
#define COMMANDS_SQRT_H

#include "../Parser.h"

namespace Commands {

    class Sqrt : public Command {
        public:
            Sqrt() : Command("sqrt", 1, {TokenType::DOUBLE_TYPE}) {}

            virtual Token handler(std::vector<Token *> & args) override;
            
        private:
            double sqrt(double x);
    };
}

#endif