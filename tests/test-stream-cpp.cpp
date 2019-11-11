//
// Created by kiva on 2019/11/12.
//

#include "../stream.hpp"

int main() {
    using namespace imkiva;
    auto s = Stream<int>::from(1)
        .iterate([](int x) { return x * 2; })
        .take(10);
    for (int i : s) {
        printf("%d\n", i);
    }
}
