#pragma once

#include "node.h"
#include "type_traits.h"

namespace nano {

struct list_node_base : public node {
    using self = list_node_base;

    list_node_base() noexcept = default;
    list_node_base(self* _next) :
        next(_next) {
    }

    list_node_base* next = nullptr;
};

template<typename T>
struct forward_list_node : public list_node_base {
    using self = forward_list_node<T>;

    forward_list_node() noexcept = default;
    forward_list_node(list_node_base* _next) noexcept :
        list_node_base(_next) {
    }
    forward_list_node(list_node_base* _next, const T& val) 
        noexcept(std::is_nothrow_assignable_v<T>) :
        list_node_base(_next),
        value(val) {
    }
    forward_list_node(list_node_base* _next, T&& val) 
        noexcept(std::is_nothrow_move_assignable_v<T>) :
        list_node_base(_next),
        value(std::move(val)) {
    }

    T value;
};

} //namespace nano
