#ifndef COMMANDS_LOWER_H
#define COMMANDS_LOWER_H

#include "../Parser.h"

namespace Commands {
    class Lower : public Command {
        public:
            Lower() : Command("lower", 1, {TokenType::STR_TYPE}) {}

            virtual Token handler(std::vector<Token *> & args) override;
        
        private:
            std::string lower(std::string str);
    };
}

#endif
