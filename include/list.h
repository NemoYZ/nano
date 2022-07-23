#pragma once

#include "list_node.h"

namespace nano {

template<typename DestroyFunc>
void clear_since(list_node_base* lst, const DestroyFunc& f) {
    if (nullptr == lst) {
        return;
    }
    while (lst->next && lst->next != lst) {
        list_node_base* _next = lst->next;
        lst->next = _next->next;
        f(_next);
    }
    f(lst);
}

//由于但链表和双链表的拷贝方式不一样，所以分开写
template<typename T, typename CopyFunc>
forward_list_node<T>* copy_since(forward_list_node<T>* lst, const CopyFunc& f) {
    if (nullptr == lst) {
        return lst;
    }
    list_node_base* newList = f(lst);
    list_node_base* last = newList;
    list_node_base* curr = lst->next;

    while (curr && curr != newList) {
        list_node_base* newNode = f(curr);
        last->next = newNode;
        last = newNode;
        curr = curr->next;
    }

    return static_cast<forward_list_node<T>*>(newList);
}

} //namespace nano