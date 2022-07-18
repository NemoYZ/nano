#define B_TREE_DEBUG

#include "b_tree.h"
#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <time.h>
#include <set>
#include <unordered_set>
#include <limits.h>

constexpr static int N = 10;

static nano::b_tree<int> bTree;
static std::default_random_engine e;
static std::uniform_int_distribution<int> u(0, N);
static std::vector<int> nums;

void generate();
void traverse(nano::b_tree_node<int>* root);
void test_insert_multi();
void test_insert_unique();
void test_find();
void test_erase();
void show();
void backwardShow();
bool is_unique();
bool is_multi();
/**
 * @brief compare with rb_tree(set)
 * N = 10000000
 * btree:    2839.35ms  2817.92ms   2828.84ms
 * multiset: 3490.5ms   3553.32ms   3689.82ms
 */
void bench();

int main(int argc, char** argv) {
    generate();
    //test_insert_multi();
    test_insert_unique();
    //assert(is_unique());
    //std::cout << bTree.serialize() << std::endl;
    //test_find();
    //assert(is_unique());
    //assert(is_multi());
    //test_erase();
    //std::cout << bTree.serialize() << std::endl;
    /*
    assert(bTree.size() == nums.size());
    assert(std::is_sorted(bTree.begin(), bTree.end()));
    assert(std::is_sorted(bTree.rbegin(), bTree.rend(), std::greater<int>()));
    */
    //bench();
    /*
    show();
    int prev = std::numeric_limits<int>::min();
    for (auto iter = bTree.begin();
            iter != bTree.end(); ++iter) {
        if (prev > *iter) {
            std::cout << "*iter = " << *iter << " prev = " << prev << std::endl;
            break;
        }
        prev = *iter;
    }
    */
    /*
    auto node = static_cast<nano::b_tree_node<int>*>(nano::min_node(bTree.root()).first);
    nano::degree_t index = 0;
    while (node) {
        std::cout << "node->vsz = " << node->vsz 
                << " index = " << index
                << " val = " << node->values[index] 
                << std::endl;
        auto myPair = nano::successor(node, index);
        node = static_cast<nano::b_tree_node<int>*>(myPair.first);
        index = myPair.second;
    }
    */

    auto bTree2 = bTree;
    std::cout << "bTree.size()=" << bTree.size() 
            << " bTree2.size()=" << bTree2.size() 
            << std::endl;
    for (auto iter = bTree.begin();
        iter != bTree.end(); ++iter) {
        std::cout << *iter << " " << std::flush;
    }
    std::cout << std::endl;
    for (auto iter = bTree2.begin();
        iter != bTree2.end(); ++iter) {
        std::cout << *iter << " " << std::flush;
    }
    std::cout << std::endl;
    assert(bTree2 == bTree);

    return 0;
}

void generate() {
    e.seed(time(nullptr));
    nums.reserve(N);
    for (int i = 0; i < N; ++i) {
        nums.push_back(u(e));
    }
    /*
    nums = {13, 17, 42, 18, 67, 10, 85, 
            52, 23, 67, 43, 54, 75, 11, 
            30, 4, 56, 46, 86, 26};
    */
    /*
    std::cout << "原数据为: ";
    for (size_t i = 0; i < nums.size(); ++i) {
        if (i != 0) {
            std::cout << ", ";
        }
        std::cout << nums[i];
    }
    std::cout << std::endl; 
    */
}

void test_insert_multi() {
    for (int num : nums) {
        bTree.insert_multi(num);
        //mset.insert(num);
        //std::cout << bTree.serialize() << std::endl;
    }
}

void test_insert_unique() {
    for (int num : nums) {
        bTree.insert_unique(num);
        //std::cout << bTree.serialize() << std::endl;
    }
}

void test_find() {
    static std::multiset<int> mset(nums.begin(), nums.end());
    for (int i = 0; i < N; ++i) {
        int num = rand();
        //std::cout << "num = " << num << std::endl;
        /*
        auto iter1 = mset.lower_bound(num);
        auto iter2 = bTree.lower_bound(num);
        */
        auto iter1 = mset.upper_bound(num);
        auto iter2 = bTree.upper_bound(num);
        if ((iter1 == mset.end() && iter2 != bTree.end()) ||
            (iter1 != mset.end() && iter2 == bTree.end())) {
            std::cout << "num = " << num << " ";
            if (iter1 != mset.end()) {
                std::cout << "*iter1 = " << *iter1;
            } else {
                std::cout << "*iter2 = " << *iter2;
            }
            std::cout << "not equal" << std::endl;
            break;
        }
        if (iter1 != mset.end() && 
            iter2 != bTree.end() && 
            *iter1 != *iter2) {
            std::cout << "num = " << num
                    << " *iter1 = " << *iter1 
                    << " *iter2 = " << *iter2 
                    << std::endl;
            break;
        }
    }
}

void test_erase() {
    /*
    std::multiset<int> mset(nums.begin(), nums.end());
    for (int i = 0; i < N; ++i) {
        int num = rand();
        auto iter1 = mset.lower_bound(num);
        auto iter2 = bTree.lower_bound(num);
        if ((iter1 == mset.end() && iter2 != bTree.end()) || 
            (iter1 != mset.end() && iter2 == bTree.end())) {
            std::cout << "迭代器不相等" << std::endl;
            return;
        }
        if (iter1 != mset.end() && iter2 != bTree.end()) {
            mset.erase(iter1);
            bTree.erase(iter2);
            iter2 = bTree.begin();
            for (iter1 = mset.begin();
                    iter1 != mset.end(); ++iter1) {
                assert(*iter1 == *iter2);
            }
        }
    }
    */
    std::vector<int> randomNum{13, 17, 42, 18, 67, 10, 85, 52, 23, 43, 54, 75, 11, 30, 4, 56, 46, 86, 26}; //
    int j = 0;
    e.seed(time(nullptr));
    size_t eraseCount = N; //randomNum.size()
    std::set<int> st(nums.begin(), nums.end());
    for (size_t i = 0; i < eraseCount; ++i, ++j) {
        int num = u(e); //randomNum[i]
        if (st.find(num) == st.end()) {
            continue;
        }
        //std::cout << "num = " << num << std::endl;
        st.erase(num);
        /*
        std::cout << "before erase"
            << bTree.serialize();
        */
        bTree.erase_unique(num);
        /*
        std::cout << "after erase\n"
                << bTree.serialize() 
                << std::endl;
        */
        auto iter2 = st.begin();
        if (st.size() != bTree.size()) {
            std::cout << "size not equal" << std::endl;
        }
        for (auto iter1 = bTree.begin();
                iter1 != bTree.end(); ++iter1, ++iter2) {
            /*
            std::cout << "*iter2 = " << *iter2 << std::flush;
            std::cout << " *iter1 = " << *iter1
                << std::endl;
            */
            /*
            nano::b_tree_node<int>* parent = static_cast<nano::b_tree_node<int>*>(iter1.node->parent);
            if (parent) {
                nano::degree_t index = nano::child_index(parent, iter1.node);
                std::cout << parent->values[0] << " childIndex = " << index << std::endl;
            } else {
                std::cout << "parent is null" << std::endl;
            }
            */
            if (*iter1 != *iter2) {
                assert(false);
                //std::cout << nano::child_index(iter1.node->parent, iter1.node) << std::endl;
                return;
                break;
            }
        }
        //std::cout << "finished erase\n" << std::endl;
    }
}

void bench() {
    std::cout << "一共花了: " << nano::run_time(test_insert_multi) << "毫秒" << std::endl;
}

void show() {
    for (auto iter = bTree.begin();
            iter != bTree.end(); ++iter) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
}

void backwardShow() {
    for (auto riter = bTree.rbegin();
        riter != bTree.rend(); ++riter) {
        std::cout << *riter << " ";
    }
    std::cout << std::endl;
}

bool is_unique() {
    std::set<int> st(nums.begin(), nums.end());
    if (st.size() != bTree.size()) {
        std::cout << "st.size() = " << st.size() 
                << "bTree.size() = " << bTree.size() 
                << std::endl;
        return false;
    }
    
    auto biter = bTree.begin();
    for (auto iter = st.begin(); 
            iter != st.end(); ++iter, ++biter) {
        if (*iter != *biter) {
            std::cout << "*iter = " << *iter
                    << " *biter = " << *biter 
                    << std::endl;
            return false;
        }
    }

    return true;
}

bool is_multi() {
    std::multiset<int> mst(nums.begin(), nums.end());
    if (mst.size() != bTree.size()) {
        return false;
    }

    auto biter = bTree.begin();
    for (auto iter = mst.begin(); 
            iter != mst.end(); ++iter, ++biter) {
        if (*iter != *biter) {
            std::cout << "*iter = " << *iter
                    << " *biter = " << *biter 
                    << std::endl;
            return false;
        }
    }

    return true;
}