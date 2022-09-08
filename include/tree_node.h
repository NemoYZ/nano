#pragma once

#include "node.h"
#include "type_traits.h"
#include <stdint.h>

namespace nano {

struct tree_node_base : public node {
    using self = tree_node_base;

    tree_node_base() noexcept = default;
    tree_node_base(self* _left, self* _right) noexcept :
        left(_left),
        right(_right),
        parent(nullptr) {
    }
    tree_node_base(self* _left, self* _right, self* _parent) noexcept :
        left(_left),
        right(_right),
        parent(_parent) {
    }
    tree_node_base* left = nullptr;
    tree_node_base* right = nullptr;
    tree_node_base* parent = nullptr;
};

template<typename T>
struct tree_node : public tree_node_base {
    tree_node() noexcept = default;
    tree_node(const T& _value) noexcept(std::is_nothrow_copy_assignable_v<T>):
        value(_value) {
    }
    tree_node(T&& _value) noexcept(std::is_nothrow_move_assignable_v<T>) :
        value(_value) {
    }
    tree_node(tree_node_base* left, tree_node_base* right) noexcept :
        tree_node_base(left, right) {
    }
    tree_node(tree_node_base* left, tree_node_base* right, 
            tree_node_base* parent) noexcept :
        tree_node_base(left, right, parent) {
    }
    tree_node(tree_node_base* left, tree_node_base* right,
            tree_node_base* parent, const T& _value) noexcept(std::is_nothrow_copy_assignable_v<T>) : 
        tree_node_base(left, right, parent),
        value(_value) {
    }
    tree_node(tree_node_base* left, tree_node_base* right,
            tree_node_base* parent, T&& _value) noexcept(std::is_nothrow_move_assignable_v<T>) : 
        tree_node_base(left, right, parent),
        value(std::move(_value)) {
    }

    T value;
};

using degree_t = int32_t;

struct b_tree_node_base : public node {
    using self          = b_tree_node_base;

    self* parent = nullptr;
    self** children = nullptr;
    degree_t vsz = 0;

    degree_t child_index(b_tree_node_base* child) {
        degree_t i = 0;
        while (parent->children[i] != child) {
            ++i;
        }
        return i;
    }
};

} //namespace nano