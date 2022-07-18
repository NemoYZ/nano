#include "skip_list.h"
#include "utility.h"
#include <set>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <random>
#include <set>
#include <assert.h>

typedef std::pair<int, int> MyPair;

static auto myPairComp = [](const MyPair& lhs, const MyPair& rhs)->bool{
    return lhs.first < rhs.first;
};

static nano::skip_list<int> slist;
static nano::skip_list<MyPair, decltype(myPairComp)> slist2(myPairComp);
static std::multiset<int> mst;
static std::set<int> st;
static std::default_random_engine e;
static std::uniform_int_distribution<int> u(0, 99);

constexpr static int N = 50;

void generate();
void generate2();
void remove();
void remove2();
void test();
void test2();

int main(int argc, char** argv) {
    test();

    auto slist2 = slist;
    assert(slist2 == slist);

    return 0;
}

void generate() {
    for (int i = 0; i < N; ++i) {
        slist.insert_multi(u(e));
    }
    assert(slist.size() == N);
}

void generate2() {
    slist2.emplace_multi(1, 1);
    slist2.emplace_multi(1, 2);
}

void remove() {
    
}

void remove2() {
    auto iter = slist2.begin();
    std::cout << "iter->first = " << iter->first << " iter->second = " << iter->second << std::endl;
    slist2.erase(iter);
}

void test() {
    std::cout << "一共花了" << nano::run_time(generate) << "ms" << std::endl;
    assert(std::is_sorted(slist.begin(), slist.end()));
    std::cout << "插入的数据倒序遍历为： ";
    for (auto riter = slist.rbegin();
        riter != slist.rend(); ++riter) {
        std::cout << *riter << " ";
    }
    std::cout << std::endl;
    remove();
}

void test2() {
    generate2();
    remove2();
    for (auto iter = slist2.begin();
            iter != slist2.end(); ++iter) {
        std::cout << iter->first << ", " << iter->second << std::endl;
    }
}