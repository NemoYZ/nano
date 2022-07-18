/**
 * @file construct.h
 * @brief 对象构造和析构的函数
 * @date 2022-03-19
 * @copyright Copyright (c) 2022
 */
#pragma once

#include <utility>
#include <iterator>
#include <memory>
#include "type_traits.h"
#include "concepts.h"

namespace nano {

//构造对象
template <typename T>
void construct(T* ptr) {
	::new (static_cast<void*>(ptr)) T();
}

template <typename T1, typename T2>
void construct(T1* ptr, const T2& value) {
	::new (static_cast<void*>(ptr)) T1(value);
}

template <typename T, typename... Args>
void construct(T* ptr, Args&&... args) {
	::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
}

// destroy 将对象析构
template <typename T>
void destroy(T* ptr) {
	if constexpr(!std::is_trivially_destructible_v<T>) {
		ptr->~T();
	}
}

template <std::forward_iterator ForwardIter>
void destroy(ForwardIter first, ForwardIter last) {
	if constexpr(!std::is_trivially_destructible_v<
			typename std::iterator_traits<ForwardIter>::value_type>) {
		for (; first != last; ++first) {
			destroy(&*first);
		}
	}
}

} //namespace nano