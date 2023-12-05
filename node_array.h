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
    public:
    static const int NODE_ARRAY_SIZE = 31;

    private:
    node_array_struct node_array[NODE_ARRAY_SIZE];
    int parse_tree_root = 0;
    int free_list_root = 1;
    int size_parse_tree = 0;
    int size_free_list = NODE_ARRAY_SIZE - 1;

    int max_head_length = 0;
    int max_tail_length = 0;

    public:
    NodeArray() {
        // 값 초기화
        node_array[0].head = 0;
        node_array[0].tail = 0;
        for (int i = 1; i < NODE_ARRAY_SIZE; i++) {
            node_array[i].head = 0;
            node_array[i].tail = i + 1;
        }
    }

    int alloc() {
        parse_tree_root = free_list_root;
        free_list_root = get_rchild(free_list_root);

        node_array[parse_tree_root].tail = 0;

        size_parse_tree++;
        size_free_list--;

        if (size_free_list < 0) {
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
        // 값 초기화
        node_array[0].head = 0;
        node_array[0].tail = 0;
        for (int i = 1; i < NODE_ARRAY_SIZE; i++) {
            node_array[i].head = 0;
            node_array[i].tail = i + 1;
        }

        parse_tree_root = 0;
        free_list_root = 1;
        size_parse_tree = 0;
        size_free_list = NODE_ARRAY_SIZE - 1;

        max_head_length = 0;
        max_tail_length = 0;
    }

    void garbage_collection(const int roots[], const int roots_size) {
        size_parse_tree = 0;
        size_free_list = NODE_ARRAY_SIZE - 1;

        bool is_preserved[NODE_ARRAY_SIZE] = {false, };

        for (int i = 0; i < roots_size; i++) {
            check_gc_ptr(is_preserved, roots[i]);
        }

        // GC 가능한 node 중 가장 앞의 node를 free list의 root로 함
        free_list_root = -1;
        for (int i = 1; i < NODE_ARRAY_SIZE; i++) {
            if (is_preserved[i]) continue;

            free_list_root = i;
            node_array[free_list_root].head = 0;
            size_free_list = 1;
            break;
        }
        if (free_list_root == -1) { // GC 불가능
            throw std::length_error("Size of the node array is too small: " + std::to_string(NODE_ARRAY_SIZE));
        }

        int last_node_index = free_list_root, this_node_index = free_list_root;
        for (int i = free_list_root + 1; i < NODE_ARRAY_SIZE; i++) {
            if (is_preserved[i]) continue;
            
            last_node_index = this_node_index;
            this_node_index = i;
            size_free_list++;

            node_array[this_node_index].head = 0;
            node_array[last_node_index].tail = this_node_index;
        }
        node_array[this_node_index].tail = NODE_ARRAY_SIZE;

        size_parse_tree = NODE_ARRAY_SIZE - size_free_list;
    }

    void check_gc_ptr(bool is_preserved[], const int index) const {
        if (is_preserved[index]) return;
        if (index >= NODE_ARRAY_SIZE) return;

        if (node_array[index].head > 0) {
            check_gc_ptr(is_preserved, node_array[index].head);
        }

        if (node_array[index].tail > 0) {
            check_gc_ptr(is_preserved, node_array[index].tail);
        }

        is_preserved[index] = true;
    }
};

#endif