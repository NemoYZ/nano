/**
 * 测试结果显示，在10000个元素以内效率还不错，超出10000个就不怎么好了
 */
#include "skip_list.h"
#include "utility.h"
#include <iostream>
#include <random>
#include <vector>
#include <time.h>
#include <algorithm>
#include <assert.h>

static dalib::skip_list<int> slist;

void test_insert();

int main(int argc, char** argv) {
    std::cout << "插入10000个元素共需" << dalib::run_time(&test_insert) << "秒" << std::endl;
    return 0;
}

void test_insert() {
    std::default_random_engine e(static_cast<unsigned>(time(nullptr)));
    std::uniform_int_distribution<int> u(0, 10000);
    std::vector<int> nums;
    for (int i = 0; i < 40000; ++i) {
        nums.push_back(u(e));
    }
    //std::cout << "原始数据：";
    for (int num : nums) {
        //std::cout << num << " ";
        slist.insert(num);
    }
    /*
    std::cout << std::endl;
    std::cout << "插入后的数据: ";
    for (auto iter = slist.begin();
        iter != slist.end(); ++iter) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
    */
    assert(std::is_sorted(slist.begin(), slist.end()));
}