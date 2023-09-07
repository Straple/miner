#include <iostream>
#include "sha256.hpp"

int main() {
    std::cout << sha256("hello world") << '\n';
    std::cout << sha256(sha256("hello world")) << '\n';
}