/**
 * @file heap_algo.h
 * @brief 堆算法
 * @date 2022-03-19
 * @copyright Copyright (c) 2022
 */
#pragma once 

#include <functional>
#include <iterator>

namespace nano {

/**
 * @brief 堆的插入
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param first 指向序列首元素的迭代器
 * @param last 指向序列尾的迭代器
 * @param comp 比较方法函数对象
 */
template<typename RandomAccessIter, typename Comp>
void push_heap(RandomAccessIter first, RandomAccessIter last, Comp comp) {
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

template<typename RandomAccessIter>
void push_heap(RandomAccessIter first, RandomAccessIter last) {
    using value_type = typename std::iterator_traits<RandomAccessIter>::value_type;
	push_heap(first, last, std::less<value_type>{});
}

/**
 * @brief 堆的调整
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param[in] first 指向序列首元素的迭代器
 * @param[in] last 指向序列尾的迭代器
 * @param comp 比较方法函数对象
 */
template<typename RandomAccessIter, typename Comp>
void adjust_heap(RandomAccessIter first, RandomAccessIter last, Comp comp) {
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

template<typename RandomAccessIter>
void adjust_heap(RandomAccessIter first, RandomAccessIter last) {
	using value_type = typename std::iterator_traits<RandomAccessIter>::value_type;
    adjust_heap(first, last, std::less<value_type>{});
}

/**
 * @brief 堆的删除
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param[in] first 指向序列首元素的迭代器
 * @param[in] last 指向序列尾的迭代器
 * @param comp 比较方法函数对象
 */
template<typename RandomAccessIter, typename Comp>
void pop_heap(RandomAccessIter first, RandomAccessIter last, Comp comp) {
    if (first >= last) {
        return;
    }
	--last;
	std::swap(*first, *last);
	adjust_heap(first, last, comp);
}

template<typename RandomAccessIter>
void pop_heap(RandomAccessIter first, RandomAccessIter last) {
    using value_type = typename std::iterator_traits<RandomAccessIter>::value_type;
    pop_heap(first, last, std::less<value_type>{});
}

/**
 * @brief 堆的构建
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param[in] first 指向序列首元素的迭代器
 * @param[in] last 指向序列尾的迭代器
 * @param comp 比较方法函数对象
 */
template<typename RandomAccessIter, typename Comp>
void make_heap(RandomAccessIter first, RandomAccessIter last, Comp comp) {
    /**
     * 第一个元素到最后一个元素，逐个插入
     */
    if (first >= last) {
        return;
    }

	RandomAccessIter beg = first;
	first += 2; //空堆和只有一个元素的堆不用调整
	for (; first <= last; ++first) {
		push_heap(beg, first, comp);
	}
}

template<typename RandomAccessIter>
void make_heap(RandomAccessIter first, RandomAccessIter last) {
    using value_type = typename std::iterator_traits<RandomAccessIter>::value_type;
    make_heap(first, last, std::less<value_type>{});
}

/**
 * @brief 排序一个堆
 * @tparam RandomAccessIter 
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param[in] first 指向堆首元素的迭代器
 * @param[in] last 指向堆尾的迭代器
 * @param comp 比较方法函数对象
 */
template<typename RandomAccessIter, typename Comp>
void sort_heap(RandomAccessIter first, RandomAccessIter last, Comp comp) {
	while (first < last) {
		std::swap(*first, *(last - 1));
        --last;
		adjust_heap(first, last, comp);
	}
}

template<typename RandomAccessIter>
void sort_heap(RandomAccessIter first, RandomAccessIter last) {
    using value_type = typename std::iterator_traits<RandomAccessIter>::value_type;
	sort_heap(first, last, std::less<value_type>{});
}

/**
 * @brief 堆排序
 * @tparam RandomAccessIter 随机访问迭代器
 * @tparam Comp 比较的仿函数类型
 * @param[in] first 指向序列首元素的迭代器
 * @param[in] last 指向序列尾的迭代器
 * @param comp 比较方法函数对象
 */
template<typename RandomAccessIter, typename Comp>
void heap_sort(RandomAccessIter first, RandomAccessIter last, Comp comp) {
	make_heap(first, last, comp);
	sort_heap(first, last, comp);
}

template<typename RandomAccessIter>
void heap_sort(RandomAccessIter first, RandomAccessIter last) {
    using value_type = typename std::iterator_traits<RandomAccessIter>::value_type;
	heap_sort(first, last, std::less<value_type>{});
}

} //namespace nano