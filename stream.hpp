//
// Created by kiva on 2019/11/11.
//

#pragma once

#include <vector>
#include <functional>
#include "thunk.hpp"

namespace imkiva {
    template <class A, class B, class C>
    static std::function<A(C)> compose(const std::function<C(B)> &f,
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
        std::vector<T> _evaluated;
        int _evaluatedIndex;
        const T &_source;

        std::function<T(T x)> _producer;
        std::function<bool(T x)> _predicate;
        std::function<T(T x)> _mapper;

    private:
        void eval(int toIndex) {
            if (_evaluatedIndex >= toIndex) {
                return;
            }

            T last = _evaluatedIndex > 0 ? _evaluated[_evaluatedIndex] : _source;
            while (_evaluatedIndex < toIndex) {
                T mapped = _mapper(last);
                if (_predicate(mapped)) {
                    _evaluated.push_back(mapped);
                    ++_evaluatedIndex;
                }
                last = _producer(last);
            }
        }

        explicit Stream(const T &seed)
            : _source(seed),
              _evaluatedIndex(0),
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

        static Stream from(const T &source) {
            return Stream<T>(source);
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

        std::vector<T> take(int n) {
            eval(n);
            std::vector<T> values(_evaluated.begin(), _evaluated.begin() + n);
            return std::move(values);
        }
    };
}
