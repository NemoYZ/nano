#pragma once

#include <string.h>
#include <vector>
#include <initializer_list>
#include <concepts>
#include "construct.h"
#include "type_traits.h"
#include "algo.h"

namespace nano {

/**
 * @brief 专门用于算法的vector
 * @tparam T 
 * @attention 不保证异常安全
 */
template<typename T>
class mini_vector {
public:
    using value_type                = T;
    using pointer                   = T*;
    using reference                 = T&;
    using const_reference           = const T&;
    using iterator                  = T*;
    using const_iterator            = T*;
    using reverse_itertaor          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = const std::reverse_iterator<iterator>;
    using size_type                 = size_t;
    using difference_type           = ptrdiff_t;

public:
    mini_vector();
    mini_vector(const std::initializer_list<T>& ilist);
    template<std::input_iterator InputIter>
    mini_vector(InputIter first, InputIter last);
    explicit mini_vector(size_type n);
    explicit mini_vector(size_type n, const value_type& v);
    ~mini_vector();

public:
    iterator begin() noexcept { return m_start; }
    iterator end() noexcept { return m_finish; }
    reverse_itertaor rbegin() noexcept { return std::reverse_iterator(end()); }
    reverse_itertaor rend() noexcept { return std::reverse_iterator(begin()); }
    const_iterator begin() const noexcept { return m_start; }
    const_iterator end() const noexcept { return m_finish; }
    const_reverse_iterator rbegin() const noexcept { return std::reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return std::reverse_iterator(begin()); }

public:
    void push_back(const value_type& v) {
        emplace_back(v);
    }
    void push_back(value_type&& v) {
        emplace_back(std::move(v));
    }
    template<typename... Args>
    void emplace_back(Args... args);
    void clear();
    void pop_back();
    void append(size_type n, const value_type& v);
    void resize(size_type n);
    void reserve(size_type n);
    size_type size() const noexcept { return m_finish - m_start; }
    size_type capacity() const noexcept { return m_end_of_storage - m_start; }
    reference operator[](size_type index) { return *(m_start + index); }
    reference back() noexcept { return *(m_finish - 1); }
    const_reference operator[](size_type index) const { return *(m_start + index); }
    const_reference back() const noexcept { return *(m_finish - 1); }
    value_type* data() noexcept { return m_start; }
    const value_type* data() const noexcept { return m_start; }

private:
    pointer m_start;
    pointer m_finish;
    pointer m_end_of_storage;
};

template<typename T>
mini_vector<T>::mini_vector() :
    m_start(nullptr),
    m_finish(nullptr),
    m_end_of_storage(nullptr) {
}

template<typename T>
mini_vector<T>::mini_vector(const std::initializer_list<T>& ilist) :
    mini_vector(ilist.begin(), ilist.end()) {
}

template<typename T>
mini_vector<T>::mini_vector(size_type n) :
        m_start(static_cast<pointer>(::operator new(sizeof(T) * n))),
        m_finish(m_start),
        m_end_of_storage(m_start + n * sizeof(T)) {
    append(n, T());
}

template<typename T>
template<std::input_iterator InputIter>
mini_vector<T>::mini_vector(InputIter first, InputIter last) :
        m_start(nullptr),
        m_finish(nullptr),
        m_end_of_storage(nullptr) {
    size_type n = std::distance(first, last);
    reserve(n);
    for (; first != last; ++first) {
        construct(m_finish, *first);
        ++m_finish;
    }
}

template<typename T>
mini_vector<T>::mini_vector(size_type n, const value_type& v) :
        m_start(static_cast<pointer>(::operator new(sizeof(T) * n))),
        m_finish(m_start),
        m_end_of_storage(m_start + n * sizeof(T)) {
    append(n, v);
}

template<typename T>
mini_vector<T>::~mini_vector() {
    clear();
    ::operator delete(m_start);
}

template<typename T>
void mini_vector<T>::clear() {
    destroy(m_start, m_finish);
    m_finish = m_start;
}

template<typename T>
template<typename... Args>
void mini_vector<T>::emplace_back(Args... args) {
    if (m_finish == m_end_of_storage) {
        size_type newSize = size() * 2;
        if (0 == newSize) {
            newSize = 1;
        }
        reserve(newSize);
    }
    construct(m_finish, std::forward<Args>(args)...);
    ++m_finish;
}

template<typename T>
void mini_vector<T>::pop_back() {
    destroy(m_finish - 1);
    --m_finish;
}

template<typename T>
void mini_vector<T>::append(size_type n, const value_type& v) {
    size_type leftSize = m_end_of_storage - m_finish;
    if (leftSize < n) {
        size_type newCapacity = ceil_power_of_2(leftSize);
        reserve(newCapacity);
    }

    size_type n1 = n;
    while (n1) {
        construct(m_finish, v);
        --n1;
        ++m_finish;
    }

    m_finish += n;
}

template<typename T>
void mini_vector<T>::resize(size_type n) {
    if (n < size()) {
        destroy(m_start + n, m_finish);
        m_finish = m_start + n;
    } else {
        reserve(n);
        size_type n1 = n;
        while (n1) {
            construct(m_finish, T());
            --n1;
            ++m_finish;
        }
    }
}

template<typename T>
void mini_vector<T>::reserve(size_type n) {
    if (capacity() >= n) {
        return;
    }

    size_type oldSize = size();
    size_type newSize = n;

    pointer newStart = static_cast<pointer>(::operator new(sizeof(T) * newSize));
    if constexpr(std::is_pod<T>::value) {
        memcpy(newStart, m_start, oldSize);
    } else {
        pointer p = m_start;
        pointer p2 = newStart;
        for (; p != m_finish; ++p, ++p2) {
            construct(p2, std::move(*p));
            destroy(p);
        }
    }
    ::operator delete(m_start);
    m_start = newStart;
    m_finish = m_start + oldSize;
    m_end_of_storage = m_start + newSize;
}

} //namespace nano