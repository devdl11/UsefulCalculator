#include <iostream>


// Our files
#include "static/text.h"
#include "parser/Lexer.h"
#include "parser/Parser.h"

#include "parser/commands/Commands.h"

using namespace std;

void init_commands(Parser & parser) {
    parser.addCommand(new Commands::Sqrt()); 
    parser.addCommand(new Commands::Upper());
    parser.addCommand(new Commands::Pow());
}

int main() {
    Lexer lexer = Lexer(TEXT::PROMPT);
    Parser parser = Parser();

    init_commands(parser);

    cout << TEXT::WELCOME_MESSAGE << endl;
    bool running = true;
    while (running) {
        Expression expression = lexer.getInput();
        if (not expression.isValid()) {
            cout << TEXT::ERR_INVALID_EXPRESSION << endl;
        } else if (expression.getTokens().front()->getString() == "exit") {
            running = false;
        } else {
            parser.parseExpression(expression);
        }
    }
    return 0;
}
