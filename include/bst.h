#pragma once

#include "tree.h"

namespace nano {

template<typename T>
struct bst_node : public tree_node<T> {
    bst_node() noexcept = default;
    bst_node(const T& _value) noexcept(std::is_nothrow_copy_assignable_v<T>):
        tree_node<T>(_value) {
    }
    bst_node(T&& rvalue) noexcept(std::is_nothrow_move_assignable_v<T>) :
        tree_node<T>(std::move(rvalue)) {
    }
    bst_node(tree_node_base* left, tree_node_base* right) noexcept :
        tree_node<T>(left, right) {
    }
    bst_node(tree_node_base* left, tree_node_base* right, 
            tree_node_base* parent) noexcept :
        tree_node<T>(left, right, parent) {
    }
    bst_node(tree_node_base* left, tree_node_base* right,
            tree_node_base* parent, const T& _value) noexcept(std::is_nothrow_copy_assignable_v<T>) : 
        tree_node<T>(left, right, parent, _value) {
    }
    bst_node(tree_node_base* left, tree_node_base* right,
            tree_node_base* parent, T&& rvalue) noexcept(std::is_nothrow_move_assignable_v<T>) : 
        tree_node<T>(left, right, parent, std::move(rvalue)) {
    }
};

template<typename T>
inline bst_node<T>* left_of(bst_node<T>* node) {
    tree_node_base* nodeBase = node;
    return static_cast<bst_node<T>*>(left_of(nodeBase));
}

template<typename T>
inline bst_node<T>* right_of(bst_node<T>* node) {
    tree_node_base* nodeBase = node;
    return static_cast<bst_node<T>*>(right_of(nodeBase));
}

template<typename T>
inline bst_node<T>* parent_of(bst_node<T>* node) {
    tree_node_base* nodeBase = node;
    return static_cast<bst_node<T>*>(parent_of(nodeBase));
}

template<typename T>
inline bst_node<T>* precursor(bst_node<T>* node) {
    tree_node_base* nodeBase = node;
    return static_cast<bst_node<T>*>(precursor(nodeBase));
}

template<typename T>
inline bst_node<T>* successor(bst_node<T>* node) {
    tree_node_base* nodeBase = node;
    return static_cast<bst_node<T>*>(successor(nodeBase));
}

template<typename T>
inline bst_node<T>* 
transplant(bst_node<T>* target, bst_node<T>* repNode, bst_node<T>** root) {
    tree_node_base* targetBase = target;
    tree_node_base* repNodeBase = repNode;
    tree_node_base* rootBase = *root;
    transplant(targetBase, repNodeBase, &rootBase);
	*root = rootBase;
	return *root;
}

template<typename T, typename Comp = std::less<T>>
std::pair<bst_node<T>*, bool> bst_get_insert_multi(bst_node<T>* root,
        const T& val, const Comp& comp = Comp()) {
    bool insetrLeft = false;
    bst_node<T>* last = nullptr;
    while (root) {
        last = root;
        if (comp(val, root->value)) {
            insetrLeft = true;
            root = left_of(root);
        } else {
            insetrLeft = false;
            root = right_of(root);
        }
    }

    return { last, insetrLeft };
}

template<typename T, typename Comp = std::less<T>>
std::pair<bst_node<T>*, int> bst_get_insert_unique(bst_node<T>* root, 
        const T& val, const Comp& comp = Comp()) {
    int insetrLeft = 0;
    while (root) {
        if (comp(root->value, val)) {
            insetrLeft = -1;
            root = left_of(root);
        } else {
            insetrLeft = 1;
            root = right_of(root);
        }
    }

    return { root, insetrLeft };
}

template<typename T, typename Comp = std::less<T>>
bst_node<T>* bst_insert_node_multi(bst_node<T>* node, 
        bst_node<T>** root, const Comp& comp = Comp()) {
    if (nullptr == *root) {
        *root = node;
        return *root;
    }

    auto [parent, insertLeft] = bst_get_insert_multi(*root, node->value, comp);
    node->parent = parent;
    if (insertLeft) {
        parent->left = node;
    } else {
        parent->right = node;
    }

    return *root;
}

template<typename T, typename Comp = std::less<T>>
std::pair<bst_node<T>*, bool> bst_insert_node_unique(bst_node<T>* node, 
        bst_node<T>** root, const Comp& comp = Comp()) {
    if (nullptr == *root) {
        *root = node;
        return { *root, true };
    }
    
    auto [parent, insertLeft] = bst_get_insert_unique(*root, node->value, comp);
    switch (insertLeft) {
        case -1:
            node->parent = parent;
            parent->left = node;
            break;
        case 1:
            node->parent = parent;
            parent->right = node;
            break;
        case 0:
            [[fallthrough]];
        default:
            break;
    }
    return { *root, insertLeft != 0 };
}

template<typename T>
bst_node<T>* bst_erase_node(bst_node<T>* node, bst_node<T>** root) {
    bst_node<T>* nsuccessor = successor(node);
    bst_node<T>* repNode = nullptr; //replace node

    // 交换要删除的节点和后继节点的位置
    // 转化为只有右孩子的情况
    if (left_of(node) && right_of(node)) { 
        if constexpr(std::is_move_assignable_v<T>) {
            //直接把后继节点的值赋给当前要删除的节点，然后删除后继节点
            node->value = std::move(nsuccessor->value);
            repNode = nsuccessor;
            nsuccessor = node;
            node = repNode;
        } else {
            bst_node<T>* sparent = parent_of(nsuccessor);
            bst_node<T>* srchild = right_of(nsuccessor);
            nsuccessor->left = node->left;
            node->left->parent = nsuccessor;
            transplant(node, nsuccessor, root);
            node->left = nullptr;

            if (nsuccessor == node->right) {
                node->right = nsuccessor->right;
                if (nsuccessor->right) {
                    nsuccessor->right->parent = node;
                }
                nsuccessor->right = node;
                node->parent = nsuccessor;
            } else {
                nsuccessor->right = node->right;
                node->right->parent = nsuccessor;
                node->parent = sparent;
                sparent->left = node;				//_successor肯定是它父亲结点的左孩子
                node->right = srchild;
                if (srchild) {
                    srchild->parent = node;
                }
            }
        }
    }

	if (nullptr == left_of(node) && nullptr == right_of(node)) {
		if (node->parent) {
			if (node == node->parent->left) {
				node->parent->left = nullptr;
			} else {
				node->parent->right = nullptr;
			}
		}
	} else if (nullptr == left_of(node)) {
		repNode = right_of(node);
		transplant(node, repNode, root);
	} else if (nullptr == right_of(node)) {
		repNode = left_of(node);
		transplant(node, repNode, root);
	}
	
	return nsuccessor;
}

template<typename T, typename Comp = std::less<T>>
bst_node<T>* bst_lbound(const T& val, bst_node<T>* root, const Comp& comp = Comp()) {
    bst_node<T>* parent = nullptr;
    while (root) {
        if (!comp(root->value, val)) {
            parent = root;
            root = left_of(root);
        } else {
            root = right_of(root);
        }
    }

    return parent;
}

template<typename T, typename Comp = std::less<T>>
bst_node<T>* bst_ubound(const T& val, bst_node<T>* root, const Comp& comp = Comp()) {
    bst_node<T>* parent = nullptr;
    while (root) {
        if (comp(val, root->value)) {
            parent = root;
            root = left_of(root);
        } else {
            root = right_of(root);
        }
    }

    return parent;
}

template<typename T, typename Comp = std::less<T>>
std::pair<bst_node<T>*, bst_node<T>*> bst_equal_range_multi(const T& val, bst_node<T>* root,
        const Comp& comp = Comp()) {
    return { bst_lbound(val, root, comp), bst_ubound(val, root, comp) };
}

template<typename T, typename Comp = std::less<T>>
std::pair<bst_node<T>*, bst_node<T>*> bst_equal_range_unique(const T& val, bst_node<T>* root,
        const Comp& comp = Comp()) {
    bst_node<T>* node = bst_lbound(val, root, comp);
    return { node, static_cast<bst_node<T>*>(successor(node)) };
}

template<typename T, typename Size = size_t, typename Comp = std::less<T>>
Size bst_count_multi(const T& val, bst_node<T>* root, const Comp& comp = Comp()) {
    Size n = 0;
    std::pair<bst_node<T>*, bst_node<T>*> myPair = bst_equal_range_multi(val, root, comp);
    bst_node<T>* lnode = myPair.first;
    bst_node<T>* rnode = myPair.second;
    while (lnode != rnode) {
        lnode = static_cast<bst_node<T>*>(successor(lnode));
        ++n;
    }

    return n;
}

template<typename T, typename Size = size_t, typename Comp = std::less<T>>
Size bst_count_unique(const T& val, bst_node<T>* root, const Comp& comp = Comp()) {
    bst_node<T>* node = bst_lbound(val, root, comp); //node->value >= val
    if (node || !comp(val, node->value)) { //val >= node->value
        return 1;
    }

    return 0;
}

} //namespace nano