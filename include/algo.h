#pragma once

#include <stdint.h>
#include <iterator>
#include <bit>
#include <algorithm>
#include "concepts.h"

namespace nano {

/**
 * @brief 算以2为底的对数，结果向下取整
 * @tparam T 
 * @param n 
 * @return T 
 */
template<std::integral T>
T log2(T n) {
    T result = 0;
    while (n > 1) {
        n >>= 1;
        ++result;
    }
    return result;
}

/**
 * @brief计算 第一个大于等于n的，为2的次幂的数
 * @param n 
 * @return uint32_t 
 */
template<std::integral T>
constexpr T ceil_power_of_2(T n) {
    return n ? std::bit_ceil(n) : 1;
}
/**
 * @brief 移动拷贝
 * @tparam InputIter 
 * @tparam OutputIter 
 * @param first 
 * @param last 
 * @param dest 
 * @return OutputIter 
 */
template<typename InputIter, typename OutputIter>
OutputIter move_copy(InputIter first, InputIter last, OutputIter dest) {
    return std::copy(std::move_iterator(first), std::move_iterator(last), dest);
}

/**
 * @brief 移动从后往前拷贝
 * @tparam BidirectionalIter1 
 * @tparam BidirectionalIter2 
 * @param first 
 * @param last 
 * @param dest 
 * @return BidirectionalIter2 
 */
template<typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2 move_copy_backward(BidirectionalIter1 first, 
        BidirectionalIter1 last, BidirectionalIter2 dest) {
    return std::copy_backward(std::move_iterator(first), std::move_iterator(last), dest);
}

} //namespace nano