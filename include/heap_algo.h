/**
 * @file heap_algo.h
 * @brief 堆算法
 * @date 2022-03-19
 * @copyright Copyright (c) 2022
 */
#pragma once 

#include <functional>
#include <iterator>
#include "concepts.h"

namespace nano {

/**
 * @brief 堆的插入
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param first 指向序列首元素的迭代器
 * @param last 指向序列尾的迭代器
 * @param comp 比较方法
 */
template<std::random_access_iterator RandomAccessIter, 
		typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type>>
void push_binary_heap(RandomAccessIter first, RandomAccessIter last, 
		const Comp& comp = Comp()) {
    if (first == last) {
        return;
    }

	using Distance = typename std::iterator_traits<RandomAccessIter>::difference_type;
	--last;
	Distance parentIndex = (last - first - 1) / 2;
	while (first < last && comp(*(first + parentIndex), *last)) {
		std::swap(* (first + parentIndex), *last);
		last = first + parentIndex;
		parentIndex = (last - first - 1) / 2;
	}
}

/**
 * @brief 堆的调整
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param[in] first 指向序列首元素的迭代器
 * @param[in] last 指向序列尾的迭代器
 * @param comp 比较方法
 */
template<std::random_access_iterator RandomAccessIter, 
		typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type>>
void adjust_binary_heap(RandomAccessIter first, RandomAccessIter last, 
		const Comp& comp = Comp()) {
    if (first >= last) {
        return;
    }

	using Distance = typename std::iterator_traits<RandomAccessIter>::difference_type;
	using value_type = typename std::iterator_traits<RandomAccessIter>::value_type;
	Distance parentIndex = 0;
	Distance len = last - first;
	value_type value = *first;
	for (Distance childIndex = parentIndex * 2 + 1; childIndex < len; childIndex = childIndex * 2 + 1) {
		if (childIndex + 1 < len && 
				comp(*(first + childIndex), *(first + childIndex + 1))) {
			++childIndex;
		}
		if (comp(value, *(first + childIndex))) {
			*(first + parentIndex) = *(first + childIndex);
			parentIndex = childIndex;
		} else {
			break;
		}
	}
	*(first + parentIndex) = value;
}

/**
 * @brief 堆的删除
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param[in] first 指向序列首元素的迭代器
 * @param[in] last 指向序列尾的迭代器
 * @param comp 比较方法
 */
template<std::random_access_iterator RandomAccessIter, 
	typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type>>
void pop_binary_heap(RandomAccessIter first, RandomAccessIter last, 
		const Comp& comp = Comp()) {
    if (first >= last) {
        return;
    }
	--last;
	std::swap(*first, *last);
	adjust_binary_heap(first, last, comp);
}

/**
 * @brief 堆的构建
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param[in] first 指向序列首元素的迭代器
 * @param[in] last 指向序列尾的迭代器
 * @param comp 比较方法
 */
template<std::random_access_iterator RandomAccessIter, 
		typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type>>
void make_binary_heap(RandomAccessIter first, RandomAccessIter last, 
		const Comp& comp = Comp()) {
    /**
     * 第一个元素到最后一个元素，逐个插入
     */
    if (first >= last) {
        return;
    }

	RandomAccessIter beg = first;
	first += 2; //空堆和只有一个元素的堆不用调整
	for (; first <= last; ++first) {
		push_binary_heap(beg, first, comp);
	}
}

/**
 * @brief 排序一个堆
 * @tparam RandomAccessIter 
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param[in] first 指向堆首元素的迭代器
 * @param[in] last 指向堆尾的迭代器
 * @param comp 比较方法
 */
template<std::random_access_iterator RandomAccessIter, 
		typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type>>
void sort_binary_heap(RandomAccessIter first, RandomAccessIter last, const Comp& comp = Comp()) {
	while (first < last) {
		std::swap(*first, *(last - 1));
        --last;
		adjust_binary_heap(first, last, comp);
	}
}

/**
 * @brief 堆排序
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param[in] first 指向序列首元素的迭代器
 * @param[in] last 指向序列尾的迭代器
 * @param comp 比较方法函数对象
 */
template<std::random_access_iterator RandomAccessIter, 
		typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type>>
void heap_sort(RandomAccessIter first, RandomAccessIter last, 
		const Comp& comp = Comp()) {
	make_binary_heap(first, last, comp);
	sort_binary_heap(first, last, comp);
}

} //namespace nano