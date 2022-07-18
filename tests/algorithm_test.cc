#include <iostream>
#include <random>
#include <vector>
#include <list>
#include "algorithm.h"
#include "utility.h"
#include "assert.h"

static std::default_random_engine e;
static std::uniform_int_distribution<> u;
static std::vector<int> nums;
static std::vector<std::pair<int, int>> nums2;
static constexpr int N = 100000000;

/**
 * @brief vc sort and gnuc sort, N = 100000000, optimize(-O3)
 * vc(32): 7229.06ms 7263.45ms 7163.54ms
 * vc(shell_sort, 32): 21835.6ms 24247.5ms
 * gnuc(16): 7270.35ms 7466.41ms 7453.01ms
 * gnuc(64): 6903.02ms 6880.72ms 6777.01ms
 * tim_sort (0<=nums[i]<=99): 7377.77ms 7311.84ms 7321.02ms
 * tim_sort (INT_MIN<=nums[i]<=INT_MAX): 20857.8ms 21581.6ms 21141.7ms
 */

void generate();
void generate2();
void test_sort();
void copy_test();

int main(int argc, char** argv) {
    //generate();
    //generate2();
    //double totalSpend = nano::run_time(test_sort);
    //std::cout << "共花了: " << totalSpend << "毫秒" << std::endl;
    /*
    for (int num : nums) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    */
    //assert(std::is_sorted(nums.begin(), nums.end()));
    //assert(std::is_sorted(nums2.begin(), nums2.end()));
    //copy_test();

    return 0;
}

void generate() {
    e.seed(time(nullptr));
    //std::cout << "原始数据: ";
    nums.reserve(N);
    /*
    nums = {52, 71, 73, 64, 61, 94, 33, 60, 76, 76, 
            68, 4, 27, 32, 30, 27, 12, 28, 8, 36, 
            99, 23, 17, 65, 98, 0, 49, 17, 86, 
            57, 77, 9, 26};
    */
    
    for (int i = 0; i < N; ++i) {
        nums.push_back(u(e));
        /*
        if (i != 0) {
            std::cout << ",";
        }
        std::cout << nums[i];
        */
    }
    //std::cout << std::endl;
}

void generate2() {
    e.seed(time(nullptr));
    //std::cout << "原始数据: ";
    nums.reserve(N);
    int j = 0;
    for (int i = 0; i < N; ++i) {
        nums2.emplace_back(u(e), j++);
        /*
        if (i != 0) {
            std::cout << ",";
        }
        std::cout << nums[i];
        */
    }
}

void test_sort() {
    nano::intro_sort(nums.begin(), nums.end());
    //nano::binary_insert_sort(nums.begin(), nums.end());
    nano::tim_sort(nums2.begin(), nums2.end(), [](const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) {
        return lhs.first < rhs.first;
    });
    //std::sort(nums.begin(), nums.end());
    //nano::shell_sort(nums.begin(), nums.end());
    /*
    std::list<int> lst(nums.begin(), nums.end());
    nano::insert_sort(lst.begin(), lst.end());
    nano::insert_sort(nums.begin(), nums.end());
    assert(std::is_sorted(lst.begin(), lst.end()));
    assert(std::is_sorted(nums.begin(), nums.end()));
    */
}

void copy_test() {
    std::vector<std::string> vec1{"My", "name", "is", "Van"};
    std::vector<std::string> vec2;
    nano::move_copy(vec1.begin(), vec1.end(), std::back_insert_iterator(vec2));
    for (const auto& str : vec1) {
        std::cout << str << " ";
    }
    std::cout << std::endl;
    for (const auto& str : vec2) {
        std::cout << str << " ";
    }
    std::cout << std::endl;
}