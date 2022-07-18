#pragma once

#include "node.h"
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