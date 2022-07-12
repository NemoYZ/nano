#include "allocator.h"
#include <string>
#include <iostream>

class Foo {
public:
    Foo() {
        std::cout << "called default constructor" << std::endl;
    }

    Foo(const std::string& s) {
        std::cout << "passed parameter: " << s << std::endl;
    }

    ~Foo() {
        std::cout << "called distructor" << std::endl;
    }
};

int main(int argc, char** argv) {
    nano::allocator<Foo> alloc;
    Foo* foo = alloc.allocate();
    alloc.construct(foo);
    alloc.destroy(foo);
    alloc.construct(foo, "Van");
    alloc.destroy(foo);
    
    return 0;
}