#pragma once

#include "node.h"
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
struct ht_entry {
	//可以考虑使用variant.
	union {
		ht_list_node<T, cache>* list_node;	//也可以考虑用数组
		ht_tree_node<T, cache>* tree_node;	
		uint64_t flag;						//list or tree
	};
};
#else
/**
 * @brief 32位Linux地址所有位都有效，这样就不能用最高位来区分
 * 		  链表和红黑树，只能自己使用额外的一个bitset来标记
 * @tparam T 
 */
template<typename T, bool cache = false>
struct ht_entry {
	union  {
		ht_list_node<T, cache>* list_node;	//也可以考虑用数组
		ht_tree_node<T, cache>* tree_node;
	};
};
#endif //BIT64

template<typename T, bool cache>
inline ht_list_node<T>* as_list_ptr(ht_entry<T, cache> entry) {
#ifdef BIT64
	return reinterpret_cast<ht_list_node<T>*>(entry.list_node & !(mask));
#else
	return entry.list_node;
#endif //BIT64
}

template<typename T, bool cache>
inline ht_tree_node<T>* as_tree_ptr(ht_entry<T, cache> entry) {
#ifdef BIT64
	return reinterpret_cast<ht_tree_node<T>*>(entry.tree_node & !(mask));
#else
	return entry.tree_node;
#endif //BIT64
}

template<typename T, bool cache, typename Hash, typename Pred>
class hash_table;

template<typename T, bool cache = false, typename Hash = hash_value<T>, typename Pred = std::equal_to<T>>
struct ht_iterator_base : public std::iterator<std::forward_iterator_tag, T> {  
	using difference_type   = ptrdiff_t;
	using value_type 		= T;
	using reference			= T&; 
	using pointer 			= T*;
	using size_type 		= size_t;
	using self              = ht_iterator_base<T, cache, Hash, Pred>;
    using container         = hash_table<T, cache, Hash, Pred>;
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
		if (ht->is_list_node(bIndex)) {
			return entry.tree_node->value;
		} else {
			return entry.list_node->value;
		}
	}
	pointer operator->() const noexcept { return &(operator*());}

	size_type bIndex;	//bucket index
    entry_type entry;  	//桶内部节点
	container* ht;      //哈希表
};

template<typename T, bool cache, typename Hash, typename Pred>
void ht_iterator_base<T, cache, Hash, Pred>::increase() noexcept {
	if (ht->is_tree(bIndex)) {
		entry.tree_node = static_cast<tree_node_ptr>(successor(as_tree_ptr(entry)));
		if (nullptr == as_tree_ptr(entry)) {
			while (bIndex != ht->bucket_count() && 
					nullptr == ht->m_buckets[bIndex]) {
				++bIndex;
			}
			if (bIndex != ht->bucket_count()) {
				entry.tree_node = as_tree_ptr(ht->m_buckets[bIndex]);
			}
		}	
	} else {
		entry.list_node = static_cast<list_node_ptr>(as_list_ptr(entry)->next);
		if (nullptr == as_list_node(entry)) {
			while (bIndex != ht->bucket_count() && 
					nullptr == ht->m_buckets[bIndex]) {
				++bIndex;
			}
			if (bIndex != ht->bucket_count()) {
				entry.tree_node = as_list_ptr(ht->m_buckets[bIndex]);
			}
		}
	}
}

template<typename T, bool cache, typename Hash, typename Pred>
struct ht_iterator;

template<typename T, bool cache, typename Hash, typename Pred>
struct ht_const_iterator;

template<typename T, bool cache = false, typename Hash = hash_value<T>, typename Pred = std::equal_to<T>>
struct ht_iterator : public ht_iterator_base<T, cache, Hash, Pred> {
    using iterator_category = std::forward_iterator_tag;
	using difference_type   = ptrdiff_t;
	using value_type 		= T;
	using const_reference 	= T&;   
    using self              = ht_iterator<T, cache, Hash, Pred>;
	using size_type			= typename ht_iterator_base<T, cache, Hash, Pred>::size_type;
	using pointer 			= typename ht_iterator_base<T, cache, Hash, Pred>::pointer;
	using reference	 		= typename ht_iterator_base<T, cache, Hash, Pred>::reference;
	using container         = typename ht_iterator_base<T, cache, Hash, Pred>::container;
	using tree_node 		= typename ht_iterator_base<T, cache, Hash, Pred>::tree_node;
	using list_node 		= typename ht_iterator_base<T, cache, Hash, Pred>::list_node;
	using tree_node_ptr 	= typename ht_iterator_base<T, cache, Hash, Pred>::tree_node_ptr;
	using list_node_ptr 	= typename ht_iterator_base<T, cache, Hash, Pred>::list_node_ptr;
	using entry_type		= typename ht_iterator_base<T, cache, Hash, Pred>::entry_type;

	ht_iterator() noexcept = default;
	ht_iterator(size_type bucketIndex,
			entry_type _entry, container* _ht) noexcept :
			ht_iterator_base<T, cache, Hash, Pred>(bucketIndex, _entry, _ht) {
	}

	ht_iterator(const ht_const_iterator<T, cache, Hash, Pred>& other) :
			ht_iterator_base<T, cache, Hash, Pred>(other.bIndex, other.entry, other.ht) {
	}

    self& operator++() noexcept {
        this->increase();
		return *this;
	}

	self& operator++(int) noexcept {
		self temp = *this;
		++*this;
		return temp;
	}
};

template<typename T, bool cache = false, typename Hash = hash_value<T>, typename Pred = std::equal_to<T>>
struct ht_const_iterator : public ht_iterator_base<T, cache, Hash, Pred> {
    using iterator_category = std::forward_iterator_tag;
	using difference_type   = ptrdiff_t;
	using value_type 		= T;
	using const_reference 	= T&;   
    using self              = ht_iterator<T, cache, Hash, Pred>;
	using size_type			= typename ht_iterator_base<T, cache, Hash, Pred>::size_type;
	using pointer 			= typename ht_iterator_base<T, cache, Hash, Pred>::pointer;
	using reference	 		= typename ht_iterator_base<T, cache, Hash, Pred>::reference;
	using container         = typename ht_iterator_base<T, cache, Hash, Pred>::container;
	using tree_node 		= typename ht_iterator_base<T, cache, Hash, Pred>::tree_node;
	using list_node 		= typename ht_iterator_base<T, cache, Hash, Pred>::list_node;
	using tree_node_ptr 	= typename ht_iterator_base<T, cache, Hash, Pred>::tree_node_ptr;
	using list_node_ptr 	= typename ht_iterator_base<T, cache, Hash, Pred>::list_node_ptr;
	using entry_type		= typename ht_iterator_base<T, cache, Hash, Pred>::entry_type;

	ht_const_iterator() noexcept = default;
	ht_const_iterator(size_type bucketIndex,
			entry_type _entry, container* _ht) noexcept :
			ht_iterator_base<T, cache, Hash, Pred>(bucketIndex, _entry, _ht) {
	}

	ht_const_iterator(const ht_iterator<T, cache, Hash, Pred>& other) :
			ht_iterator_base<T, cache, Hash, Pred>(other.bIndex, other._entry, other.ht) {
	}

	self& operator++() noexcept {
        this->increase();
		return *this;
	}

	self& operator++(int) noexcept {
		self temp = *this;
		++*this;
		return temp;
	}
};

#define DEFAULT_MLF 1.0f //default max load factor

template<typename T, bool cache = false, typename Hash = hash_value<T>, typename Pred = std::equal_to<T>>
class hash_table {
friend struct ht_iterator_base<T, cache, Hash, Pred>;
friend struct ht_iterator<T, cache, Hash, Pred>;
friend struct ht_const_iterator<T, cache, Hash, Pred>;
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
  	using key_equal 				= Pred;

public:
	iterator begin() noexcept { 
		std::pair<size_type, entry_type> myPair = beg();
		return iterator(myPair.first, myPair.second, this); 
	}
	const_iterator begin() const noexcept { 
		std::pair<size_type, entry_type> myPair = beg();
		return const_iterator(myPair.first, myPair.second, this); 
	}

	iterator end() noexcept { return iterator(bucket_count(), nullptr, this); }
	const_iterator end() const noexcept { 
		return const_iterator(bucket_count(), nullptr, this); 
	}
	const_iterator cbegin() const noexcept { return begin(); }
	const_iterator cend()   const noexcept { return end(); }

public:
	// 构造、复制、移动、析构函数
	explicit hash_table(size_type n = 16,
						const hasher& hf = hasher(),
						const key_equal& eql = key_equal());

	template <std::input_iterator InputIter>
	hash_table(InputIter first, InputIter last,
				size_type n = 16,
				const hasher& hf = hasher(),
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
	iterator insert_multi_noresize(const value_type& value);
	std::pair<iterator, bool> insert_unique_noresize(const value_type& value);

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
	size_type count(const key_type& key) const;

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

private:
	using tree_node 	= typename ht_iterator_base<T, cache, Hash, Pred>::tree_node;
	using list_node 	= typename ht_iterator_base<T, cache, Hash, Pred>::list_node;
	using tree_node_ptr = typename ht_iterator_base<T, cache, Hash, Pred>::tree_node_ptr;
	using list_node_ptr = typename ht_iterator_base<T, cache, Hash, Pred>::list_node_ptr;
	using entry_type	= typename ht_iterator_base<T, cache, Hash, Pred>::entry_type;
	using entry_ptr  	= entry_type*;

private:
	std::pair<size_type, entry_type> beg();

private:
	bool is_tree(size_type index) {
	#ifdef BIT64	
		return m_buckets[index].flag & mask;
	#else
		return m_bitset[index];
	#endif //BIT64
	}
	bool is_list(size_type index) {
		return !is_tree(index);
	}
	tree_node_ptr listNode2TreeNode(list_node_ptr node);
	list_node_ptr treeNode2ListNode(tree_node_ptr node);

private:
	entry_ptr allocate_entry(size_type n) {
		size_type nBytes = sizeof(entry_type) * n;
		entry_ptr newEntry = static_cast<entry_ptr>(::operator new(nBytes));
		mem_zero(newEntry, nBytes);
		return newEntry;
	}
	void deallocate_entry(entry_ptr _entry, size_type) {
		::operator delete(_entry);
	}
	void copy_entry_unchecked(entry_ptr buckets, size_type n);
	template<typename Node, typename... Args>
	Node* create_node(Args&&... args);
	template<typename NodePtr>
	void destroy_node(NodePtr node);
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
	iterator insert_tree_node_multi(size_type index, tree_node_ptr node);
	iterator insert_list_node_multi(size_type index, list_node_ptr node);
	std::pair<iterator, bool> insert_tree_node_unique(size_type index, tree_node_ptr node);
	std::pair<iterator, bool> insert_list_node_unique(size_type index, list_node_ptr node);

private:
	size_type m_bucket_count;	///< 桶个数
	entry_ptr m_buckets;		///< 桶数组
	size_type m_size;			///< 有效保存了值节点个数
#ifdef BIT32
	dynamic_bitset m_bitset;	///< 区分链表还是树的标志
#endif //BIT64
	float m_mlf;				///< max load factor
	const hasher& m_hash;
	const key_equal& m_equal;
};

template<typename T, bool cache, typename Hash, typename Pred>
void hash_table<T, cache, Hash, Pred>::copy_entry_unchecked(entry_ptr buckets, size_type n) {
	for (size_type i = 0; i != n; ++i) {
		if (buckets[i].flag) {
			if (is_tree(buckets[i])) {
				m_buckets[i] = copy_since(as_tree_ptr(buckets[i]));
			} else {
				m_buckets[i] = copy_since(as_list_ptr(buckets[i]));
			}
		}
	}
}

template<typename T, bool cache, typename Hash, typename Pred>
template<typename Node, typename... Args>
Node* hash_table<T, cache, Hash, Pred>::create_node(Args&&... args) {
	Node* newNode = static_cast<Node*>(::operator new(sizeof(Node)));
	try {
		construct(&newNode->value, std::forward<Args>(args)...);
	} catch (...) {
		::operator delete(newNode);
		throw;
	}
	return newNode;
}

template<typename T, bool cache, typename Hash, typename Pred>
template<typename NodePtr>
void hash_table<T, cache, Hash, Pred>::destroy_node(NodePtr node) {
	destroy(&node->value);
	::operator delete(node);
}

template<typename T, bool cache, typename Hash, typename Pred>
hash_table<T, cache, Hash, Pred>::hash_table(size_type n,
		const hasher& hf, const key_equal& eql) :
	m_bucket_count(ceil_power_of_2(n)),
	m_buckets(allocate_entry(m_bucket_count)),
	m_size(0),
#ifdef BIT32
	m_bitset(m_buckest.size()),
#endif //BIT32
	m_mlf(DEFAULT_MLF),
	m_hash(hf),
	m_equal(eql) {
}

template<typename T, bool cache, typename Hash, typename Pred>
template <std::input_iterator InputIter>
hash_table<T, cache, Hash, Pred>::hash_table(InputIter first, InputIter last,
		size_type n, const hasher& hf, const key_equal& eql) :
		hash_table(n, hf, eql) {
	for (; first != last; ++first) {
		emplace_multi(*first);
	}
}

template<typename T, bool cache, typename Hash, typename Pred>
hash_table<T, cache, Hash, Pred>::hash_table(const hash_table& other) :
		m_bucket_count(other.m_bucket_count),
		m_buckets(allocate_entry(m_bucket_count)),
		m_size(other.m_size),
#ifdef BIT32
		m_bitset(other.m_bitset),
#endif //BIT32
		m_mlf(other.m_mlf),
		m_hash(other.m_hash),
		m_equal(other.m_equal) {
	copy_entry_unchecked(other.m_buckets, other.m_bucket_count);
}

template<typename T, bool cache, typename Hash, typename Pred>
hash_table<T, cache, Hash, Pred>::hash_table(hash_table&& other) noexcept :
		m_bucket_count(other.m_bucket_count),
		m_buckets(other.m_buckets),
		m_size(other.m_size),
#ifdef BIT32
		m_bitset(std::move(other.m_bitset)),
#endif //BIT32
		m_mlf(other.m_mlf),
		m_hash(other.m_hash),
		m_equal(other.m_equal) {
	other.m_bucket_count = 0;
	other.m_buckets = nullptr;
	other.m_size = 0;
	other.m_mlf = DEFAULT_MLF;
}

template<typename T, bool cache, typename Hash, typename Pred>
hash_table<T, cache, Hash, Pred>&
hash_table<T, cache, Hash, Pred>::operator=(const hash_table& other) {
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
	}
}

template<typename T, bool cache, typename Hash, typename Pred>
hash_table<T, cache, Hash, Pred>& 
hash_table<T, cache, Hash, Pred>::operator=(hash_table&& other) noexcept {
	if (this != &other) {
		clear();
		deallocate_entry(m_buckets, m_bucket_count);
		m_bucket_count = other.m_bucket_count;
		m_buckets = other.m_buckets;
		m_size = other.m_size;
		m_mlf = other.m_mlf;
		other.m_bucket_count = 0;
		other.m_buckets = nullptr;
		other.m_size = 0;
		other.m_mlf = DEFAULT_MLF;
	}
}

template<typename T, bool cache, typename Hash, typename Pred>
hash_table<T, cache, Hash, Pred>::~hash_table() {
	clear();
	deallocate_entry(m_buckets, m_bucket_count);
}

// emplace
template<typename T, bool cache, typename Hash, typename Pred>
template <typename... Args>
typename hash_table<T, cache, Hash, Pred>::iterator 
hash_table<T, cache, Hash, Pred>::emplace_multi(Args&& ...args) {
	if (static_cast<float>(m_size + 1) > static_cast<float>(m_bucket_count) * max_load_factor()) {
		rehash(m_bucket_count * 2); //m_bucket_count << 1
	}
	list_node_ptr newNode = create_node<list_node, Args&&...>(std::forward<Args>(args)...);
	size_t hashVal = m_hash(newNode->value);
	if constexpr(cache) {
		newNode->hash_value = hashVal;
	}
	size_type index = hashVal & (bucket_count() - 1); //hashVal % bucket_count()
	//assert(hashVal == iter->hash_val);
	if (is_list(index)) {
		return insert_list_node_multi(index, newNode);
	} else {
		return insert_tree_node_multi(index, listNode2TreeNode(newNode));
	}
}

template<typename T, bool cache, typename Hash, typename Pred>
template <typename... Args>
std::pair<typename hash_table<T, cache, Hash, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Pred>::emplace_unique(Args&& ...args) {
	if (static_cast<float>(m_size + 1) > static_cast<float>(m_bucket_count) * max_load_factor()) {
		rehash(m_bucket_count * 2); //m_bucket_count << 1
	}
	list_node_ptr newNode = create_node<list_node, Args&&...>(std::forward<Args>(args)...);
	size_t hashVal = m_hash(newNode->values);
	if constexpr(cache) {
		newNode->hash_val = hashVal;
	}
	size_type index = hashVal & (bucket_count() - 1); //hashVal % bucket_count()
	//assert(hashVal == iter->hash_val);
	if (is_list(index)) {
		return insert_list_node_unique(index, newNode);
	} else {
		return insert_tree_node_unique(index, listNode2TreeNode(newNode));
	}
}
// insert
template<typename T, bool cache, typename Hash, typename Pred>
typename hash_table<T, cache, Hash, Pred>::iterator 
hash_table<T, cache, Hash, Pred>::insert_multi_noresize(const value_type& value) {
	
}

template<typename T, bool cache, typename Hash, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Pred>::insert_unique_noresize(const value_type& value) {

}

template<typename T, bool cache, typename Hash, typename Pred>
typename hash_table<T, cache, Hash, Pred>::iterator 
hash_table<T, cache, Hash, Pred>::insert_multi(const value_type& value) {

}

template<typename T, bool cache, typename Hash, typename Pred>
typename hash_table<T, cache, Hash, Pred>::iterator 
hash_table<T, cache, Hash, Pred>::insert_multi(value_type&& value) {

}

template<typename T, bool cache, typename Hash, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Pred>::insert_unique(const value_type& value) {

}

template<typename T, bool cache, typename Hash, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Pred>::iterator, bool> 
hash_table<T, cache, Hash, Pred>::insert_unique(value_type&& value) {

}

template<typename T, bool cache, typename Hash, typename Pred>
template <std::input_iterator InputIter>
void hash_table<T, cache, Hash, Pred>::insert_multi(InputIter first, InputIter last) {
	for (; first != last; ++first) {
		insert_multi(*first);
	}
}

template<typename T, bool cache, typename Hash, typename Pred>
template <std::input_iterator InputIter>
void hash_table<T, cache, Hash, Pred>::insert_unique(InputIter first, InputIter last) {
	for (; first != last; ++first) {
		insert_unique(*first);
	}
}

// erase
template<typename T, bool cache, typename Hash, typename Pred>
void hash_table<T, cache, Hash, Pred>::erase(const_iterator position) {

}

template<typename T, bool cache, typename Hash, typename Pred>
void hash_table<T, cache, Hash, Pred>::erase(const_iterator first, const_iterator last) {
	for (; first != last; ++first) {
		erase(first);
	}
}

template<typename T, bool cache, typename Hash, typename Pred>
typename hash_table<T, cache, Hash, Pred>::size_type 
hash_table<T, cache, Hash, Pred>::erase_multi(const key_type& key) {
	return 0;
}

template<typename T, bool cache, typename Hash, typename Pred>
typename hash_table<T, cache, Hash, Pred>::size_type 
hash_table<T, cache, Hash, Pred>::erase_unique(const key_type& key) {
	return 0;
}

template<typename T, bool cache, typename Hash, typename Pred>
void hash_table<T, cache, Hash, Pred>::clear() {
	for (size_type i = 0; i != m_bucket_count; ++i) {
		if (is_tree(i)) {
			clear_since(m_buckets[i].tree_node, [this](tree_node_base* node){
				destroy_node(static_cast<tree_node_ptr>(node));
			});
		} else {
			clear_since(m_buckets[i].list_node, [this](list_node_base* node){
				destroy_node(static_cast<list_node_ptr>(node));
			});
		}
	}
	m_size = 0;
}

template<typename T, bool cache, typename Hash, typename Pred>
void hash_table<T, cache, Hash, Pred>::swap(hash_table& other) noexcept {
	m_buckets.swap(other.m_buckets);
#ifdef BIT32
	m_bitset.swap(other.m_bitset);
#endif //BIT32
	std::swap(m_size, other.m_size);
}

template<typename T, bool cache, typename Hash, typename Pred>
typename hash_table<T, cache, Hash, Pred>::size_type 
hash_table<T, cache, Hash, Pred>::count(const key_type& key) const {
	return 0;
}

template<typename T, bool cache, typename Hash, typename Pred>
typename hash_table<T, cache, Hash, Pred>::iterator 
hash_table<T, cache, Hash, Pred>::find(const key_type& key) {
	return 0;
}

template<typename T, bool cache, typename Hash, typename Pred>
typename hash_table<T, cache, Hash, Pred>::const_iterator 
hash_table<T, cache, Hash, Pred>::find(const key_type& key) const {
	return 0;
}

template<typename T, bool cache, typename Hash, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Pred>::iterator, 
		typename hash_table<T, cache, Hash, Pred>::iterator> 
hash_table<T, cache, Hash, Pred>::equal_range_multi(const key_type& key) {
	return { };
}

template<typename T, bool cache, typename Hash, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Pred>::const_iterator, 
		typename hash_table<T, cache, Hash, Pred>::const_iterator> 
hash_table<T, cache, Hash, Pred>::equal_range_multi(const key_type& key) const {
	return { };
}

template<typename T, bool cache, typename Hash, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Pred>::iterator, 
		typename hash_table<T, cache, Hash, Pred>::iterator> 
hash_table<T, cache, Hash, Pred>::equal_range_unique(const key_type& key) {
	return { };
}

template<typename T, bool cache, typename Hash, typename Pred>
std::pair<typename hash_table<T, cache, Hash, Pred>::const_iterator, 
		typename hash_table<T, cache, Hash, Pred>::const_iterator> 
hash_table<T, cache, Hash, Pred>::equal_range_unique(const key_type& key) const {
	return { };
}

template<typename T, bool cache, typename Hash, typename Pred>
typename hash_table<T, cache, Hash, Pred>::size_type 
hash_table<T, cache, Hash, Pred>::bucket(const key_type& key) {
	return 0;
}

template<typename T, bool cache, typename Hash, typename Pred>
void hash_table<T, cache, Hash, Pred>::rehash(size_type count) {

}

} //namespace nano