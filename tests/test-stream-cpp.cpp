//
// Created by kiva on 2019/11/12.
//

#include <string>
#include <cassert>
#include "../stream.hpp"

int main() {
    using namespace imkiva;

    {
        printf("== Testing infinite Stream\n");
        // head \
        //  $ takeWhile (<=5000000) \
        //  $ drop 5 \
        //  $ dropWhile (<=100000) \
        //  $ filter (>1000) \
        //  $ map (\x -> x - 1) \
        //  $ iterate (*2) 1

        Stream<int>::iterate(1, [](int x) { return x * 2; })
            .map([](int x) { return x - 1; })
            .filter([](int x) { return x > 1000; })
            .dropWhile([](int x) { return x <= 100000; })
            .drop(5)
            .takeWhile([](int x) { return x <= 5000000; })
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
        printf("== Testing finite Stream: drop()\n");
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
        printf("== Testing finite Stream: reduce()\n");
        std::vector<int> v{1, 2, 3, 4, 5};
        int r = Stream<int>::of(v)
            .map([](int x) { return x * x; })
            .reduce<int>(0, [](int acc, int e) { return acc + e; });
        assert(r == (1 + 4 + 9 + 16 + 25));
    }

    {
        printf("== Testing finite Stream: any()\n");
        std::vector<int> v{1, 2, 3, 4, 5};
        bool r = Stream<int>::of(v)
            .any([](int x) { return x % 2 == 0; });
        assert(r);
    }

    {
        printf("== Testing finite Stream: none()\n");
        std::vector<int> v{1, 2, 3, 4, 5};
        bool r = Stream<int>::of(v)
            .none([](int x) { return x == 6; });
        assert(r);
    }

    {
        printf("== Testing finite Stream: all()\n");
        std::vector<int> v{1, 2, 3, 4, 5};
        bool r = Stream<int>::of(v)
            .all([](int x) { return x >= 3; });
        assert(!r);
    }

    {
        printf("== Testing infinite Stream: any()\n");
        bool r = Stream<int>::iterate(1, [](int x) { return x * 2; })
            .any([](int x) { return x % 8 == 0; });
        assert(r);
    }

    {
        printf("== Testing infinite Stream: none()\n");
        bool r = Stream<int>::iterate(1, [](int x) { return x * 2; })
            .none([](int x) { return x < 0; });
        assert(r);
    }

    {
        printf("== Testing infinite Stream: all()\n");
        bool r = Stream<int>::iterate(1, [](int x) { return x * 2; })
            .all([](int x) { return x <= 1000; });
        assert(!r);
    }
}
