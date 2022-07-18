#include "heap_algo.h"
#include <random>
#include <time.h>
#include <vector>
#include <iostream>
#include <assert.h>
#include <algorithm.h>

int main(int argc, char** argv) {
    std::default_random_engine e((unsigned)time(nullptr));
    std::uniform_int_distribution<int> u(0, 100);
    std::vector<int> nums;
    for (int i = 0; i < 20; ++i) {
        nums.push_back(u(e));
    }
    std::cout << "原始数据: ";
    for (int num : nums) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    nano::heap_sort(nums.begin(), nums.end(), std::greater<int>{});
    std::cout << "排序后: ";
    for (int num : nums) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    assert(std::is_sorted(nums.begin(), nums.end(), std::greater<int>{}));
    //std::cout << "is_sorted: " << std::boolalpha << std::is_sorted(nums.begin(), nums.end());

    return 0;
}