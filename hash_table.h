#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string>
#include <iostream>
#include <stdexcept>

struct hash_table_struct {
    std::string symbol = "";
    int link_of_value = 0;
};

class HashTable {
    public:
    static const int HASH_TABLE_SIZE = 101;
    static const int MAX_SYMBOL_SIZE = 10;

    private:
    hash_table_struct hash_table[HASH_TABLE_SIZE];

    int max_nonzero_index = 0;
    int max_length_of_symbol = 0;
    int max_length_of_link_ptr = 0;

    int string_to_int(const std::string& str) const {
        int length = static_cast<int>(str.length());
        int answer = 0;

        if (length % 2 == 1) {
            answer = str.at(length - 1);
            length--;
        }

        for (int i = 0; i < length; i += 2) {
            answer += str.at(i);
            answer += (static_cast<int>(str.at(i + 1))) << 8;
        }

        return answer % HASH_TABLE_SIZE;
    }

    public:
    int get_hash_value(std::string input_str) {
        // cut string which is out of MAX_SYMBOL_SIZE
        if (input_str.size() > MAX_SYMBOL_SIZE) {
            input_str = input_str.substr(0, MAX_SYMBOL_SIZE);
        }

        int tmp_hash = string_to_int(input_str);
        const int orig_tmp_hash = tmp_hash;

        while (hash_table[tmp_hash].symbol != "" && hash_table[tmp_hash].symbol != input_str) {
            tmp_hash++;
            if (tmp_hash >= HASH_TABLE_SIZE) {
                tmp_hash = 0;
            }

            // hash table is full
            if (tmp_hash == orig_tmp_hash) {
                throw std::length_error("Size of the hash table is too small:" + std::to_string(HASH_TABLE_SIZE));
            }
        }

        if (tmp_hash > max_nonzero_index) {
            max_nonzero_index = tmp_hash;
        }
        if (input_str.length() > max_length_of_symbol) {
            max_length_of_symbol = input_str.length();
        }
        if (1 > max_length_of_link_ptr) {
            max_length_of_link_ptr = 1;
        }

        hash_table[tmp_hash].symbol = input_str;
        return -tmp_hash;
    }

    bool is_existing(std::string input_str) {
        // cut string which is out of MAX_SYMBOL_SIZE
        if (input_str.size() > MAX_SYMBOL_SIZE) {
            input_str = input_str.substr(0, MAX_SYMBOL_SIZE);
        }

        int tmp_hash = string_to_int(input_str);
        const int orig_tmp_hash = tmp_hash;
        while (hash_table[tmp_hash].symbol != input_str) {
            tmp_hash++;
            if (tmp_hash >= HASH_TABLE_SIZE) {
                tmp_hash = 0;
            }

            // hash table is full
            if (tmp_hash == orig_tmp_hash) {
                return false;
            }
        }

        return true;
    }

    void set_pointer(const int hash, const int pointer) {
        check_size(-hash);
        hash_table[-hash].link_of_value = pointer;
    }

    int get_pointer(const int hash) const {
        check_size(-hash);
        return hash_table[-hash].link_of_value;
    }

    const std::string& get_value(const int hash) const {
        check_size(-hash);
        return hash_table[-hash].symbol;
    }

    int size() const {
        return HASH_TABLE_SIZE;
    }

    const hash_table_struct* get_hash_table() const {
        return hash_table;
    }
    
    const hash_table_struct& get_hash_struct(const int index) const {
        check_size(-index);
        return hash_table[-index];
    }

    int get_max_nonzero_index() const {
        return max_nonzero_index;
    }

    int get_max_length_of_symbol() const {
        return max_length_of_symbol;
    }

    int get_max_length_of_link_ptr() const {
        return max_length_of_link_ptr;
    }

    void check_size(const int hash_inv) const {
        if (hash_inv >= HASH_TABLE_SIZE || hash_inv < 0) {
            throw std::range_error(
                "Size of the hash table is smaller than the entered hash index: \
                    hash index(" + std::to_string(hash_inv) +
                ") >= HASH_TABLE_SIZE(" + std::to_string(HASH_TABLE_SIZE) + ")");
        }
    }

    void clear() {
        for (hash_table_struct& i : hash_table) {
            i.symbol = "";
            i.link_of_value = 0;
        }

        max_nonzero_index = 0;
        max_length_of_symbol = 0;
        max_length_of_link_ptr = 0;
    }
};

#endif