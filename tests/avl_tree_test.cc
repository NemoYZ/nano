#define AVL_DEBUUG
#include "avl_tree.h"
#include <iostream>
#include <random>
#include <vector>
#include <time.h>
#include <algorithm>
#include <assert.h>
#include <functional>
#include <set>
#include <vector>

static std::default_random_engine e(static_cast<unsigned>(time(nullptr)));
static std::uniform_int_distribution<int> u(0, 100);
static std::vector<int> nums;    
static std::vector<int> eraseNums;

static auto numComp = [](int lhs, int rhs){
    return lhs > rhs;
};

bool numCompFunc(int lhs, int rhs) { return lhs < rhs; }

static nano::avl_tree<int, decltype(numCompFunc)> avlTree(numCompFunc);
static std::set<int> intSet;

void test_insert();
void test_find();
void test_erase();
void test_insert_string();
void test_set_insert();
void test_efficiency();
void test_set_efficiency();
void test();

int main(int argc, char** argv) {
    e.seed(time(nullptr));

    //test();
    std::cout << "原始数据: ";
    for (int i = 0; i < 10; ++i) {
        nums.push_back(u(e));
        //std::cout << nums.back() << ",";
    }
    std::cout << std::endl;
    std::cout << "*******************" << std::endl;  
    test_insert();
    //std::cout << "avl树共用时: " << nano::run_time(&test_efficiency) << "秒" << std::endl;
    //test_insert_string();
    //test_find();
    //test_erase();
    auto avlTree2(avlTree);
    assert(avlTree2 == avlTree);
    
    return 0;
}

void test_efficiency() {
    for (int i = 0; i < 10000; ++i) {
        test_insert();
    }
}

void test_set_efficiency() {
    for (int i = 0; i < 100; ++i) {
        test_set_insert();
    }
}

void test_insert() {
    for (int num : nums) {
        avlTree.insert_multi(num);
        //std::string treeStr = avlTree.serialize();
        //std::cout << "序列化: " << treeStr << std::endl;
    }
    
    //std::cout << "插入完成" << std::endl;
    
    std::cout << "插入后的数据: " << std::endl;
    for (auto iter = avlTree.begin();
            iter != avlTree.end(); ++iter) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
    
    //std::cout << "初始层序遍历: " << avlTree.serialize() << std::endl;
    
    if (!avlTree.check_header()) {
        std::cout << "头节点指针错误" << std::endl;
        assert(false);
    }
    if (!avlTree.balanced()) {
        std::cout << "非平衡的" << std::endl;
        assert(false);
    } else {
        std::cout << "平衡的" << std::endl;
    }
    if (!std::is_sorted(avlTree.begin(), avlTree.end(), numComp)) {
        std::cout << "不是有序的" << std::endl;
    } else {
        std::cout << "是有序的" << std::endl;
    }
}

void test_find() {
    test_insert();
    for (int i = 0; i < 20; ++i) {
        int num = u(e);
        auto iter = avlTree.find(num);
        if (iter != avlTree.end()) {
            std::cout << "找到了: " << *iter << std::endl;
        } else {
            std::cout << "没找到: " << num << std::endl;
        }
    }
}

void test_erase() {
    test_insert();
    for (int i = 0; i < 10000; ++i) {
        int num = u(e);
        //std::cout << "要删除的数据: " << num << std::endl;
        if (avlTree.erase_unique(num)) {
            std::cout << "删除" << num << "成功" << std::endl;
        } else {
            //std::cout << "删除" << num << "失败" << std::endl;    
        }
        //std::cout << "层序遍历: " << avlTree.serialize() << std::endl;
        if (!avlTree.check_header()) {
            std::cout << "头节点指针错误" << std::endl;
            assert(false);
        }
        if (!avlTree.balanced()) {
            std::cout << "非平衡的" << std::endl;
            assert(false);
        } else {
            //std::cout << "平衡的" << std::endl;
        }
        if (!std::is_sorted(avlTree.begin(), avlTree.end())) {
            std::cout << "不是有序的" << std::endl;
        } else {
            //std::cout << "是有序的" << std::endl;
        }
    }
}

void test_insert_string() {
    nano::avl_tree<std::string> strTree;
    std::vector<std::string> strs = { "My", "name", "is", "Van", "I'm", "an", "artist.",
            "I'm", "a", "performance", "artist.", "I'm", "hired", "for", "people", "to",
            "fullfill", "their", "fantasies", "the", "deep", "dark", "fantasies."};
    for (const auto& str : strs) {
        strTree.insert_unique(str);
    }

    for (auto iter = strTree.begin();
        iter != strTree.end(); ++iter) {
        std::cout << *iter << std::endl;
    }
    assert(std::is_sorted(strTree.begin(), strTree.end()));
}

void test_set_insert() {
    for (int num : nums) {
        intSet.insert(num);
    }    
}

void test() {
    nano::avl_tree<int> st1 = { 1, 2, 4 };
    nano::avl_tree<int> st2 = { 1, 2, 3 };
    std::cout << std::boolalpha << (st1 == st2) << std::endl; 
}