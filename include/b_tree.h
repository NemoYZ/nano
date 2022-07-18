#pragma once

#include <stdint.h>
#include <type_traits>
#include <functional>
#include <algorithm>
#include <assert.h>
#include <queue>
#include <string>
#include "tree.h"
#include <iterator>
#include "construct.h"
#include "type_traits.h"
#include "utility.h"

#ifdef B_TREE_DEBUG
#include <iostream>
#endif //B_TREE_DEBUG

namespace nano {

inline constexpr degree_t DEFAULT_DEGREE = 4;

//TODO: 动态增长, 不要一开始就申请很大的一块内存
template<typename T>
struct b_tree_node : public b_tree_node_base {
    //不需要单独记录values数组的长度，因为B数孩子节点 = 值个数 + 1
    T* values = nullptr;    
};

template<typename T>
struct b_tree_iterator_base : public std::iterator<std::bidirectional_iterator_tag, T> {
    using self			= b_tree_iterator_base;
	using node_base_ptr = b_tree_node_base*;
	using node_ptr 		= b_tree_node<T>*;

	b_tree_iterator_base() noexcept = default;

	b_tree_iterator_base(node_base_ptr _node, node_base_ptr _header, degree_t _index) noexcept :
		node(_node),
		header(_header),
        index(_index) {
	}

	bool operator==(const self& other) const noexcept { 
		if (nullptr == node && nullptr == other.node) { //end
			return true;
		}
		return node == other.node && index == other.index;
	}
	bool operator!=(const self& other) const noexcept { 
		return !(*this == other);	
	}

	node_base_ptr node = nullptr;
	node_base_ptr header = nullptr;
    degree_t index = 0;
};

template<typename T>
struct b_tree_iterator : public b_tree_iterator_base<T> {
	using category 			= std::bidirectional_iterator_tag;
	using size_type			= size_t;
	using value_type 		= T;
	using pointer 			= T*;
	using reference	 		= T&;
	using const_reference 	= T&;
	using node_base_ptr		= typename b_tree_iterator_base<T>::node_base_ptr;
    using node_ptr          = typename b_tree_iterator_base<T>::node_ptr;
	using self 				= b_tree_iterator<T>;

	b_tree_iterator() noexcept = default;
	b_tree_iterator(node_base_ptr _node, degree_t _index, node_base_ptr _header) noexcept : 
		b_tree_iterator_base<T>(_node, _header, _index) { 
	}

	b_tree_iterator(node_ptr _node, degree_t _index, node_base_ptr _header) noexcept : 
		b_tree_iterator_base<T>(_node, _header, _index) { 
	}

	self& operator++() noexcept {
		if (nullptr == this->node) {
			this->node = static_cast<b_tree_node<T>*>(this->header->children[0]);
			this->index = 0;
		} else {
			std::pair<node_base_ptr, degree_t> myPair = successor(this->node, this->index);
			this->node = myPair.first;
			this->index = myPair.second;
		}
		return *this;
	}

	self& operator++(int) noexcept {
		self temp = *this;
		++*this;
		return temp;
	}

	self& operator--() noexcept {
		if (nullptr == this->node) {
			b_tree_node<T>* node1 = static_cast<b_tree_node<T>*>(this->header->children[1]);
			this->node = node1;
			this->index = node1->vsz - 1;
		} else {
			std::pair<node_base_ptr, degree_t> myPair = precursor(this->node, this->index);
			this->node = myPair.first;
			this->index = myPair.second;
		}
		return *this;
	}

	self& operator--(int) noexcept {
		self temp = *this;
		--* this;
		return temp;
	}

	reference operator*() const noexcept {
		return (static_cast<node_ptr>(this->node))->values[this->index];
	}

	pointer operator->() const noexcept {
		return &(operator*());
	}
};

template<typename T>
struct b_tree_const_iterator : public b_tree_iterator_base<T> {
	using category 			= std::bidirectional_iterator_tag;
	using size_type			= size_t;
	using value_type 		= T;
	using pointer 			= T*;
	using reference	 		= T&;
	using const_reference 	= T&;
	using node_base_ptr		= typename b_tree_iterator_base<T>::node_base_ptr;
    using node_ptr          = typename b_tree_iterator_base<T>::node_ptr;
	using self 				= b_tree_const_iterator<T>;

	b_tree_const_iterator() noexcept = default;

	b_tree_const_iterator(node_ptr _node, degree_t _index, node_base_ptr _header) noexcept : 
		b_tree_iterator_base<T>(_node, _header, _index) { 
	}

	b_tree_const_iterator(node_base_ptr _node, degree_t _index, node_base_ptr _header) noexcept : 
		b_tree_iterator_base<T>(_node, _header, _index) { 
	}

	self& operator++() noexcept {
		if (nullptr == this->node) {
			this->node = static_cast<b_tree_node<T>*>(this->header->children[0]);
			this->index = 0;
		} else {
			std::pair<node_base_ptr, degree_t> myPair = successor(this->node, this->index);
			this->node = myPair.first;
			this->index = myPair.second;
		}
		return *this;
	}

	self& operator++(int) noexcept {
		self temp = *this;
		++*this;
		return temp;
	}

	self& operator--() noexcept {
		if (nullptr == this->node) {
			b_tree_node<T>* node1 = static_cast<b_tree_node<T>*>(this->header->children[1]);
			this->node = node1;
			this->index = node1->vsz - 1;
		} else {
			std::pair<node_base_ptr, degree_t> myPair = precursor(this->node, this->index);
			this->node = myPair.first;
			this->index = myPair.second;
		}
		return *this;
	}

	self& operator--(int) noexcept {
		self temp = *this;
		--* this;
		return temp;
	}

	reference operator*() const noexcept {
		return (static_cast<node_ptr>(this->node))->values[this->index];
	}

	pointer operator->() const noexcept {
		return &(operator*());
	}
};

/**
 * @brief 
 * 
 * @tparam T 
 * @tparam Comp 
 * @tparam degree 最大度数
 */
template<typename T, typename Comp = std::less<T>, 
    degree_t degree = DEFAULT_DEGREE>
class b_tree {    
    static_assert(degree >= 3, "degree at least 3");
	static_assert(std::is_move_assignable<T>::value || std::is_trivially_move_assignable<T>::value, 
		"move/trivailly assignable required");
	static_assert(std::is_copy_assignable<T>::value, "copy assignable required");
	static_assert(std::is_move_constructible<T>::value,
		"move constructile required");

public:
	constexpr static degree_t order = degree - 1;

public:
	using key_type 					= T;
	using value_type                = T;
	using pointer                   = T*;
	using const_pointer             = const T*;
	using reference                 = T&;
	using const_reference           = const T&;
	using size_type                 = size_t;
	using difference_type           = ptrdiff_t;
	using const_iterator            = b_tree_const_iterator<value_type>;
	using iterator                  = b_tree_iterator<value_type>;
    using reverse_iterator          = const std::reverse_iterator<iterator>;
	using const_reverse_iterator    = const std::reverse_iterator<const_iterator>;

public:
    iterator begin() noexcept { return iterator(m_header->children[0], 0, m_header); }
	iterator end() noexcept { return iterator(static_cast<node_base_ptr>(nullptr), degree, m_header); }
    reverse_iterator rbegin() noexcept { return std::reverse_iterator<iterator>(end()); }
	reverse_iterator rend() noexcept { return std::reverse_iterator<iterator>(begin()); }
	const_iterator begin() const noexcept { return const_iterator(m_header->children[0], degree, m_header); }
	const_iterator end() const noexcept { return const_iterator(static_cast<node_base_ptr>(nullptr), 0, m_header); }
    const_reverse_iterator rbegin() const noexcept { return std::reverse_iterator<const_iterator>(end()); }
	const_reverse_iterator rend() const noexcept { return std::reverse_iterator<const_iterator>(begin()); }

public:
    b_tree(const Comp& comp = Comp());

	b_tree(const std::initializer_list<T>& ilist, const Comp& comp = Comp()) :
		b_tree(ilist.begin(), ilist.end(), comp) {
	}

	template<std::input_iterator InputIter>
	b_tree(InputIter first, InputIter last, const Comp& comp = Comp());

	b_tree(const b_tree& other);

	b_tree(b_tree&& other);

	~b_tree();

	b_tree& operator=(const b_tree& other);

	b_tree& operator=(b_tree&& other);

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
	iterator insert_multi(const key_type& key) {
		return emplace_multi(key);
	}

	iterator insert_multi(key_type&& key) { 
		return emplace_multi(std::move(key)); 
	}

	iterator insert_multi(iterator hint, const key_type& key) {
		return emplace_multi_use_hint(hint, key);
	}

	iterator insert_multi(iterator hint, key_type&& key) {
		return emplace_multi_use_hint(hint, std::move(key));
	}

	template <std::input_iterator InputIter>
	void insert_multi(InputIter first, InputIter last);

	std::pair<iterator, bool> insert_unique(const key_type& key) {
		return emplace_unique(key);
	}

	std::pair<iterator, bool> insert_unique(key_type&& key) {
		return emplace_unique(std::move(key));
	}

	std::pair<iterator, bool> 
	insert_unique_hint(iterator hint, const key_type& key) {
		return emplace_unique_hint(hint, key);
	}

	std::pair<iterator, bool> 
	insert_unique_hint(iterator hint, key_type&& key) {
		return emplace_unique_hint(hint, std::move(key));
	}

	template <std::input_iterator InputIter>
	void insert_unique(InputIter first, InputIter last);

	//erase
	iterator erase(iterator hint);
  	size_type erase_multi(const key_type& key);
  	size_type erase_unique(const key_type& key);
  	void erase(iterator first, iterator last);
  	void clear();

	//find
	iterator find(const key_type& key) noexcept;
	const_iterator find(const key_type& key) const noexcept;

	size_type count_multi(const key_type& key) const noexcept;
	size_type count_unique(const key_type& key) const noexcept;

	iterator lower_bound(const key_type& key) noexcept { return lbound(key); }
	const_iterator lower_bound(const key_type& key) const noexcept{ 
		iterator iter = lbound(key);
		return const_iterator(iter.node, iter.index, iter.header); 
	}

	iterator upper_bound(const key_type& key) noexcept { return ubound(key); }
	const_iterator upper_bound(const key_type& key) const noexcept { 
		iterator iter = ubound(key);
		return const_iterator(iter.node, iter.index, iter.header); 
	}

	std::pair<iterator, iterator>             
	equal_range_multi(const key_type& key) noexcept { return { lower_bound(key), upper_bound(key) }; }

	std::pair<const_iterator, const_iterator> 
	equal_range_multi(const key_type& key) const noexcept{ return { lower_bound(key), upper_bound(key) }; }

	std::pair<iterator, iterator> equal_range_unique(const T& key) noexcept ;
	std::pair<const_iterator, const_iterator> equal_range_unique(const T& key) const noexcept ;

	//other
	void swap(b_tree& rhs) noexcept;
	size_type size() const noexcept { return m_size; }
	bool empty() const noexcept { return 0 == m_size; }
	bool disk_read() { return true; }
	bool disk_write() { return true; }
#ifdef B_TREE_DEBUG
	bool balanced() { 
		return is_balanced(static_cast<node_ptr>(m_header->parent), m_comp); 
	}
	std::string serialize();
	b_tree_node<T>* root() { return static_cast<node_ptr>(m_header->parent); }
	degree_t check(b_tree_node<T>* tree);
	bool is_balanced(b_tree_node<T>* root);
#endif //B_TREE_DEBUG

private:
    using node_ptr 					= b_tree_node<T>*;
	using node_base_ptr				= b_tree_node_base*;

private:
	//node operation
	node_ptr create_node(degree_t _vsz);
	node_base_ptr create_node_base(degree_t _vsz);
	void deallocate_node(node_ptr node);
	static void destroy_node(node_base_ptr node);
	static void destroy_node_base(node_base_ptr node);
	node_ptr copy_node(node_base_ptr node);
	void clear_since(node_ptr node);
	
private:
	//auxiliary functions
	iterator lbound(const key_type& key);
	iterator ubound(const key_type& key);
	iterator get_insert_multi(const key_type& key);
	iterator insert_value(iterator iter, key_type& key);
	iterator erase_value(key_type key);
	std::pair<iterator, bool> get_insert_unique(const key_type& key);

private: 
	//other node operaion
	node_ptr merge_node(node_base_ptr parent, degree_t childIndex, degree_t vIndex);
	void split_child(node_base_ptr parent, degree_t childIndex);
	void split_node(node_base_ptr node);
	bool node_filled(node_ptr node) { 
		return node ? node->vsz + 1 == degree : false; 
	}
	degree_t value_lbound(node_ptr node, const T& val) {
		return std::lower_bound(node->values, node->values + node->vsz, val, m_comp) - 
				node->values;
	}
	degree_t value_ubound(node_ptr node, const T& val) {
		return std::upper_bound(node->values, node->values + node->vsz, val, m_comp) - 
				node->values;
	}

private:
    node_base_ptr m_header = nullptr;
    size_type m_size = 0;
    const Comp& m_comp;
};

template<typename T, typename Comp, degree_t degree>
void b_tree<T, Comp, degree>::deallocate_node(node_ptr node) {
	::operator delete(node->children);
	::operator delete(node->values);
	::operator delete(node);
}
	

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::node_ptr 
b_tree<T, Comp, degree>::create_node(degree_t _vsz) {
	node_ptr newNode = static_cast<node_ptr>(::operator new(sizeof(b_tree_node<T>)));
	newNode->values = static_cast<T*>(::operator new(sizeof(T) * order));
	newNode->children = static_cast<node_base_ptr*>(::operator new(sizeof(node_base_ptr) * degree));
	newNode->parent = nullptr;
	newNode->vsz = 0;
	mem_zero(&newNode->children[0], sizeof(node_base_ptr) * degree);
	//newNode->children[0] = nullptr;
	static_cast<void>(_vsz);
	return newNode;
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::node_base_ptr 
b_tree<T, Comp, degree>::create_node_base(degree_t _vsz) {
	node_base_ptr newNode = static_cast<node_base_ptr>(::operator new(sizeof(b_tree_node_base)));
	newNode->children = static_cast<node_base_ptr*>(::operator new(sizeof(node_base_ptr) * 2));
	static_cast<void>(_vsz);
	return newNode;
}

template<typename T, typename Comp, degree_t degree>
void b_tree<T, Comp, degree>::destroy_node(node_base_ptr node) {
	node_ptr node1 = static_cast<node_ptr>(node);
	::operator delete(node1->children);
	for (degree_t i = 0; i < node1->vsz; ++i) {
		destroy(&node1->values[i]);
	}
	::operator delete(node1->values);
	::operator delete(node1);
}

template<typename T, typename Comp, degree_t degree>
void b_tree<T, Comp, degree>::destroy_node_base(node_base_ptr node) {
	::operator delete(node->children);
	::operator delete(node);
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::node_ptr 
b_tree<T, Comp, degree>::copy_node(node_base_ptr node) {
	node_ptr node1 = static_cast<node_ptr>(node);
	node_ptr newNode = create_node(0);
	for (degree_t i = 0; i < node1->vsz; ++i) {
		construct(newNode->values + i, node1->values[i]);
	}
	newNode->vsz = node1->vsz;

	return newNode;
}

template<typename T, typename Comp, degree_t degree>
void b_tree<T, Comp, degree>::clear_since(node_ptr node) {
	if (node) {
		for (degree_t i = 0; i < node->vsz; ++i) {
			destroy(&node->values[i]);
			if (!is_leaf(node)) {
				clear_since(static_cast<node_ptr>(node->children[i]));
			}
		}
		if (!is_leaf(node)) {
			clear_since(static_cast<node_ptr>(node->children[node->vsz]));
		}
		deallocate_node(node);
	}
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::iterator
b_tree<T, Comp, degree>::lbound(const key_type& key) {
	node_ptr root = static_cast<node_ptr>(m_header->parent);
	node_base_ptr parent = nullptr;
	degree_t index = 0;
	degree_t index1 = 0;
		
	while (root) {
		//为了减少比较次数，可以先与root->values最后一个元素比较
		//若root->values最后一个元素小于key, 比较次数可将比较次数从log(root->vsz)减少到1次
		if (m_comp(root->values[root->vsz - 1], key)) {
			index = root->vsz;
		} else {
			index = value_lbound(root, key); //该节点第一个大于等于key的下标, 不会为root->vsz
			parent = root;
			index1 = index;
		}
		root = static_cast<node_ptr>(root->children[index]);
	}

	if (nullptr == parent) { //从根开始，大于所有节点的最大值, 返回end
	}

	return iterator(parent, index1, m_header);
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::iterator
b_tree<T, Comp, degree>::ubound(const key_type& key) {
	node_ptr root = static_cast<node_ptr>(m_header->parent);
	node_base_ptr parent = nullptr;
	degree_t index = 0;
	degree_t index1 = 0;
		
	while (root) {
		//为了减少比较次数，可以先与node->values最后一个元素比较
		//如果成功，比较次数由logn次减少为1次
		if (!m_comp(key, root->values[root->vsz - 1])) {
			index = root->vsz;
		} else {
			index = value_ubound(root, key);
			parent = root;
			index1 = index;
		}
		root = static_cast<node_ptr>(root->children[index]);
	}

	if (nullptr == parent) {	//从根开始，大于所有节点的最大值, 返回end
	}
	return iterator(parent, index1, m_header);
}

template<typename T, typename Comp, degree_t degree>
void b_tree<T, Comp, degree>::split_child(node_base_ptr parent, degree_t childIndex) {
	node_ptr parentNode = static_cast<node_ptr>(parent);
	if (node_filled(parentNode)) {  //保证父亲节点不为满节点
		split_node(parentNode);
	}

	node_ptr child2 = create_node(0);	//分裂后的右边		
	node_ptr child1 = static_cast<node_ptr>(parentNode->children[childIndex]);
	constexpr static degree_t splitValueCount = (degree - 1) / 2;
	child2->vsz = splitValueCount;
	degree_t n1 = child1->vsz - 1;
	degree_t n2 = child2->vsz - 1;

	//依从从后往前拷贝后半部分的值
	while (n2 >= 0) {
		construct(&child2->values[n2], std::move(child1->values[n1]));
		destroy(&child1->values[n1]);
		if (!is_leaf(child1)) {  //如果不是叶子，还需要拷贝后半部分的孩子结点指针
			child2->children[n2 + 1] = child1->children[n1 + 1];
			child1->children[n1 + 1]->parent = child2;
			child1->children[n1 + 1] = nullptr;
		}
		--n1;
		--n2;
	}

	if (!is_leaf(child1)) {  //如果不是叶子，还需要拷贝后半部分的孩子结点指针
		child2->children[0] = child1->children[n1 + 1];
		child1->children[n1 + 1]->parent = child2;
		child1->children[n1 + 1] = nullptr;
	}
	
	child1->vsz -= child2->vsz;
	//孩子结点中间值上升
	degree_t parentVsz = parentNode->vsz;
	//给中间节点让出位置
	construct(&parentNode->values[parentVsz], parentNode->values[parentVsz - 1]);
	parentNode->children[parentVsz + 1] = parentNode->children[parentVsz];
	for (degree_t i = parentVsz - 1; i > childIndex; --i) {
		parentNode->values[i] = std::move(parentNode->values[i - 1]);
		parentNode->children[i + 1] = parentNode->children[i];
	}
	parentNode->values[childIndex] = std::move(child1->values[child1->vsz - 1]);
	parentNode->children[childIndex + 1] = child2;
	child2->parent = parentNode;
	destroy(&child1->values[child1->vsz - 1]);
	--child1->vsz;
	++parentNode->vsz;
	if (m_header->children[1] == child1 ||
		m_header->children[1] == parentNode) {
		m_header->children[1] = child2;
	}
}

template<typename T, typename Comp, degree_t degree>
void b_tree<T, Comp, degree>::split_node(node_base_ptr node) {
	node_ptr parent = static_cast<node_ptr>(node->parent);
	if (nullptr == parent) { 	//分裂根节点
		parent = create_node(0);
		m_header->parent = parent;
		parent->children[0] = node;
		node->parent = parent;
		if (m_header->children[1] == node) {
			m_header->children[1] = parent;
		}
	}
	split_child(parent, child_index(parent, node) /*0*/);
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::node_ptr 
b_tree<T, Comp, degree>::merge_node(node_base_ptr parent, 
		degree_t childIndex, degree_t vIndex) {
	node_ptr parentNode = static_cast<node_ptr>(parent);
	node_ptr child1 = static_cast<node_ptr>(parentNode->children[childIndex]);
	node_ptr child2 = static_cast<node_ptr>(parentNode->children[childIndex + 1]);
	degree_t vsz1 = child1->vsz;
	degree_t vsz2 = child2->vsz;
	construct(&child1->values[vsz1], std::move(parentNode->values[vIndex])); //把关键字合并到child1
	++vsz1; 
	//把child2结点的值合并到child1
	//把孩子也拷贝过去
	for (degree_t i = 0; i < vsz2; ++i) {
		construct(&child1->values[i + vsz1], std::move(child2->values[i]));
		if (!is_leaf(child1)) { //also !is_leaf(child2)
			child1->children[i + vsz1] = child2->children[i];
			child2->children[i]->parent = child1;
		}
	}
	vsz1 += vsz2;
	if (!is_leaf(child1)) { //also !is_leaf(child2)
		child1->children[vsz1] = child2->children[vsz2];
		child2->children[vsz2]->parent = child1;
	}
	child1->vsz = vsz1;
	destroy_node(child2);

	//在父亲结点中删除关键字, 为了删除child2, 顺便把孩子结点向前覆盖
	degree_t pvsz = parentNode->vsz;
	for (degree_t i = vIndex; i + 1 < pvsz; ++i) {
		parentNode->values[i] = std::move(parentNode->values[i + 1]);
		parentNode->children[i + 1] = parentNode->children[i + 2];
	}
	parentNode->children[pvsz] = nullptr; //置空最后一个孩子
	destroy(&parentNode->values[pvsz - 1]);
	--parentNode->vsz;

	if (0 == parentNode->vsz) {
		node_base_ptr grandParent = parentNode->parent;
		if (nullptr == grandParent) {
			m_header->parent = child1;
		} else {
			degree_t parentIndex = child_index(parentNode->parent, parentNode);
			grandParent->children[parentIndex] = child1;
		}
		child1->parent = grandParent;
		destroy_node(parentNode);
	}
	//m_header->children[0] = min_node(m_header->parent).first;
	m_header->children[1] = max_node(m_header->parent).first; //TODO: 优化
	return child1;
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::iterator 
b_tree<T, Comp, degree>::get_insert_multi(const key_type& val) {
	//由于insert_multi一定会成功
	//我们可以一边向下查找，一边调整
	node_ptr curr = static_cast<node_ptr>(m_header->parent);
	if (nullptr == curr) {
		curr = create_node(0);
		m_header->parent = curr;
		m_header->children[0] = m_header->children[1] = curr;
		return iterator(curr, 0, m_header);
	} else {
		if (node_filled(curr)) {		//顺便向下调整
			split_node(curr);
			curr = static_cast<node_ptr>(m_header->parent);
		}
	}

	degree_t index = 0;
	node_ptr parent = nullptr;
	
	while (curr) {
		index = value_ubound(curr, val);
		if (node_filled(static_cast<node_ptr>(curr->children[index]))) {
			split_child(curr, index);
			if (m_comp(curr->values[index], val)) {
				++index;
			}
		}
		parent = curr;
		curr = static_cast<node_ptr>(curr->children[index]);
	}
	
	return iterator(parent, index, m_header);
}

template<typename T, typename Comp, degree_t degree>
std::pair<typename b_tree<T, Comp, degree>::iterator, bool> 
b_tree<T, Comp, degree>::get_insert_unique(const key_type& val) {
	if (nullptr == m_header->parent) {
		node_ptr node = create_node(0);
		m_header->parent = m_header->children[0] = m_header->children[1] = node;
		return { iterator(m_header->parent, 0, m_header), true };
	}
	iterator iter = lbound(val);
	node_ptr node1 = static_cast<node_ptr>(iter.node);
	degree_t index1 = iter.index;
	
	if (nullptr == iter.node) {
		node1 = static_cast<node_ptr>(m_header->children[1]);
		index1 = static_cast<node_ptr>(m_header->children[1])->vsz;
	} else {
		T& val1 = node1->values[index1];
		//val1>=val!comp(val1, val)==true
		if (!m_comp(val, val1)) { //val1 <= val, equal
			return { iter, false };
		}
	}
	
	while (!is_leaf(node1)) {	//继续向下走到根节点
		node1 = static_cast<node_ptr>(node1->children[index1]);
		index1 = value_ubound(node1, val);
	}
	iter.node = node1;
	iter.index = index1;

	return { iter, true };
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::iterator 
b_tree<T, Comp, degree>::insert_value(iterator iter, key_type& key) {
	node_ptr node1 = static_cast<node_ptr>(iter.node);
	degree_t index1 = iter.index;
	
	if (node_filled(node1)) {
		split_node(node1);
		/*
		degree_t chil1Index = child_index(node1->parent, node1);
		T& midVal = static_cast<node_ptr>(node1->parent)->values[chil1Index];
		if (m_comp(midVal, key)) {
			node1 = static_cast<node_ptr>(node1->parent->children[chil1Index + 1]);
			index1 = value_ubound(node1, key, m_comp);
		}
		*/
		iter = get_insert_multi(key);
		node1 = static_cast<node_ptr>(iter.node);
		index1 = iter.index;
	}
	
	for (degree_t i = node1->vsz; i > index1; --i) {
		node1->values[i] = std::move(node1->values[i - 1]);
	}
	node1->values[index1] = std::move(key);
	++node1->vsz;
	node1->children[node1->vsz] = nullptr;
	++m_size;
	return iterator(node1, index1, m_header);
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::iterator 
b_tree<T, Comp, degree>::erase_value(key_type key) {
	/**
	* 情况一: 在node中找到关键字val并且node是叶子结点
	*		1) 直接从node结点中删除val
	* 情况二: 在node中找到关键字val但node不是叶子结点
	*		a: 前于val的孩子结点(也就是node->children[i],暂时称为child1)中的关键字个数如果不少于degree个
	*			1) 在child1中找出val的前驱(val')
	*			2) 把val'赋值给val
	*			3）递归地在child1中删除val的val'
	*		b: 后于val的孩子结点(也就是node->children[i + 1],暂时称为child2)中的关键字个数不少于degree个
	*			1) 在child2中找出val的后继(val')
	*			2) 把val'赋值给val
	*			3）递归地在child2中删除val的val'
	*		c: child1和child2中的关键字个数都少于degree个
	*			1) 把child2和val都合并到child1中并且删除结点child2
	*			2) 从child1中递归地删除val
	* 情况三: 在node中没找到关键字但在node的孩子(child)中找到关键字
	*		a: child只有degree - 1个关键字，但child的一个相邻兄弟结点有degree个关键字
	*			1) 把node中的某一个关键字插入到child中
	*			2) node向child的该兄弟结点要一个关键字
	*			3) 把兄弟结点中的相应的孩子指针移动到child中
	*		b: 当兄弟的也难，child的所有相邻兄弟结点的关键字个数都为degree - 1
	*			1) 把child和其中一个相邻的兄弟结点合并
	*			2) 将node中的一个关键字合并到新合成的结点中去，使之成为中间的关键字
	*		执行情况三的操作后，对node的某个合适的孩子进行递归而结束
	*/
	node_ptr node = static_cast<node_ptr>(m_header->parent);
	degree_t index = value_lbound(node, key);
	constexpr static degree_t minVsz = degree / 2 - 1;
	while (node) {
		/*
		std::cout << "key=" << key
				<< " node[0]=" << node->values[0]
				<< " index=" << index
				<< std::endl;
		std::cout << serialize() << std::endl;
		*/
		if (is_leaf(node) && !m_comp(key, node->values[index])) { //情况一, 叶子节点，直接删除
			for (degree_t i = index; i < node->vsz - 1; ++i) {
				node->values[i] = std::move(node->values[i + 1]);
			}
			destroy(&node->values[node->vsz - 1]);
			--node->vsz;
			if (m_header->parent == node && 0 == node->vsz) {
				destroy_node(m_header->parent);
				m_header->parent = m_header->children[0] = m_header->children[1] = nullptr;
				node = nullptr;
			}
			break;
		} else if (index < node->vsz && !m_comp(key, node->values[index])) {	//情况二, 要删除的值在当前节点，但当前节点不为叶子
			if (node->children[index]->vsz > minVsz) {	//a, 左兄弟有多的值
				std::pair<node_base_ptr, degree_t>  pre = max_node(node->children[index]);
				key = static_cast<node_ptr>(pre.first)->values[pre.second];
				node_ptr lBrother = static_cast<node_ptr>(node->children[index]);
				node->values[index] = key; //move? 好像不行
				node = lBrother;
				index = value_lbound(node, key);
			} else if (node->children[index + 1]->vsz > minVsz) { //b, 右兄弟有多的值
				std::pair<node_base_ptr, degree_t>  next = min_node(node->children[index + 1]);
				key = static_cast<node_ptr>(next.first)->values[next.second];
				node_ptr rBrother = static_cast<node_ptr>(node->children[index + 1]);
				node->values[index] = key;
				node = rBrother;
				index = value_lbound(node, key);
			} else {	//c, 左右兄弟都没有多的值
				if (index >= node->vsz) { //永远合并的是右兄弟
					--index;
				}
				degree_t index1 = node->children[index]->vsz;
				node = merge_node(node, index, index);
				index = index1; //value_lbound(node, key)
				key = node->values[index];
			}
		} else { //情况三, 要删除的值在孩子节点中
			if (node->children[index]->vsz <= minVsz) { //孩子没有多的值
				if (index > 0 && node->children[index - 1]->vsz > minVsz) { //a，左兄弟有多的值
					node_ptr child = static_cast<node_ptr>(node->children[index]);
					node_ptr lBrother = static_cast<node_ptr>(node->children[index - 1]);
					degree_t childVsz = child->vsz;
					degree_t lBrotherVsz = lBrother->vsz;

					construct(&child->values[childVsz], std::move(child->values[childVsz - 1]));
					child->children[childVsz + 1] = child->children[childVsz];
					for (degree_t i = childVsz - 1; i > 0; --i) { //为父亲节点新来的值让出位置
						child->values[i] = std::move(child->values[i - 1]);
						if (!is_leaf(child)) {
							child->children[i + 1] = child->children[i];
						}
					}
					child->children[1] = child->children[0];
					child->values[0] = std::move(node->values[index - 1]); 		//父亲节点值下来
					node->values[index - 1] = std::move(lBrother->values[lBrotherVsz - 1]); //左兄弟节点值上升到父亲节点
					if (!is_leaf(child)) { //also !is_leaf(lBrother)
						child->children[0] = lBrother->children[lBrotherVsz];
						lBrother->children[lBrotherVsz]->parent = child;
						lBrother->children[lBrotherVsz] = nullptr;
					}
					++child->vsz;
					destroy(&lBrother->values[lBrotherVsz - 1]); //在兄弟节点中删除该值
					--lBrother->vsz;
					node = child;
					index = value_lbound(node, key);
				} else if (index < node->vsz && node->children[index + 1]->vsz > minVsz) { //b, 右兄弟有多的值
					node_ptr child = static_cast<node_ptr>(node->children[index]);
					node_ptr rBrother = static_cast<node_ptr>(node->children[index + 1]);
					degree_t childVsz = child->vsz;
					degree_t rBrotherVsz = rBrother->vsz;

					//父亲节点的值下来
					construct(&child->values[childVsz], std::move(node->values[index]));
					//--node->vsz
					if (!is_leaf(child)) { //also !is_leaf(rBrother)
						child->children[childVsz + 1] = rBrother->children[0];
						rBrother->children[0]->parent = child;
					}
					++child->vsz;
					node->values[index] = std::move(rBrother->values[0]);  //右兄弟节点的值上升到父亲节点
					//++node->vsz
					for (degree_t i = 0; i + 1 < rBrotherVsz; ++i) {			//在右兄弟节点中删除该值
						rBrother->values[i] = std::move(rBrother->values[i + 1]);
						if (!is_leaf(rBrother)) {
							rBrother->children[i] = rBrother->children[i + 1];
						}
					}
					if (!is_leaf(rBrother)) {
						rBrother->children[rBrotherVsz - 1] = rBrother->children[rBrotherVsz];
						rBrother->children[rBrother->vsz + 1] = nullptr;
					}
					destroy(&rBrother->values[rBrotherVsz - 1]);
					--rBrother->vsz;
					node = child;
					index = value_lbound(node, key);
				} else { //c. 都没有多的值
					if (index >= node->vsz) { //永远合并的是右兄弟
						--index;
					}
					node = merge_node(node, index, index);
					index = value_lbound(node, key);
				}
			} else {	//孩子有多的值
				node = static_cast<node_ptr>(node->children[index]);
				index = value_lbound(node, key);
			}
		}
	}

	--m_size;
	return iterator(node, index, m_header);
}	

template<typename T, typename Comp, degree_t degree>
b_tree<T, Comp, degree>::b_tree(const Comp& comp) :
	m_header(create_node_base(0)),
	m_size(0),
	m_comp(comp) {
}

template<typename T, typename Comp, degree_t degree>
template<std::input_iterator InputIter>
b_tree<T, Comp, degree>::b_tree(InputIter first, InputIter last, const Comp& comp) :
		m_header(create_node_base(0)),
		m_size(0),
		m_comp(comp) {
	static_assert(is_input_iterator_v<InputIter>, "input iterator required");
	for (; first != last; ++first) {
		insert_multi(first, last);
	}
}

template<typename T, typename Comp, degree_t degree>
b_tree<T, Comp, degree>::b_tree(const b_tree& other) :
		m_header(create_node_base(0)),
		m_size(0),
		m_comp(other.m_comp) {
	if (other.size()) {
		clear();
		m_header->parent = copy_since(other.m_header->parent, [this](node_base_ptr node){
			return copy_node(node);
		});
		m_header->children[0] = min_node(m_header->parent).first;
		m_header->children[1] = max_node(m_header->parent).first;
		m_size = other.m_size;
	}
}

template<typename T, typename Comp, degree_t degree>
b_tree<T, Comp, degree>::b_tree(b_tree&& other) :
		m_header(other.m_header),
		m_size(other.m_size) {
	other.m_header = create_node_base();
	other.m_size = 0;
}

template<typename T, typename Comp, degree_t degree>
b_tree<T, Comp, degree>::~b_tree() {
	clear();
	destroy_node_base(m_header);
}

template<typename T, typename Comp, degree_t degree>
b_tree<T, Comp, degree>& 
b_tree<T, Comp, degree>::operator=(const b_tree& other) {
	if (this != &other) {
		if (other.size()) {
			clear();
			m_header->parent = copy_since(other.m_header->parent, [this](b_tree_node_base* node){
				return copy_node(node);
			});
			m_header->parent->parent = m_header;
			m_header->children[0] = min_node(m_header->parent);
			m_header->children[1] = max_node(m_header->parent);
			m_size = other.m_size;
		}
	}
}

template<typename T, typename Comp, degree_t degree>
b_tree<T, Comp, degree>&
b_tree<T, Comp, degree>::operator=(b_tree&& other) {
	if (this != &other) {
		m_header = other.m_header;
		m_size = other.m_size;
		other.m_header = create_node_base();
		other.m_size = 0;
	}
}

template<typename T, typename Comp, degree_t degree>
template <typename ...Args>
typename b_tree<T, Comp, degree>::iterator 
b_tree<T, Comp, degree>::emplace_multi(Args&& ...args) {
	T val(std::forward<Args>(args)...);
	iterator iter = get_insert_multi(val);
	insert_value(iter, val);
	return iter;
}

template<typename T, typename Comp, degree_t degree>
template <typename ...Args>
typename b_tree<T, Comp, degree>::iterator 
b_tree<T, Comp, degree>::emplace_multi_hint(iterator hint, Args&& ...args) {
	T key(std::forward<Args>(args)...);
	if (is_leaf(hint.node)) {
		degree_t index = value_ubound(key);
		if (index != 0 && index != static_cast<node_ptr>(hint.node)->vsz) {
			return insert_value(hint, key);
		}
	}

	return emplace_multi(std::move(key));
}

template<typename T, typename Comp, degree_t degree>
template <typename ...Args>
std::pair<typename b_tree<T, Comp, degree>::iterator, bool> 
b_tree<T, Comp, degree>::emplace_unique(Args&& ...args) {
	T key(std::forward<Args>(args)...);
	std::pair<iterator, bool> myPair = get_insert_unique(key);
	if (!myPair.second) {
		return myPair;
	}
	iterator iter =	insert_value(myPair.first, key);
	
	return {iterator(iter.node, iter.index, iter.header), true};
}

template<typename T, typename Comp, degree_t degree>
template <typename ...Args>
std::pair<typename b_tree<T, Comp, degree>::iterator, bool> 
b_tree<T, Comp, degree>::emplace_unique_hint(iterator hint, Args&& ...args) {
	T key(std::forward<Args>(args)...);
	degree_t index = value_ubound(hint.node, key);
	node_ptr node1 = static_cast<node_ptr>(hint.node);
	if (index != 0 && index != node1->vsz) {
		if (!m_comp(node1->values[index - 1], key)) { //equal
			hint.index = index;
			return { iterator(hint, index, m_header), false };
		}
	}

	return emplace_unique(std::move(key));
}

template<typename T, typename Comp, degree_t degree>
template <std::input_iterator InputIter>
void b_tree<T, Comp, degree>::insert_multi(InputIter first, InputIter last) {
	static_assert(is_input_iterator_v<InputIter>, "input iterator required");
	for (; first != last; ++first) {
		insert_multi(*first);
	}
}

template<typename T, typename Comp, degree_t degree>
template <std::input_iterator InputIter>
void b_tree<T, Comp, degree>::insert_unique(InputIter first, InputIter last) {
	static_assert(is_input_iterator_v<InputIter>, "input iterator required");
	for ( ; first != last; ++first) {
		insert_unique(*first);
	}
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::iterator 
b_tree<T, Comp, degree>::erase(iterator hint) {
	node_ptr node = static_cast<node_ptr>(hint.node);
	degree_t minVsz = degree / 2 - 1;
	if (is_leaf(node) && node->vsz > minVsz) { //叶子节点，并且值个数大于最小值个数，直接删除
		degree_t index = hint.index;
		degree_t nvsz = node->vsz;
		for (degree_t i = index; i < nvsz - 1; ++i) {
			node->values[i] = std::move(node->values[i + 1]);
		}
		destroy(&node->values[nvsz - 1]);
		--node->vsz;
		--m_size;
		return iterator(node, index, m_header);
	} 
	
	return erase_value(*hint);
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::size_type 
b_tree<T, Comp, degree>::erase_multi(const key_type& key) {
	size_type n = 0;
	while (true) {
		iterator iter = lbound(key);
		if (!m_comp(key, *iter)) {
			erase(iter);
			++n;
		} else {
			break;
		}
	}
	return n;
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::size_type 
b_tree<T, Comp, degree>::erase_unique(const T& key) {
	iterator iter = lbound(key);
	if (end() != iter && !m_comp(key, *iter)) {
		erase(iter);
		return 1;
	}

	return 0;
}

template<typename T, typename Comp, degree_t degree>
void b_tree<T, Comp, degree>::erase(iterator first, iterator last) {
	for (; first != last; ++first) {
		erase(first);
	}
}

template<typename T, typename Comp, degree_t degree>
void b_tree<T, Comp, degree>::clear() {
	clear_since(static_cast<node_ptr>(m_header->parent));
	m_size = 0;
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::iterator 
b_tree<T, Comp, degree>::find(const key_type& key) noexcept {
	iterator iter = lbound(key);
	if (m_comp(static_cast<node_ptr>(iter.node)->values[iter.node], key)) {
		return end();
	}

	return iter;
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::const_iterator 
b_tree<T, Comp, degree>::find(const key_type& key) const noexcept {
	iterator iter = lbound(key);
	if (m_comp(*iter, key)) {
		return end();
	}

	return const_iterator(iter.node, iter.index);
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::size_type 
b_tree<T, Comp, degree>::count_multi(const key_type& key) const noexcept {
	iterator iter = lbound(key);
	size_type n = 0;
	while (iter != end() && m_comp(*iter, key)) {
		++n;
		++iter;
	}

	return n;
}

template<typename T, typename Comp, degree_t degree>
typename b_tree<T, Comp, degree>::size_type 
b_tree<T, Comp, degree>::count_unique(const key_type& key) const noexcept {
	return find(key) == end() ? 0 : 1;
}

template<typename T, typename Comp, degree_t degree>
std::pair<typename b_tree<T, Comp, degree>::iterator, 
	typename b_tree<T, Comp, degree>::iterator> 
b_tree<T, Comp, degree>::equal_range_unique(const key_type& key) noexcept {
	iterator iter = find(key);
	if (end() == iter) {
		return { iter, iter };
	}
	return { iter, ++iter };
}

template<typename T, typename Comp, degree_t degree>
std::pair<typename b_tree<T, Comp, degree>::const_iterator, 
	typename b_tree<T, Comp, degree>::const_iterator> 
b_tree<T, Comp, degree>::equal_range_unique(const key_type& key) const noexcept {
	const_iterator iter = find(key);
	if (end() == iter) {
		return { iter, iter };
	}
	return { iter, ++iter };
}

template<typename T, typename Comp, degree_t degree>
void b_tree<T, Comp, degree>::swap(b_tree& rhs) noexcept {
	std::swap(m_header, rhs.m_header);
	std::swap(m_size, rhs.m_size);
}

template<typename T, typename Comp, degree_t degree>
bool operator<(const b_tree<T, Comp, degree>& lhs, const b_tree<T, Comp, degree>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, typename Comp, degree_t degree>
bool operator>(const b_tree<T, Comp, degree>& lhs, const b_tree<T, Comp, degree>& rhs) {
	return rhs < lhs;
}

template<typename T, typename Comp, degree_t degree>
bool operator<=(const b_tree<T, Comp, degree>& lhs, const b_tree<T, Comp, degree>& rhs) {
	return !(rhs < lhs);
}

template<typename T, typename Comp, degree_t degree>
bool operator>=(const b_tree<T, Comp, degree>& lhs, const b_tree<T, Comp, degree>& rhs) {
	return !(lhs < rhs);
}

template<typename T, typename Comp, degree_t degree>
bool operator==(const b_tree<T, Comp, degree>& lhs, const b_tree<T, Comp, degree>& rhs) {
	if (lhs.size() != rhs.size()) {
		return false;
	}
	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, typename Comp, degree_t degree>
bool operator!=(const b_tree<T, Comp, degree>& lhs, const b_tree<T, Comp, degree>& rhs) {
	return !(lhs == rhs);
}

#ifdef B_TREE_DEBUG
template<typename T, typename Comp, degree_t degree>
std::string 
b_tree<T, Comp, degree>::serialize() {
	node_ptr root = static_cast<node_ptr>(m_header->parent);
	if (nullptr == root) {
		return "";
	}

	std::queue<node_ptr> que;
	std::string result;
	que.push(root);
	while (!que.empty()) {
		size_t sz = que.size();
		while (sz) {
			root = que.front();
			que.pop();
			std::string str = "[";
			for (degree_t i = 0; i < root->vsz; ++i) {
				if (i != 0) {
					str.append(1, ',');
				}
				str += std::to_string(root->values[i]);
				if (!is_leaf(root)) {
					que.push(static_cast<node_ptr>(root->children[i]));
				}
			}
			if (!is_leaf(root)) {
				que.push(static_cast<node_ptr>(root->children[root->vsz]));
			}
			str.append(1, ']');
			result += str;
			--sz;
		}
		result.append(1, '\n');
	}

	return result;
}

template<typename T, typename Comp, degree_t degree>
degree_t b_tree<T, Comp, degree>::check(b_tree_node<T>* tree) {
	if (nullptr == tree) {
		return 0;
	}

	//孩子节点数不能大于度数
	if (tree->vsz + 1 > degree) {
		std::cout << "num of node->children=" << tree->vsz + 1 << std::endl;
		return -1;
	}

	//非叶节点至少有degree / 2个孩子
	if (!is_leaf(tree)) {
		if ((tree->vsz + 1) < degree / 2) {
			std::cout << "num of node->children < " << degree / 2 << std::endl;
			return -1;
		}
	}
	if (!std::is_sorted(tree->values, tree->values + tree->vsz, m_comp)) {
		std::cout << "node is not sorted" << std::endl;
		return -1;
	}

	degree_t h = check(static_cast<b_tree_node<T>*>(tree->children[0]), m_comp);
	for (degree_t i = 1; i < tree->vsz; ++i) {
		if (h != check(static_cast<b_tree_node<T>*>(tree->children[i]), m_comp)) {
			std::cout << "height of node->children not equal" << std::endl;
			return -1;
		}
	}

	return h + 1;
}

template<typename T, typename Comp, degree_t degree>
bool b_tree<T, Comp, degree>::is_balanced(b_tree_node<T>* root) {
	if (nullptr == root) {
		return true;
	}

	//非叶子的根节点的孩子数在[2,degree]之间
	if (!is_leaf(root) && (root->vsz + 1 < 2 || root->vsz > degree)) {
		std::cout << "num of root->children=" << root->vsz + 1 << std::endl;
		return false;
	}
	
	return check(root) != -1;
}

#endif //B_TREE_DEBUG

} //namespace nano