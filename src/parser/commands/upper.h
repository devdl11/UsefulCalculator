#ifndef COMMANDS_UPPER_H
#define COMMANDS_UPPER_H

#include "../Parser.h"

namespace Commands {
    class Upper : public Command {
        public:
            Upper() : Command("upper", 1, {TokenType::STR_TYPE}) {}

            virtual Token handler(std::vector<Token *> & args) override;
        
        private:
            std::string upper(std::string str);
    };
}

#endif
