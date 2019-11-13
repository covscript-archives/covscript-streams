//
// Created by kiva on 2019/11/12.
//

#include <string>
#include "../stream.hpp"

int main() {
    using namespace imkiva;

    {
        printf("== Testing infinite Stream\n");
        Stream<int>::iterate(1, [](int x) { return x * 2; })
            .map([](int x) { return x - 1; })
            .filter([](int x) { return x > 1000; })
            .dropWhile([](int x) { return x <= 1000; })
            .drop(5)
            .takeWhile([](int x) { return x <= 300000; })
            .forEach([](int x) { printf("%d\n", x); });
    }

    {
        printf("== Testing finite Stream\n");
        std::vector<int> v{1, 2, 3, 4, 5};
        std::vector<int> f = Stream<int>::of(v)
            .map([](int x) { return x * x; })
            .collect();
        for (int i : f) {
            printf("%d\n", i);
        }
    }

    {
        printf("== Testing dropping finite Stream\n");
        std::vector<int> v{1, 2, 3, 4, 5};
        std::vector<int> f = Stream<int>::of(v)
            .map([](int x) { return x * x; })
            .drop(4)
            .tail()
            .collect();
        assert(f.empty());
        for (int i : f) {
            printf("%d\n", i);
        }
    }

    {
        printf("== Testing foldr finite Stream\n");
        std::vector<int> v{1, 2, 3, 4, 5};
        int r = Stream<int>::of(v)
            .map([](int x) { return x * x; })
            .foldr<int>(0, [](int acc, int e) { return acc + e; });
        assert(r == (1 + 4 + 9 + 16 + 25));
    }
}
