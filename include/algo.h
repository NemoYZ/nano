#pragma once

#include <stdint.h>
#include <iterator>

namespace nano {

template<typename T>
T log2(T n) {
    static_assert(std::is_integral_v<T>, "integral required");
    int result = 0;
    while (n > 1) {
        n >>= 1;
        ++result;
    }
    return result;
}

int32_t ceil_to_power_of_2(int32_t n) {
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    ++n;
    return n;
}

int64_t ceil_to_power_of_2(int64_t n) {
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    ++n;
    return n;
}

uint64_t ceil_to_power_of_2(uint64_t n) {
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    ++n;
    return n;
}

template<typename InputIter, typename OutputIter>
OutputIter move_copy(InputIter first, InputIter last, OutputIter dest) {
    return std::copy(std::move_iterator(first), std::move_iterator(last), dest);
}

template<typename BindirectionalIter1, typename BindirectionalIter2>
BindirectionalIter2 move_copy_backward(BindirectionalIter1 first, 
        BindirectionalIter1 last, BindirectionalIter2 dest) {
    return std::copy_backward(std::move_iterator(first), std::move_iterator(last), dest);
}

} //namespace nano