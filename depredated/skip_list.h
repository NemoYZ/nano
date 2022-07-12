/**
 * @file skip_list.h
 * @brief 纵向连接用链表的结构实现跳表，并不采用redius里的数组方式
 * @date 2022-03-19
 * @copyright Copyright (c) 2022
 */
#pragma once

#include "iterator.h"
#include "allocator.h"
#include <initializer_list>
#include <random>
#include <functional>
#include "utility.h"

namespace dalib {
/**
  * 3种节点介绍: 
  * 索引节点:
  * 值节点:
  */

/**
 * @brief 索引节点前置声明
 */
struct list_index_node;

/**
 * @brief 值结点前置声明
 */
template<typename T>
struct list_value_node;

/**
 * @brief 链表结点定义 
 */
struct list_node_base {
	using node_base = list_node_base ;

	list_node_base() noexcept:
			prev(nullptr),
			next(nullptr),
			down(nullptr) {
	}

	list_node_base(node_base* _prev, node_base* _next) noexcept:
			prev(_prev),
			next(_next),
			down(nullptr) {
	}

	list_node_base(node_base* _prev, node_base* _next, node_base* down) noexcept:
			prev(_prev),
			next(_next),
			down(down) {
	}

	list_index_node* as_index_node() {
		return (list_index_node*)(&*this);
	}

	template<typename T>
	list_value_node<T>* as_value_node() {
		return (list_value_node<T>*)(&*this);
	}

	node_base* prev;		//前驱结点(左边的结点)
	node_base* next;		//后继结点(右边的结点)
	node_base* down;		//下一个结点(下面的结点)
};

/**
 * @brief 索引结点的定义
 */
struct list_index_node : public list_node_base {
	list_index_node() = default;
	list_index_node(void* ptr) :
		pval(ptr) {
	}

	list_index_node* as_self() {
		return (list_index_node*)(&*this);
	}

	template<typename T>
	const T& get_value() const {
		return *(const T*)(pval);
	}

	const void* pval = nullptr;
};

/**
 * @brief 值结点的定义
 */
template<typename T>
struct list_value_node : public list_node_base {
	list_value_node() :
			value() {
	}
	list_value_node(const T& val) :
			value(val) {
	}

	const T& get_value() const noexcept {
		return value;
	}

	list_value_node* as_self() {
		return static_cast<list_value_node*>(&*this);
	}

	T value;
};

template<typename T>
struct list_iterator_base : public std::iterator<std::bidirectional_iterator_tag, T> {
	using node_base_ptr 	= list_node_base*;
	using iterator_category = std::bidirectional_iterator_tag;

	list_iterator_base() = default;

	list_iterator_base(node_base_ptr _node) :
		node(_node) {
	}

	bool operator==(const list_iterator_base& other) const noexcept { 
		return node == other.node; 
	}

	bool operator!=(const list_iterator_base& other) const noexcept { 
		return node != other.node; 
	}

	node_base_ptr node = nullptr;
};

template<typename T>
struct list_iterator : public list_iterator_base<T> {
	using size_type         = size_t;
	using difference_type   = ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type        = T;
	using node_base_ptr     = list_node_base*;
	using node_ptr          = list_value_node<T>*;
	using self              = list_iterator<T>;
	using pointer           = T*;
	using reference         = T&;

	list_iterator() = default;

	list_iterator(node_base_ptr node) :
		list_iterator_base<T>(node) {
	}

	reference operator*() const {
		list_value_node<T>* node = (list_value_node<T>*)(&*this->node);
		return node->value;
	}

	pointer operator->() const {
		return &(operator*());
	}

	self& operator++() noexcept {
		this->node = this->node->next;
		return *this;
	}

	self operator++(int) noexcept {
		self temp = *this;
		this->operator++();
		return temp;
	}
	self& operator--() noexcept {
		this->node = this->_node->prev;
		return *this;
	}
	self operator--(int) noexcept {
		self temp = *this;
		this->operator--();
		return temp;
	}
};

template<typename T>
struct list_const_iterator : public list_iterator_base<T> {
	using size_type         = size_t;
	using difference_type   = ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type        = T;
	using node_base_ptr     = list_node_base*;
	using node_ptr          = list_value_node<T>*;
	using self              = list_iterator<T>;
	using pointer           = T*;
	using reference         = T&;

	list_const_iterator() = default;

	list_const_iterator(node_base_ptr node) :
		list_iterator_base<T>(node) {
	}

	reference operator*() const {
		return this->_node->as_value_node()->value;
	}
	pointer operator->() const {
		return &(operator*());
	}
	self& operator++() noexcept {
		this->_node = this->_node->next;
		return *this;
	}
	self operator++(int) noexcept {
		self temp = *this;
		this->operator++();
		return temp;
	}
	self& operator--() noexcept {
		this->_node = this->_node->prev;
		return *this;
	}
	self operator--(int) noexcept {
		self temp = *this;
		this->operator--();
		return temp;
	}
};

template<typename T, typename Comp = std::less<T>, typename Alloc = allocator<T>>
class skip_list {
public:
	using allocator_type            = Alloc;
	using value_type                = T;
	using data_allocator            = Alloc;
	using node_base_allocator       = std::allocator<list_node_base>;
	using value_node_allocator      = std::allocator<list_value_node<value_type>>;
	using index_node_allocator      = std::allocator<list_index_node>;
	using node_allocator            = value_node_allocator;
	using pointer                   = T*;
	using const_pointer             = const T*;
	using reference                 = T&;
	using const_reference           = const T&;
	using size_type                 = size_t;
	using difference_type           = ptrdiff_t;
	using const_iterator            = list_const_iterator<value_type>;
	using iterator                  = list_iterator<value_type>;
	using reverse_iterator          = const std::reverse_iterator<iterator>;
	using const_reverse_iterator    = const reverse_iterator;
	using key_compare               = Comp;
	using value_compare             = Comp;

private:
	using node_base_ptr             = list_node_base*;
	using value_node_ptr            = list_value_node<T>*;
	using index_node_ptr            = list_index_node*;
	using node_ptr                  = value_node_ptr;

public:
	skip_list();
	skip_list(const std::initializer_list<value_type>& ilist);
	template<typename InputIter, typename = InputIterRequired<InputIter>>
	skip_list(InputIter first, InputIter last);
	skip_list(const skip_list& other);
	skip_list(skip_list&& other) noexcept;
	~skip_list() noexcept;

public:
	skip_list& operator=(const skip_list& other);
	skip_list& operator=(skip_list&& other);
	
public:
	bool operator==(const skip_list& other) const noexcept;
	bool operator!=(const skip_list& other) const noexcept;

public:
	iterator begin() noexcept { return m_node->next; }
	const_iterator begin() const noexcept { return m_node->next; }
	iterator end() noexcept{ return m_node; }
	const_iterator end() const noexcept { return m_node; }

	reverse_iterator rbegin() noexcept {
		return std::reverse_iterator<iterator>(end());
	}
	const_reverse_iterator rbegin() const noexcept { return std::reverse_iterator<iterator>(end()); }
	reverse_iterator rend() noexcept { return std::reverse_iterator<iterator>(begin()); }
	const_reverse_iterator rend() const noexcept { return std::reverse_iterator<iterator>(begin()); }

public:
	key_compare key_comp() const;
	value_compare value_comp() const;

public:
	//emplace
	template <typename ...Args>
	iterator emplace_multi(Args&& ...args);

	template <typename ...Args>
	std::pair<iterator, bool> emplace_unique(Args&& ...args);

	//insert
	iterator insert_multi(const value_type& value);
	iterator insert_multi(value_type&& value);
	template<typename InputIter, typename = InputIterRequired<InputIter>>
	iterator insert_multi(InputIter first, InputIter last);
	std::pair<iterator, bool> insert_unique(const value_type& value);
	std::pair<iterator, bool> insert_unique(value_type&& value);
	template<typename InputIter, typename = InputIterRequired<InputIter>>
	iterator insert_unique(InputIter first, InputIter last);
	
	//find
	iterator find(const T& value);
	const_iterator find(const T& value) const;
	size_type count_multi(const key_type& key);
	size_type count_multi(const key_type& key) const;
	size_type count_unique(const key_type& key);
	size_type count_unique(const key_type& key) const;
	std::pair<const_iterator, const_iterator> equal_range(const value_type& val) const;
	std::pair<iterator, iterator> equal_range(const value_type& val);
	iterator lower_bound(const value_type& value);
	const_iterator lower_bound(const value_type& value) const;
	iterator upper_bound(const value_type& value);
	const_iterator upper_bound(const value_type& value) const;
	std::pair<iterator, iterator>             
	equal_range_multi(const key_type& key) {
		return { lower_bound(key), upper_bound(key) };
	}

	std::pair<const_iterator, const_iterator> 
	equal_range_multi(const key_type& key) const {
		return { lower_bound(key), upper_bound(key) };
	}

	std::pair<iterator, iterator>             
	equal_range_unique(const key_type& key) {
		iterator iter = find(key);
		iterator next = iter;
		if (end() == iter) { 
			return { iter, iter };
		}

		return { iter, ++next };
	}
	std::pair<const_iterator, const_iterator> 
	equal_range_unique(const key_type& key) const {
		const_iterator iter = find(key);
		const_iterator next = iter;
		if (end() == iter) {
			return { iter, iter };	
		}
		return { iter, ++next };
	}

	//erase
	size_type erase(const value_type& value) noexcept;
	iterator erase(iterator position) noexcept;
	iterator erase(iterator first, iterator last) noexcept;
	size_type erase_multi(const key_type& key);
  	size_type erase_unique(const key_type& key);
	void clear();
	void pop_back();
	void pop_front();

	//other
	void swap(skip_list& other) noexcept;
	bool empty() const noexcept;
	size_type size() const noexcept;
	size_type max_size() const noexcept;
	const_reference front() const noexcept;
	const_reference back() const noexcept;
	allocator_type get_allocator() const noexcept;

#ifdef SKIP_LIST_DEBUG
#include <vector>
	std::vector<std::vector<T>> show() {
		std::vector<std::vector<T>> vec(m_levelSize, std::vector<T>(m_size));
		node_base_ptr curr = m_node;
		int column = 0;
		while (curr->next != m_node) {
			curr = curr->next;
			node_base_ptr node = curr;
			std::vector<T> stk;
			while (node->down != curr) {
				stk.push_back(node->down->as_index_node<value_type>()->get_value());
				node = node->down;
			}
			stk.push_back(curr->as_value_node<value_type>()->value);
			int currRow = levelSize() - 1;
			
			while (!stk.empty()) {
				vec.at(currRow).at(column) = stk.back();
				stk.pop_back();
				--currRow;
			}
			++column;
		}

		return vec;
	}

	size_type level_size() const noexcept {
		return m_levelSize;
	}
#endif // SKIP_LIST_DEBUG


private:
	/**
	 * list operation 
	 */
	node_base_ptr create_list_node_base();
	node_ptr create_list_node();
	template<typename... Args>
	node_ptr create_list_node(Args&&... args);
	value_node_ptr create_value_node();
	template<typename... Args>
	value_node_ptr create_value_node(Args&&... args);
	index_node_ptr create_index_node();
	index_node_ptr create_index_node(const_reference ref);
	
	void node_link_self(node_base_ptr node, bool isCircle = true);
	void node_unlink(node_base_ptr node);

	void destroy_node_base(node_base_ptr node);
	void destroy_list_node(node_ptr node);
	void destroy_value_node(value_node_ptr node);
	void destroy_index_node(index_node_ptr node);
	
	void list_base_clear(node_base_ptr lst);
	void list_clear(node_ptr lst);
	void list_value_clear(value_node_ptr lst);
	void list_index_clear(index_node_ptr lst);

	void list_insert(node_base_ptr position, node_base_ptr node);
	void list_insert_after(node_base_ptr position, node_base_ptr node);
	void list_value_erase(value_node_ptr position);
	void list_index_erase(index_node_ptr position);
	void list_erase(node_ptr position);

private:
	/**
	 * level operation
	 */
	void increase_level(size_type level = 1);
	size_type level_size() const noexcept;
	size_type max_level_size() const noexcept;

	/**
	 * 随机层
	 */
	size_type random_level();

	/**
	 * auxiliary functions 
	 */
	iterator insertAux(value_node_ptr newNode);

private:
	node_base_allocator& get_node_base_alloc() {
		static node_base_allocator alloc;
		return alloc;
	}
	node_allocator& get_node_alloc() {
		static node_allocator alloc;
		return alloc;
	}
	value_node_allocator& get_value_node_alloc() {
		static value_node_allocator alloc;
		return alloc;
	}
	index_node_allocator& get_index_node_alloc() {
		static index_node_allocator alloc;
		return alloc;
	}
	data_allocator& get_value_alloc() {
		static data_allocator alloc;
		return alloc;
	}

private:
	node_base_ptr m_header;		///< 链表头节点
	size_type m_levelSize;		///< 层数
	size_type m_size;			///< 值节点个数
};

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::node_base_ptr
skip_list<T, Comp,  Alloc>::create_list_node_base() {
	node_base_ptr node = get_node_base_alloc().allocate(1);
	node_link_self(node);
	return node;
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::node_ptr
skip_list<T, Comp,  Alloc>::create_list_node() {
	return create_value_node();
}

template<typename T,  typename Comp, typename Alloc>
template<typename... Args>
typename skip_list<T, Comp,  Alloc>::node_ptr 
skip_list<T, Comp,  Alloc>::create_list_node(Args&&... args) {
	return create_value_node(std::forward<Args>(args)...);
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::value_node_ptr 
skip_list<T, Comp,  Alloc>::create_value_node() {
	value_node_ptr node = get_value_node_alloc().allocate(1);
	get_value_alloc().construct(&node->value);
	node_link_self(node);
	return node;
}

template<typename T,  typename Comp, typename Alloc>
template<typename... Args>
typename skip_list<T, Comp,  Alloc>::value_node_ptr 
skip_list<T, Comp,  Alloc>::create_value_node(Args&&... args) {
	value_node_ptr node = get_value_node_alloc().allocate(1);
	get_value_alloc().construct(&node->value, std::forward<Args>(args)...);
	node_link_self(node);
	return node;
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::index_node_ptr 
skip_list<T, Comp,  Alloc>::create_index_node() {
	index_node_ptr node = get_index_node_alloc().allocate();
	node->pval = nullptr;
	node_link_self(node);
	return node;
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::index_node_ptr 
skip_list<T, Comp,  Alloc>::create_index_node(const_reference ref) {
	index_node_ptr node = get_index_node_alloc().allocate(1);
	node->pval = &ref;
	node_link_self(node);
	return node;
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::node_link_self(node_base_ptr node, bool isCircle) {
	node->down = node;
	if (isCircle) {
		node->prev = node->next = node;
	}
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::node_unlink(node_base_ptr node) {
	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->prev = node->next = node->down = nullptr;
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::destroy_node_base(node_base_ptr node) {
	get_node_base_alloc().deallocate(node);
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::destroy_list_node(node_ptr node) {
	destroy_value_node(node);
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::destroy_value_node(value_node_ptr node) {
	get_value_alloc().destroy(&node->value);
	get_node_alloc().deallocate(node, 1);
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::destroy_index_node(index_node_ptr node) {
	get_index_node_alloc().deallocate(node, 1);
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::list_base_clear(node_base_ptr lst) {
	if (nullptr == lst) {
		return;
	}

	while (lst->next != lst) {
		node_base_ptr node = lst->next;
		lst->next = node->next;
		get_node_base_alloc().deallocate(node);
	}
	lst->next = lst->prev = lst;
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::list_clear(node_ptr lst) {
	list_value_clear(lst);
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::list_value_clear(value_node_ptr lst) {
	if (nullptr == lst) {
		return;
	}

	while (lst->next != lst) {
		value_node_ptr node = lst->next;
		lst->next = node->next;
		destroy_value_node(node);
	}
	lst->next = lst->prev = lst;
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::list_index_clear(index_node_ptr lst) {
	if (nullptr == lst) {
		return;
	}

	while (lst->next != lst) {
		index_node_ptr node = lst->next;
		lst->next = node->next;
		destroy_index_node(node);
	}
	lst->next = lst->prev = lst;
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::list_insert(node_base_ptr position, node_base_ptr node) {
	node->prev = position->prev;
	position->prev->next = node;
	node->next = position;
	position->prev = node;
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::list_insert_after(node_base_ptr position, node_base_ptr node) {
	node->prev = position;
	node->next = position->next;
	node->next->prev = node;
	position->next = node;
}

template<typename T, typename Comp, typename Alloc>
void skip_list<T, Comp, Alloc>::list_value_erase(value_node_ptr position) {
	return list_erase(position);
}

template<typename T, typename Comp, typename Alloc>
void skip_list<T, Comp, Alloc>::list_index_erase(index_node_ptr position) {
	position->prev->next = position->next;
	position->next->prev = position->prev;
	destroy_index_node(position);
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::list_erase(node_ptr position) {
	position->prev->next = position->next;
	position->next->prev = position->prev;
	destroy_list_node(position);
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::size_type 
skip_list<T, Comp,  Alloc>::level_size() const noexcept {
	return m_levelSize;
}

template<typename T, typename Comp, typename Alloc>
typename skip_list<T, Comp, Alloc>::size_type 
skip_list<T, Comp, Alloc>::max_level_size() const noexcept{
	return sizeof(size_type) << 3;
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::increase_level(size_type sz) {
	m_levelSize += sz;
	while (sz) {
		node_base_ptr node = create_list_node_base();
		if (nullptr == m_node) {
			m_node = node;
		} else {
			node->down = m_node->down;
			m_node->down = node;
		}
		--sz;
	}
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::size_type
skip_list<T, Comp,  Alloc>::random_level() {
	static std::default_random_engine e;
	static std::uniform_int_distribution<int> u(0, 1);

	size_type level = 1;
	//size_type maxLevl = levelSize();

	for (size_type i = 1; i < maxLevelSize(); ++i) {
		if (1 == u(e)) {
			++level;
		} else {
			break;
		}
	}

	return level;
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::iterator
skip_list<T, Comp,  Alloc>::insertAux(value_node_ptr newNode) {
	size_type rLevel = random_level();
	if (rLevel > levelSize()) {
		increase_level(rLevel - levelSize());
	}

	node_base_ptr lastNode = nullptr;
	node_base_ptr firstIdxNode = nullptr;
	node_base_ptr curr = m_node->down;
	node_base_ptr currLevel = curr;
	key_compare comp;

	while (currLevel != m_node) {
		while (curr->next != currLevel && comp(curr->next->as_index_node()->get_value<value_type>(), newNode->get_value())) {
			curr = curr->next;
		}
		index_node_ptr	newIdxNode = create_index_node(newNode->value);
		list_insert_after(curr, newIdxNode);
		if (nullptr == lastNode) {
			firstIdxNode = newIdxNode;
			lastNode = newIdxNode;
		} else {
			lastNode->down = newIdxNode;
			lastNode = newIdxNode;
		}
		curr = curr->down;
		currLevel = currLevel->down;
		--rLevel;
	}

	/**
	 * 回到原始链表
	 */
	while (curr->next != m_node && comp(curr->next->as_value_node<value_type>()->get_value(), newNode->get_value())) {
		curr = curr->next;
	}
	list_insert_after(curr, newNode);
	if (lastNode) {
		lastNode->down = newNode;
		newNode->down = firstIdxNode;
	}
	++m_size;
	return newNode;
}

template<typename T,  typename Comp, typename Alloc>
skip_list<T, Comp,  Alloc>::skip_list() :
	m_node(create_list_node_base()),
	m_levelSize(1),
	m_size(0) {
}

template<typename T,  typename Comp, typename Alloc>
skip_list<T, Comp,  Alloc>::skip_list(const std::initializer_list<value_type>& ilist) :
	skip_list(ilist.begin(), ilist.end()) {
}

template<typename T,  typename Comp, typename Alloc>
template<typename InputIter, typename>
skip_list<T, Comp,  Alloc>::skip_list(InputIter first, InputIter last) :
	skip_list(){
	for (; first != last; ++first) {
		insert(*first);
	}
}

template<typename T,  typename Comp, typename Alloc>
skip_list<T, Comp,  Alloc>::skip_list(const skip_list& other) :
	m_node(nullptr),
	m_levelSize(0),
	m_size(0) {

}

template<typename T,  typename Comp, typename Alloc>
skip_list<T, Comp,  Alloc>::skip_list(skip_list&& other) noexcept :
	m_node(other.m_node),
	m_levelSize(other.m_levelSize),
	m_size(other.m_size) {
	other.m_node = create_list_node_base();
	other.m_levelSize = 1;
	other.m_size = 0;
}

template<typename T,  typename Comp, typename Alloc>
skip_list<T, Comp,  Alloc>::~skip_list() noexcept {
	clear();
	while (m_node->down != m_node) {
		node_base_ptr node = m_node->down;
		m_node->down = node->down;
		get_node_base_alloc().deallocate(node, 1);
	}
	get_node_base_alloc().deallocate(m_node, 1);
}

template<typename T,  typename Comp, typename Alloc>
bool skip_list<T, Comp,  Alloc>::operator==(const skip_list& other) const noexcept {
	if (size() != other.size()) {
		return false;
	}

	iterator iter1 = begin();
	iterator iter2 = other.begin();

	while (iter1 != end() && iter2 != other.end()) {
		if (*iter1++ != *iter2++) {
			return false;
		}
	}

	return true;
}

template<typename T,  typename Comp, typename Alloc>
bool skip_list<T, Comp,  Alloc>::operator!=(const skip_list& other) const noexcept {
	return !(this->operator==(other));
}

template<typename T,  typename Comp, typename Alloc>
template <typename ...Args>
typename skip_list<T, Comp, Alloc>::iterator 
skip_list<T, Comp, Alloc>::emplace_multi(Args&& ...args) {
	node_ptr newNode = create_value_node(std::forward<Args>(args...));
	return insertAux(newNode);
}

template<typename T,  typename Comp, typename Alloc>
template <typename ...Args>
std::pair<typename skip_list<T, Comp, Alloc>::iterator , bool> 
skip_list<T, Comp, Alloc>::emplace_unique(Args&& ...args) {
	node_ptr newNode = create_value_node(std::forward<Args>(args...));
	iterator iter = find(newNode->value);
	if (end() == iter) {
		return { insertAux(newNode), true };
	}

	return { iter, false };
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp, Alloc>::iterator
skip_list<T, Comp,  Alloc>::insert(const value_type& value) {
	value_node_ptr newNode = nullptr;
	try {
		newNode = create_value_node(value);
	} catch (...) {

	}
	return insertAux(newNode);
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp, Alloc>::iterator
skip_list<T, Comp,  Alloc>::insert(value_type&& value) {
	value_node_ptr newNode = create_value_node(std::move(value));
	return insertAux(newNode);
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp, Alloc>::iterator
skip_list<T, Comp,  Alloc>::insert(size_type n, const value_type& value) {
	iterator iter = end();
	while (n) {
		iter = insert(value);
		--n;
	}

	return iter;
}

template<typename T,  typename Comp, typename Alloc>
template<typename InputIter, typename>
typename skip_list<T, Comp,  Alloc>::iterator
skip_list<T, Comp,  Alloc>::insert(InputIter first, InputIter last) {
	iterator iter = end();
	for (; first != last; ++first) {
		iter = insert(*first);
	}

	return iter;
}

template<typename T, typename Comp, typename Alloc>
typename skip_list<T, Comp, Alloc>::iterator
skip_list<T, Comp, Alloc>::find(const T& value) {
	node_base_ptr curr = m_node->down;
	Comp comp;
	size_type level = levelSize();
	bool isFound = false;

	node_base_ptr currLevel = curr;
	while (level > 1) {
		bool compareTrue = true;
		while (curr->next != currLevel) {
			if (!comp(curr->next->as_index_node()->get_value<value_type>(), value)) {
				compareTrue = false;
				break;
			} 
			curr = curr->next;
		}
		
		if (curr->next != currLevel) {
			if (!compareTrue && !comp(value, curr->next->as_index_node()->get_value<value_type>())) {
				isFound = true;
				break;
			}	
		}
		currLevel = currLevel->down;
		curr = curr->down;
		--level;
	}

	/**
	 * 已经找到了，只需要找到在原始链表中该节点就行了
	 */
	if (isFound) {
		curr = curr->next;
		while (level > 1) {
			curr = curr->down;
			--level;
		}
		return curr;
	}
	/**
	 * 回到原始链表, 继续寻找
	 */
	bool compareTrue = true;
	while (curr->next != m_node) {
		if (!comp(curr->next->as_value_node<value_type>()->get_value(), value)) {
			compareTrue = false;
			break;
		} 
		curr = curr->next;
	}
	if (!compareTrue && !comp(value, curr->next->as_value_node<value_type>()->get_value())) {
		return curr->next;
	}
	return end();
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::const_iterator
skip_list<T, Comp,  Alloc>::find(const T& value) const {
	node_base_ptr curr = m_node->down;
	Comp comp;
	size_type level = levelSize();
	bool isFound = false;

	node_base_ptr currLevel = curr;
	while (level > 1) {
		bool compareTrue = true;
		while (curr->next != currLevel) {
			if (!comp(curr->next->as_index_node()->get_value<value_type>(), value)) {
				compareTrue = false;
				break;
			}
			curr = curr->next;
		}

		if (curr->next != currLevel) {
			if (!compareTrue && !comp(value, curr->next->as_index_node()->get_value<value_type>())) {
				isFound = true;
				break;
			}
		}
		currLevel = currLevel->down;
		curr = curr->down;
		--level;
	}

	/**
	 * 已经找到了，只需要找到在原始链表中该节点就行了
	 */
	if (isFound) {
		curr = curr->next;
		while (level > 1) {
			curr = curr->down;
			--level;
		}
		return curr;
	}
	/**
	 * 回到原始链表, 继续寻找
	 */
	bool compareTrue = true;
	while (curr->next != m_node) {
		if (!comp(curr->next->as_value_node<value_type>()->get_value(), value)) {
			compareTrue = false;
			break;
		}
		curr = curr->next;
	}
	if (!compareTrue && !comp(value, curr->next->as_value_node<value_type>()->get_value())) {
		return curr->next;
	}
	return end();
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::size_type
skip_list<T, Comp,  Alloc>::erase(const value_type& value) noexcept {
	node_base_ptr curr = m_node->down;
	Comp comp;
	size_type level = levelSize();
	bool isFound = false;

	node_base_ptr currLevel = curr;
	while (level > 1) {
		bool compareTrue = true;
		while (curr->next != currLevel) {
			if (!comp(curr->next->as_index_node()->get_value<value_type>(), value)) {
				compareTrue = false;
				break;
			}
			curr = curr->next;
		}

		if (curr->next != currLevel) {
			if (!compareTrue && !comp(value, curr->next->as_index_node()->get_value<value_type>())) {
				isFound = true;
				break;
			}
		}
		currLevel = currLevel->down;
		curr = curr->down;
		--level;
	}

	/**
	 * 已经找到了一路向下删除
	 */
	if (isFound) {
		curr = curr->next;
		while (level > 1) {
			index_node_ptr idxNode = curr->as_index_node();
			curr = curr->down;
			node_unlink(idxNode);
			destroy_index_node(idxNode);
			--level;
		}
		node_base_ptr _next = curr->next;
		node_unlink(curr);
		destroy_value_node(curr->as_value_node<value_type>());
		--m_size;
		return 1;
	} 

	/**
	 * 暂时还没找到，在原始链表上继续寻找
	 */
	bool compareTrue = true;
	while (curr->next != m_node) {
		if (!comp(curr->next->as_value_node<value_type>()->get_value(), value)) {
			compareTrue = false;
			break;
		}
		curr = curr->next;
	}
	if (!compareTrue && !comp(value, curr->next->as_value_node<value_type>()->get_value())) {
		erase(iterator(curr->next));
		return 1;
	}

	return 0;
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::iterator 
skip_list<T, Comp,  Alloc>::erase(iterator position) noexcept {
	node_base_ptr curr = position._node;
	while (curr->down != curr) {
		index_node_ptr idxNode = curr->down->as_index_node();
		node_unlink(idxNode);
		curr->down = idxNode->down;
		destroy_index_node(idxNode);
	}
	node_base_ptr _next = curr->next;
	node_unlink(curr);
	destroy_value_node(curr->as_value_node<value_type>());
	--m_size;
	return _next;
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::iterator
skip_list<T, Comp,  Alloc>::erase(iterator first, iterator last) noexcept {
	for (; first != last; ++first) {
		first = erase(first);
	}
	return first;
}

template<typename T,  typename Comp, typename Alloc>
template<typename... Args>
typename skip_list<T, Comp,  Alloc>::iterator 
skip_list<T, Comp,  Alloc>::emplace(Args&&... args) {
	return insertAux(create_value_node(std::forward<Args>(args)...));
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::swap(skip_list& other) noexcept {
	node_base_ptr node = other.m_node;
	size_type _levelSize = other.m_levelSize;
	size_type _size = other.m_size;
	other.m_node = m_node;
	other.m_levelSize = m_levelSize;
	other.m_size = m_size;
	m_node = node;
	m_levelSize = _levelSize;
	m_size = _size;
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::pop_back() {
	erase(--end());
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::pop_front() {
	erase(begin());
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::clear() noexcept {
	node_base_ptr curr = m_node;
	while (curr->down != m_node) {
		while (curr->next != curr) {
			index_node_ptr idxNode = curr->next->as_index_node();
			curr->next = idxNode->next;
			destroy_index_node(idxNode);
		}
		curr->prev = curr;
		curr = curr->down;
	}
	while (m_node->next != m_node) {
		value_node_ptr valNode = m_node->next->as_value_node<value_type>();
		m_node->next = valNode->next;
		destroy_value_node(valNode);
	}
	m_node->prev = m_node;
	m_size = 0;
}

template<typename T,  typename Comp, typename Alloc>
void skip_list<T, Comp,  Alloc>::remove(const value_type& value) noexcept {
	return erase(value);
}

template<typename T,  typename Comp, typename Alloc>
bool skip_list<T, Comp,  Alloc>::empty() const noexcept {
	return 0 == m_size;
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::size_type 
skip_list<T, Comp,  Alloc>::size() const noexcept {
	return m_size;
}

template<typename T, typename Comp, typename Alloc>
typename skip_list<T, Comp, Alloc>::size_type 
skip_list<T, Comp, Alloc>::max_size() const noexcept {
	return size_type(-1);
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::const_reference 
skip_list<T, Comp,  Alloc>::front() const noexcept {
	return *begin();
}

template<typename T,  typename Comp, typename Alloc>
typename skip_list<T, Comp,  Alloc>::const_reference 
skip_list<T, Comp,  Alloc>::back() const noexcept {
	return *--end();
}

} //namespace dalib