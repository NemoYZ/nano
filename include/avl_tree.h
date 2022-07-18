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
#include <queue>
#include "algorithm.h"
#include "tree.h"
#include "construct.h"
#include "type_traits.h"
#include "concepts.h"

namespace nano {

struct avl_tree_node_base : tree_node_base {
	using self 			= avl_tree_node_base;
	using node_base_ptr = self*;
	using height_type 	= int;
	
	avl_tree_node_base() noexcept = default;

	avl_tree_node_base(node_base_ptr left, node_base_ptr right) noexcept :
        tree_node_base(left, right) {
    }

    avl_tree_node_base(node_base_ptr left, node_base_ptr right, 
            node_base_ptr parent) noexcept :
			tree_node_base(left, right, parent) {
    }

	height_type height = 1;	//高度
};

template<typename T>
struct avl_tree_node : public avl_tree_node_base {
	using node_base_ptr = avl_tree_node_base*;
	using height_type 	= typename avl_tree_node_base::height_type;
	using self 			= avl_tree_node<T>;

	avl_tree_node() noexcept = default;

	avl_tree_node(const T& _value) noexcept(std::is_nothrow_assignable_v<T>) :
		value(_value) {
	}

	avl_tree_node(T&& rvalue) noexcept(std::is_nothrow_move_assignable_v<T>) :
		value(std::move<T>(rvalue)) {
	}
    
	avl_tree_node(node_base_ptr left, node_base_ptr right) noexcept :
        avl_tree_node_base(left, right) {
    }

    avl_tree_node(node_base_ptr left, node_base_ptr right, 
            node_base_ptr parent) noexcept :
			avl_tree_node_base(left, right, parent) {
    }

    avl_tree_node(node_base_ptr left, node_base_ptr right, 
            node_base_ptr parent, const T& _value) noexcept(std::is_nothrow_assignable_v<T>) :
        avl_tree_node_base(left, right, parent),
		value(_value) {
    }

	avl_tree_node(node_base_ptr left, node_base_ptr right,
            node_base_ptr parent, T&& rvalue) noexcept(std::is_nothrow_move_assignable_v<T>) :
        avl_tree_node_base(left, right, parent),
		value(std::move(rvalue)) {
    }

	self* get_node_ptr() {
		return &*this;
	}

	T value;
};

avl_tree_node_base::height_type height_of(avl_tree_node_base* node) {
	return node ? node->height : 0;
}

avl_tree_node_base::height_type balance_factor(avl_tree_node_base* node) {
	return node ? height_of(static_cast<avl_tree_node_base*>(node->left)) - 
		height_of(static_cast<avl_tree_node_base*>(node->right)) : 0;
}

avl_tree_node_base* avl_precursor(avl_tree_node_base* node) {
	if (-1 == height_of(node)) { //header
		return static_cast<avl_tree_node_base*>(node->right);
	}
	return static_cast<avl_tree_node_base*>(precursor(node));
}

template<typename T>
struct avl_iterator_base : public std::iterator<std::bidirectional_iterator_tag, T> {
	using self			= avl_iterator_base;
	using node_base_ptr = avl_tree_node_base*;
	using node_ptr 		= avl_tree_node<T>*;

	avl_iterator_base() noexcept = default;

	avl_iterator_base(node_base_ptr _node) noexcept :
		node(_node) {
	}

	avl_iterator_base(node_ptr _node) noexcept :
		node(_node) {
	}

	bool operator==(const self& other) const noexcept { return node == other.node; }

	bool operator!=(const self& other) const noexcept { return node != other.node; }

	node_base_ptr node = nullptr;
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

	avl_iterator() noexcept = default;
	avl_iterator(node_ptr _node) noexcept : 
		avl_iterator_base<T>(_node) { 
	}
	avl_iterator(node_base_ptr _node) noexcept :
		avl_iterator_base<T>(_node) {
	}

	self& operator++() noexcept {
		this->node = static_cast<avl_tree_node_base*>(successor(this->node));
		return *this;
	}

	self& operator++(int) noexcept {
		self temp = *this;
		++*this;
		return temp;
	}

	self& operator--() noexcept {
		this->node = avl_precursor(this->node);
		return *this;
	}

	self& operator--(int) noexcept {
		self temp = *this;
		--* this;
		return temp;
	}

	reference operator*() const noexcept {
		return (static_cast<avl_tree_node<T>*>(this->node))->value;
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

	avl_const_iterator() noexcept = default;

	avl_const_iterator(avl_tree_node<T>* _node) noexcept : 
		avl_iterator_base<T>(_node) { 
	}

	avl_const_iterator(node_base_ptr _node) noexcept : 
		avl_iterator_base<T>(_node) { 
	}

	self& operator++() noexcept {
		this->node = static_cast<avl_tree_node_base*>(successor(this->node));
		return *this;
	}

	self& operator++(int) noexcept {
		self temp = *this;
		++*this;
		return temp;
	}

	self& operator--() noexcept {
		this->node = avl_precursor(this->node);
		return *this;
	}

	self& operator--(int) noexcept {
		self temp = *this;
		--* this;
		return temp;
	}

	reference operator*() const noexcept {
		return (static_cast<avl_tree_node<T>*>(this->node))->value;
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
	using node_ptr 					= avl_tree_node<T>*;
	using node_base_ptr				= avl_tree_node_base*;

public:
	iterator begin() noexcept { return (node_base_ptr)m_header->left; }
	iterator end() noexcept { return m_header; }
	reverse_iterator rbegin() noexcept { return std::reverse_iterator<iterator>(end()); }
	reverse_iterator rend() noexcept { return std::reverse_iterator<iterator>(begin()); }

	const_iterator begin() const noexcept { return (node_base_ptr)min_node(m_header->parent); }
	const_iterator end() const noexcept { return m_header; }
	const_reverse_iterator rbegin() const noexcept { return std::reverse_iterator<const_iterator>(end()); }
	const_reverse_iterator rend() const noexcept { return std::reverse_iterator<const_iterator>(begin()); }

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
		iterator iter = find(key);
		iterator next = iter;
		if (end() == iter) { 
			return { iter, iter };
		}

		return { iter, ++next };
	}
	std::pair<const_iterator, const_iterator> 
	equal_range_unique(const key_type& key) const noexcept {
		const_iterator iter = find(key);
		const_iterator next = iter;
		if (end() == iter) {
			return { iter, iter };	
		}
		return { iter, ++next };
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
	node_ptr avl_left_rotate(node_ptr node, node_base_ptr* root);
	node_ptr avl_right_rotate(node_ptr node, node_base_ptr* root);
	node_ptr avl_left_right_rotate(node_ptr node, node_base_ptr* root);
	node_ptr avl_right_left_rotate(node_ptr node, node_base_ptr* root);
	iterator lbound(const key_type& key);
	iterator ubound(const key_type& key);

private:
	template<typename... Args>
	node_ptr create_node(Args&&... args);
	node_base_ptr create_node_base();
	static void destroy_node(tree_node_base* node);
	static void destroy_node_base(node_base_ptr node);
	node_ptr insert_fixup(node_ptr node);
	node_ptr erase_fixup(node_ptr node);
	std::pair<node_ptr, bool> get_insert_muti(const key_type& key);
	std::pair<node_ptr, int> get_insert_unique(const key_type& key);
	iterator insert_node(node_ptr parent, node_ptr node, bool insert_left);
	node_ptr copy_node(node_base_ptr node);

private:
	node_base_ptr m_header = nullptr; 	///< 与根节点互为父亲节点
	size_type m_size = 0;				///< 节点数量
	const Comp& m_comp;
};

#ifdef AVL_DEBUUG
template<typename T, typename Comp>
std::string avl_tree<T, Comp>::serialize() {
	std::string strTree;
	node_ptr root = (node_ptr)m_header->parent;

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
typename avl_tree<T, Comp>::node_ptr 
avl_tree<T, Comp>::avl_left_rotate(node_ptr node, node_base_ptr* root) {
	node = static_cast<node_ptr>(left_rotate(node, reinterpret_cast<tree_node_base**>(root)));
	node_ptr lchild = static_cast<node_ptr>(node->left);
	lchild->height = std::max(height_of(static_cast<node_ptr>(lchild->left)), 
			height_of(static_cast<node_ptr>(lchild->right))) + 1;
	node->height = std::max(height_of(static_cast<node_ptr>(node->left)), 
			height_of(static_cast<node_ptr>(node->right))) + 1;
	return node;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::node_ptr 
avl_tree<T, Comp>::avl_right_rotate(node_ptr node, node_base_ptr* root) {
	node = static_cast<node_ptr>(right_rotate(node, reinterpret_cast<tree_node_base**>(root)));
	node_ptr rchild = (node_ptr)node->right;
	rchild->height = std::max(height_of(static_cast<node_ptr>(rchild->left)), 
			height_of(static_cast<node_ptr>(rchild->right))) + 1;
	node->height = std::max(height_of(static_cast<node_ptr>(node->left)), 
			height_of(static_cast<node_ptr>(node->right))) + 1;
	return node;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::node_ptr 
avl_tree<T, Comp>::avl_left_right_rotate(node_ptr node, node_base_ptr* root) {
	avl_left_rotate(static_cast<node_ptr>(node->left), root);
	return avl_right_rotate(node, root);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::node_ptr 
avl_tree<T, Comp>::avl_right_left_rotate(node_ptr node, node_base_ptr* root) {
	avl_right_rotate(static_cast<node_ptr>(node->right), root);
	return avl_left_rotate(node, root);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::lbound(const key_type& key) {
	node_ptr root = (node_ptr)m_header->parent;
	node_base_ptr parent = m_header; //最后一个不小于key的节点
	while (root) {
		if (!m_comp(root->value, key)) {
			parent = root;
			root = (node_ptr)root->left;
		} else {
			root = (node_ptr)root->right;
		}
	}

	return parent;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::ubound(const key_type& key) {
	node_ptr root = static_cast<node_ptr>(m_header->parent);
	node_base_ptr parent = m_header; //第一个大于key的节点
	while (root) {
		if (m_comp(key, root->value)) {
			parent = root;
			root = static_cast<node_ptr>(root->left);
		} else {
			root = static_cast<node_ptr>(root->right);
		}
	}

	return parent;
}

template<typename T, typename Comp>
template<typename... Args>
typename avl_tree<T, Comp>::node_ptr
avl_tree<T, Comp>::create_node(Args&&... args) {
	node_ptr newNode = static_cast<node_ptr>(::operator new(sizeof(avl_tree_node<T>)));
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
	node_base_ptr node = static_cast<node_base_ptr>(::operator new(sizeof(avl_tree_node_base)));
	node->left = node->right = node;
	node->parent = nullptr;
	node->height = -1;
	return node;
}

template<typename T, typename Comp>
void avl_tree<T, Comp>::destroy_node(tree_node_base* node) {
	destroy(&(static_cast<node_ptr>(node))->value);
	::operator delete(node);
}

template<typename T, typename Comp>
void avl_tree<T, Comp>::destroy_node_base(node_base_ptr node) {
	::operator delete(node);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::node_ptr 
avl_tree<T, Comp>::insert_fixup(node_ptr node) {
	while (node != m_header) {
		node->height = std::max(height_of(static_cast<node_ptr>(node->left)), 
				height_of(static_cast<node_ptr>(node->right))) + 1;
		typename avl_tree_node<T>::height_type bf = balance_factor(static_cast<node_ptr>(node));
		if (bf > 1) {
			typename avl_tree_node<T>::height_type lbf = balance_factor(static_cast<node_ptr>(node->left));
			if (lbf > 0) {
				node = avl_right_rotate(node, reinterpret_cast<node_base_ptr*>(&m_header->parent));
			} else if (lbf < 0) {
				node = avl_left_right_rotate(node, reinterpret_cast<node_base_ptr*>(&m_header->parent));
			}
		} else if (bf < -1) {
			typename avl_tree_node<T>::height_type rbf = balance_factor(static_cast<node_ptr>(node->right));
			if (rbf < 0) {
				node = avl_left_rotate(node, reinterpret_cast<node_base_ptr*>(&m_header->parent));
			} else if (rbf > 0) {
				node = avl_right_left_rotate(node, reinterpret_cast<node_base_ptr*>(&m_header->parent));
			}
		}
		node = static_cast<node_ptr>(parent_of(node));
	}
	
	return node;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::node_ptr  
avl_tree<T, Comp>::erase_fixup(node_ptr node) {
	while (node != m_header) {
		node->height = std::max(height_of((node_ptr)node->left), height_of((node_ptr)node->right)) + 1;
		typename avl_tree_node<T>::height_type bf = balance_factor(node);
		/**
		* 解释一下这里为什么是>=0或者<=0
		* 在插入当中，我们是不可能出现bf为+2并且右子树的bf为0的情况，但在删除的时候是有可能的，下面举例
		*			 node											node			(右旋就可以解决问题，也就是第一个if语句)
		*			/	\				删除结点right			   /
		*		 left   right		 ------------------>         left
		*		/												 /
		*	   lleft										   lleft
		* 在插入的时候是绝对不可能出现以上情况的，你不可能在已经不平衡的条件下再进行插入
		*/
		if (bf > 1) {
			if (balance_factor(static_cast<node_ptr>(node->left)) >= 0) {
				node = avl_right_rotate(node, reinterpret_cast<node_base_ptr*>(&m_header->parent));
			} else {
				node = avl_left_right_rotate(node, reinterpret_cast<node_base_ptr*>(&m_header->parent));
			}
		} else if (bf < -1) {
			if (balance_factor((node_ptr)node->right) <= 0) {
				node = avl_left_rotate(node, reinterpret_cast<node_base_ptr*>(&m_header->parent));
			} else {
				node = avl_right_left_rotate(node, reinterpret_cast<node_base_ptr*>(&m_header->parent));
			}
		}
		node = static_cast<node_ptr>(parent_of(node));
	}
	
	return node;
}

template<typename T, typename Comp>
std::pair<typename avl_tree<T, Comp>::node_ptr, bool>
avl_tree<T, Comp>::get_insert_muti(const key_type& key) {
	node_ptr root = static_cast<node_ptr>((m_header->parent));
	node_ptr parent = static_cast<node_ptr>((m_header));
	bool insert_left = true;
	while (root && root != m_header) {
		parent = root;
		insert_left = m_comp(key, root->value);
		root = static_cast<node_ptr>((insert_left ? root->left : root->right));
	}	
	return { parent, insert_left };
}

template<typename T, typename Comp>
std::pair<typename avl_tree<T, Comp>::node_ptr, int>
avl_tree<T, Comp>::get_insert_unique(const key_type& key) {
	node_ptr root = static_cast<node_ptr>(m_header->parent);
	node_base_ptr parent = m_header;
	int insert_left = 1;
	while (root && root != m_header) {	
		parent = root;
		if (m_comp(key, root->value)) {
			root = static_cast<node_ptr>(root->left);
			insert_left = 1;
		} else if (m_comp(root->value, key)) {
			root = static_cast<node_ptr>(root->right);
			insert_left = -1;
		} else {
			insert_left = 0;
			break;
		}
	}	
	return { static_cast<node_ptr>(parent), insert_left };
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::insert_node(node_ptr parent, node_ptr node, bool insert_left) {
	node->parent = parent;
	if (parent == m_header) {
		m_header->parent = node;
		m_header->left = m_header->right = node;
	}  else {
		if (insert_left) {
			parent->left = node;
			if (parent == m_header->left) {
				m_header->left = node;
			}
		} else {
			parent->right = node;
			if (parent == m_header->right) {
				m_header->right = node;
			}
		}
	}
	insert_fixup(node);
	++m_size;
	return node;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::node_ptr 
avl_tree<T, Comp>::copy_node(node_base_ptr node) {
	node_ptr newNode = create_node((static_cast<node_ptr>(node))->value);
	newNode->height = node->height;
	newNode->left = newNode->right = nullptr;
	return newNode;
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
		m_header->parent = copy_since(other.m_header->parent, [this](tree_node_base* node){
			return static_cast<tree_node_base*>(copy_node(static_cast<node_base_ptr>(node)));
		});
		m_header->parent->parent = m_header;
		m_header->left = min_node(m_header->parent);
		m_header->right = max_node(m_header->parent);
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
			m_header->parent = copy_since(other.m_header->parent, m_header, copy_node);
			m_header->left = min_node(m_header);
			m_header->right = max_node(m_header);
		}
    	m_size = other.m_size;
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
	}
	
	return *this;
}

template<typename T, typename Comp>
template <typename ...Args>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::emplace_multi(Args&& ...args) {
	node_ptr newNode = create_node(std::forward<Args>(args)...);
	std::pair<node_ptr, bool> pos = get_insert_muti(newNode->value);
	return insert_node(pos.first, newNode, pos.second);
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
	std::pair<node_ptr, int> pos = get_insert_unique(newNode->value);
	if (pos.second != 0) {
		return { insert_node(pos.first, newNode, pos.second == 1), true };
	}

	//插入失败
	destroy_node(newNode);
	return { pos.first, false };
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
	iterator iter = hint;
	if (end() == iter) {
		return end();
	}

	node_base_ptr node = iter.node;
	node_base_ptr replace_node = nullptr;
	node_base_ptr fix_node = nullptr;
	node_base_ptr next = (node_base_ptr)successor(node);

	if (node->left && node->right) {
		/**
		* 1、找到node的后继结点replace_node
		* 2、用replace_node的右孩子代替replace_node
		* 3、用replace_node代替node
		*	这里要注意一下，有可能replace_node是node的右孩子
		*/
		replace_node = next;
		if (replace_node == node->right) { //替代结点为node的右孩子
			fix_node = replace_node;
			replace_node->left = node->left;
			node->left->parent = replace_node;
			transplant(node, replace_node, &m_header->parent);
		} else {
			fix_node = (node_base_ptr)replace_node->parent;
			replace_node->parent->left = replace_node->right;
			if (replace_node->right) {
				replace_node->right->parent = replace_node->parent;
			}

			replace_node->left = node->left;
			replace_node->right = node->right;
			node->left->parent = replace_node;
			node->right->parent = replace_node;
			transplant(node, replace_node, &m_header->parent);
			((node_ptr)replace_node)->height = ((node_ptr)node)->height;
		}
	} else {
		fix_node = (node_base_ptr)node->parent;
		if (nullptr == node->left) {
			replace_node = (node_base_ptr)(node->right);
		} else if (nullptr == node->right) {
			replace_node = (node_base_ptr)node->left; //这里replace_node是不可能为空的
		}
		transplant(node, replace_node, &m_header->parent);
	}
	
	erase_fixup((node_ptr)fix_node);
	if (node == m_header->left) {
		m_header->left = min_node(m_header->parent);
	}
	if (node == m_header->right) {
		m_header->right = max_node(m_header->parent);
	}
	destroy_node((node_ptr)node);

	--m_size;
	if (0 == m_size) {
		m_header->left = m_header->right = m_header;
	}
	
	return next;
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
	clear_since(m_header->parent, &destroy_node);
	m_header->parent = m_header->left = m_header->right = nullptr;
}

//find
template<typename T, typename Comp>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::find(const key_type& key) noexcept {
	iterator iter = lbound(key);
	if (end() == iter || m_comp(key, *iter)) {
		return end();
	}

	return iter;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::const_iterator 
avl_tree<T, Comp>::find(const key_type& key) const noexcept {
	iterator iter = lbound(key);
	if (end() == iter || m_comp(key, *iter)) {
		return end();
	}

	return iter;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::size_type 
avl_tree<T, Comp>::count_multi(const key_type& key) noexcept {
	std::pair<iterator, iterator> p = equal_range_multi(key);
	return std::distance(p.first, p.last);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::size_type 
avl_tree<T, Comp>::count_multi(const key_type& key) const noexcept {
	std::pair<const_iterator, const_iterator> p = equal_range_multi(key);
	return std::distance(p.first, p.last);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::size_type 
avl_tree<T, Comp>::count_unique(const key_type& key) noexcept {
	iterator iter = find(key);
	return end() == iter ? 0 : 1;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::size_type 
avl_tree<T, Comp>::count_unique(const key_type& key) const noexcept {
	const_iterator iter = find(key);
	return end() == iter ? 0 : 1;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::lower_bound(const key_type& key) noexcept {
	return lbound(key).node;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::const_iterator 
avl_tree<T, Comp>::lower_bound(const key_type& key) const noexcept {
	return lbound(key).node;
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::iterator 
avl_tree<T, Comp>::upper_bound(const key_type& key) noexcept {
	return ubound(key);
}

template<typename T, typename Comp>
typename avl_tree<T, Comp>::const_iterator 
avl_tree<T, Comp>::upper_bound(const key_type& key) const noexcept {
	return ubound(key);
}

template<typename T, typename Comp>
bool operator<(const avl_tree<T, Comp>& lhs, const avl_tree<T, Comp>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), 
			rhs.begin(), rhs.end());
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