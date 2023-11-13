#ifndef NODE_ARRAY_H
#define NODE_ARRAY_H

#include <iostream>

int length_of_int(int i) {
    return std::to_string(i).length();
}

struct node_array_struct {
    int head = 0; // hash value
    int tail = 0; // next node
};

class NodeArray {
    private:
    class LinkedList {
        private:
        int head = 0;
        int tail = 0;
        
        public:
        void push_back(const node_array_struct& item) {
            
        }
    };

    static const int NODE_ARRAY_SIZE = 2000;
    node_array_struct node_array[NODE_ARRAY_SIZE];
    int parse_tree_root = 0;
    int free_list_root = 1;
    int size_parse_tree = 0;
    int size_free_list = NODE_ARRAY_SIZE;

    int max_head_length = 0;
    int max_tail_length = 0;

    public:
    int alloc() {
        parse_tree_root = free_list_root;
        free_list_root++;

        size_parse_tree++;
        size_free_list--;

        if (size_free_list <= 0) {
            throw std::length_error("Size of the node array is too small: " + std::to_string(NODE_ARRAY_SIZE));
        }

        return parse_tree_root;
    }

    int get_address(const node_array_struct& item) {
        for (int i = 0; i < NODE_ARRAY_SIZE; i++) {
            if (&(node_array[i]) == &item) {
                return i;
            }
        }

        return -1;
    }

    const node_array_struct& operator[](const int index) const {
        chech_size(index);
        return node_array[index];
    }

    void set_head(const int index, const int value) {
        node_array[index].head = value;
        if (length_of_int(value) > max_head_length) {
            max_head_length = length_of_int(value);
        }
    }

    int at_head(const int index) const {
        chech_size(index);
        return node_array[index].head;
    }

    void set_tail(const int index, const int value) {
        node_array[index].tail = value;

        if (length_of_int(value) > max_tail_length) {
            max_tail_length = length_of_int(value);
        }
    }

    int at_tail(const int index) const {
        chech_size(index);
        return node_array[index].tail;
    }

    int get_lchild(const int index) const {
        return node_array[index].head;
    }

    int get_rchild(const int index) const {
        return node_array[index].tail;
    }

    int get_free_list_root() const {
        return free_list_root;
    }

    int get_size_parse_tree() const {
        return size_parse_tree;
    }

    int get_size_free_list() const {
        return size_free_list;
    }

    int get_max_head_length() const {
        return max_head_length;
    }

    int get_max_tail_length() const {
        return max_tail_length;
    }

    const node_array_struct* get_node_array() const {
        return node_array;
    }

    void chech_size(const int index) const {
        if (index >= NODE_ARRAY_SIZE) {
            throw std::range_error(
                "Size of the node array is smaller than the entered index: \
                    index(" + std::to_string(index) +
                ") >= NODE_ARRAY_SIZE(" + std::to_string(NODE_ARRAY_SIZE) + ")");
        }
    }

    void free() {
        parse_tree_root = 0;
        free_list_root = 1;
        size_parse_tree = 0;
        size_free_list = NODE_ARRAY_SIZE;

        max_head_length = 0;
        max_tail_length = 0;
    }
};

#endif