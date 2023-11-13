#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <cstring>
#include <cmath>
#include <string>
#include <stdexcept>

#include "node_array.h"
#include "hash_table.h"

inline int max(const int a, const int b) {
    return (a < b) ? b : a;
}

class Interpreter {
    private:
    NodeArray node_array;
    HashTable hash_table;

    std::string input_str;
    int input_str_read_ptr = 0;
    int parse_tree_root_ptr = 1;

    int read_number_of_left_paren = 0;

    class EvalFuncStack {
        private:
        static const int MAX_PARAMS = 5;

        hash_table_struct stack_array[MAX_PARAMS];
        int top_ptr = -1;

        public:
        void push(const hash_table_struct& element) {
            try {
                if (top_ptr >= MAX_PARAMS - 1) {
                    throw std::range_error("The stack is full!");
                }
            } catch (std::range_error& e) {
                std::cerr << e.what() << "MAX_PARAMS(" << MAX_PARAMS << ")\n";
                exit(-1);
            }
            
            top_ptr++;
            stack_array[top_ptr] = element;
        }

        void push(const std::string& func_name, const int value) {
            hash_table_struct temp_element;
            temp_element.symbol = func_name;
            temp_element.link_of_value = value;

            push(temp_element);
        }

        const hash_table_struct& top() const {
            return stack_array[top_ptr];
        }

        int size() const {
            return top_ptr + 1;
        }

        void pop() {
            if (top_ptr >= 0) {
                top_ptr--;
            }
        }
    };

    void get_output(const int index, const bool is_start, std::string& output) const {
        if (index == 0) {
            output += "() ";
        } else if (index < 0) {
            output += hash_table.get_value(index) + " ";
        } else { // if (index > 0)
            if (is_start) {
                output += "(";
            }

            get_output(node_array.at_head(index), true, output);

            if (node_array.at_tail(index) != 0) {
                get_output(node_array.at_tail(index), false, output);
            } else {
                if (*(output.end() - 1) == ' ') {
                    *(output.end() - 1) = ')';
                } else {
                    output += ')';
                }
                output += ' ';
            }
        }
    }

    const std::string preprocessing() {
        for (char& i : input_str) {
            if (i == '\t') { // tab to space
                i = ' ';
            } else if (i >= 'A' && i <= 'Z') { // capital to lower
                i += 'a' - 'A';
            }
        }

        std::string temp_str = "";
        temp_str = preprocessing(temp_str);
        reset_tokenizer();
        
        return temp_str;
    }

    const std::string& preprocessing(std::string& new_command) {
        std::string token_value = "";
        while (token_value = get_next_token(), token_value != "_END_OF_LINE") {
            if (token_value == "define") {
                new_command.append(" " + token_value);

                token_value = get_next_token();
                if (token_value == "(") {
                    // (define (square x) (* x x))
                    // => (define square (lambda (x) (* x x)))
                    token_value = get_next_token();
                    new_command.append(" " + token_value + " ( lambda ( "); 
                    preprocessing(new_command);
                    new_command.append(" ) ");
                } else {
                    // non-function define
                    new_command.append(" " + token_value);
                }
            } else if (token_value == "'") {
                // '(a b c) => (quote (a b c))
                new_command.append(" ( quote ");

                int number_of_left_paren = 0;
                do {
                    token_value = get_next_token();
                    new_command.append(" " + token_value);

                    if (token_value == "(") {
                        number_of_left_paren++;
                    } else if (token_value == ")") {
                        number_of_left_paren--;
                    }
                } while (number_of_left_paren > 0);
                new_command.append(" ) ");
            } else {
                new_command.append(" " + token_value);
            }
        }

        return new_command;
    }

    std::string get_next_token() {
        std::string tmp_str = get_next_token_non_dec();

        if (is_number(tmp_str)) {
            // 부동소수점 오차 제거
            return trunc_decimal(tmp_str);
        }

        return tmp_str;
    }

    std::string get_next_token_non_dec() {
        if (input_str_read_ptr >= input_str.size()) {
            return "_END_OF_LINE";
        }

        if (input_str[input_str_read_ptr] == '(') {
            input_str_read_ptr++;
            return "(";
        } else if (input_str[input_str_read_ptr] == ')') {
            input_str_read_ptr++;
            return ")";
        } else if (input_str[input_str_read_ptr] == '\'') {
            input_str_read_ptr++;
            return "'";
        } else if (input_str[input_str_read_ptr] == ' ') {
            input_str_read_ptr++;
        }

        std::string tmp_str = "";
        while (input_str_read_ptr < input_str.size()) {
            if (input_str[input_str_read_ptr] == '(') {
                input_str_read_ptr++;
                return "(";
            } else if (input_str[input_str_read_ptr] == ')') {
                if (tmp_str == "") {
                    input_str_read_ptr++;
                    return ")";
                } else {
                    return tmp_str;
                }
            } else if (input_str[input_str_read_ptr] == '\'') {
                if (tmp_str == "") {
                    input_str_read_ptr++;
                    return "'";
                } else {
                    return tmp_str;
                }
            } else if (input_str[input_str_read_ptr] == ' ') {
                input_str_read_ptr++;
                if (tmp_str != "") {
                    return tmp_str;
                }
            } else { // symbol
                tmp_str += input_str[input_str_read_ptr];
                input_str_read_ptr++;
            }
        }

        return tmp_str;
    }

    void reset_tokenizer() {
        input_str_read_ptr = 0;
    }

    const std::string trunc_decimal(const std::string& num_str) {
        int iter_to = num_str.size();
        bool decimal_point = false, no_more_del = false;
        for (auto iter = num_str.rbegin(); iter != num_str.rend(); iter++) {
            if (*iter == '.') {
                decimal_point = true;
                if (no_more_del == false) {
                    iter_to = static_cast<int>(num_str.rend() - iter) - 1;
                }
                break;

            } else if (no_more_del == false && *iter != '0') {
                no_more_del = true;
                iter_to = static_cast<int>(num_str.rend() - iter);
            }
        }

        if (decimal_point == true) {
            return num_str.substr(0, iter_to);
        }

        return num_str;
    }

    bool is_number(const std::string& num_str) {
        if (num_str.size() == 0) return false;

        char* end_str;
        double result = std::strtod(num_str.c_str(), &end_str);
        return (strlen(end_str) < 1);
    }

    // @param num_str: std::string 형태의 수.
    // @return num_str을 double로 변환한 값.
    double get_val(const std::string& num_str) {
        char* end_str;
        double result = std::strtod(num_str.c_str(), &end_str);
        if (strlen(end_str) > 0) {
            std::cerr << "Warn: '" << num_str << "' is not pure number. Using '" << result << "'.\n";
        }

        return result;
    }

    inline int get_lchild(const int index) const {
        return node_array.get_lchild(index);
    }

    inline int get_rchild(const int index) const {
        return node_array.get_rchild(index);
    }

    public:
    // @param input: 명령어 문자열.
    // @return 명령어가 전부 입력되었는지의 여부.
    bool read(const std::string& input) {
        // 괄호 개수 확인 -> 명령어가 전부 입력되었는지 확인
        for (const char i : input) {
            input_str += i;
            if (i == '(') {
                read_number_of_left_paren++;
            } else if (i == ')') {
                read_number_of_left_paren--;
            }

            // 완전한 형태의 명령이 들어올 따마다 read 및 preprocessing
            if (read_number_of_left_paren == 0) {
                reset_tokenizer();
                parse_tree_root_ptr = node_array.get_free_list_root();
                input_str = preprocessing();
                const int result = read();

                input_str = "";
            }
        }

        // 명령어가 전부 입력되지 않음
        if (read_number_of_left_paren != 0) {
            return false;
        }

        return true;
    }

    int read() {
        int temp_ptr = 0;
        int root_ptr = 0;
        bool first = true;

        std::string token_value = get_next_token();

        if (token_value == "_END_OF_LINE") {
            return root_ptr;
        } else if (token_value == "(") {
            while (token_value = get_next_token(), token_value != ")") {
                if (first == true) {
                    temp_ptr = node_array.alloc();
                    root_ptr = temp_ptr;

                    first = false;
                } else {
                    node_array.set_tail(temp_ptr, node_array.alloc());
                    temp_ptr = node_array[temp_ptr].tail;
                }

                if (token_value == "(") {
                    input_str_read_ptr--;
                    node_array.set_head(temp_ptr, read());
                } else {
                    node_array.set_head(temp_ptr, hash_table.get_hash_value(token_value));
                }
                node_array.set_tail(temp_ptr, 0);
            }

            return root_ptr;
        } else {
            return hash_table.get_hash_value(token_value);
        }
    }

    void eval() {
        int result = eval(parse_tree_root_ptr);
        if (result == 0) {
            std::cout << "()\n\n";
        } else if (result < 0) {
            // hash
            std::cout << hash_table.get_value(result) << "\n\n";
        } else { // if (result > 0)
            // node pointer
            std::string output = "";
            get_output(result, true, output);
            std::cout << output << "\n\n"; 
        }
    }

    // @param root: root node 포인터.
    // @return 결과 해시 값 또는 node 포인터.
    int eval(const int root) {
        // print();

        if (root == 0) {
            return 0;
        }

        if (root < 0) { // symbol
            if (is_number(hash_table.get_value(root))) { // symbol is a number
                return root;
            } else { // symbol is not a number
                return hash_table.get_pointer(root);
            }
        }

        std::string token_index = hash_table.get_value(get_lchild(root));

        if (token_index == "+" || token_index == "-" || token_index == "*" || token_index == "/" || token_index == "%") {
            double result = 0.0;
            switch (token_index[0]) {
            case '+':
                result = get_val(hash_table.get_value(eval(get_lchild(get_rchild(root))))) +
                         get_val(hash_table.get_value(eval(get_lchild(get_rchild(get_rchild(root))))));
                break;

            case '-':
                result = get_val(hash_table.get_value(eval(get_lchild(get_rchild(root))))) -
                         get_val(hash_table.get_value(eval(get_lchild(get_rchild(get_rchild(root))))));
                break;
            
            case '*':
                result = get_val(hash_table.get_value(eval(get_lchild(get_rchild(root))))) *
                         get_val(hash_table.get_value(eval(get_lchild(get_rchild(get_rchild(root))))));
                break;
            
            case '/':
                result = get_val(hash_table.get_value(eval(get_lchild(get_rchild(root))))) /
                         get_val(hash_table.get_value(eval(get_lchild(get_rchild(get_rchild(root))))));
                break;
            }

            std::string result_str = std::to_string(result);

            // 부동소수점 오차 제거
            return hash_table.get_hash_value(trunc_decimal(result_str));

        } else if (token_index == "=") {
            if (eval(get_lchild(get_rchild(root))) == eval(get_lchild(get_rchild(get_rchild(root))))) {
                return hash_table.get_hash_value("#t");
            } else {
                return hash_table.get_hash_value("#f");
            }

        } else if (token_index == "number?") {
            if (is_number(hash_table.get_value(eval(get_lchild(get_rchild(root)))))) {
                return hash_table.get_hash_value("#t");
            } else {
                return hash_table.get_hash_value("#f");
            }
            
        } else if (token_index == "symbol?") {
            if ((get_lchild(get_rchild(root)) < 0 && hash_table.get_pointer(get_lchild(get_rchild(root))) != 0) ||
                (get_lchild(get_rchild(root)) > 0 && eval(get_lchild(get_rchild(root))) != 0)) {
                return hash_table.get_hash_value("#t");
            } else {
                return hash_table.get_hash_value("#f");
            }

        } else if (token_index == "null?") {
            if (get_rchild(root) == 0 || eval(get_lchild(get_rchild(root))) == 0) {
                return hash_table.get_hash_value("#t");
            } else {
                return hash_table.get_hash_value("#f");
            }

        } else if (token_index == "cons") {
            int temp_ptr = node_array.alloc();
            node_array.set_head(temp_ptr, eval(get_lchild(get_rchild(root))));
            node_array.set_tail(temp_ptr, eval(get_lchild(get_rchild(get_rchild(root)))));
            return temp_ptr;

        } else if (token_index == "cond") {
            int temp_root = root;
            while (get_rchild(get_rchild(temp_root)) != 0) {
                temp_root = get_rchild(temp_root);

                if (hash_table.get_value(eval(get_lchild(get_lchild(temp_root)))) == "#t") {
                    return eval(get_lchild(get_rchild(get_lchild(temp_root))));
                }
            }

            if (hash_table.get_value(get_lchild(get_lchild(get_rchild(temp_root)))) != "else") {
                throw "error";
            }

            return eval(get_lchild(get_rchild(get_lchild(get_rchild(temp_root)))));

        } else if (token_index == "car") {
            return get_lchild(eval(get_lchild(get_rchild(root))));

        } else if (token_index == "cdr") {
            return get_rchild(eval(get_lchild(get_rchild(root))));

        } else if (token_index == "define") {
            if (hash_table.get_value(get_lchild(get_lchild(get_rchild(get_rchild(root))))) == "lambda") {
                // function define
                hash_table.set_pointer(get_lchild(get_rchild(root)), get_lchild(get_rchild(get_rchild(root))));
            } else {
                // value define
                if (get_lchild(get_rchild(get_rchild(root))) < 0) {
                    // symbol define
                    hash_table.set_pointer(get_lchild(get_rchild(root)), eval(get_rchild(get_rchild(root))));
                } else {
                    // list define
                    hash_table.set_pointer(get_lchild(get_rchild(root)), eval(get_lchild(get_rchild(get_rchild(root)))));
                }
                
            }

            return root;

        } else if (token_index == "quote") {
            return get_lchild(get_rchild(root));

        } else if (token_index == "<" || token_index == ">") {
            if (!is_number(hash_table.get_value(eval(get_lchild(get_rchild(root)))))) {
                throw std::logic_error(
                    "'" + hash_table.get_value(eval(get_lchild(get_rchild(root)))) + "' is not a number!");
            }
            if (!is_number(hash_table.get_value(eval(get_lchild(get_rchild(get_rchild(root))))))) {
                throw std::logic_error(
                    "'" + hash_table.get_value(eval(get_lchild(get_rchild(get_rchild(root))))) + "' is not a number!");
            }

            bool is_true = false;

            switch (token_index[0]) {
                case '<':
                    is_true = (get_val(hash_table.get_value(eval(get_lchild(get_rchild(root))))) <
                               get_val(hash_table.get_value(eval(get_lchild(get_rchild(get_rchild(root)))))));
                    break;
                case '>':
                    is_true = (get_val(hash_table.get_value(eval(get_lchild(get_rchild(root))))) >
                               get_val(hash_table.get_value(eval(get_lchild(get_rchild(get_rchild(root)))))));
            }

            return is_true ? hash_table.get_hash_value("#t") : hash_table.get_hash_value("#f");
            
        } else if (token_index == "print" || token_index == "display") {
            // 출력
            return eval(get_lchild(get_rchild(root)));

        } else if (hash_table.get_pointer(hash_table.get_hash_value(token_index)) != 0) {
            // user defined function / value
            EvalFuncStack eval_func_stack; // 함수 호출 전의 hash table 조각의 값을 저장
            EvalFuncStack temp_arg_stack; // 인자(argument)로 넣을 값을 임시로 저장(모든 인자 계산이 끝나기 전까지 hash table을 건드리면 안 됨)

            const int func_ptr = hash_table.get_pointer(get_lchild(root));
            int param = get_lchild(get_rchild(func_ptr));
            int argument = get_rchild(root);
            
            // iterate all params
            while (param != 0 && argument != 0) {
                eval_func_stack.push(hash_table.get_hash_struct(get_lchild(param)));
                temp_arg_stack.push(hash_table.get_value(get_lchild(param)), eval(get_lchild(argument)));
                
                param = get_rchild(param);
                argument = get_rchild(argument);
            }

            // 인자 계산이 끝난 후 함수 호출에 필요한 포인터 값 삽입
            while (temp_arg_stack.size() >= 1) {
                hash_table.set_pointer(hash_table.get_hash_value(temp_arg_stack.top().symbol), temp_arg_stack.top().link_of_value);
                temp_arg_stack.pop();
            }

            const int result = eval(get_lchild(get_rchild(get_rchild(hash_table.get_pointer(get_lchild(root))))));

            // 함수 호출 전의 포인터 값으로 복원
            while (eval_func_stack.size() >= 1) {
                hash_table.set_pointer(hash_table.get_hash_value(eval_func_stack.top().symbol), eval_func_stack.top().link_of_value);
                eval_func_stack.pop();
            }

            return result;

        } else {
            throw std::logic_error("'" + token_index + "' doesn't exist!");
        }
    }

    void print() const {
        std::cout << "] "
                  << "Free list's root = " << node_array.get_free_list_root() << "\n";
        std::cout << "Parse tree's root = " << parse_tree_root_ptr << "\n";
        std::cout << "\n";

        const node_array_struct* tmp_node_array = node_array.get_node_array();
        
        std::cout << "Node array: \n"
                  << std::string((max(length_of_int(node_array.get_size_parse_tree()) - 5, 0) + 1) / 2, ' ')
                  << "Index"
                  << std::string(max(length_of_int(node_array.get_size_parse_tree()) - 5, 0) / 2, ' ')

                  << " | "
                  
                  << std::string((max(node_array.get_max_head_length() - 4, 0) + 1) / 2, ' ')
                  << "Head"
                  << std::string(max(node_array.get_max_head_length() - 4, 0) / 2, ' ')

                  << " | "
                 
                  << std::string((max(node_array.get_max_tail_length() - 4, 0) + 1) / 2, ' ')
                  << "Tail"
                  << std::string(max(node_array.get_max_tail_length() - 4, 0) / 2, ' ')

                  << "\n"
                  << std::string(max(length_of_int(node_array.get_size_parse_tree()), 5)
                                 + 3
                                 + max(node_array.get_max_head_length(), 4)
                                 + 3
                                 + max(node_array.get_max_tail_length(), 4), '-')
                  << "\n";                      
        for (int i = 1; i <= node_array.get_size_parse_tree(); i++) {
            std::cout << std::string(max(length_of_int(node_array.get_size_parse_tree()), 5) // 5 = length of "Index"
                                     - length_of_int(i), ' ')
                      << i << " | "
                      
                      << std::string(max(node_array.get_max_head_length(), 4) // 4 = length of "Head"
                                     - length_of_int(tmp_node_array[i].head), ' ')
                      << tmp_node_array[i].head << " | "
                      
                      << std::string(max(node_array.get_max_tail_length(), 4) // 4 = length of "Tail"
                                     - length_of_int(tmp_node_array[i].tail), ' ')
                      << tmp_node_array[i].tail
                      << "\n";
        }
        std::cout << "\n";

        const hash_table_struct* tmp_hash_table = hash_table.get_hash_table();

        std::cout << "Hash table: \n"
                  << std::string((max(length_of_int(hash_table.get_max_nonzero_index() * (-1)) - 5, 0) + 1) / 2, ' ')
                  << "Index"
                  << std::string(max(length_of_int(hash_table.get_max_nonzero_index() * (-1)) - 5, 0) / 2, ' ')

                  << " | "

                  << std::string((max(hash_table.get_max_length_of_symbol() - 6, 0) + 1) / 2, ' ')
                  << "Symbol"
                  << std::string(max(hash_table.get_max_length_of_symbol() - 6, 0) / 2, ' ')                  
                  
                  << " | "
                  
                  << std::string((max(hash_table.get_max_length_of_link_ptr() - 4, 0) + 1) / 2, ' ')
                  << "Link"
                  << std::string(max(hash_table.get_max_length_of_link_ptr() - 4, 0) / 2, ' ')
                  
                  << "\n"

                  << std::string(max(length_of_int(hash_table.get_max_nonzero_index()), 5)
                                 + 3
                                 + max(hash_table.get_max_length_of_symbol(), 6)
                                 + 3
                                 + max(hash_table.get_max_length_of_link_ptr(), 4), '-')
                  << "\n";
        for (int i = 0; i < hash_table.size(); i++) {
            if (tmp_hash_table[i].symbol != "") {
                std::cout << std::string(max(length_of_int(hash_table.get_max_nonzero_index() * (-1)), 5) // 5 = length of "Index"
                                         - std::to_string(-i).length(), ' ')
                          << -i << " | "

                          << std::string(max(hash_table.get_max_length_of_symbol(), 6) // 6 = length of "Symbol"
                                         - tmp_hash_table[i].symbol.length(), ' ')
                          << tmp_hash_table[i].symbol << " | "

                          << std::string(max(hash_table.get_max_length_of_link_ptr(), 4) // 4 = length of "Link"
                                         - length_of_int(tmp_hash_table[i].link_of_value), ' ')
                          << tmp_hash_table[i].link_of_value
                          << "\n";
            }
        }
        std::cout << "\n";

        std::string output = "";
        get_output(parse_tree_root_ptr, true, output);
        std::cout << output << "\n"; 
    }

    void init() {
        node_array.free();
        
        input_str = "";
        input_str_read_ptr = 0;
        parse_tree_root_ptr = 1;
    }
};

#endif