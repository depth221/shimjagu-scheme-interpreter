#include <iostream>
#include <string>

#include "interpreter.h"

int main(void) {
    Interpreter interpreter;
    std::string input = "";

    interpreter.init();

    do {
        std::cout << "> ";
        do {
            input = "";
            std::getline(std::cin, input);
            if (std::cin.eof()) {
                std::cout << "\n";
                return 0;
            } else if (input[0] == ';') { // comment
                input = "";
            }
        } while (input == "" || !interpreter.read(input));

        interpreter.print();

        interpreter.eval();
        interpreter.print();
        std::cout << "\n";
    } while (true);

    return 0;
}