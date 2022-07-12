/**
 * @file utility.h
 * @brief 额外的工具类或函数
 * @date 2022-03-19
 * @copyright Copyright (c) 2022
 */
#pragma once

#include <utility>
#include <chrono>
#include <string.h>

namespace nano {
    
/**
 * @brief 测试函数运行时间
 * @tparam Func 函数类型
 * @tparam Args 函数参数类型
 * @param func 函数对象
 * @param args 函数参数
 * @return 运行时间(毫秒)
 */
template<typename Func, typename... Args>
double run_time(Func f, Args&&... args) {
    std::chrono::system_clock::time_point start = std::chrono::high_resolution_clock::now();
    f(std::forward<Args>(args)...);
    std::chrono::system_clock::time_point finish = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(finish - start).count();
}

void mem_zero(void* dest, size_t n) {
    memset(dest, 0, n);
}

} //namespace nano