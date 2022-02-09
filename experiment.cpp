//
// Created by Jan Presperín on 23.01.2022.
//
#include <iostream>
#include "trie.hpp"

int main() {
    trie t1({"aa", "bc", "efg", "xkcd"});
    trie t2({"efg", "xkcd", "aaa", "bc"});
    std::cout << (t1 < t2);
    return 0;



}