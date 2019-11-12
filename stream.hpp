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
    public:
        template <typename R>
        using Mapper = std::function<R(T x)>;

        using Predicate = Mapper<bool>;
        using Producer = Mapper<T>;

    private:
        T _head;

        Producer _producer;
        Predicate _predicate;
        Mapper<T> _mapper;

    private:
        T takeHead() {
            T head = _head;
            _head = _producer(_head);
            return std::move(head);
        }

        T evalHead() {
            T mapped = _mapper(takeHead());
            while (!_predicate(mapped)) {
                mapped = _mapper(takeHead());
            }
            return std::move(mapped);
        }

        void dropHead() {
            (void) evalHead();
        }

        explicit Stream(const T &head)
            : _head(head),
              _producer([](T x) { return x; }),
              _predicate([](T x) { return true; }),
              _mapper([](T x) { return x; }) {
        }

    public:
        Stream() = delete;

        ~Stream() = default;

        Stream<T> &operator=(const Stream<T> &rhs) {
            // Copyright (c) 2019 mikecovlee
            this->~Stream();
            new(this) Stream(rhs);
            return *this;
        }

        Stream<T> &iterate(const Producer &iterator) {
            this->_producer = compose<T, T, T>(iterator, this->_producer);
            return *this;
        }

        Stream<T> &filter(const Predicate &predicate) {
            this->_predicate = booleanCompose<T>(predicate, this->_predicate);
            return *this;
        }

        template <typename R>
        Stream<R> map(const Mapper<R> &mapper) {
            // TODO: construct a new Stream<R>
            assert(false);

        }

        Stream<T> &map(const Mapper<T> &mapper) {
            this->_mapper = compose<T, T, T>(mapper, this->_mapper);
            return *this;
        }

        Stream<T> &drop(int n) {
            while (n-- > 0) {
                dropHead();
            }
            return *this;
        }

        Stream<T> &dropWhile(const Predicate &predicate) {
            return filter([=](T x) { return !predicate(x); });
        }

        std::vector<T> take(int n) {
            std::vector<T> values;
            values.reserve(n);
            while (n-- > 0) {
                values.emplace_back(evalHead());
            }
            return std::move(values);
        }

        std::vector<T> takeWhile(const Predicate &predicate) {
            std::vector<T> values;
            T head = evalHead();
            while (predicate(head)) {
                values.emplace_back(std::move(head));
                head = evalHead();
            }
            return std::move(values);
        }

        Stream<T> &tail() {
            dropHead();
            return *this;
        }

        T head() {
            return evalHead();
        }

    public:
        static Stream<T> repeat(const T &head) {
            return Stream<T>(head);
        }
    };
}
