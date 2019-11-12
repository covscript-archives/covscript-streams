//
// Created by kiva on 2019/11/12.
//

#include <string>
#include "../stream.hpp"

int main() {
    using namespace imkiva;
    auto s = Stream<int>::repeat(1)
        .iterate([](int x) { return x * 2; })
        .map([](int x) { return x - 1; })
        .filter([](int x) { return x > 1000; })
        .dropWhile([](int x) { return x <= 1000; })
        .drop(5)
        .takeWhile([](int x) { return x <= 300000; });
    printf("size: %zd\n", s.size());
    for (int i : s) {
        printf("%d\n", i);
    }

    auto f = Stream<int>::repeat(1)
        .iterate([](int x) { return x + 1; })
        .map([](int x) -> std::string { return std::to_string(x) + ": stream map";})
        .take(10);
    for (auto i : f) {
        printf("%s\n", i.c_str());
    }
}
