#pragma once

#include "rb_tree.h"
#include "list.h"
#include "mini_vector.h"
#include "system.h"
#include "algo.h"
#include "hash.h"
#include <assert.h>
#include <iterator>

#ifdef BIT32
#include "dynamic_bitset.h"
#endif //BIT32

namespace nano {

/**
 * @brief 
 * @tparam T 
 * @tparam cache 在rehash时加速
 * @attention 如果cache为true，那么一定要保证同一个值在任何时候在你提供
 * 			  的hash函数的计算下，得到的hash值都是相同的
 */
template<typename T, bool cache = false>
struct ht_list_node;

template<typename T>
struct ht_list_node<T, false> : public forward_list_node<T> {
};

template<typename T>
struct ht_list_node<T, true> : public forward_list_node<T> {
	size_t hash_val;
};

template<typename T, bool cache>
ht_list_node<T, cache>* next_of(ht_list_node<T, cache>* node) {
	list_node_base* nodeBase = node;
	return static_cast<ht_list_node<T, cache>*>(next_of(nodeBase));
}

template<typename T, bool cache>
inline void 
list_link_after(ht_list_node<T, cache>* node, ht_list_node<T, cache>** target) {
	list_node_base* nodeBase = node;
	list_node_base* targetBase = *target;
	list_link_after(nodeBase, &targetBase);
	*target = static_cast<ht_list_node<T, cache>*>(targetBase);
}

template<typename T, bool cache>
inline ht_list_node<T, cache>*
list_unlink_after(ht_list_node<T, cache>* node) {
	forward_list_node<T>* nodeBase = node;
	forward_list_node<T>* nextNode = list_unlink_after(nodeBase);
	return static_cast<ht_list_node<T, cache>*>(nextNode);
}

template<typename T, bool cache, typename BinOp>
inline ht_list_node<T, cache>*
list_find_first_of(ht_list_node<T, cache>* head, const T& val, const BinOp& binop) {
	forward_list_node<T>* headBase = head;
	return static_cast<ht_list_node<T, cache>*>(list_find_first_of(headBase, val, binop));
}

/**
 * @brief 
 * @tparam T 
 * @tparam cache 在rehash时加速
 * @attention 如果cache为true，那么一定要保证同一个值在任何时候在你提供
 * 			  的hash函数的计算下，得到的hash值都是相同的
 */
template<typename T, bool cache = false>
struct ht_tree_node;

template<typename T>
struct ht_tree_node<T, false> : public rb_tree_node<T> {
};

template<typename T>
struct ht_tree_node<T, true> : public rb_tree_node<T> {
	size_t hash_val;
};

template<typename T, bool cache>
inline ht_tree_node<T, cache>* 
left_of(ht_tree_node<T, cache>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<ht_tree_node<T, cache>*>(left_of(nodeBase));
}

template<typename T, bool cache>
inline ht_tree_node<T, cache>* 
right_of(ht_tree_node<T, cache>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<ht_tree_node<T, cache>*>(right_of(nodeBase));
}

template<typename T, bool cache>
inline ht_tree_node<T, cache>* 
parent_of(ht_tree_node<T, cache>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<ht_tree_node<T, cache>*>(parent_of(nodeBase));
}

template<typename T, bool cache>
inline ht_tree_node<T, cache>* 
precursor(ht_tree_node<T, cache>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<ht_tree_node<T, cache>*>(precursor(nodeBase));
}

template<typename T, bool cache>
inline ht_tree_node<T, cache>* 
successor(ht_tree_node<T, cache>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<ht_tree_node<T, cache>*>(successor(nodeBase));
}

template<typename T, bool cache>
inline ht_tree_node<T, cache>* 
min_node(ht_tree_node<T, cache>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<ht_tree_node<T, cache>*>(min_node(nodeBase));
}

template<typename T, bool cache>
inline ht_tree_node<T, cache>* 
max_node(ht_tree_node<T, cache>* node) {
	tree_node_base* nodeBase = node;
	return static_cast<ht_tree_node<T, cache>*>(max_node(nodeBase));
}

template<typename T, bool cache, typename Comp>
inline ht_tree_node<T, cache>* 
ht_tree_insert_node_multi(ht_tree_node<T, cache>* node, 
		ht_tree_node<T, cache>** root, const Comp& comp = Comp()) {
	rb_tree_node<T>* rbRoot = *root;
	rb_insert_node_multi(node, &rbRoot, comp);
	*root = static_cast<ht_tree_node<T, cache>*>(rbRoot);
	return *root;
}

template<typename T, bool cache, typename Comp>
inline std::pair<ht_tree_node<T, cache>*, bool> 
ht_tree_insert_node_unique(ht_tree_node<T, cache>* node, 
		ht_tree_node<T, cache>** root, const Comp& comp = Comp()) {
	rb_tree_node<T>* rbRoot = *root;
	std::pair<rb_tree_node<T>*, bool> result = rb_insert_node_unique(node, &rbRoot, comp);
	*root = static_cast<ht_tree_node<T, cache>*>(rbRoot);
	return { *root, result.second };
}

template<typename T, bool cache>
inline ht_tree_node<T, cache>* 
ht_tree_erase_node(ht_tree_node<T, cache>* node,
		ht_tree_node<T, cache>** root) {
	rb_tree_node<T>* rbRoot = *root;
	rb_erase_node(node, &rbRoot);
	*root = static_cast<ht_tree_node<T, cache>*>(rbRoot);
	return *root;
}

template<typename T, bool cache, typename Comp>
inline ht_tree_node<T, cache>* 
ht_tree_lbound(const T& val, ht_tree_node<T, cache>* root, const Comp& comp) {
	return static_cast<ht_tree_node<T, cache>*>(bst_lbound(val, root, comp));
}

template<typename T, bool cache, typename Comp>
inline ht_tree_node<T, cache>* 
ht_tree_rbound(const T& val, ht_tree_node<T, cache>* root, const Comp& comp) {
	return static_cast<ht_tree_node<T, cache>*>(bst_rbound(val, root, comp));
}

template<typename T, bool cache, typename Size, typename Comp>
inline Size 
ht_tree_count_multi(const T& val, ht_tree_node<T, cache>* root, const Comp& comp) {
    return bst_count_multi<T, Size, Comp>(val, root, comp);
}

template<typename T, bool cache, typename Comp>
inline std::pair<ht_tree_node<T, cache>*, ht_tree_node<T, cache>*> 
ht_tree_equal_range_multi(const T& val, ht_tree_node<T, cache>* root,
        const Comp& comp = Comp()) {
    return { ht_tree_lbound(val, root, comp), ht_tree_ubound(val, root, comp) };
}

#ifdef BIT64
inline constexpr static uint64_t mask = 0x8000000000000000;

/**
 * @brief 由于在64位Linux(Windows也一样)上地址最高三位在用户态下是不使用的
 * 		  这里我们可以把地址最高位拿来做一个标志，如果最高位为1表示当前桶已
 * 		  经转化为一个红黑树了，否则是单链表。当然如果使用的是ptmalloc，最
 * 		  低位由于对齐也不使用，同样可以拿来做标志，并且在32位机器上也能运行，
 * 		  但我不能保证你所用的malloc一定是内存对齐的，所以保守起见，不使用最
 * 		  低位来做标志。
 * @tparam T 
 */
template<typename T, bool cache = false>
union ht_entry { //可以考虑使用variant.
	ht_entry() = default;
	ht_entry(const ht_entry<T, cache>& other) :
		flag(other.flag) {
	}
	ht_entry(ht_list_node<T, cache>* node) :
		list_node(node) {
	}
	ht_entry(ht_tree_node<T, cache>* node) :
		tree_node(node) {
	}
	ht_entry& operator=(ht_list_node<T, cache>* node) {
		uint64_t addr = reinterpret_cast<uint64_t>(node);
		list_node = reinterpret_cast<ht_list_node<T>*>(addr | (flag & mask));
		return *this;
	}
	ht_entry& operator=(ht_tree_node<T, cache>* node) {
		uint64_t addr = reinterpret_cast<uint64_t>(node);
		tree_node = reinterpret_cast<ht_tree_node<T>*>(addr | (flag & mask));
		return *this;
	}
	ht_entry& operator=(const ht_entry<T, cache>& other) {
		if (this != &other) {
			flag = other.flag;
		}
		return *this;
	}

	ht_list_node<T, cache>* as_list_node_ptr() const {
		return reinterpret_cast<ht_list_node<T, cache>*>(flag & (~mask));
	}

	ht_tree_node<T, cache>* as_tree_node_ptr() const {
		return reinterpret_cast<ht_tree_node<T, cache>*>(flag & (~mask));
	}

	bool isnull() const {
		return 0 == flag;
	}

	ht_list_node<T, cache>* list_node;	//也可以考虑用数组
	ht_tree_node<T, cache>* tree_node;	
	uint64_t flag = 0;					//list or tree
};
#else
/**
 * @brief 32位Linux地址所有位都有效，这样就不能用最高位来区分
 * 		  链表和红黑树，只能自己使用额外的一个bitset来标记
 * @tparam T 
 */
template<typename T, bool cache = false>
union ht_entry {
	ht_entry& operator=(ht_list_node<T, cache>* node) {
		list_node = node;
		return *this;
	}
	ht_entry& operator=(ht_tree_node<T, cache>* node) {
		tree_node = node;
		return *this;
	}
	ht_entry& operator=(const ht_entry<T, cache>& other) {
		if (this != &other) {
			list_node = other.list_node;
		}
	}

	ht_list_node<T, cache>* as_list_node_ptr() const {
		return list_node;
	}

	ht_tree_node<T, cache>* as_tree_node_ptr() const {
		return tree_node;
	}

	bool isnull() const {
		return nullptr == list_node;
	}

	ht_list_node<T, cache>* list_node = nullptr;	//也可以考虑用数组
	ht_tree_node<T, cache>* tree_node = nullptr;
};
#endif //BIT64

//如果一个类可以比较大小，那么一定可以加上逻辑判断来比较相等和不相等
template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
class hash_table;

template<typename T, bool cache = false, typename Hash = hash_value<T>, 
	typename Comp = std::less<T>, typename Pred = std::equal_to<T>>
struct ht_iterator_base : public std::iterator<std::forward_iterator_tag, T> {  
	using difference_type   = ptrdiff_t;
	using value_type 		= T;
	using reference			= T&; 
	using pointer 			= T*;
	using size_type 		= size_t;
	using self              = ht_iterator_base<T, cache, Hash, Comp, Pred>;
    using container         = hash_table<T, cache, Hash, Comp, Pred>;
	using tree_node 		= ht_tree_node<T, cache>;
	using list_node 		= ht_list_node<T, cache>;
	using tree_node_ptr 	= tree_node*;
	using list_node_ptr  	= list_node*;
	using entry_type		= ht_entry<T, cache>;

	ht_iterator_base() noexcept = default;
	ht_iterator_base(size_type buckestIndex, entry_type _entry, container* _ht) :
			bIndex(buckestIndex),
			entry(_entry),
			ht(_ht) {
	}

	void increase() noexcept;
    bool operator==(const self& other) const noexcept { 
		return entry.list_node == other.entry.list_node; //比较tree_node也可以
	}
    bool operator!=(const self& other) const noexcept { return !(*this == other); }
    reference operator*() const noexcept { 
		if (ht->is_list(bIndex)) {
			return entry.as_list_node_ptr()->value;
		} else {
			return entry.as_tree_node_ptr()->value;
		}
	}
	pointer operator->() const noexcept { return &(operator*());}

	size_type bIndex;	///< bucket index
    entry_type entry;  	///< 桶内部节点
	container* ht;      ///< 哈希表
};

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
void ht_iterator_base<T, cache, Hash, Comp, Pred>::increase() noexcept {
	if (ht->is_list(bIndex)) {
		list_node_ptr head = entry.as_list_node_ptr();
		head = next_of(head);
		if (nullptr == head) {
			std::pair<size_type, entry_type> myPair = ht->next_bucket_entry(bIndex);
			bIndex = myPair.first;
			entry = myPair.second;
		} else {
			entry = head;
		}
	} else {
		tree_node_ptr root = entry.as_tree_node_ptr();
		root = successor(root);
		if (nullptr == root) {
			std::pair<size_type, entry_type> myPair = ht->next_bucket_entry(bIndex);
			bIndex = myPair.first;
			entry = myPair.second;
		} else {
			entry = root;
		}
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
struct ht_iterator;

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
struct ht_const_iterator;

template<typename T, bool cache = false, typename Hash = hash_value<T>, 
	typename Comp = std::less<T>, typename Pred = std::equal_to<T>>
struct ht_iterator : public ht_iterator_base<T, cache, Hash, Comp, Pred> {
    using iterator_category = std::forward_iterator_tag;
	using difference_type   = ptrdiff_t;
	using value_type 		= T;
	using const_reference 	= const T&;   
    using self              = ht_iterator<T, cache, Hash, Comp, Pred>;
	using size_type			= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::size_type;
	using pointer 			= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::pointer;
	using reference	 		= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::reference;
	using container         = typename ht_iterator_base<T, cache, Hash, Comp, Pred>::container;
	using tree_node 		= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::tree_node;
	using list_node 		= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::list_node;
	using tree_node_ptr 	= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::tree_node_ptr;
	using list_node_ptr 	= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::list_node_ptr;
	using entry_type		= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::entry_type;

	ht_iterator() noexcept = default;
	ht_iterator(size_type bucketIndex,
			entry_type _entry, container* _ht) noexcept :
			ht_iterator_base<T, cache, Hash, Comp, Pred>(bucketIndex, _entry, _ht) {
	}
	ht_iterator(const ht_const_iterator<T, cache, Hash, Comp, Pred>& other) noexcept :
			ht_iterator_base<T, cache, Hash, Comp, Pred>(other.bIndex, other.entry, other.ht) {
	}
	ht_iterator(const ht_iterator<T, cache, Hash, Comp, Pred>& other) noexcept :
			ht_iterator_base<T, cache, Hash, Comp, Pred>(other.bIndex, other.entry, other.ht) {
	}

	self& operator=(const ht_const_iterator<T, cache, Hash, Comp, Pred>& other) noexcept {
		if (this != &other) {
			this->bIndex = other.bIndex;
			this->entry = other.entry;
			this->ht = other.ht;
		}

		return *this;
	}

	self& operator=(const ht_iterator<T, cache, Hash, Comp, Pred>& other) noexcept {
		if (this != &other) {
			this->bIndex = other.bIndex;
			this->entry = other.entry;
			this->ht = other.ht;
		}

		return *this;
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
};

template<typename T, bool cache = false, typename Hash = hash_value<T>, 
	typename Comp = std::less<T>, typename Pred = std::equal_to<T>>
struct ht_const_iterator : public ht_iterator_base<T, cache, Hash, Comp, Pred> {
    using iterator_category = std::forward_iterator_tag;
	using difference_type   = ptrdiff_t;
	using value_type 		= T;
	using const_reference 	= const T&;   
    using self              = ht_const_iterator<T, cache, Hash, Comp, Pred>;
	using size_type			= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::size_type;
	using pointer 			= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::pointer;
	using reference	 		= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::reference;
	using container         = typename ht_iterator_base<T, cache, Hash, Comp, Pred>::container;
	using tree_node 		= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::tree_node;
	using list_node 		= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::list_node;
	using tree_node_ptr 	= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::tree_node_ptr;
	using list_node_ptr 	= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::list_node_ptr;
	using entry_type		= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::entry_type;

	ht_const_iterator() noexcept = default;
	ht_const_iterator(size_type bucketIndex,
			entry_type _entry, container* _ht) noexcept :
			ht_iterator_base<T, cache, Hash, Comp, Pred>(bucketIndex, _entry, _ht) {
	}

	ht_const_iterator(const ht_iterator<T, cache, Hash, Comp, Pred>& other) noexcept :
			ht_iterator_base<T, cache, Hash, Comp, Pred>(other.bIndex, other.entry, other.ht) {
	}

	ht_const_iterator(const ht_const_iterator<T, cache, Hash, Comp, Pred>& other) noexcept :
			ht_iterator_base<T, cache, Hash, Comp, Pred>(other.bIndex, other.entry, other.ht) {
	}

	self& operator=(const ht_const_iterator<T, cache, Hash, Comp, Pred>& other) noexcept {
		if (this != &other) {
			this->bIndex = other.bIndex;
			this->entry = other.entry;
			this->ht = other.ht;
		}

		return *this;
	}

	self& operator=(const ht_iterator<T, cache, Hash, Comp, Pred>& other) noexcept {
		if (this != &other) {
			this->bIndex = other.bIndex;
			this->entry = other.entry;
			this->ht = other.ht;
		}

		return *this;
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
};

template<typename T, bool cache = false, typename Hash = hash_value<T>,
	typename Comp = std::less<T>, typename Pred = std::equal_to<T>>
class hash_table {
friend struct ht_iterator_base<T, cache, Hash, Comp, Pred>;
friend struct ht_iterator<T, cache, Hash, Comp, Pred>;
friend struct ht_const_iterator<T, cache, Hash, Comp, Pred>;
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
	using const_iterator            = ht_const_iterator<value_type>;
	using iterator                  = ht_iterator<value_type>;
	using reverse_iterator          = const std::reverse_iterator<iterator>;
	using const_reverse_iterator    = const std::reverse_iterator<const_iterator>;
	using hasher					= Hash;
	using key_compare				= Comp;
  	using key_equal 				= Pred;

public:
	iterator begin() noexcept { 
		std::pair<size_type, entry_type> myPair = beg();
		return iterator(myPair.first, myPair.second, this); 
	}
	const_iterator begin() const noexcept { 
		std::pair<size_type, entry_type> myPair = beg();
		return const_iterator(myPair.first, myPair.second, 
			const_cast<hash_table<T, cache, Hash, Comp, Pred>*>(this)); 
	}

	iterator end() noexcept { return iterator(bucket_count(), entry_type(), this); }
	const_iterator end() const noexcept { 
		return const_iterator(bucket_count(), entry_type(), 
			const_cast<hash_table<T, cache, Hash, Comp, Pred>*>(this)); 
	}
	const_iterator cbegin() const noexcept { return begin(); }
	const_iterator cend()   const noexcept { return end(); }

public:
	// 构造、复制、移动、析构函数
	explicit hash_table(size_type n = 16,
						const hasher& hf = hasher(),
						const key_compare& comp = key_compare(),
						const key_equal& eql = key_equal());

	template <std::input_iterator InputIter>
	hash_table(InputIter first, InputIter last,
				size_type n = 16,
				const hasher& hf = hasher(),
				const key_compare& comp = key_compare(),
				const key_equal& eql = key_equal());
	hash_table(const hash_table& other);
	hash_table(hash_table&& other) noexcept;
	hash_table& operator=(const hash_table& other);
	hash_table& operator=(hash_table&& other) noexcept;
	~hash_table();

public:
	bool empty() const noexcept { return 0 == m_size; }
	size_type size() const noexcept { return m_size; }

	// emplace
	template <typename... Args>
	iterator emplace_multi(Args&& ...args);

	template <typename... Args>
	std::pair<iterator, bool> emplace_unique(Args&&... args);

	template <typename... Args>
	iterator emplace_multi_use_hint(const_iterator hint, Args&&... args) { 
		static_cast<void>(hint);
		return emplace_multi(std::forward<Args>(args)...); 
	}

	template <typename... Args>
	iterator emplace_unique_use_hint(const_iterator hint, Args&& ...args) { 
		static_cast<void>(hint);
		return emplace_unique(std::forward<Args>(args)...).first; 
	}

	// insert
	iterator insert_multi_norehash(const value_type& value);
	iterator insert_multi_norehash(value_type&& value);
	std::pair<iterator, bool> insert_unique_norehash(const value_type& value);
	std::pair<iterator, bool> insert_unique_norehash(value_type&& value);

	iterator insert_multi(const value_type& value);
	iterator insert_multi(value_type&& value);

	std::pair<iterator, bool> insert_unique(const value_type& value);
	std::pair<iterator, bool> insert_unique(value_type&& value);

	// [note]: 同 emplace_hint
	iterator insert_multi_use_hint(const_iterator hint, const value_type& value) { 
		static_cast<void>(hint);
		return insert_multi(value); 
	}
	iterator insert_multi_use_hint(const_iterator hint, value_type&& value) { 
		static_cast<void>(hint);
		return insert_multi(std::move(value)); 
	}

	iterator insert_unique_use_hint(const_iterator hint, const value_type& value) { 
		static_cast<void>(hint);
		return insert_unique(value).first; 
	}
	iterator insert_unique_use_hint(const_iterator hint, value_type&& value) { 
		static_cast<void>(hint);
		return insert_unique(std::move(value)); 
	}

	template <std::input_iterator InputIter>
	void insert_multi(InputIter first, InputIter last);
	
	template <std::input_iterator InputIter>
	void insert_unique(InputIter first, InputIter last);

	// erase
	void erase(const_iterator position);
	void erase(const_iterator first, const_iterator last);

	size_type erase_multi(const key_type& key);
	size_type erase_unique(const key_type& key);
	void clear();
	void swap(hash_table& rhs) noexcept;

	// 查找相关操作
	size_type count_multi(const key_type& key) const;
	size_type count_unique(const key_type& key) const {
		return find(key) == end() ? 0 : 1;
	}

	iterator find(const key_type& key);
	const_iterator find(const key_type& key) const;
	std::pair<iterator, iterator> equal_range_multi(const key_type& key);
	std::pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const;
	std::pair<iterator, iterator> equal_range_unique(const key_type& key);
	std::pair<const_iterator, const_iterator> equal_range_unique(const key_type& key) const;

	size_type bucket_count() const noexcept { return m_bucket_count; }
	size_type bucket(const key_type& key);

	// hash policy
	float load_factor() const noexcept { 
		size_type nbuckets = bucket_count();
		return  nbuckets ? static_cast<float>(m_size) / nbuckets : 0.0f; 
	}
	void rehash(size_type n);
	void reserve(size_type n) {
		if (n > bucket_count()) {
			rehash(ceil_power_of_2(n));
		}
	}
	hasher hash_function() const { return m_hash; }
	key_equal key_eq() const { return m_equal; }
	float max_load_factor() const noexcept { return m_mlf; }
	void max_load_factor(float mlf) noexcept { m_mlf = mlf; }
#ifdef HASH_TABLE_DEBUG
	std::string show() {
		std::string str;
		for (size_t i = 0; i < m_bucket_count; ++i) {
			if (!m_buckets[i].isnull()) {
				str += "bucket" + std::to_string(i) +  " is";
				if (is_list(i)) {
					str += " list:";
					list_node_ptr head = m_buckets[i].as_list_node_ptr();
					while (head) {
						str += " ";
						str += std::to_string(head->value);
						head = static_cast<list_node_ptr>(head->next);
					}
				} else { 
					str += " tree:";
					tree_node_ptr root = m_buckets[i].as_tree_node_ptr();
					inorder(root, [&str, this](tree_node_base* node){
						str += " ";
						str += std::to_string(static_cast<tree_node_ptr>(node)->value);
					});
				}
				str += "\n";
			}
		}
		return str;
	}
#endif //HASH_TABLE_DEBUG
private:
	constexpr static size_type TREEFY_THRESHOLD = 8;
	constexpr static size_type UNTREEFY_THRESHOLD = 6;
	constexpr static float DEFAULT_MLF = 1.0f; ///< default max load factor

private:
	using tree_node 	= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::tree_node;
	using list_node 	= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::list_node;
	using tree_node_ptr = typename ht_iterator_base<T, cache, Hash, Comp, Pred>::tree_node_ptr;
	using list_node_ptr = typename ht_iterator_base<T, cache, Hash, Comp, Pred>::list_node_ptr;
	using entry_type	= typename ht_iterator_base<T, cache, Hash, Comp, Pred>::entry_type;
	using entry_ptr  	= entry_type*;

private:
	std::pair<size_type, entry_type> beg() const;

private:
	bool is_tree(size_type index) const {
#ifdef BIT64	
		return m_buckets[index].flag & mask;
#else
		return m_bitset[index];
#endif //BIT64
	}
	bool is_list(size_type index) const {
		return !is_tree(index);
	}
	tree_node_ptr listNode2TreeNode(list_node_ptr node);
	list_node_ptr treeNode2ListNode(tree_node_ptr node);
	size_type get_bucket_index(size_t hashVal) const { return hashVal & (m_bucket_count - 1); }
	size_type next_bucket(size_type index);
	void mark(size_type index) {
#ifdef BIT64
		m_buckets[index].flag |= mask;
#else 
		m_bitset[index] = 1;
#endif //BIT64
	}
	void unmark(size_type index) {
#ifdef BIT64
		m_buckets[index].flag &= (~mask);
#else
		m_bitset[index] = 0;
#endif //BIT64
	}
	void treefy(size_type index);
	void untreefy(size_type index);
	
private:
	entry_ptr allocate_entry(size_type n) {
		return new entry_type[n]();
	}
	void deallocate_entry(entry_ptr entry, size_type) {
		delete[] entry;
	}
	void copy_entry_unchecked(entry_ptr buckets, size_type n);
	entry_type bucket_entry(size_type index) const;
	std::pair<size_type, entry_type> next_bucket_entry(size_type index);
	template<typename... Args>
	list_node_ptr create_list_node_nohash(Args&&... args);
	template<typename... Args>
	tree_node_ptr create_tree_node_nohash(Args&&... args);
	template<typename... Args>
	list_node_ptr create_list_node(Args&&... args);
	template<typename... Args>
	tree_node_ptr create_tree_node(Args&&... args);
	void destroy_node(list_node_ptr node);
	void destroy_node(tree_node_ptr node);
	void destroy_tree(tree_node_ptr tree) { 
		clear_since(tree, [this](tree_node_base* node){
			this->destroy_node(static_cast<tree_node_ptr>(node));
		}); 
	}
	void destroy_list(list_node_ptr lst) { 
		clear_since(lst, [this](list_node_base* node){
			this->destroy_node(static_cast<list_node_ptr>(node));
		});
	}

private:
	iterator insert_list_node_multi(size_type index, list_node_ptr node);
	iterator insert_tree_node_multi(size_type index, tree_node_ptr node);
	iterator insert_node_multi(size_type index, list_node_ptr node);
	iterator insert_node_multi(size_type index, tree_node_ptr node);
	std::pair<iterator, bool> insert_list_node_unique(size_type index, list_node_ptr node);
	std::pair<iterator, bool> insert_tree_node_unique(size_type index, tree_node_ptr node);
	std::pair<iterator, bool> insert_node_unique(size_type index, list_node_ptr node);
	std::pair<iterator, bool> insert_node_unique(size_type index, tree_node_ptr node);
	void rehash_if(size_type n) {
		if (static_cast<float>(m_size + n) > 
			static_cast<float>(m_bucket_count) * max_load_factor()) {
			rehash(m_size + n);
		}
	}

private:
	size_type m_bucket_count;	///< 桶个数
	entry_ptr m_buckets;		///< 桶数组
	size_type m_size;			///< 有效保存了值节点个数
#ifdef BIT32
	dynamic_bitset m_bitset;	///< 区分链表还是树的标志
#endif //BIT64
	float m_mlf;				///< max load factor
	const hasher& m_hash;		///< hash
	const key_compare& m_comp;	///< compare
	const key_equal& m_equal;	///< equal
};

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::size_type, 
		typename hash_table<T, cache, Hash, Comp, Pred>::entry_type> 
hash_table<T, cache, Hash, Comp, Pred>::beg() const {
	size_type index = 0;
	for (; index != m_bucket_count; ++index) {
		if (!m_buckets[index].isnull()) { //也可以判断tree_node
			break;
		}
	}
	entry_type entry = bucket_entry(index);

	return { index, entry };
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::tree_node_ptr 
hash_table<T, cache, Hash, Comp, Pred>::listNode2TreeNode(list_node_ptr node) {
	tree_node_ptr tnode = static_cast<tree_node_ptr>(::operator new(sizeof(tree_node)));
	construct(&tnode->value, std::move(node->value));
	if constexpr(cache) {
		tnode->hash_val = node->hash_val;
	}
	tnode->left = tnode->right = tnode->parent = nullptr;
	tnode->color = NodeColor::RED;
	destroy_node(node);
	return tnode;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::list_node_ptr 
hash_table<T, cache, Hash, Comp, Pred>::treeNode2ListNode(tree_node_ptr node) {
	list_node_ptr lnode = static_cast<list_node_ptr>(::operator new(sizeof(list_node)));
	construct(&lnode->value, std::move(node->value));
	if constexpr(cache) {
		lnode->hash_val = node->hash_val;
	}
	lnode->next = nullptr;
	destroy_node(node);
	return lnode;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::size_type 
hash_table<T, cache, Hash, Comp, Pred>::next_bucket(size_type index) {
	size_type nextIndex = index + 1;
	for (; nextIndex != m_bucket_count; ++nextIndex) {
		if (m_buckets[nextIndex].list_node) { //也可以判断tree_node
			break;
		}
	}

	return nextIndex;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
void hash_table<T, cache, Hash, Comp, Pred>::treefy(size_type index) {
	if (is_tree(index)) {
		return;
	}

	list_node_ptr head = m_buckets[index].as_list_node_ptr();
	tree_node_ptr root = nullptr;
	while (head->next) {
		list_node_ptr lnode = list_unlink_after(head);
		tree_node_ptr tnode = listNode2TreeNode(lnode);
		ht_tree_insert_node_multi(tnode, &root, m_comp);
	}
	list_node_ptr lnode = head;
	tree_node_ptr tnode = listNode2TreeNode(lnode);
	ht_tree_insert_node_multi(tnode, &root, m_comp);
	m_buckets[index] = root;
	mark(index);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
void hash_table<T, cache, Hash, Comp, Pred>::untreefy(size_type index) {
	if (is_list(m_buckets[index])) {
		return;
	}

	tree_node_ptr root = m_buckets[index].as_tree_node_ptr();
	list_node_ptr head = nullptr;
	postorder(root, [this, &head](tree_node_base* node) {
		tree_node_base* parent = parent_of(node);
		if (parent) {
			if (left_of(parent) == node) {
				parent->left = nullptr;
			} else {
				parent->right = nullptr;
			}
			node->parent = nullptr;
		}
		tree_node_ptr tnode = node;
		list_node_ptr lnode = treeNode2ListNode(tnode);
		list_link_after(lnode, &head);
	});
	m_buckets[index] = head;
	unmark(index);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
void hash_table<T, cache, Hash, Comp, Pred>::copy_entry_unchecked(entry_ptr buckets, size_type n) {
	for (size_type i = 0; i != n; ++i) {
		if (!buckets[i].isnull()) {
			if (is_tree(i)) {
				tree_node_base* newTree = copy_since(buckets[i].as_tree_node_ptr(), 
					[this](tree_node_base* node){
						tree_node_ptr tnode = static_cast<tree_node_ptr>(node);
						return create_tree_node(tnode->value);
				});
				m_buckets[i] = static_cast<tree_node_ptr>(newTree);
			} else {
				list_node_base* newList = copy_since(buckets[i].as_list_node_ptr(), 
					[this](list_node_base* node){
						list_node_ptr lnode = static_cast<list_node_ptr>(node);
						return create_list_node(lnode->value);
				});
				m_buckets[i] = static_cast<list_node_ptr>(newList);
			}
		}
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::entry_type 
hash_table<T, cache, Hash, Comp, Pred>::bucket_entry(size_type index) const {
	if (index != m_bucket_count) {
		if (is_list(index)) {
			return m_buckets[index];
		} else {
			return min_node(m_buckets[index].as_tree_node_ptr());
		}
	} 
	return entry_type();
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::size_type,
		typename hash_table<T, cache, Hash, Comp, Pred>::entry_type>
hash_table<T, cache, Hash, Comp, Pred>::next_bucket_entry(size_type index) {
	size_type nextIndex = next_bucket(index);
	return { nextIndex, bucket_entry(nextIndex) };
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
template<typename... Args>
typename hash_table<T, cache, Hash, Comp, Pred>::list_node_ptr 
hash_table<T, cache, Hash, Comp, Pred>::create_list_node_nohash(Args&&... args) {
	list_node_ptr newNode = static_cast<list_node_ptr>(::operator new(sizeof(list_node)));
	try {
		construct(&newNode->value, std::forward<Args>(args)...);
	} catch (...) {
		::operator delete(newNode);
		throw;
	}
	newNode->next = nullptr;

	return newNode;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
template<typename... Args>
typename hash_table<T, cache, Hash, Comp, Pred>::tree_node_ptr 
hash_table<T, cache, Hash, Comp, Pred>::create_tree_node_nohash(Args&&... args) {
	tree_node_ptr newNode = static_cast<tree_node_ptr>(::operator new(sizeof(tree_node)));
	try {
		construct(&newNode->value, std::forward<Args>(args)...);
	} catch (...) {
		::operator delete(newNode);
		throw;
	}
	newNode->left = newNode->right = newNode->parent = nullptr;
	newNode->color = NodeColor::RED;

	return newNode;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
template<typename... Args>
typename hash_table<T, cache, Hash, Comp, Pred>::list_node_ptr
hash_table<T, cache, Hash, Comp, Pred>::create_list_node(Args&&... args) {
	list_node_ptr newNode = create_list_node_nohash(std::forward<Args>(args)...);
	if constexpr(cache) {
		newNode->hash_val = m_hash(newNode->value);
	}

	return newNode;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
template<typename... Args>
typename hash_table<T, cache, Hash, Comp, Pred>::tree_node_ptr
hash_table<T, cache, Hash, Comp, Pred>::create_tree_node(Args&&... args) {
	tree_node_ptr newNode = create_tree_node_nohash(std::forward<Args>(args)...);
	if constexpr(cache) {
		newNode->hash_val = m_hash(newNode->value);
	}
	return newNode;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
void hash_table<T, cache, Hash, Comp, Pred>::destroy_node(list_node_ptr node) {
	destroy(&node->value);
	::operator delete(node);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
void hash_table<T, cache, Hash, Comp, Pred>::destroy_node(tree_node_ptr node) {
	destroy(&node->value);
	::operator delete(node);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::iterator 
hash_table<T, cache, Hash, Comp, Pred>::insert_list_node_multi(size_type index, list_node_ptr node) {
	if (m_buckets[index].isnull()) {
		m_buckets[index] = node;
		++m_size;
		return iterator(index, m_buckets[index], this);
	}

	list_node_ptr head = m_buckets[index].as_list_node_ptr();
	list_node_ptr last = head;
	size_type nodeCount = 1;
	//找到是否有相等的
	while (last) {
		if (m_equal(last->value, node->value)) {
			break;
		}
		last = next_of(last);
		++nodeCount;
	}
	
	if (last) {
		//如果为true，证明有相等的
		//那就直接插入到相等元素的后面
		list_link_after(node, &last);
	} else {
		//如果为false，证明没有相等的，那就插入到链表头部
		list_link_after(node, &head);
		m_buckets[index] = head;
	}
	while (last) {
		//继续统计有多少个相等的元素
		last = next_of(last);
		++nodeCount;
	}
	if (nodeCount >= TREEFY_THRESHOLD) {
		//达到条件转为树
		treefy(index);
	}
	
	++m_size;
	return iterator(index, node, this);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::iterator 
hash_table<T, cache, Hash, Comp, Pred>::insert_tree_node_multi(size_type index, tree_node_ptr node) {
	//不可能为空
	tree_node_ptr root = m_buckets[index].as_tree_node_ptr();
	ht_tree_insert_node_multi(node, &root, m_comp);
	m_buckets[index] = root;
	++m_size;
	return iterator(index, node, this);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::iterator 
hash_table<T, cache, Hash, Comp, Pred>::insert_node_multi(size_type index, list_node_ptr node) {
	if (is_list(index)) {
		return insert_list_node_multi(index, node);
	}
	tree_node_ptr tnode = listNode2TreeNode(node);
	return insert_tree_node_multi(index, tnode);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::iterator 
hash_table<T, cache, Hash, Comp, Pred>::insert_node_multi(size_type index, tree_node_ptr node) {
	if (is_list(index)) {
		list_node_ptr lnode = treeNode2ListNode(node);
		return insert_list_node_multi(index, lnode);
	}
	return insert_tree_node_multi(index, node);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Comp, Pred>::insert_list_node_unique(size_type index, list_node_ptr node) {
	if (m_buckets[index].isnull()) {
		m_buckets[index] = node;
		++m_size;
		return { iterator(index, node, this), true };
	}

	list_node_ptr head = m_buckets[index].as_list_node_ptr();
	list_node_ptr last = head;
	size_type nodeCount = 1;
	while (last) {
		if (m_equal(last->value, node->value)) {
			destroy_node(node);
			return { iterator(index, last, this), false };
		}
		last = next_of(last);
		++nodeCount;
	}
	list_link_after(node, &head);
	m_buckets[index] = head;
	while (last) {
		++nodeCount;
		last = next_of(last);
	}
	if (nodeCount >= TREEFY_THRESHOLD) {
		treefy(index);
	}
	++m_size;
	return { iterator(index, node, this), true };	
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Comp, Pred>::insert_tree_node_unique(size_type index, tree_node_ptr node) {
	//不可能为空
	tree_node_ptr root = m_buckets[index].as_tree_node_ptr();
	std::pair<tree_node_ptr, bool> myPair = ht_tree_insert_node_unique(node, &root, m_comp);
	if (!myPair.second) {
		destroy_node(node);
	} else {
		++m_size;
	}

	return { iterator(index, node, this), myPair.second };
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Comp, Pred>::insert_node_unique(size_type index, tree_node_ptr node) {
	if (is_list(index)) {
		list_node_ptr lnode = treeNode2ListNode(node);
		return insert_list_node_unique(index, lnode);	
	} else {
		return insert_tree_node_unique(index, node);
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Comp, Pred>::insert_node_unique(size_type index, list_node_ptr node) {
	if (is_list(index)) {
		return insert_list_node_unique(index, node);
	} else {
		tree_node_ptr tnode = listNode2TreeNode(node);
		return insert_tree_node_unique(index, tnode);
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
hash_table<T, cache, Hash, Comp, Pred>::hash_table(size_type n,
		const hasher& hf, const key_compare& comp, const key_equal& eql) :
	m_bucket_count(ceil_power_of_2(n)),
	m_buckets(allocate_entry(m_bucket_count)),
	m_size(0),
#ifdef BIT32
	m_bitset(m_bucket_count),
#endif //BIT32
	m_mlf(DEFAULT_MLF),
	m_hash(hf),
	m_comp(comp),
	m_equal(eql) {
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
template <std::input_iterator InputIter>
hash_table<T, cache, Hash, Comp, Pred>::hash_table(InputIter first, InputIter last,
		size_type n, const hasher& hf, const key_compare& comp, const key_equal& eql) :
		hash_table(n, hf, comp, eql) {
	for (; first != last; ++first) {
		emplace_multi(*first);
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
hash_table<T, cache, Hash, Comp, Pred>::hash_table(const hash_table& other) :
		m_bucket_count(other.m_bucket_count),
		m_buckets(allocate_entry(m_bucket_count)),
		m_size(other.m_size),
#ifdef BIT32
		m_bitset(other.m_bitset),
#endif //BIT32
		m_mlf(other.m_mlf),
		m_hash(other.m_hash),
		m_comp(other.m_comp),
		m_equal(other.m_equal) {
	copy_entry_unchecked(other.m_buckets, other.m_bucket_count);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
hash_table<T, cache, Hash, Comp, Pred>::hash_table(hash_table&& other) noexcept :
		m_bucket_count(other.m_bucket_count),
		m_buckets(other.m_buckets),
		m_size(other.m_size),
#ifdef BIT32
		m_bitset(std::move(other.m_bitset)),
#endif //BIT32
		m_mlf(other.m_mlf),
		m_hash(other.m_hash),
		m_comp(other.m_comp),
		m_equal(other.m_equal) {
	other.m_bucket_count = 0;
	other.m_buckets = nullptr;
	other.m_size = 0;
	other.m_mlf = DEFAULT_MLF;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
hash_table<T, cache, Hash, Comp, Pred>&
hash_table<T, cache, Hash, Comp, Pred>::operator=(const hash_table& other) {
	if (this != &other) {
		clear();
		if (m_bucket_count < other.m_bucket_count) {
			deallocate_entry(m_buckets, m_bucket_count);
			m_buckets = allocate_entry(other.m_bucket_count);
		}
		copy_entry_unchecked(other.m_buckets, other.m_bucket_count);
		m_bucket_count = other.m_bucket_count;
		m_size = other.m_size;
		m_mlf = other.m_mlf;
		m_hash = other.m_hash;
		m_comp = other.m_comp;
		m_equal = other.m_equal;
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
hash_table<T, cache, Hash, Comp, Pred>& 
hash_table<T, cache, Hash, Comp, Pred>::operator=(hash_table&& other) noexcept {
	if (this != &other) {
		clear();
		deallocate_entry(m_buckets, m_bucket_count);
		m_bucket_count = other.m_bucket_count;
		m_buckets = other.m_buckets;
		m_size = other.m_size;
		m_mlf = other.m_mlf;
		m_hash = other.m_hash;
		m_comp = other.m_comp;
		m_equal = other.m_equal;
		other.m_bucket_count = 0;
		other.m_buckets = nullptr;
		other.m_size = 0;
		other.m_mlf = DEFAULT_MLF;
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
hash_table<T, cache, Hash, Comp, Pred>::~hash_table() {
	//clear();
	deallocate_entry(m_buckets, m_bucket_count);
}

// emplace
template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
template <typename... Args>
typename hash_table<T, cache, Hash, Comp, Pred>::iterator 
hash_table<T, cache, Hash, Comp, Pred>::emplace_multi(Args&& ...args) {
	rehash_if(1);
	//就像cpu里流水线一样，预测分支
	//这里我们预测为插入链表节点
	list_node_ptr newNode = create_list_node(std::forward<Args>(args)...);
	size_t hashVal = 0;
	if constexpr(cache) {
		hashVal = newNode->hashVal;
	} else {
		hashVal = m_hash(newNode->value);
	}
	size_type index = get_bucket_index(hashVal);
	return insert_node_multi(index, newNode);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
template <typename... Args>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Comp, Pred>::emplace_unique(Args&& ...args) {
	rehash_if(1);
	list_node_ptr newNode = create_list_node(std::forward<Args>(args)...);
	size_t hashVal = 0;
	if constexpr(cache) {
		hashVal = newNode->hash_val;
	} else {
		hashVal = m_hash(newNode->value);
	}
	size_type index = get_bucket_index(hashVal); 
	return insert_node_unique(index, newNode);
}

// insert
template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::iterator 
hash_table<T, cache, Hash, Comp, Pred>::insert_multi_norehash(const value_type& value) {
	size_t hashVal = m_hash(value);
	size_type index = get_bucket_index(hashVal);
	if (is_list(index)) {
		list_node_ptr newNode = create_list_node_nohash(value);
		if constexpr(cache) {
			newNode->hash_val = hashVal;
		}
		return insert_list_node_multi(index, newNode);
	} else {
		tree_node_ptr newNode = create_tree_node_nohash(value);
		if constexpr(cache) {
			newNode->hash_val = hashVal;
		}
		return insert_tree_node_multi(index, newNode);
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::iterator 
hash_table<T, cache, Hash, Comp, Pred>::insert_multi_norehash(value_type&& value) {
	size_t hashVal = m_hash(value);
	size_type index = get_bucket_index(hashVal);
	if (is_list(index)) {
		list_node_ptr newNode = create_list_node_nohash(std::move(value));
		if constexpr(cache) {
			newNode->hash_val = hashVal;
		}
		return insert_list_node_multi(index, newNode);
	} else {
		tree_node_ptr newNode = create_tree_node_nohash(std::move(value));
		if constexpr(cache) {
			newNode->hash_val = hashVal;
		}
		return insert_tree_node_multi(index, newNode);
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Comp, Pred>::insert_unique_norehash(const value_type& value) {
	size_t hashVal = m_hash(value);
	size_type index = get_bucket_index(hashVal);
	if (is_list(index)) {
		list_node_ptr newNode = create_list_node_nohash(value);
		if constexpr(cache) {
			newNode->hash_val = hashVal;
		}
		return insert_list_node_unique(index, newNode);
	} else {
		tree_node_ptr newNode = create_tree_node_nohash(value);
		if constexpr(cache) {
			newNode->hash_val = hashVal;
		}
		return insert_tree_node_unique(index, newNode);
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Comp, Pred>::insert_unique_norehash(value_type&& value) {
	size_t hashVal = m_hash(value);
	size_type index = get_bucket_index(hashVal);
	if (is_list(index)) {
		list_node_ptr newNode = create_list_node_nohash(std::move(value));
		if constexpr(cache) {
			newNode->hash_val = hashVal;
		}
		return insert_list_node_unique(index, newNode);
	} else {
		tree_node_ptr newNode = create_tree_node_nohash(std::move(value));
		if constexpr(cache) {
			newNode->hash_val = hashVal;
		}
		return insert_tree_node_unique(index, newNode);
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::iterator 
hash_table<T, cache, Hash, Comp, Pred>::insert_multi(const value_type& value) {
	rehash_if(1);
	return insert_multi_norehash(value);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::iterator 
hash_table<T, cache, Hash, Comp, Pred>::insert_multi(value_type&& value) {
	rehash_if(1);
	return insert_multi_norehash(std::move(value));
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Comp, Pred>::insert_unique(const value_type& value) {
	rehash_if(1);
	return insert_unique_norehash(value);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Comp, Pred>::insert_unique(value_type&& value) {
	rehash_if(1);
	return insert_unique_norehash(std::move(value));
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
template <std::input_iterator InputIter>
void hash_table<T, cache, Hash, Comp, Pred>::insert_multi(InputIter first, InputIter last) {
	for (; first != last; ++first) {
		insert_multi(*first);
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
template <std::input_iterator InputIter>
void hash_table<T, cache, Hash, Comp, Pred>::insert_unique(InputIter first, InputIter last) {
	for (; first != last; ++first) {
		insert_unique(*first);
	}
}

// erase
template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
void hash_table<T, cache, Hash, Comp, Pred>::erase(const_iterator position) {
	if (end() == position) {
		return;
	}

	size_type index = position.bIndex;
	entry_type entry = position.entry;
	if (!entry.isnull()) {
		if (is_list(index)) {
			list_node_ptr head = m_buckets[index].as_list_node_ptr();
			list_node_ptr node = position.entry.as_list_node_ptr(); //可以直接position.entry.list_node
			if (head != node) {
				list_node_ptr prev = nullptr;
				while (head && head != node) {
					prev = head;
					head = next_of(head);
				}
				//assert next_of(head) == node
				list_unlink_after(prev);
			} else {
				m_buckets[index] = next_of(head);
			}
			destroy_node(node);
		} else {
			tree_node_ptr root = m_buckets[index].as_tree_node_ptr();
			tree_node_ptr node = position.entry.as_tree_node_ptr(); //可以直接position.entry.tree_node
			ht_tree_erase_node(node, &root);
			m_buckets[index] = root;
			destroy_node(node);
		}
		--m_size;
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
void hash_table<T, cache, Hash, Comp, Pred>::erase(const_iterator first, const_iterator last) {
	for (; first != last; ++first) {
		erase(first);
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::size_type 
hash_table<T, cache, Hash, Comp, Pred>::erase_multi(const key_type& key) {
	size_type n = 0;
	while (true) {
		iterator iter = find(key);
		if (iter != end()) {
			++n;
			erase(iter);
			continue;
		}
		break;
	}

	return n;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::size_type 
hash_table<T, cache, Hash, Comp, Pred>::erase_unique(const key_type& key) {
	iterator iter = find(key);
	if (iter != end()) {
		erase(iter);
		return 1;
	}

	return 0;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
void hash_table<T, cache, Hash, Comp, Pred>::clear() {
	for (size_type i = 0; i != m_bucket_count; ++i) {
		if (is_tree(i)) {
			clear_since(m_buckets[i].as_tree_node_ptr(), [this](tree_node_base* node){
				destroy_node(static_cast<tree_node_ptr>(node));
			});
		} else {
			clear_since(m_buckets[i].as_list_node_ptr(), [this](list_node_base* node){
				destroy_node(static_cast<list_node_ptr>(node));
			});
		}
	}
	m_size = 0;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
void hash_table<T, cache, Hash, Comp, Pred>::swap(hash_table& other) noexcept {
	std::swap(m_buckets, other.m_buckets);
	std::swap(m_bucket_count, other.m_bucket_count);
#ifdef BIT32
	std::swap(m_bitset, other.m_bitset);
#endif //BIT32
	std::swap(m_size, other.m_size);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::size_type
hash_table<T, cache, Hash, Comp, Pred>::count_multi(const key_type& key) const {
	size_t hashVal = m_hash(key);
	size_type index = get_bucket_index(hashVal);
	size_type n = 0;

	if (is_list(index)) {
		list_node_ptr head = m_buckets[index].as_list_node_ptr();
		while (head) {
			if (m_equal(head->value, key)) {
				++n;
				break;
			}
			head = next_of(head);
		}
		if (head) {
			head = next_of(head);
			while (head && m_equal(head->value, key)) {
				++n;
				head = next_of(head);	
			}
		}
	} else {
		tree_node_ptr root = m_buckets[index].as_tree_node_ptr();
		n = ht_tree_count_multi<T, cache, size_type, Comp>(key, root, m_comp);
	}

	return n;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::iterator 
hash_table<T, cache, Hash, Comp, Pred>::find(const key_type& key) {
	size_t hashVal = m_hash(key);
	size_type index = get_bucket_index(hashVal);
	if (is_list(index)) {
		forward_list_node<T>* fnode = list_find_first_of(m_buckets[index].as_list_node_ptr(), key, m_equal);
		list_node_ptr node = static_cast<list_node_ptr>(fnode);
		return iterator(index, node, this);
	} else {
		tree_node_ptr root = m_buckets[index].as_tree_node_ptr();
		tree_node_ptr node = ht_tree_lbound(key, root, m_comp); //node->value >= key
		if (m_comp(key, node->value)) { //not equal
			node = nullptr;
		} 
		return iterator(index, node, this);
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::const_iterator 
hash_table<T, cache, Hash, Comp, Pred>::find(const key_type& key) const {
	size_t hashVal = m_hash(key);
	size_type index = get_bucket_index(hashVal);
	if (is_list(index)) {
		list_node_ptr node = list_find_first_of(m_buckets[index].as_list_node_ptr(), key, m_equal);
		return const_iterator(index, node, this);
	} else {
		tree_node_ptr root = m_buckets[index].as_tree_node_ptr();
		tree_node_ptr node = ht_tree_lbound(key, root, m_comp); //node->value >= key
		if (m_comp(key, node->value)) { //not equal
			node = nullptr;
		} 
		return const_iterator(index, node, this);
	}
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::iterator, 
		typename hash_table<T, cache, Hash, Comp, Pred>::iterator> 
hash_table<T, cache, Hash, Comp, Pred>::equal_range_multi(const key_type& key) {
	size_t hashVal = m_hash(key);
	size_type index = get_bucket_index(hashVal);
	if (is_list(index)) {
		list_node_ptr head = m_buckets[index].as_list_node_ptr();
		while (head) {
			if (m_equal(key, head->value)) {
				list_node_ptr last = next_of(head);
				while (last) {
					if (!m_equal(key, last->value)) { //找到不相等的位置
						return { iterator(index, head, this), iterator(index, last, this) };
					}
				}
				//剩余整条链表都和key相等, 找到下一个entry
				size_type nextIndex = next_bucket(index);
				entry_type nextEntry = bucket_entry(nextIndex);
				//没有后继了
				return { iterator(index, head, this), 
						iterator(nextIndex, nextEntry, this) };
			}
			head = next_of(head);
		}
	} else {
		tree_node_ptr root = m_buckets[index].as_tree_node_ptr();
		std::pair<tree_node_ptr, tree_node_ptr> myPair = ht_tree_equal_range_multi(key, root, m_comp);
		if (myPair.first) {
			if (myPair.second) {
				return { iterator(index, myPair.first, this),
						iterator(index, myPair.second, this) };
			} else {
				return { iterator(index, myPair.first, this), end() };
			}
		}
	}

	//没有相等
	return { end(), end() }; 
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::const_iterator, 
		typename hash_table<T, cache, Hash, Comp, Pred>::const_iterator> 
hash_table<T, cache, Hash, Comp, Pred>::equal_range_multi(const key_type& key) const {
	size_t hashVal = m_hash(key);
	size_type index = get_bucket_index(hashVal);
	if (is_list(index)) {
		list_node_ptr head = m_buckets[index].as_list_node_ptr();
		while (head) {
			if (m_equal(key, head->value)) {
				list_node_ptr last = next_of(head);
				while (last) {
					if (!m_equal(key, last->value)) { //找到不相等的位置
						return { const_iterator(index, head, this), 
								const_iterator(index, last, this) };
					}
				}
				//剩余整条链表都和key相等, 找到下一个entry
				size_type nextIndex = next_bucket(index);
				entry_type nextEntry = bucket_entry(nextIndex);
				//没有后继了
				return { const_iterator(index, head, this), 
						const_iterator(nextIndex, nextEntry, this) };
			}
			head = next_of(head);
		}
	} else {
		tree_node_ptr root = m_buckets[index].as_tree_node_ptr();
		std::pair<tree_node_ptr, tree_node_ptr> myPair = ht_tree_equal_range_multi(key, root, m_comp);
		if (myPair.first) {
			if (myPair.second) {
				return { const_iterator(index, myPair.first, this),
						const_iterator(index, myPair.second, this) };
			} else {
				return { const_iterator(index, myPair.first, this), end() };
			}
		}
	}

	//没有相等
	return { end(), end() };  
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::iterator, 
		typename hash_table<T, cache, Hash, Comp, Pred>::iterator> 
hash_table<T, cache, Hash, Comp, Pred>::equal_range_unique(const key_type& key) {
	iterator iter = lower_bound(key);
	iterator nextIter = iter;
	if (iter != end()) {
		++nextIter;
	}

	return { iter, nextIter };
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Comp, Pred>::const_iterator, 
		typename hash_table<T, cache, Hash, Comp, Pred>::const_iterator> 
hash_table<T, cache, Hash, Comp, Pred>::equal_range_unique(const key_type& key) const {
	const_iterator iter = lower_bound(key);
	const_iterator nextIter = iter;
	if (iter != end()) {
		++nextIter;
	}

	return { iter, nextIter };
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
typename hash_table<T, cache, Hash, Comp, Pred>::size_type 
hash_table<T, cache, Hash, Comp, Pred>::bucket(const key_type& key) {
	size_type hashVal = m_hash(key);
	return get_bucket_index(hashVal);
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
void hash_table<T, cache, Hash, Comp, Pred>::rehash(size_type count) {
	hash_table<T, cache, Hash, Comp, Pred> other(count, m_hash, m_comp, m_equal);
	if (other.bucket_count() <= bucket_count()) {
		return;
	}

	other.max_load_factor(m_mlf);

	for (size_type i = 0; i != m_bucket_count; ++i) {
		if (!m_buckets[i].isnull()) {
			if (is_list(i)) {
				list_node_ptr head = m_buckets[i].as_list_node_ptr();
				size_t hashVal = 0;
				size_type index = 0;

				while (next_of(head)) {
					list_node_ptr node = list_unlink_after(head);
					if constexpr(cache) {
						hashVal = node->hash_val;
					} else {
						hashVal = m_hash(node->value);
					}
					index = other.get_bucket_index(hashVal);
					//这里不用担心unique的情况
					//因为如果为unique，在rehash之前能保证容器里元素唯一
					//insert_multi也没关系
					other.insert_node_multi(index, node);
				}
				
				if constexpr(cache) {
					hashVal = head->hash_val;
				} else {
					hashVal = m_hash(head->value);
				}
				index = other.get_bucket_index(hashVal);
				other.insert_node_multi(index, head);
			} else {
				tree_node_ptr root = m_buckets[i].as_tree_node_ptr();
				postorder(root, [this, &other](tree_node_base* node) {
					tree_node_base* parent = parent_of(node);
					if (parent) {
						if (node == parent->left) {
							parent->left = nullptr;
						} else {
							parent->right = nullptr;
						}
						node->parent = nullptr;
					}
					tree_node_ptr node1 = static_cast<tree_node_ptr>(node);
					size_t hashVal = 0;
					if constexpr(cache) {
						hashVal = node1->hash_val;
					} else {
						hashVal = m_hash(node1->value);
					}
					size_type index = other.get_bucket_index(hashVal);
					other.insert_node_multi(index, node1);
				});
			}
		}
	}

	deallocate_entry(m_buckets, m_bucket_count);
	m_buckets = other.m_buckets;
	m_bucket_count = other.m_bucket_count;
#ifdef BIT32
	m_bitset = std::move(other.m_bitset);
#endif //BIT32
	other.m_buckets = nullptr;
	other.m_bucket_count = 0;
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
bool operator==(const hash_table<T, cache, Hash, Comp, Pred>& lhs,
		const hash_table<T, cache, Hash, Comp, Pred>& rhs) {
	if (lhs.size() != rhs.size()) {
		return false;
	}
	return std::equal(lhs.begin(), rhs.begin(), rhs.end());
}

template<typename T, bool cache, typename Hash, typename Comp, typename Pred>
bool operator!=(const hash_table<T, cache, Hash, Comp, Pred>& lhs,
		const hash_table<T, cache, Hash, Comp, Pred>& rhs) {
	return !(lhs == rhs);
}

} //namespace nano