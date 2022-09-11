/**
 * @file avl_tree.h
 * @brief avl树的实现
 * @date 2022-03-21
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <iterator>
#include <functional>
#include <iostream>
#include <string>
#include <compare> //C++20

#ifdef AVL_DEBUUG
#include <queue>
#endif //AVL_DEBUUG

#include "algorithm.h"
#include "bst.h"
#include "construct.h"
#include "type_traits.h"
#include "concepts.h"

namespace nano {

template<typename T>
struct avl_tree_node : public bst_node<T> {
	using node_base_ptr = tree_node_base*;
	using height_type 	= int;		//int8 enough
	using self 			= avl_tree_node<T>;

	avl_tree_node() noexcept = default;

	avl_tree_node(const T& value) noexcept(std::is_nothrow_assignable_v<T>) :
		bst_node<T>(value) {
	}

	avl_tree_node(T&& rvalue) noexcept(std::is_nothrow_move_assignable_v<T>) :
		bst_node<T>(std::move<T>(rvalue)) {
	}
    
	avl_tree_node(node_base_ptr left, node_base_ptr right) noexcept :
        bst_node<T>(left, right) {
    }

    avl_tree_node(node_base_ptr left, node_base_ptr right, 
            node_base_ptr parent) noexcept :
			bst_node<T>(left, right, parent) {
    }

    avl_tree_node(node_base_ptr left, node_base_ptr right, 
            node_base_ptr parent, const T& value) 
			noexcept(std::is_nothrow_assignable_v<T>) :
        bst_node<T>(left, right, parent, value) {
    }

	avl_tree_node(node_base_ptr left, node_base_ptr right,
            node_base_ptr parent, T&& rvalue) 
			noexcept(std::is_nothrow_move_assignable_v<T>) :
        bst_node<T>(left, right, parent, std::move(rvalue)) {
    }

	height_type height;
};

template<typename T>
inline avl_tree_node<T>* left_of(avl_tree_node<T>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<avl_tree_node<T>*>(left_of(nodeBase));
}

template<typename T>
inline avl_tree_node<T>* right_of(avl_tree_node<T>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<avl_tree_node<T>*>(right_of(nodeBase));
}

template<typename T>
inline avl_tree_node<T>* parent_of(avl_tree_node<T>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<avl_tree_node<T>*>(parent_of(nodeBase));
}

template<typename T>
inline avl_tree_node<T>* precursor(avl_tree_node<T>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<avl_tree_node<T>*>(precursor(nodeBase));
}

template<typename T>
inline avl_tree_node<T>* successor(avl_tree_node<T>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<avl_tree_node<T>*>(successor(nodeBase));
}

template<typename T>
inline avl_tree_node<T>* 
transplant(avl_tree_node<T>* target, avl_tree_node<T>* repNode, avl_tree_node<T>** root) {
    tree_node_base* rootBase = *root;
    transplant(target, repNode, &rootBase);
	*root = static_cast<avl_tree_node<T>*>(rootBase);
	return *root;
}

template<typename T>
inline avl_tree_node<T>*
left_rotate(avl_tree_node<T>* node, avl_tree_node<T>** root) {
	tree_node_base* rootBase = *root;
	node = static_cast<avl_tree_node<T>*>(left_rotate(node, &rootBase));
	avl_tree_node<T>* lchild = left_of(node);
	lchild->height = std::max(height_of(left_of(lchild)), 
			height_of(right_of(lchild))) + 1;
	node->height = std::max(height_of(left_of(node)), 
			height_of(right_of(node))) + 1;
	*root = static_cast<avl_tree_node<T>*>(rootBase);
	return node;
}

template<typename T>
inline avl_tree_node<T>*
right_rotate(avl_tree_node<T>* node, avl_tree_node<T>** root) {
	tree_node_base* rootBase = *root;
	node = static_cast<avl_tree_node<T>*>(right_rotate(node, &rootBase));
	avl_tree_node<T>* rchild = right_of(node);
	rchild->height = std::max(height_of(left_of(rchild)), 
			height_of(right_of(rchild))) + 1;
	node->height = std::max(height_of(left_of(node)), 
			height_of(right_of(node))) + 1;
	*root = static_cast<avl_tree_node<T>*>(rootBase);
	return node;
}

template<typename T>
inline avl_tree_node<T>*
left_right_rotate(avl_tree_node<T>* node, avl_tree_node<T>** root) {
	left_rotate(left_of(node), root);
	return right_rotate(node, root);
}

template<typename T>
inline avl_tree_node<T>*
right_left_rotate(avl_tree_node<T>* node, avl_tree_node<T>** root) {
	right_rotate(right_of(node), root);
	return left_rotate(node, root);
}

template<typename T>
inline typename avl_tree_node<T>::height_type 
height_of(avl_tree_node<T>* node) {
	return node ? node->height : 0;
}

template<typename T>
inline typename avl_tree_node<T>::height_type 
balance_factor(avl_tree_node<T>* node) {
	return node ? height_of(left_of(node)) - height_of(right_of(node)) : 0;
}

template<typename T>
avl_tree_node<T>* avl_insert_fixup(avl_tree_node<T>* node, avl_tree_node<T>** root) {
	using height_type = typename avl_tree_node<T>::height_type;
	while (node) {
		node->height = std::max(height_of(left_of(node)), 
				height_of(right_of(node))) + 1;
		height_type bf = balance_factor(node);
		if (bf > 1) {
			height_type lbf = balance_factor(left_of(node));
			if (lbf > 0) {
				node = right_rotate(node, root);
			} else if (lbf < 0) {
				node = left_right_rotate(node, root);
			}
		} else if (bf < -1) {
			height_type rbf = balance_factor(right_of(node));
			if (rbf < 0) {
				node = left_rotate(node, root);
			} else if (rbf > 0) {
				node = right_left_rotate(node, root);
			}
		}
		node = parent_of(node);
	}
	
	return node;
}

template<typename T>
void avl_erase_fixup(avl_tree_node<T>* node, avl_tree_node<T>** root) {
	using height_type = typename avl_tree_node<T>::height_type;
	while (node) {
		node->height = std::max(height_of(left_of(node)), height_of(right_of(node))) + 1;
		height_type bf = balance_factor(node);
		/**
		* 解释一下这里为什么是>=0或者<=0
		* 在插入当中，我们是不可能出现bf为+2并且右子树的bf为0的情况，但在删除的时候是有可能的，下面举例
		*			 node											node	(右旋就可以解决问题，也就是第一个if语句)
		*			/	\				删除结点right			   /
		*		 left   right		 ------------------>         left
		*		/												 /
		*	   lleft										   lleft
		* 在插入的时候是绝对不可能出现以上情况的，你不可能在已经不平衡的条件下再进行插入
		*/
		if (bf > 1) {
			if (balance_factor(left_of(node)) >= 0) {
				node = right_rotate(node, root);
			} else {
				node = left_right_rotate(node, root);
			}
		} else if (bf < -1) {
			if (balance_factor(right_of(node)) <= 0) {
				node = left_rotate(node, root);
			} else {
				node = right_left_rotate(node, root);
			}
		}
		node = parent_of(node);
	}
}

template<typename T, typename Comp = std::less<T>>
avl_tree_node<T>* avl_insert_node_multi(avl_tree_node<T>* node, 
        avl_tree_node<T>** root, const Comp& comp = Comp(), 
		tree_node_base* header = nullptr) {
	bst_node<T>* rootBase = *root;
	bst_insert_node_multi(node, &rootBase, comp, header);
	*root = static_cast<avl_tree_node<T>*>(rootBase);
	avl_insert_fixup(node, root);
	
	return node;
}

template<typename T, typename Comp = std::less<T>>
std::pair<avl_tree_node<T>*, bool> avl_insert_node_unique(avl_tree_node<T>* node, 
        avl_tree_node<T>** root, const Comp& comp = Comp(),
		tree_node_base* header = nullptr) {
	using MyPair = std::pair<bst_node<T>*, bool>;
	bst_node<T>* rootBase = *root;
    MyPair myPair = bst_insert_node_unique(node, &rootBase, comp, header);
	*root = static_cast<avl_tree_node<T>*>(rootBase);
	if (myPair.second) {
		avl_insert_fixup(node, root);
	}
    
    return { static_cast<avl_tree_node<T>*>(myPair.first), myPair.second };
}

template<typename T>
std::pair<avl_tree_node<T>*, avl_tree_node<T>*>
avl_erase_node(avl_tree_node<T>* node, avl_tree_node<T>** root,
		tree_node_base* header = nullptr) {
	avl_tree_node<T>* repNode = nullptr;
	avl_tree_node<T>* fixNode = nullptr;
	avl_tree_node<T>* nsuccessor = successor(node);

	if (left_of(node) && right_of(node)) {
		if constexpr(std::is_move_assignable_v<T>) {
			node->value = std::move(nsuccessor->value);
			//swap(nsuccessor, node)
			repNode = nsuccessor;
            nsuccessor = node;
            node = repNode;
			fixNode = parent_of(node);
			transplant(node, right_of(node), root);
		} else {
			/**
			* 1、找到node的后继结点repNode
			* 2、用repNode的右孩子代替repNode
			* 3、用repNode代替node
			*	这里要注意一下，有可能repNode是node的右孩子
			*/
			repNode = nsuccessor;
			if (repNode == right_of(node)) { //替代结点为node的右孩子
				fixNode = repNode;
				repNode->left = left_of(node);
				node->left->parent = repNode;
				transplant(node, repNode, root);
			} else {
				fixNode = parent_of(repNode);
				repNode->parent->left = right_of(repNode);
				if (right_of(repNode)) {
					repNode->right->parent = parent_of(repNode);
				}

				repNode->left = left_of(node);
				repNode->right = right_of(node);
				node->left->parent = repNode;
				node->right->parent = repNode;
				transplant(node, repNode, root);
				repNode->height = height_of(node);
			}
		}
	} else {
		fixNode = parent_of(node);
		if (nullptr == left_of(node)) {
			repNode = right_of(node);
		} else if (nullptr == right_of(node)) {
			repNode = left_of(node);
		}
		transplant(node, repNode, root);
	}
	
	avl_erase_fixup(fixNode, root);
	if (header) {
		if (left_of(header) == node && 
			right_of(header) == node) {
			header->left = header->right = header;
			header->parent = nullptr;
		} else if (left_of(header) == node) {
			header->left = left_most(parent_of(header));
		} else if (right_of(header) == node) {
			header->right = right_most(parent_of(header));
		}
	}
	
	return { node, nsuccessor };
}

template<typename T, typename Comp = std::less<T>>
inline avl_tree_node<T>* 
avl_lbound(const T& val, avl_tree_node<T>* root, const Comp& comp = Comp()) {
    return static_cast<avl_tree_node<T>*>(bst_lbound(val, root, comp));
}

template<typename T, typename Comp = std::less<T>>
inline avl_tree_node<T>* 
avl_ubound(const T& val, avl_tree_node<T>* root, const Comp& comp = Comp()) {
    return static_cast<avl_tree_node<T>*>(bst_ubound(val, root, comp));
}

template<typename T, typename Comp = std::less<T>>
inline std::pair<avl_tree_node<T>*, avl_tree_node<T>*> 
avl_equal_range_multi(const T& val, avl_tree_node<T>* root,
        const Comp& comp = Comp()) {
    return { avl_lbound(val, root, comp), avl_ubound(val, root, comp) };
}

template<typename T, typename Comp = std::less<T>>
inline std::pair<avl_tree_node<T>*, avl_tree_node<T>*> 
avl_equal_range_unique(const T& val, avl_tree_node<T>* root,
        const Comp& comp = Comp()) {
    std::pair<bst_node<T>*, bst_node<T>*> myPair = bst_equal_range_unique(val, root, comp);
	return { static_cast<avl_tree_node<T>*>(myPair.first), 
			static_cast<avl_tree_node<T>*>(myPair.second) };
}

template<typename T, typename Size = size_t, typename Comp = std::less<T>>
inline Size 
avl_count_multi(const T& val, avl_tree_node<T>* root, const Comp& comp = Comp()) {
    return bst_count_multi<T, Size, Comp>(val, root, comp);
}

template<typename T, typename Size = size_t, typename Comp = std::less<T>>
inline Size
avl_count_unique(const T& val, avl_tree_node<T>* root, const Comp& comp = Comp()) {
    return bst_count_unique<T, Size, Comp>(val, root, comp);
}

template<typename T>
struct avl_iterator_base : public std::iterator<std::bidirectional_iterator_tag, T> {
	using node_base_ptr = typename avl_tree_node<T>::node_base_ptr;
	using node_ptr 		= avl_tree_node<T>*;
	using self			= avl_iterator_base;
	
	avl_iterator_base(node_base_ptr _header) noexcept :
		header(_header) {
	}

	avl_iterator_base(node_base_ptr _node, node_base_ptr _header) noexcept :
		node(_node),
		header(_header) {
	}

	bool operator==(const self& other) const noexcept { return node == other.node; }

	bool operator!=(const self& other) const noexcept { return node != other.node; }

	void increase() {
		node = successor(node);
		if (nullptr == node) {
			node = header;
		}
	}

	void decrease() {
		node = precursor(node);
		if (nullptr == node) {
			node = header;
		}
	}

	node_base_ptr node = nullptr;
	node_base_ptr header = nullptr;
};

template<typename T>
struct avl_iterator : public avl_iterator_base<T> {
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type   = ptrdiff_t;
	using size_type			= size_t;
	using value_type 		= T;
	using pointer 			= T*;
	using reference	 		= T&;
	using const_reference 	= T&;
	using node_base_ptr		= typename avl_iterator_base<T>::node_base_ptr;
	using node_ptr			= typename avl_iterator_base<T>::node_ptr;
	using self 				= avl_iterator<T>;

	avl_iterator(node_base_ptr header) noexcept :
		avl_iterator_base<T>(header) {
	}

	avl_iterator(node_base_ptr node, node_base_ptr header) noexcept : 
		avl_iterator_base<T>(node, header) { 
	}

	self& operator++() noexcept {
		this->increase();
		return *this;
	}

	self operator++(int) noexcept {
		self temp = *this;
		++*this;
		return temp;
	}

	self& operator--() noexcept {
		this->decrease();
		return *this;
	}

	self operator--(int) noexcept {
		self temp = *this;
		--* this;
		return temp;
	}

	reference operator*() const noexcept {
		return (static_cast<node_ptr>(this->node))->value;
	}

	pointer operator->() const noexcept {
		return &(operator*());
	}
};

template<typename T>
struct avl_const_iterator : public avl_iterator_base<T> {
	using category 			= std::bidirectional_iterator_tag;
	using size_type			= size_t;
	using value_type 		= T;
	using pointer 			= T*;
	using reference	 		= T&;
	using const_reference 	= T&;
	using node_base_ptr		= typename avl_iterator_base<T>::node_base_ptr;
	using node_ptr			= typename avl_iterator_base<T>::node_ptr;
	using self 				= avl_const_iterator<T>;

	avl_const_iterator(node_base_ptr header) noexcept :
		avl_iterator_base<T>(header) {
	}

	avl_const_iterator(node_base_ptr node, node_base_ptr header) noexcept : 
		avl_iterator_base<T>(node) { 
	}

	self& operator++() noexcept {
		this->increase();
		return *this;
	}

	self operator++(int) noexcept {
		self temp = *this;
		++*this;
		return temp;
	}

	self& operator--() noexcept {
		this->decrease();
		return *this;
	}

	self operator--(int) noexcept {
		self temp = *this;
		--* this;
		return temp;
	}

	reference operator*() const noexcept {
		return (static_cast<node_ptr>(this->node))->value;
	}

	pointer operator->() const noexcept {
		return &(operator*());
	}
};

template<typename T, typename Comp = std::less<T>>
class avl_tree {
friend class avl_iterator<T>;
friend class avl_const_iterator<T>;
public:
	using key_type 					= T;
	using mapped_type 				= T;
	using value_type                = T;
	using pointer                   = T*;
	using const_pointer             = const T*;
	using reference                 = T&;
	using const_reference           = const T&;
	using size_type                 = size_t;
	using difference_type           = ptrdiff_t;
	using const_iterator            = avl_const_iterator<value_type>;
	using iterator                  = avl_iterator<value_type>;
	using reverse_iterator          = const std::reverse_iterator<iterator>;
	using const_reverse_iterator    = const std::reverse_iterator<const_iterator>;

private:
	using node_base 				= tree_node_base;
	using node 						= avl_tree_node<T>;
	using node_base_ptr				= node_base*;
	using node_ptr 					= node*;
	using height_type				= typename avl_tree_node<T>::height_type;

public:
	iterator begin() noexcept { 
		return iterator(static_cast<node_ptr>(left_of(m_header)), m_header); 
	}
	iterator end() noexcept { 
		return iterator(m_header, m_header); 
	}
	reverse_iterator rbegin() noexcept { 
		return std::reverse_iterator<iterator>(end()); 
	}
	reverse_iterator rend() noexcept { 
		return std::reverse_iterator<iterator>(begin()); 
	}

	const_iterator begin() const noexcept { 
		return const_iterator(static_cast<node_ptr>(left_of(m_header)), m_header); 
	}
	const_iterator end() const noexcept { 
		return const_iterator(m_header, m_header); 
	}
	const_reverse_iterator rbegin() const noexcept { 
		return std::reverse_iterator<const_iterator>(end()); 
	}
	const_reverse_iterator rend() const noexcept { 
		return std::reverse_iterator<const_iterator>(begin()); 
	}
// public:
// 	friend auto operator<=>(const avl_tree<T, Comp>& lhs, 
// 			const avl_tree<T, Comp>& rhs) {
// 		return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(),
// 			rhs.begin(), rhs.end(), lhs.m_comp);
// 	}

public:
	avl_tree(const Comp& comp = Comp()) ;

	avl_tree(const std::initializer_list<key_type>& ilist, const Comp& comp = Comp()) :
		avl_tree(ilist.begin(), ilist.end(), comp) {
	}

	template<std::input_iterator InputIter>
	avl_tree(InputIter first, InputIter last, const Comp& comp = Comp());

	avl_tree(const avl_tree& other);

	avl_tree(avl_tree&& other);

	~avl_tree();

	avl_tree& operator=(const avl_tree& other);

	avl_tree& operator=(avl_tree&& other);

	//emplace
	template <typename ...Args>
	iterator emplace_multi(Args&& ...args);

	template <typename ...Args>
	iterator emplace_multi_hint(iterator hint, Args&& ...args);

	template <typename ...Args>
	std::pair<iterator, bool> emplace_unique(Args&& ...args);

	template <typename ...Args>
	std::pair<iterator, bool> emplace_unique_hint(iterator hint, Args&& ...args);

	//insert
	iterator insert_multi(const value_type& value) {
		return emplace_multi(value);
	}

	iterator insert_multi(value_type&& value) { 
		return emplace_multi(std::move(value)); 
	}

	iterator insert_multi(iterator hint, const value_type& value) {
		return emplace_multi_use_hint(hint, value);
	}

	iterator insert_multi(iterator hint, value_type&& value) {
		return emplace_multi_use_hint(hint, std::move(value));
	}

	template <std::input_iterator InputIter>
	void insert_multi(InputIter first, InputIter last);

	std::pair<iterator, bool> insert_unique(const value_type& value) {
		return emplace_unique(value);
	}

	std::pair<iterator, bool> insert_unique(value_type&& value) {
		return emplace_unique(std::move(value));
	}

	std::pair<iterator, bool> 
	insert_unique_hint(iterator hint, const value_type& value) {
		return emplace_unique_hint(hint, value);
	}

	std::pair<iterator, bool> 
	insert_unique_hint(iterator hint, value_type&& value) {
		return emplace_unique_hint(hint, std::move(value));
	}

	template <std::input_iterator InputIter>
	void insert_unique(InputIter first, InputIter last);

	//erase
	iterator  erase(iterator hint);
  	size_type erase_multi(const key_type& key);
  	size_type erase_unique(const key_type& key);
  	void erase(iterator first, iterator last);
  	void clear();

	//find
	iterator find(const key_type& key) noexcept;
	const_iterator find(const key_type& key) const noexcept;

	size_type count_multi(const key_type& key) noexcept;
	size_type count_multi(const key_type& key) const noexcept;
	size_type count_unique(const key_type& key) noexcept;
	size_type count_unique(const key_type& key) const noexcept;

	iterator lower_bound(const key_type& key) noexcept;
	const_iterator lower_bound(const key_type& key) const noexcept;

	iterator upper_bound(const key_type& key) noexcept;
	const_iterator upper_bound(const key_type& key) const noexcept;

	std::pair<iterator, iterator>             
	equal_range_multi(const key_type& key) noexcept {
		return { lower_bound(key), upper_bound(key) };
	}

	std::pair<const_iterator, const_iterator> 
	equal_range_multi(const key_type& key) const noexcept {
		return { lower_bound(key), upper_bound(key) };
	}

	std::pair<iterator, iterator>             
	equal_range_unique(const key_type& key) noexcept {
		using MyPair = std::pair<node_ptr, node_ptr>;
		MyPair myPair = avl_equal_range_unique(key, parent_of(m_header));
		return { iterator(myPair.first, myPair.second), iterator(myPair.second) };
	}
	std::pair<const_iterator, const_iterator> 
	equal_range_unique(const key_type& key) const noexcept {
		using MyPair = std::pair<node_ptr, node_ptr>;
		MyPair myPair = avl_equal_range_unique(key, parent_of(m_header));
		return { const_iterator(myPair.first, myPair.second), 
				const_iterator(myPair.second) };
	}

	//other
	void swap(avl_tree& rhs) noexcept;
	size_type size() const noexcept { return m_size; }
	bool empty() const noexcept { return 0 == m_size; }

#ifdef AVL_DEBUUG
public:
	bool balanced() { return is_balanced(m_header->parent); }
	std::string serialize();
	bool check_header();
#endif //AVL_DEBUUG
	
private:
	template<typename... Args>
	node_ptr create_node(Args&&... args);
	node_base_ptr create_node_base();
	void destroy_node(node_ptr node);
	void destroy_node_base(node_base_ptr node);
	node_ptr get_root() const noexcept {
		return static_cast<node_ptr>(parent_of(m_header));
	}
	node_ptr copy_node(node_ptr node) {
		return create_node(node->value);
	}

private:
	node_base_ptr m_header = nullptr; 	///< 与根节点互为父亲节点
	size_type m_size = 0;				///< 节点数量
	const Comp& m_comp;
};

#ifdef AVL_DEBUUG
template<typename T, typename Comp>
std::string avl_tree<T, Comp>::serialize() {
	std::string strTree;
	node_ptr root = get_root();

	std::queue<node_ptr> que;
	que.push(root);
	while (!que.empty()) {
		decltype(que.size()) sz = que.size();
		bool allNull = true;
		while (sz) {
			node_ptr currNode = que.front();
			que.pop();
			--sz;
			if (currNode != root) {
				strTree.push_back(' ');
			}
			if (currNode) {
				strTree += std::to_string(currNode->value) + "," + std::to_string(currNode->height);
				allNull = false;
				que.push((node_ptr)currNode->left);
				que.push((node_ptr)currNode->right);
			} else {
				strTree += "null";
			}
		}
		if (allNull) {
			break;
		}
	}

	return strTree;
}

template<typename T, typename Comp>
bool avl_tree<T, Comp>::check_header() {
	bool result = true;
	if (0 == m_size) {
		return result;
	}
	if (m_header->left != min_node(m_header->parent)) {
		std::cout << "m_header->left错误" << std::endl;
		result = false;
	}
	if (m_header->right != max_node(m_header->parent)) {
		std::cout << "m_header->right错误" << std::endl;
		result = false;
	}

	return result;
}
#endif //AVL_DEBUUG

template<typename T, typename Comp>
template<typename... Args>
typename avl_tree<T, Comp>::node_ptr
avl_tree<T, Comp>::create_node(Args&&... args) {
	node_ptr newNode = static_cast<node_ptr>(::operator new(sizeof(node)));
	try {
		construct(&newNode->value, std::forward<Args>(args)...);
	} catch (...) {
		::operator delete(newNode);
		throw;
	}
	newNode->left = newNode->right = newNode->parent = nullptr;
	return newNode;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::node_base_ptr
avl_tree<T, Comp>::create_node_base() {
	node_base_ptr node = static_cast<node_base_ptr>(::operator new(sizeof(node_base)));
	node->left = node->right = node;
	node->parent = nullptr;
	return node;
}

template<typename T, typename Comp>
void avl_tree<T, Comp>::destroy_node(node_ptr node) {
	destroy(&node->value);
	::operator delete(node);
}

template<typename T, typename Comp>
void avl_tree<T, Comp>::destroy_node_base(node_base_ptr node) {
	::operator delete(node);
}

template<typename T, typename Comp>
avl_tree<T, Comp>::avl_tree(const Comp& comp):
	m_header(create_node_base()),
	m_size(0),
	m_comp(comp) {
}

template<typename T, typename Comp>
template<std::input_iterator InputIter>
avl_tree<T, Comp>::avl_tree(InputIter first, InputIter last, const Comp& comp) :
		m_header(create_node_base()),
		m_size(0),
		m_comp(comp) {
	for ( ; first != last; ++first) {
		insert_multi(*first);
	}
}

template<typename T, typename Comp>
avl_tree<T, Comp>::avl_tree(const avl_tree& other) :
		m_header(create_node_base()),
		m_size(other.m_size),
		m_comp(other.m_comp) {
	if (other.size()) {
		m_header->parent = copy_since(parent_of(other.m_header), [this](tree_node_base* node) {
			return copy_node(static_cast<node_ptr>(node));
		});
		m_header->left = min_node(parent_of(m_header));
		m_header->right = max_node(parent_of(m_header));
	}
}

template<typename T, typename Comp>
avl_tree<T, Comp>::avl_tree(avl_tree&& other) :
		m_size(other.m_size),
		m_header(other.m_header) {
	other.m_size = 0;
	other.m_header = create_node_base();
}

template<typename T, typename Comp>
avl_tree<T, Comp>::~avl_tree() {
	clear();
	destroy_node_base(m_header);
}

template<typename T, typename Comp>
avl_tree<T, Comp>&
avl_tree<T, Comp>::operator=(const avl_tree& other) {
	if (this != &other) {
		clear();
		if (other.m_size != 0) {
			m_header->parent = copy_since(parent_of(other.m_header), [this](tree_node_base* node){
				return copy_node(static_cast<node_ptr>(node));
			});
			m_header->left = min_node(parent_of(m_header));
			m_header->right = max_node(parent_of(m_header));
		}
    	m_size = other.m_size;
		m_comp = other.m_comp;
	}

	return *this;
}

template<typename T, typename Comp>
avl_tree<T, Comp>& 
avl_tree<T, Comp>::operator=(avl_tree&& other) {
	if (this != &other) {
		clear();
		m_size = other.m_size;
		m_header = other.m_header;
		other.m_size = 0;
		other.m_header = create_node_base();
		m_comp = other.m_comp;
	}
	
	return *this;
}

template<typename T, typename Comp>
template <typename ...Args>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::emplace_multi(Args&& ...args) {
	node_ptr newNode = create_node(std::forward<Args>(args)...);
	node_ptr root = get_root();
	node_ptr node = avl_insert_node_multi(newNode, &root, m_comp, m_header);
	m_header->parent = root;
	++m_size;
	return iterator(node, m_header);
}

template<typename T, typename Comp>
template <typename ...Args>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::emplace_multi_hint(iterator hint, Args&& ...args) {
	return emplace_multi(std::forward<Args>(args)...);
}

template<typename T, typename Comp>
template <typename ...Args>
std::pair<typename avl_tree<T, Comp>::iterator, bool> 
avl_tree<T, Comp>::emplace_unique(Args&& ...args) {
	node_ptr newNode = create_node(std::forward<Args>(args)...);
	node_ptr root = get_root();
	std::pair<node_ptr, bool> myPair = avl_insert_node_unique(newNode, &root, m_comp, m_header);
	if (!myPair.second) {
		//插入失败
		destroy_node(newNode);
	} else {
		++m_size;
		m_header->parent = root;
	}
	return { iterator(myPair.first, m_header), myPair.second };
}

template<typename T, typename Comp>
template <typename ...Args>
std::pair<typename avl_tree<T, Comp>::iterator, bool> 
avl_tree<T, Comp>::emplace_unique_hint(iterator hint, Args&& ...args) {
	return emplace_unique(std::forward<Args>(args)...);
}

//insert
template<typename T, typename Comp>
template <std::input_iterator InputIter>
void avl_tree<T, Comp>::insert_multi(InputIter first, InputIter last) {
	for (; first != last; ++last) {
		insert_multi(*first);
	}
}

template<typename T, typename Comp>
template <std::input_iterator InputIter>
void avl_tree<T, Comp>::insert_unique(InputIter first, InputIter last) {
	for (; first != last; ++last) {
		insert_unique(*first);
	}
}

//erase
template<typename T, typename Comp>
typename avl_tree<T, Comp>::iterator
avl_tree<T, Comp>::erase(iterator hint) {
	node_ptr node = static_cast<node_ptr>(hint.node);
	node_ptr root = static_cast<node_ptr>(parent_of(m_header));
	std::pair<node_ptr, node_ptr> myPair = avl_erase_node(node, &root, m_header);
	m_header->parent = root;
	destroy_node(myPair.first);
	--m_size;
	return iterator(myPair.second, m_header);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::size_type 
avl_tree<T, Comp>::erase_multi(const key_type& key) {
	std::pair<iterator, iterator> p = equal_range_multi(key);
	size_type n = std::distance(p.first, p.last);
	erase(p.first, p.last);
	return n;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::size_type 
avl_tree<T, Comp>::erase_unique(const key_type& key) {
	iterator iter = find(key);
	if (iter != end()) {
		erase(iter);
		return 1;
	}
	return 0;
}

template<typename T, typename Comp>
void avl_tree<T, Comp>::erase(iterator first, iterator last) {
	if (begin() == first && end() == last) {
		clear();
	} else {
		for ( ; first != last; ) {
			erase(first++);
		}
	}
}

template<typename T, typename Comp>
void avl_tree<T, Comp>::clear() {
	clear_since(m_header->parent, [this](tree_node_base* node) {
		this->destroy_node(static_cast<node_ptr>(node));
	});
	m_header->parent = m_header->left = m_header->right = nullptr;
}

//find
template<typename T, typename Comp>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::find(const key_type& key) noexcept {
	node_ptr node = avl_lbound(key, static_cast<node_ptr>(parent_of(m_header)));
	if (nullptr == node || m_comp(key, node->value)) {
		return end();
	}

	return iterator(node, m_header);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::const_iterator 
avl_tree<T, Comp>::find(const key_type& key) const noexcept {
	node_ptr node = avl_lbound(key, static_cast<node_ptr>(parent_of(m_header)));
	if (nullptr == node || m_comp(key, node->value)) {
		return end();
	}

	return const_iterator(node, m_header);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::size_type 
avl_tree<T, Comp>::count_multi(const key_type& key) noexcept {
	node_ptr root = static_cast<node_ptr>(parent_of(m_header));
	return avl_count_multi<T, size_type, Comp>(key, root, m_comp);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::size_type 
avl_tree<T, Comp>::count_multi(const key_type& key) const noexcept {
	node_ptr root = static_cast<node_ptr>(parent_of(m_header));
	return avl_count_multi<T, size_type, Comp>(key, root, m_comp);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::size_type 
avl_tree<T, Comp>::count_unique(const key_type& key) noexcept {
	node_ptr root = static_cast<node_ptr>(parent_of(m_header));
	return avl_count_unique<T, size_type, Comp>(key, root, m_comp);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::size_type 
avl_tree<T, Comp>::count_unique(const key_type& key) const noexcept {
	node_ptr root = static_cast<node_ptr>(parent_of(m_header));
	return avl_count_unique<T, size_type, Comp>(key, root, m_comp);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::lower_bound(const key_type& key) noexcept {
	node_ptr root = static_cast<node_ptr>(parent_of(m_header));
	return iterator(avl_lbound(key, root, m_comp), m_header);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::const_iterator 
avl_tree<T, Comp>::lower_bound(const key_type& key) const noexcept {
	node_ptr root = static_cast<node_ptr>(parent_of(m_header));
	return const_iterator(avl_lbound(key, root, m_comp), m_header);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::upper_bound(const key_type& key) noexcept {
	node_ptr root = static_cast<node_ptr>(parent_of(m_header));
	return iterator(avl_ubound(key, root, m_comp), m_header);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::const_iterator 
avl_tree<T, Comp>::upper_bound(const key_type& key) const noexcept {
	node_ptr root = static_cast<node_ptr>(parent_of(m_header));
	return const_iterator(avl_ubound(key, root, m_comp), m_header);
}

template<typename T, typename Comp>
bool operator<(const avl_tree<T, Comp>& lhs, const avl_tree<T, Comp>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), 
			rhs.begin(), rhs.end(), Comp());
}

template<typename T, typename Comp>
bool operator>(const avl_tree<T, Comp>& lhs, const avl_tree<T, Comp>& rhs) {
	return rhs < lhs;
}

template<typename T, typename Comp>
bool operator<=(const avl_tree<T, Comp>& lhs, const avl_tree<T, Comp>& rhs) {
	return !(rhs < lhs);
}

template<typename T, typename Comp>
bool operator>=(const avl_tree<T, Comp>& lhs, const avl_tree<T, Comp>& rhs) {
	return !(lhs < rhs);
}

template<typename T, typename Comp>
bool operator==(const avl_tree<T, Comp>& lhs, const avl_tree<T, Comp>& rhs) {
	if (lhs.size() != rhs.size()) {
		return false;
	}
	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, typename Comp>
bool operator!=(const avl_tree<T, Comp>& lhs, const avl_tree<T, Comp>& rhs) {
	return !(lhs == rhs);
}

} //namespace nano