#include <iostream>


// Our files
#include "static/text.h"
#include "parser/Lexer.h"
#include "parser/Parser.h"

using namespace std;

int main() {
    Lexer lexer = Lexer(TEXT::PROMPT);

    cout << TEXT::WELCOME_MESSAGE << endl;
    bool running = true;
    while (running) {
        Expression expression = lexer.getInput();
        if (not expression.isValid()) {
            cout << TEXT::ERR_INVALID_EXPRESSION << endl;
        } else if (expression.getTokens().front()->getString() == "exit") {
            running = false;
        } else {
            Parser::parseExpression(expression);
        }
    }
    return 0;
}
