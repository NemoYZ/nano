#pragma once

#include "utility.h"
#include "tree_node.h"
#include <stdint.h>
#include <algorithm>
#include <unistd.h>
#include <iostream>

namespace nano {

/**
 * @brief 二叉树某节点的父亲节点
 * @param node 
 * @return tree_node_base* 
 */
inline tree_node_base* parent_of(tree_node_base* node) {
    return node ? node->parent : nullptr;
}

/**
 * @brief 二叉树某节点的左子节点
 * @param node 
 * @return tree_node_base* 
 */
inline tree_node_base* left_of(tree_node_base* node) {
    return node ? node->left : nullptr;
}

/**
 * @brief 二叉树某节点的右子节点
 * @param node 
 * @return tree_node_base* 
 */
inline tree_node_base* right_of(tree_node_base* node) {
    return node ? node->right : nullptr;
}

/**
 * @brief 树结点的左旋
 * @param[in] node 要旋转的结点
 * @param[in] root 树根
 * @return 旋转后的结点 
 */
/*      parent									 parent
 *			\										  \
 *			 node			------->				rchild
 *          /	 \									/	  \
 *        lchild  rchild						  node    rr
 *                /    \						/	  \
 *               rl    rr					  lchild   rl
 */
inline tree_node_base* left_rotate(tree_node_base* node, tree_node_base** root) {
	tree_node_base* rchild = node->right;
	node->right = rchild->left;
	if (rchild->left) {
		rchild->left->parent = node;
	}
	rchild->parent = node->parent;
	if (nullptr == node->parent || node == *root) {
		*root = rchild;
	} else if (node == node->parent->left) {
		node->parent->left = rchild;
	} else {
		node->parent->right = rchild;
	}
	rchild->left = node;
	node->parent = rchild;

	return rchild;
}

/**
 * @brief 树结点的右旋
 * @param[in] node 要旋转的结点
 * @param[in] root 树根
 * @return 旋转后的结点 
 */
/*		parent											parent
 *			\												\
 *			node				------->					lchild
 *		   /    \											/	 \
 *     lchild	rchild									  ll     node
 *     /    \													 /	\
 *   ll     lr													lr	rchild
 */
inline tree_node_base* right_rotate(tree_node_base* node, tree_node_base** root) {
	tree_node_base* lchild = node->left;
	node->left = lchild->right;
	if (lchild->right) {
		lchild->right->parent = node;
	}
	lchild->parent = node->parent;
	if (nullptr == node->parent || node == *root) {
		*root = lchild;
	} else if (node == node->parent->left) {
		node->parent->left = lchild;
	} else {
		node->parent->right = lchild;
	}
	lchild->right = node;
	node->parent = lchild;

	return lchild;
}

/*		parent						parent					parent
 *			\							\						\
 *			node	------->			node ------>			 lr
 *		   /    						/	 				   /	\
 *     lchild						   lr     				lchild	node
 *          \						  /							 
 *   	    lr						lchild
 */
inline tree_node_base* left_right_rotate(tree_node_base* node, tree_node_base** root) { //先右旋node->left再右旋node
	left_rotate(node->left, root);
	return right_rotate(node, root);
}

/*		parent						parent					parent
 *			\							\						\
 *			node	------->			node       ------>		 rl
 *		   	  \    						  \	 				   /	\
 *     		 rchild						   rl     			 node	rchild
 *           /						        \							 
 *   	    rl						     rchild
 */
inline tree_node_base* right_left_rotate(tree_node_base* node, tree_node_base** root) { //先右旋node->right再左旋node
	node->right = right_rotate(node->right, root);
	return left_rotate(node, root);
}

/**
 * @brief 用node2来替代node1
 * @param[in] node1 被替代的结点
 * @param[in] node2 替代结点
 * @param[in] root 树根
 */
inline void transplant(tree_node_base* node1, tree_node_base* node2, tree_node_base** root) {
	if (node2) {
		node2->parent = node1->parent;
	}
	if (nullptr == node1->parent || node1 == *root) { //根结点
		*root = node2;
	} else if (node1 == node1->parent->left) {
		node1->parent->left = node2;
	} else {
		node1->parent->right = node2;
	}
}

/**
 * @brief 二叉搜索树最大值节点
 * @param root 
 * @return tree_node_base* 
 */
tree_node_base* max_node(tree_node_base* root);

/**
 * @brief 二叉搜索树最小值节点
 * @param root 
 * @return tree_node_base* 
 */
tree_node_base* min_node(tree_node_base* root);

/**
 * @brief 二叉搜索树某节点的后继节点
 * @param node 
 * @return tree_node_base* 
 */
tree_node_base* successor(tree_node_base* node);

/**
 * @brief 二叉搜索树某节点的前驱节点
 * @param node 
 * @return tree_node_base* 
 */
tree_node_base* precursor(tree_node_base* node);

bool is_balanced(tree_node_base* root);

template<typename CopyFun>
tree_node_base* __copy_since(tree_node_base* node1, 
		tree_node_base* node2, const CopyFun& f) {
	tree_node_base* top = f(node1);
	top->parent = node2;
	if (node1->right) {	//先拷贝右边
		top->right = __copy_since(node1->right, top, f);
	}

	//拷贝左边
	node2 = top;
	node1 = node1->left;
	while (node1) {
		tree_node_base* newNode = f(node1);
		node2->left = newNode;
		newNode->parent = node2;
		if (node1->right) {
			newNode->right = __copy_since(node1->right, newNode, f);
		}
		node2 = newNode;
		node1 = node1->left;
	}

	return top;
}

template<typename CopyFun>
tree_node_base* copy_since(tree_node_base* node1, const CopyFun& f) {
	/*
	static_assert(std::is_convertible<decltype(f(node1)), tree_node_base*>::value, 
		"return type of CopyFunc missmatch");
	*/
	if (nullptr == node1) {
		return nullptr;
	}
	return __copy_since(node1, nullptr, f);
}

template<typename DestroyFunc>
void __clear_since(tree_node_base* root, const DestroyFunc& f) {
	while (root) {
    	clear_since(root->right, f);
    	tree_node_base* lchild = root->left;
		f(root);
    	root = lchild;
  	}
}

template<typename DestroyFunc>
void clear_since(tree_node_base* root, const DestroyFunc& f) {
	__clear_since(root, f);
}

inline bool is_leaf(b_tree_node_base* node) {
	return nullptr == node || 
			nullptr == node->children || 
			nullptr == node->children[0];
}

degree_t child_index(b_tree_node_base* parent, b_tree_node_base* node);

std::pair<b_tree_node_base*, degree_t> min_node(b_tree_node_base* node);

std::pair<b_tree_node_base*, degree_t> max_node(b_tree_node_base* node);

std::pair<b_tree_node_base*, degree_t> successor(b_tree_node_base* node, degree_t index);

std::pair<b_tree_node_base*, degree_t> precursor(b_tree_node_base* node, degree_t index);

template<typename CopyFunc>
b_tree_node_base* __copy_since(b_tree_node_base* node1, 
		b_tree_node_base* node2, const CopyFunc& f) {
	b_tree_node_base* newNode = nullptr;
	if (node1) {
		newNode = f(node1);
		newNode->parent = node2;
		std::cout << "here" << std::endl;
		for (degree_t i = 0; i < node1->vsz + 1; ++i) {
			newNode->children[i] = __copy_since(node1->children[i], newNode, f);
		}
	}
	return newNode;
}

template<typename CopyFunc>
b_tree_node_base* copy_since(b_tree_node_base* node1, const CopyFunc& f) {
	static_assert(std::is_convertible<decltype(f(node1)), b_tree_node_base*>::value, 
		"return type of CopyFunc missmatch");
	if (nullptr == node1) {
		return nullptr;
	}

	return __copy_since(node1, nullptr, f);
}

} //namespace nano