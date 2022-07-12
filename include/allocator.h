/**
 * @file allocator.h
 * @brief 一个简易分配器的实现
 * @date 2022-03-19
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <cstddef>
#include "construct.h"

namespace nano {

template<typename T>
class simple_allocator;

template<typename T>
using allocator = simple_allocator<T>;

/**
 * @brief 分配器
 */
template <typename T>
class simple_allocator
{
public:
	using value_type		= T;
	using pointer			= T*;
	using const_pointer		= const T*;
	using reference			= T&;
	using const_reference	= const T&;
	using size_type			= size_t;
	using difference_type	= ptrdiff_t;

public:
	pointer allocate() { 
        return static_cast<pointer>(::operator new(sizeof(value_type))); 
    }

	pointer allocate(size_type n) { 
        return 0 == n ? nullptr : static_cast<pointer>(::operator new(n * sizeof(value_type))); 
    }

	void deallocate(pointer ptr) {
		if (ptr) {
			::operator delete(ptr);
		}
	}

	void deallocate(pointer ptr, size_type n) {
		if (ptr) {
			::operator delete(ptr);
		}
	}

	void construct(pointer ptr) { 
		nano::construct(ptr); 
	}

	void construct(pointer ptr, const_reference value) { 
		nano::construct(ptr, value); 
	}

	void construct(pointer ptr, value_type&& value) { 
		::new(static_cast<void*>(ptr)) value_type(std::move(value)); 
	}

	template <typename... Args>
	void construct(pointer ptr, Args&& ...args) {
		nano::construct(ptr, std::forward<Args>(args)...); 
	}

	void destroy(pointer ptr) { 
        nano::destroy(ptr); 
    }

	void destroy(pointer first, pointer last) {
        nano::destroy(first, last); 
    }
};

} //namespace nano