#include "algorithm.h"
#include <stdint.h>
#include "tree.h"

namespace nano {

tree_node_base* left_most(tree_node_base* root) {
	if (nullptr == root) {
		return nullptr;
	}

	while (root->left) {
		root = root->left;
	}

	return root;
}

tree_node_base* right_most(tree_node_base* root) {
	if (nullptr == root) {
		return nullptr;
	}
	while (root->right) {
		root = root->right;
	}

	return root;
}

tree_node_base* successor(tree_node_base* node) {
	if (right_of(node)) {
		return min_node(right_of(node));
	} else {
		tree_node_base* parent = parent_of(node);
		while (node == right_of(parent) && node != parent_of(parent)) {
			node = parent;
			parent = parent_of(node);
		}
		if (node != right_of(parent)) {
			node = parent;
		}
		return parent;
	}
}

tree_node_base* precursor(tree_node_base* node) {
	if (left_of(node)) {
		return max_node(left_of(node));
	} else {
		tree_node_base* parent = parent_of(node);
		while (node == left_of(parent) && node != parent_of(parent)) {
			node = parent;
			parent = parent_of(node);
		}
		if (node != left_of(parent)) {
			node = parent;
		}
		return parent;
	}
}

static int32_t height(tree_node_base * root) {
	if (nullptr == root) {
		return 0;
	}
	int lh = height(root->left);
	int rh = height(root->right);
	if (lh == -1 || rh == -1 || abs(lh - rh) > 1) {
		return -1;
	} else {
		return std::max(lh, rh) + 1;
	}
}

bool is_balanced(tree_node_base* root) {
	return height(root) >= 0;
}

degree_t child_index(b_tree_node_base* parent, b_tree_node_base* node) {
	degree_t i = 0;
	while (parent->children[i] != node) {
		++i;
	}
	return i;
	//return static_cast<degree_t>(&node - parent->children);
}

std::pair<b_tree_node_base*, degree_t> min_node(b_tree_node_base* node) {
	while (!(is_leaf(node))) {
        node = node->children[0];
    }
    return { node, 0 };
}

std::pair<b_tree_node_base*, degree_t> max_node(b_tree_node_base* node) {
	while (!(is_leaf(node))) {
        node = node->children[node->vsz];
    }
    return { node, node->vsz - 1};
}

std::pair<b_tree_node_base*, degree_t> successor(b_tree_node_base* node, degree_t index) {
    if (is_leaf(node)) {
        if (index + 1 < node->vsz) { //如果还没达到当前结点的最大值，就直接把下标往后移动
            ++index;
        } else { //向上回溯
            do {
				b_tree_node_base* parent = node->parent;
                degree_t i = 0;
				if (parent) { 
					i = child_index(parent, node);
				}
				node = parent;
				if (parent) {
					if (parent->vsz == i) { //最后一个孩子
						parent = node->parent;
					} else {
						index = i;
						break;
					}
				}
            } while (node);
        }
		return { node, index };
    }   
	return min_node(node->children[index + 1]);
}

std::pair<b_tree_node_base*, degree_t> precursor(b_tree_node_base* node, degree_t index) {
    if (is_leaf(node)) {
        if (index > 0) { //如果还没达到当前结点的最小值，就直接把下标往前移动
            --index;
        } else {        //向上回溯
            do {
				b_tree_node_base* parent = node->parent;
                degree_t i = 0;
				if (parent) {
					i = child_index(parent, node);
				}
                node = parent;
				if (parent) {
					if (0 == i) {   //第一个孩子
						parent = node->parent;
					} else {
						index = i - 1;
						break;
					}
				}
            } while (node);
        }
		return { node, index };
    }   
	return max_node(node->children[index]);
}

} //namespace nano