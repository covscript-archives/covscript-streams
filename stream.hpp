//
// Created by kiva on 2019/11/11.
//

#pragma once

#include <vector>
#include <deque>
#include <list>
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
        using Consumer = Mapper<void>;

    private:
        T _head;
        std::deque<T> _finiteData;
        bool _remaining = true;
        bool _finiteStream = false;

        Producer _producer;
        Predicate _predicate;
        Mapper<T> _mapper;

    private:
        T produceNext(const T &head) {
            if (_finiteStream) {
                if (this->_finiteData.empty()) {
                    this->_remaining = false;
                    return head;
                }
                T x = this->_finiteData.front();
                this->_finiteData.pop_front();
                return x;
            } else {
                return _producer(head);
            }
        }

        T takeHead() {
            T head = _head;
            if (_remaining) {
                _head = produceNext(_head);
            }
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

        Stream<T> &iterate(const Producer &iterator) {
            this->_producer = compose<T, T, T>(iterator, this->_producer);
            return *this;
        }

        explicit Stream(const T &head)
            : _head(head),
              _remaining(true),
              _finiteStream(false),
              _producer([](T x) { return x; }),
              _predicate([](T x) { return true; }),
              _mapper([](T x) { return x; }) {
        }

        explicit Stream(std::deque<T> list)
            : _head(),
              _finiteData(std::move(list)),
              _finiteStream(true),
              _remaining(!_finiteData.empty()),
              _producer([](T x) { return x; }),
              _predicate([](T x) { return true; }),
              _mapper([](T x) { return x; }) {
            // Bind the head to the first element of list
            dropHead();
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

        Stream<T> &filter(const Predicate &predicate) {
            this->_predicate = booleanCompose<T>(predicate, this->_predicate);
            return *this;
        }

        Stream<T> &dropWhile(const Predicate &predicate) {
            return filter([=](T x) { return !predicate(x); });
        }

        Stream<T> &map(const Mapper<T> &mapper) {
            this->_mapper = compose<T, T, T>(mapper, this->_mapper);
            return *this;
        }

        template <typename R>
        Stream<R> map(const Mapper<R> &mapper) {
            // TODO: construct a new Stream<R>
            assert(false);
        }

    public:
        Stream<T> &drop(int n) {
            while (_remaining && n-- > 0) {
                dropHead();
            }
            return *this;
        }

        Stream<T> &go(const Predicate &predicate) {
            if (_remaining) {
                T head = evalHead();
                while (_predicate(head)) {
                    if (!predicate(head) || !_remaining) {
                        break;
                    }
                    head = evalHead();
                }
            }
            return *this;
        }

        std::vector<T> takeAll(const Predicate &predicate) {
            std::vector<T> values;
            if (_remaining) {
                T head = evalHead();
                while (predicate(head)) {
                    values.emplace_back(std::move(head));
                    if (!_remaining) {
                        break;
                    }
                    head = evalHead();
                }
            }
            return std::move(values);
        }

        Stream<T> &peek(const Consumer &consumer) {
            return go([&](T t) {
                consumer(t);
                return true;
            });
        }

        Stream<T> takeWhile(const Predicate &predicate) {
            return Stream<T>::of(takeAll(predicate));
        }

        std::vector<T> take(int n) {
            std::vector<T> values;
            values.reserve(n);
            while (_remaining && n-- > 0) {
                values.emplace_back(evalHead());
            }
            return std::move(values);
        }

        std::vector<T> collect() {
            return takeAll([](T) { return true; });
        }

        /**
         * Drop the first element
         * @return Stream containing the rest elements
         */
        Stream<T> &tail() {
            return drop(1);
        }

        /**
         * Take the first element of the Stream
         * @return the first element
         */
        T head() {
            return take(1)[0];
        }

        /**
         * Take the first element of the Stream
         * @return the first element
         */
        T headOr(T backup) {
            auto &&ts = take(1);
            if (ts.empty()) {
                return backup;
            }
            return ts[0];
        }

        void forEach(const Consumer &consumer) {
            peek(consumer);
        }

        template <typename U = T>
        U reduce(U identity, const std::function<U(U, T)> &f) {
            U acc = identity;
            forEach([&](T t) {
                acc = f(acc, t);
            });
            return acc;
        }

        bool any(const Predicate &predicate) {
            bool match = false;
            go([&](T t) {
                if (predicate(t)) {
                    match = true;
                }
                return !match;
            });
            return match;
        }

        bool none(const Predicate &predicate) {
            return any([&](T x) { return !predicate(x); });
        }

        bool all(const Predicate &predicate) {
            return !none(predicate);
        }

        size_t count(const Predicate &predicate) {
            return takeAll(predicate).size();
        }

        size_t count() {
            return collect().size();
        }

    public:
        /**
         * Construct a stream by repeating a value.
         * @param head
         * @return Stream
         */
        static Stream<T> repeat(const T &head) {
            return Stream<T>(head);
        }

        /**
         * Construct a stream by repeatedly applying a function.
         * @param head The first element
         * @param iterator The mapper function
         * @return Stream
         */
        static Stream<T> iterate(T head, const Producer &iterator) {
            return Stream<T>::repeat(head).iterate(iterator);
        }

        /**
         * Construct a stream from a list.
         * @param list The list
         * @return Stream
         */
        static Stream<T> of(const std::vector<T> &list) {
            std::deque<T> d;
            std::copy(list.begin(), list.end(), std::back_inserter(d));
            return Stream<T>(std::move(d));
        }

        /**
         * Construct a stream from a list.
         * @param list The list
         * @return Stream
         */
        static Stream<T> of(const std::list<T> &list) {
            std::deque<T> d;
            std::copy(list.begin(), list.end(), std::back_inserter(d));
            return Stream<T>(std::move(d));
        }

        /**
        * Construct a stream from a list.
        * @param list The list
        * @return Stream
        */
        static Stream<T> of(const std::deque<T> &list) {
            std::deque<T> d;
            std::copy(list.begin(), list.end(), std::back_inserter(d));
            return Stream<T>(std::move(d));
        }
    };
}
