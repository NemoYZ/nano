#pragma once

#include "list_node.h"

namespace nano {

inline list_node_base* next_of(list_node_base* node) {
    return node->next;
}

inline void 
list_link_after(list_node_base* node, list_node_base** target) {
    if (nullptr == *target) {
        *target = node;
    } else {
        node->next = (*target)->next;
        (*target)->next = node;
    }
}

inline list_node_base* list_unlink_after(list_node_base* node) {
    if (nullptr == node) {
        return nullptr;
    }
    list_node_base* nextNode = next_of(node);
    node->next = next_of(nextNode);
    nextNode->next = nullptr;
    return nextNode;
}

template<typename T>
forward_list_node<T>* next_of(forward_list_node<T>* node) {
    list_node_base* nodeBase = node;
    return static_cast<forward_list_node<T>*>(next_of(nodeBase));
}

template<typename T>
inline void
list_link_after(forward_list_node<T>* node, forward_list_node<T>** target) {
    list_node_base* targetBase = *target;
    list_node_base* nodeBase = node;
    list_link_after(nodeBase, &targetBase);
    *target = static_cast<forward_list_node<T>*>(targetBase);
}

template<typename T>
inline forward_list_node<T>* 
list_unlink_after(forward_list_node<T>* node) {
    list_node_base* nodeBase = node;
    return static_cast<forward_list_node<T>*>(list_unlink_after(nodeBase));
}

template<typename T, typename BinOp>
forward_list_node<T>* list_find_first_of(forward_list_node<T>* head, 
        const T& val, const BinOp& binop) {
    while (head && !binop(head->value, val)) {
        head = static_cast<forward_list_node<T>*>(head->next);
    }
    return head;
}

template<typename T, typename DestroyFunc>
void clear_since(forward_list_node<T>* lst, const DestroyFunc& f) {
    if (nullptr == lst) {
        return;
    }
    while (next_of(lst) && lst != next_of(lst)) {
        list_node_base* nextNode = list_unlink_after(lst);
        f(nextNode);
    }
    f(lst);
}

//由于单链表和双链表的拷贝方式不一样，所以分开写
template<typename T, typename CopyFunc>
forward_list_node<T>* copy_since(forward_list_node<T>* lst, const CopyFunc& f) {
    if (nullptr == lst) {
        return lst;
    }
    list_node_base* newList = f(lst);
    list_node_base* last = newList;
    forward_list_node<T>* curr = next_of(lst);

    while (curr && curr != lst) {
        list_node_base* newNode = f(curr);
        list_link_after(newNode, &last);
        last = next_of(last);
        curr = next_of(curr);
    }

    return static_cast<forward_list_node<T>*>(newList);
}

} //namespace nano