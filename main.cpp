#include <iostream>
#include <string>

#include "interpreter.h"

int main(void) {
    std::cout.precision(HashTable::MAX_SYMBOL_SIZE);
    
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

            // input의 좌우 공백 제거
            int non_blank_lpos = input.find_first_not_of(' ');
            if (non_blank_lpos != -1) input.erase(0, non_blank_lpos);
            int non_blank_rpos = input.find_last_not_of(' ');
            if (non_blank_rpos != input.size() - 1) input.erase(non_blank_rpos + 1);

        } while (input == "" || !interpreter.read(input));

        // interpreter.print();

        interpreter.eval();
        // interpreter.print();
        std::cout << "\n";
    } while (true);

    return 0;
}