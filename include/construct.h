/**
 * @file construct.h
 * @brief 对象构造和析构的函数
 * @date 2022-03-19
 * @copyright Copyright (c) 2022
 */
#pragma once

#include <utility>
#include <iterator>

namespace nano {

//构造对象
template <typename T>
void construct(T* ptr) {
	::new ((void*)ptr) T();
}

template <typename T1, typename T2>
void construct(T1* ptr, const T2& value) {
	::new ((void*)ptr) T1(value);
}

template <typename T, typename... Args>
void construct(T* ptr, Args&&... args) {
	::new ((void*)ptr) T(std::forward<Args>(args)...);
}

// destroy 将对象析构
template <typename T>
void destroy_one(T*, std::true_type) {}

template <typename T>
void destroy_one(T* ptr, std::false_type) {
	if (ptr != nullptr) {
		ptr->~T();
	}
}

template <typename ForwardIter>
void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

template <typename ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type) {
	for (; first != last; ++first) {
		destroy(&*first);
    }
}

template <typename T>
void destroy(T* ptr) {
	destroy_one(ptr, std::is_trivially_destructible<T>{});
}

template <typename ForwardIter>
void destroy(ForwardIter first, ForwardIter last) {
	destroy_cat(first, last, std::is_trivially_destructible<
		typename std::iterator_traits<ForwardIter>::value_type>{});
}

} //namespace nano