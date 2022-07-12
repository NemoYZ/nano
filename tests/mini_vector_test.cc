#include "mini_vector.h"
#include <random>
#include <iostream>
#include "algorithm.h"

static std::default_random_engine e;
static std::uniform_int_distribution<> u(0, 99);
static constexpr int N = 10;
static nano::mini_vector<int> vec;

void generate();

int main(int argc, char** argv) {
    /*
    generate();
    nano::mini_vector<int> vec2{1, 3, 3, 5, 7};
    for (int num : vec2) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    */
    nano::mini_vector<std::string> strVec{"My "};
    strVec.push_back("name");
    strVec.emplace_back(" is Van");
    for (auto iter = strVec.begin();
            iter !=strVec.end(); ++iter) {
        std::cout << *iter;
    }
    std::cout << std::endl;
    
    return 0;
}

void generate() {
    e.seed(time(nullptr));
    for (int i = 0; i < 10; ++i) {
        vec.push_back(u(e));
    }

    nano::binary_insert_sort(vec.begin(), vec.end());
    for (auto iter = vec.begin();
        iter != vec.end(); ++iter) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
}