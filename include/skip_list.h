#pragma once

#include "construct.h"
#include <iterator>
#include "utility.h"
#include <functional>
#include <initializer_list>
#include "type_traits.h"
#include "concepts.h"

namespace nano {

struct skip_list_node_base;

struct skip_list_level {
    skip_list_node_base* forward = nullptr;
};

struct skip_list_node_base {
    skip_list_node_base* backward = nullptr;
    skip_list_level* level = nullptr;
};

template<typename T>
struct skip_list_node : public skip_list_node_base {
    T value;
};


template<typename T>
struct skip_list_iterator_base : public std::iterator<std::bidirectional_iterator_tag, T> {
    using self			= skip_list_iterator_base;
    using node_base_ptr = skip_list_node_base*;
	using node_ptr 		= skip_list_node<T>*;

	skip_list_iterator_base() noexcept = default;

    skip_list_iterator_base(node_base_ptr _node) noexcept :
        node(_node) {
    }

	skip_list_iterator_base(node_ptr _node) noexcept :
		node(_node) {
	}

	bool operator==(const self& other) const noexcept { return node == other.node; }
	bool operator!=(const self& other) const noexcept { return node != other.node; }

    node_base_ptr node;
};

template<typename T>
struct skip_list_iterator : public skip_list_iterator_base<T> {
	using category 			= std::bidirectional_iterator_tag;
	using size_type			= size_t;
	using value_type 		= T;
	using pointer 			= T*;
	using reference	 		= T&;
	using const_reference 	= T&;
    using node_base_ptr     = typename skip_list_iterator_base<T>::node_base_ptr;
	using node_ptr		    = typename skip_list_iterator_base<T>::node_ptr;
	using self 				= skip_list_iterator;

	skip_list_iterator() noexcept = default;
	skip_list_iterator(node_ptr _node) noexcept : 
		skip_list_iterator_base<T>(_node) { 
	}
	skip_list_iterator(node_base_ptr _node) noexcept :
		skip_list_iterator_base<T>(_node) {
	}

	self& operator++() noexcept {
		this->node = this->node->level[0].forward;
		return *this;
	}

	self operator++(int) noexcept {
		self temp = *this;
		++*this;
		return temp;
	}

    self& operator--() noexcept {
        this->node = this->node->backward;
        return *this;
    }

    self operator--(int) noexcept {
        self temp = *this;
        --*this;
        return temp;
    }

	reference operator*() const noexcept {
		return static_cast<node_ptr>(this->node)->value;
	}

	pointer operator->() const {
		return &(operator*());
	}
};

template<typename T>
struct skip_list_const_iterator : public skip_list_iterator_base<T> {
	using category 			= std::forward_iterator_tag;
	using size_type			= size_t;
	using value_type 		= T;
	using pointer 			= T*;
	using reference	 		= T&;
	using const_reference 	= T&;
	using node_base_ptr     = typename skip_list_iterator_base<T>::node_base_ptr;
	using node_ptr		    = typename skip_list_iterator_base<T>::node_ptr;
	using self 				= skip_list_const_iterator<T>;

	skip_list_const_iterator() noexcept = default;

	skip_list_const_iterator(node_ptr* _node) noexcept : 
		skip_list_iterator_base<T>(_node) { 
	}

	skip_list_const_iterator(node_base_ptr _node) noexcept : 
		skip_list_iterator_base<T>(_node) { 
	}

	self& operator++() noexcept {
		this->node = this->node->level[0].forward;
		return *this;
	}

	self operator++(int) noexcept {
		self temp = *this;
		++*this;
		return temp;
	}

    self& operator--() noexcept {
        this->node = this->node->backward;
    }

    self operator--(int) noexcept {
        self temp = *this;
        --*this;
        return temp;
    }

	reference operator*() const noexcept {
		return static_cast<node_ptr>(this->node)->value;
	}

	pointer operator->() const noexcept {
		return &(operator*());
	}
};

template<typename T, typename Comp = std::less<T>>
class skip_list {
public:
    using level_type                = int;

public:
    constexpr static level_type MAX_LEVEL = 32;
    constexpr static int P = 4;

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
	using const_iterator            = skip_list_const_iterator<value_type>;
	using iterator                  = skip_list_iterator<value_type>;
    using reverse_iterator          = const std::reverse_iterator<iterator>;
	using const_reverse_iterator    = const std::reverse_iterator<const_iterator>;

public:
    iterator begin() noexcept { return static_cast<node_ptr>(m_header->level[0].forward); }
	iterator end() noexcept { return m_header; }
    reverse_iterator rbegin() noexcept { return std::reverse_iterator<iterator>(end()); }
	reverse_iterator rend() noexcept { return std::reverse_iterator<iterator>(begin()); }
	const_iterator begin() const noexcept { return static_cast<node_ptr>(m_header->level[0].forward); }
	const_iterator end() const noexcept { return m_header; }
    const_reverse_iterator rbegin() const noexcept { return std::reverse_iterator<const_iterator>(end()); }
	const_reverse_iterator rend() const noexcept { return std::reverse_iterator<const_iterator>(begin()); }

public:
    skip_list(const Comp& comp = Comp());

	skip_list(const std::initializer_list<key_type>& ilist, const Comp& comp = Comp()) :
		skip_list(ilist.begin(), ilist.end(), comp) {
	}

	template<std::input_iterator InputIter>
	skip_list(InputIter first, InputIter last, const Comp& comp = Comp());

	skip_list(const skip_list& other);

	skip_list(skip_list&& other);

	~skip_list();

	skip_list& operator=(const skip_list& other);

	skip_list& operator=(skip_list&& other);

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

	size_type count_multi(const key_type& key) const noexcept;
	size_type count_unique(const key_type& key) const noexcept;

	iterator lower_bound(const key_type& key) noexcept { 
        return lbound(key).node; 
    }
	const_iterator lower_bound(const key_type& key) const noexcept { 
        return lbound(key).node; 
    }

	iterator upper_bound(const key_type& key) noexcept { return ubound(key).node; }
	const_iterator upper_bound(const key_type& key) const noexcept { return ubound(key).node; }

	std::pair<iterator, iterator>             
	equal_range_multi(const key_type& key) noexcept { 
        return { lower_bound(key), upper_bound(key) }; 
    }

	std::pair<const_iterator, const_iterator> 
	equal_range_multi(const key_type& key) const noexcept { 
        return { lower_bound(key), upper_bound(key) }; 
    }

	std::pair<iterator, iterator> equal_range_unique(const key_type& key) noexcept;
	std::pair<const_iterator, const_iterator> 
    equal_range_unique(const key_type& key) const noexcept;

	//other
	void swap(skip_list& rhs) noexcept;
	size_type size() const noexcept { return m_size; }
	bool empty() const noexcept { return 0 == m_size; }

private:
    using node_ptr 					= skip_list_node<T>*;
	using node_base_ptr				= skip_list_node_base*;

private:
    iterator lbound(const key_type& key);
	iterator ubound(const key_type& key);
    level_type random_level();

private:
    template<typename... Args>
	node_ptr create_node(level_type level, Args&&... args);
	node_base_ptr create_node_base();
	static void destroy_node(node_ptr node);
	static void destroy_node_base(node_base_ptr node);
	bool get_insert_muti(const key_type& key, node_base_ptr (&update)[MAX_LEVEL]);
	bool get_insert_unique(const key_type& key, node_base_ptr (&update)[MAX_LEVEL]);
	iterator insert_node(node_ptr node, level_type level, node_base_ptr (&update)[MAX_LEVEL]);

private:
    node_base_ptr m_header;
    size_type m_size;
    level_type m_level;
    const Comp& m_comp;
};

template<typename T, typename Comp>
typename skip_list<T, Comp>::iterator 
skip_list<T, Comp>::lbound(const key_type& key) {
    node_base_ptr head = m_header;
    for (level_type i = m_level - 1; i >= 0; --i) {
        while (head->level[i].forward != m_header &&
                (m_comp(static_cast<node_ptr>(head->level[i].forward)->value, key))) {
            head = head->level[i].forward;    //走到下一个节点
        }
    }
    return head;
}

template<typename T, typename Comp>
typename skip_list<T, Comp>::iterator 
skip_list<T, Comp>::ubound(const key_type& key) {
    node_base_ptr head = m_header;
    level_type i;
    for (i = m_level - 1; i >= 0; --i) {
        while (head->level[i].forward != m_header &&
                (m_comp(static_cast<node_ptr>(head->level[i].forward)->value, key))) {
            head = head->level[i].forward;    //走到下一个节点
        }
    }

    //跳过相等的节点
    while (head->level[i].forward != m_header &&
            (!m_comp(key, static_cast<node_ptr>(head->level[i].forward)->value))) {
        head = head->level[i].forward;
    }
    return head;
}

template<typename T, typename Comp>
typename skip_list<T, Comp>::level_type 
skip_list<T, Comp>::random_level() {
    constexpr int mask = 0xFFFF;  //const static
    level_type level = 1;
    while ((random() & mask) < (mask / P)) {
        ++level;
    }
    return (level < MAX_LEVEL) ? level : MAX_LEVEL;
}

template<typename T, typename Comp>
template<typename... Args>
typename skip_list<T, Comp>::node_ptr 
skip_list<T, Comp>::create_node(level_type level, Args&&... args) {
    node_ptr newNode = static_cast<node_ptr>(::operator new(sizeof(skip_list_node<T>)));
    newNode->level = static_cast<skip_list_level*>(::operator new(sizeof(skip_list_level) * level));
    try {
        construct(&newNode->value, std::forward<Args>(args)...);
    } catch (...) {
        ::operator delete(newNode->level);
        ::operator delete(newNode);
        newNode = nullptr;
    }
    return newNode;
}

template<typename T, typename Comp>
typename skip_list<T, Comp>::node_base_ptr 
skip_list<T, Comp>::create_node_base() {
    node_base_ptr newNode = static_cast<node_base_ptr>(::operator new(sizeof(skip_list_node_base)));
    newNode->level = static_cast<skip_list_level*>(::operator new(sizeof(skip_list_level) * MAX_LEVEL));
    newNode->backward = newNode;
    for (level_type i = 0; i < MAX_LEVEL; ++i) {
        newNode->level[i].forward = newNode;
    }

    return newNode;
}

template<typename T, typename Comp>
void skip_list<T, Comp>::destroy_node(node_ptr node) {
    destroy(&node->value);
    ::operator delete(node->level);
    ::operator delete(node);
}

template<typename T, typename Comp>
void skip_list<T, Comp>::destroy_node_base(node_base_ptr node) {
    ::operator delete(node->level);
    ::operator delete(node);
}

template<typename T, typename Comp>
bool skip_list<T, Comp>::get_insert_muti(const T& key, 
        node_base_ptr (&update)[MAX_LEVEL]) {
    node_base_ptr head = m_header;
    for (level_type i = m_level - 1; i >= 0; --i) {
        while (head->level[i].forward != m_header &&
                (m_comp(static_cast<node_ptr>(head->level[i].forward)->value, key))) {
            head = head->level[i].forward;    //走到下一个节点
        }
        update[i] = head;
    }
    
    return true;
}

template<typename T, typename Comp>
bool skip_list<T, Comp>::get_insert_unique(const T& key, 
        node_base_ptr (&update)[MAX_LEVEL]) {
    node_base_ptr head = m_header;
    for (level_type i = m_level - 1; i >= 0; --i) {
        while (head->level[i].forward != m_header &&
                (m_comp(static_cast<node_ptr>(head->level[i].forward)->value, key))) {
            head = head->level[i].forward;    //走到下一个节点
        }
        update[i] = head;
    }
    //!(node->value < key) && !(key < node->value) => key == node->value
    if (head->level[0].forward != m_header && 
            !m_comp(key, static_cast<node_ptr>(head->level[0].forward)->value)) {
        return false;
    }


    return true;
}

template<typename T, typename Comp>
typename skip_list<T, Comp>::iterator 
skip_list<T, Comp>::insert_node(node_ptr node, level_type level, node_base_ptr (&update)[MAX_LEVEL]) {
    if (level > m_level) {
        for (level_type i = m_level; i < level; ++i) {
            update[i] = m_header;    //需要更新头
        }
        m_level = level;
    }

    for (level_type i = 0; i < level; ++i) {
        node->level[i].forward = update[i]->level[i].forward;
        update[i]->level[i].forward = node;
    }

    node->backward = update[0];
    node->level[0].forward->backward = node;
    
    ++m_size;
    return node;
}

template<typename T, typename Comp>
skip_list<T, Comp>::skip_list(const Comp& comp) :
    m_header(create_node_base()),
    m_size(0),
    m_level(1),
    m_comp(comp) {
}

template<typename T, typename Comp>
template<std::input_iterator InputIter>
skip_list<T, Comp>::skip_list(InputIter first, InputIter last, 
        const Comp& comp) :
        skip_list(comp) {
    for (; first != last; ++first) {
        insert_multi(*first);
    }
}

template<typename T, typename Comp>
skip_list<T, Comp>::skip_list(const skip_list& other) :
        skip_list(other.begin(), other.end()) {
}

template<typename T, typename Comp>
skip_list<T, Comp>::skip_list(skip_list&& other) :
        m_header(other.m_header),
        m_size(other.m_size),
        m_level(other.m_level) {
    other.m_header = create_node();
    other.m_size = 0;
    other.m_level = 1;
}

template<typename T, typename Comp>
skip_list<T, Comp>::~skip_list() {
    clear();
    destroy_node_base(m_header);
}

template<typename T, typename Comp>
skip_list<T, Comp>&
skip_list<T, Comp>::operator=(const skip_list& other) {
    if (this != &other) {
        clear();
        insert_multi(other.begin(), other.end());
    }
}

template<typename T, typename Comp>
skip_list<T, Comp>& 
skip_list<T, Comp>::operator=(skip_list&& other) {
    if (this != &other) {
        m_header = other.m_header;
        m_size = other.m_size;
        m_level = other.m_level;
        other.m_header = create_node();
        other.m_size = 0;
        other.m_level = 1;
    }
}

template<typename T, typename Comp>
template <typename ...Args>
typename skip_list<T, Comp>::iterator 
skip_list<T, Comp>::emplace_multi(Args&& ...args) {
    level_type level = random_level();

    node_ptr newNode = create_node(level, std::forward<Args>(args)...);
    node_base_ptr update[MAX_LEVEL];
    get_insert_muti(newNode->value, update);
    insert_node(newNode, level, update);
    return newNode;
}

template<typename T, typename Comp>
template <typename ...Args>
typename skip_list<T, Comp>::iterator 
skip_list<T, Comp>::emplace_multi_hint(iterator hint, Args&& ...args) {
    //temporary
    return emplace_multi(std::forward<Args>(args)...);
}

template<typename T, typename Comp>
template <typename ...Args>
std::pair<typename skip_list<T, Comp>::iterator, bool> 
skip_list<T, Comp>::emplace_unique(Args&& ...args) {
    level_type level = random_level();
    bool succeed = false;
    node_ptr newNode = create_node(level, std::forward<Args>(args)...);
    node_base_ptr update[MAX_LEVEL];
    if (get_insert_unique(newNode->value, update)) {
        insert_node(newNode, level, update);
        succeed = true;
    } else {
        destroy_node(newNode);
    }
    
    return { newNode, succeed };
}

template<typename T, typename Comp>
template <typename ...Args>
std::pair<typename skip_list<T, Comp>::iterator, bool> 
skip_list<T, Comp>::emplace_unique_hint(iterator hint, Args&& ...args) {
    //temporary
    return emplace_unique(std::forward<Args>(args)...);
}

template<typename T, typename Comp>
template <std::input_iterator InputIter>
void skip_list<T, Comp>::insert_multi(InputIter first, InputIter last) {
    for (; first != last; ++first) {
        emplace_multi(*first);
    }
}

template<typename T, typename Comp>
template <std::input_iterator InputIter>
void skip_list<T, Comp>::insert_unique(InputIter first, InputIter last) {
    for (; first != last; ++first) {
        emplace_unique(*first);
    }
}

template<typename T, typename Comp>
typename skip_list<T, Comp>::iterator  
skip_list<T, Comp>::erase(iterator hint) {
    if (end() == hint) {
        return end();
    }

    node_base_ptr update[MAX_LEVEL];
    node_base_ptr header = m_header;
    key_type& key = static_cast<node_ptr>(hint.node)->value;
    
    for (level_type i = m_level - 1; i >= 0; --i) {
        while (header->level[i].forward != m_header) {
            if (m_comp(static_cast<node_ptr>(header->level[i].forward)->value, key)) {
                header = header->level[i].forward;
            } else {
                //跳过值相等但不是目标值的节点
                if (!m_comp(key, static_cast<node_ptr>(header->level[i].forward)->value)) {
                    while (header->level[0].forward != hint.node) {
                        header = header->level[0].forward;
                        update[i] = header;
                        --i;
                    }
                    break;
                }
            }
        }
        update[i] = header;
    }

    header = header->level[0].forward;
    if (header != m_header && !m_comp(key, static_cast<node_ptr>(header)->value)) {
        for (level_type i = 0; i < m_level; i++) {
            if (update[i]->level[i].forward == header) {
                update[i]->level[i].forward = header->level[i].forward;
            }
        }
        header->level[0].forward->backward = header->backward;
        while(m_level > 1 && m_header->level[m_level - 1].forward == m_header) {
            --m_level;
        }
    }
    node_base_ptr next = header->level[0].forward;
    destroy_node(static_cast<node_ptr>(header));
    return next;
}

template<typename T, typename Comp>
typename skip_list<T, Comp>::size_type 
skip_list<T, Comp>::erase_multi(const key_type& key) {
    //temporary
    size_type count = 0;
    while (erase_unique(key)) {
        ++count;
    }
    return count;
}

template<typename T, typename Comp>
typename skip_list<T, Comp>::size_type 
skip_list<T, Comp>::erase_unique(const key_type& key) {
    node_base_ptr update[MAX_LEVEL];
    node_base_ptr header = m_header;
    
    for (level_type i = m_level - 1; i >= 0; --i) {
        while (header->level[i].forward != m_header &&
                (m_comp(static_cast<node_ptr>(header->level[i].forward)->value, key))) {
            header = header->level[i].forward;
        }
        update[i] = header;
    }
    header = header->level[0].forward;
    size_type count = 0;
    if (header != m_header && !m_comp(key, static_cast<node_ptr>(header)->value)) {
        for (level_type i = 0; i < m_level; i++) {
            if (update[i]->level[i].forward == header) {
                update[i]->level[i].forward = header->level[i].forward;
            }
        }
        header->level[0].forward->backward = header->backward;
        while(m_level > 1 && m_header->level[m_level - 1].forward == m_header) {
            --m_level;
        }
        ++count;
    }
    if (count) {
        destroy_node(static_cast<node_ptr>(header));
    }
    m_size -= count;
    return count;
}

template<typename T, typename Comp>
void skip_list<T, Comp>::erase(iterator first, iterator last) {
    while (first != last) {
        iterator next = first;
        ++next;
        erase(first);
        first = next;
    }
}

template<typename T, typename Comp>
void skip_list<T, Comp>::clear() {
    while (m_header->backward != m_header) {
        node_ptr node = static_cast<node_ptr>(m_header->backward);
        m_header->backward = node->backward;
        destroy_node(node);
    }
    m_size = 0;
    m_level = 1;
}

template<typename T, typename Comp>
typename skip_list<T, Comp>::iterator 
skip_list<T, Comp>::find(const key_type& key) noexcept {
    iterator iter = lbound(key);
    if (end() != iter && !m_comp(key, static_cast<node_ptr>(iter.node)->value)) {
        return iter.node;
    }

    return end();
}

template<typename T, typename Comp>
typename skip_list<T, Comp>::const_iterator 
skip_list<T, Comp>::find(const key_type& key) const noexcept {
    iterator iter = lbound(key);
    if (end() != iter && !m_comp(key, static_cast<node_ptr>(iter.node)->value)) {
        return iter.node;
    }

    return iter.node;
}

template<typename T, typename Comp>
typename skip_list<T, Comp>::size_type 
skip_list<T, Comp>::count_multi(const key_type& key) const noexcept {
    iterator iter = lbound(key);
    size_type count = 0;
    while (iter != end() && !m_comp(key, *iter)) {
        ++count;
    }
    
    return count;
}


template<typename T, typename Comp>
typename skip_list<T, Comp>::size_type 
skip_list<T, Comp>::count_unique(const key_type& key) const noexcept {
    iterator iter = lbound(key);
    if (end() == iter) {
        return 0;
    }
    return m_comp(key, *iter) ? 0 : 1;
}

template<typename T, typename Comp>
std::pair<typename skip_list<T, Comp>::iterator, typename skip_list<T, Comp>::iterator>             
skip_list<T, Comp>::equal_range_unique(const key_type& key) noexcept {
    iterator iter = find(key);
    iterator next = iter;
    if (end() == iter) { 
        return { iter, iter };
    }

    return { iter, ++next };
}

template<typename T, typename Comp>
std::pair<typename skip_list<T, Comp>::const_iterator, typename skip_list<T, Comp>::const_iterator> 
skip_list<T, Comp>::equal_range_unique(const key_type& key) const noexcept {
    const_iterator iter = find(key);
    const_iterator next = iter;
    if (end() == iter) {
        return { iter, iter };	
    }
    return { iter, ++next };
}

template<typename T, typename Comp>
void skip_list<T, Comp>::swap(skip_list& rhs) noexcept {
    if (this != &rhs) {
        std::swap(m_header, rhs.m_header);
        std::swap(m_size, rhs.m_size);
        std::swap(m_level, rhs.m_level);
    }
}

template<typename T, typename Comp>
bool operator<(const skip_list<T, Comp>& lhs, const skip_list<T, Comp>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, typename Comp>
bool operator>(const skip_list<T, Comp>& lhs, const skip_list<T, Comp>& rhs) {
	return rhs < lhs;
}

template<typename T, typename Comp>
bool operator<=(const skip_list<T, Comp>& lhs, const skip_list<T, Comp>& rhs) {
	return !(rhs < lhs);
}

template<typename T, typename Comp>
bool operator>=(const skip_list<T, Comp>& lhs, const skip_list<T, Comp>& rhs) {
	return !(lhs < rhs);
}

template<typename T, typename Comp>
bool operator==(const skip_list<T, Comp>& lhs, const skip_list<T, Comp>& rhs) {
	if (lhs.size() != rhs.size()) {
		return false;
	}
	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, typename Comp>
bool operator!=(const skip_list<T, Comp>& lhs, const skip_list<T, Comp>& rhs) {

	return !(lhs == rhs);
}

} //namespace nano