//
// Created by kiva on 2019/11/11.
//

#pragma once

#include <vector>
#include <functional>

namespace imkiva {
    template <class A, class B, class C>
    static std::function<C(A)> compose(const std::function<C(B)> &f,
                                       const std::function<B(A)> &g) {
        return std::bind(f, std::bind(g, std::placeholders::_1));
    }

    template <class A>
    static std::function<bool(A)> booleanCompose(const std::function<bool(A)> &f,
                                                 const std::function<bool(A)> &g) {
        return [=](A x) { return f(x) && g(x); };
    }

    template <class T>
    class Stream {
    private:
        T _head;

        std::function<T(T x)> _producer;
        std::function<bool(T x)> _predicate;
        std::function<T(T x)> _mapper;

    private:
        T takeHead() {
            T head = _head;
            _head = _producer(_head);
            return std::move(head);
        }

        void dropHead() {
            _head = _producer(_head);
        }

        explicit Stream(const T &seed)
            : _head(seed),
              _producer([](T x) { return x; }),
              _predicate([](T x) { return true; }),
              _mapper([](T x) { return x; }) {
        }

    public:
        Stream() = delete;

        ~Stream() = default;

        Stream &operator=(const Stream<T> &rhs) {
            // Copyright (c) 2019 mikecovlee
            this->~Stream();
            new(this) Stream(rhs);
            return *this;
        }

        Stream &iterate(const std::function<T(T x)> &one) {
            this->_producer = compose<T, T, T>(one, this->_producer);
            return *this;
        }

        Stream &filter(const std::function<bool(T x)> &predicate) {
            this->_predicate = booleanCompose<T>(predicate, this->_predicate);
            return *this;
        }

        Stream &map(const std::function<T(T x)> &mapper) {
            this->_mapper = compose<T, T, T>(mapper, this->_mapper);
            return *this;
        }

        Stream &drop(int n) {
            while (n-- > 0) {
                dropHead();
            }
            return *this;
        }

        Stream &tail() {
            dropHead();
            return *this;
        }

        T head() {
            return takeHead();
        }

        std::vector<T> take(int n) {
            std::vector<T> values;
            values.reserve(n);
            while (n-- > 0) {
                T mapped = _mapper(takeHead());
                if (_predicate(mapped)) {
                    values.emplace_back(std::move(mapped));
                }
            }
            return std::move(values);
        }

    public:
        static Stream repeat(const T &head) {
            return Stream<T>(head);
        }
    };
}
