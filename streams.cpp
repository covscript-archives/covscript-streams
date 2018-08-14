/*
* Covariant Script Streams API Extension
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Kiva
* Email: libkernelpanic@gmail.com
* Github: https://github.com/imkiva
*/
#include <covscript/cni.hpp>
#include <covscript/extension.hpp>

#define NO_LIMITS (-1)

static cs::extension streams_ext;
static cs::extension_t streams_ext_shared = cs::make_shared_namespace(streams_ext);

namespace streams {
    using namespace cs;

    struct streams_holder {
        cs::list collection;
        int limit;

        size_t get_available_size() {
            size_t available = collection.size();
            if (limit != NO_LIMITS) {
                available = std::min(static_cast<size_t>(limit), available);
                limit = NO_LIMITS;
            }
            return available;
        }

        void for_each_limited(const std::function<void(const var &)> &callback) {
            auto available = get_available_size();

            auto iterator = collection.begin();
            while (available-- > 0) {
                callback(*iterator++);
            }
        }
    };

    template<typename... ArgsT>
    var invoke(const var &func, ArgsT &&... _args) {
        if (func.type() == typeid(callable)) {
            vector args{std::forward<ArgsT>(_args)...};
            return func.const_val<callable>().call(args);
        } else if (func.type() == typeid(object_method)) {
            const auto &om = func.const_val<object_method>();
            vector args{om.object, std::forward<ArgsT>(_args)...};
            return om.callable.const_val<callable>().call(args);
        }
        return null_pointer;
    }

    streams_holder of(const cs::list &list) {
        return streams_holder{list, NO_LIMITS};
    }

    void for_each(streams_holder &holder, const var &predicate) {
        holder.for_each_limited([&predicate](const var &item) {
            invoke(predicate, item);
        });
    }

    streams_holder peek(streams_holder &holder, const var &predicate) {
        for_each(holder, predicate);
        return holder;
    }

    streams_holder filter(streams_holder &holder, const var &predicate) {
        cs::list new_list;
        holder.for_each_limited([&predicate, &new_list](const var &item) {
            if (invoke(predicate, item).const_val<cs::boolean>()) {
                new_list.push_back(item);
            }
        });
        std::swap(holder.collection, new_list);
        return holder;
    }

    streams_holder limit(streams_holder &holder, number n) {
        holder.limit = static_cast<int>(n);
        return holder;
    }

    cs::list to_list(streams_holder &holder) {
        return holder.collection;
    }

    streams_holder skip(streams_holder &holder, number n) {
        auto count = std::min(static_cast<size_t>(n), holder.get_available_size());
        for (int i = 0; i < count; ++i) {
            holder.collection.pop_front();
        }
        return holder;
    }

    streams_holder reverse(streams_holder &holder) {
        holder.collection.reverse();
        return holder;
    }

    number count(streams_holder &holder) {
        return number(holder.get_available_size());
    }

    cs::boolean any_match(streams_holder &holder, const var &predicate) {
        for (const var &item : holder.collection) {
            if (invoke(predicate, item).const_val<cs::boolean>()) {
                return true;
            }
        }
        return false;
    }

    cs::boolean all_match(streams_holder &holder, const var &predicate) {
        for (const var &item : holder.collection) {
            if (!invoke(predicate, item).const_val<cs::boolean>()) {
                return false;
            }
        }
        return true;
    }

    cs::boolean none_match(streams_holder &holder, const var &predicate) {
        return !any_match(holder, predicate);
    }

    var find_first(streams_holder &holder) {
        if (!holder.collection.empty()) {
            return holder.collection.front();
        }
        return null_pointer;
    }

    var find_any(streams_holder &holder) {
        return find_first(holder);
    }

    streams_holder map(streams_holder &holder, const var &mapper) {
        cs::list new_list;
        holder.for_each_limited([&mapper, &new_list](const var &item) {
            new_list.push_back(invoke(mapper, item));
        });
        std::swap(holder.collection, new_list);
        return holder;
    }

    var reduce(streams_holder &holder, const var &identity, const var &accumulator) {
        var acc = identity;
        holder.for_each_limited([&acc, &accumulator](const var &item) {
            acc = invoke(accumulator, acc, item);
        });
        return acc;
    }

    void init() {
        streams_ext.add_var("of", var::make_protect<callable>(cni(of), true));
        streams_ext.add_var("for_each", var::make_protect<callable>(cni(for_each), true));
        streams_ext.add_var("peek", var::make_protect<callable>(cni(peek), true));
        streams_ext.add_var("count", var::make_protect<callable>(cni(count), true));
        streams_ext.add_var("skip", var::make_protect<callable>(cni(skip), true));
        streams_ext.add_var("reverse", var::make_protect<callable>(cni(reverse), true));
        streams_ext.add_var("filter", var::make_protect<callable>(cni(filter), true));
        streams_ext.add_var("any_match", var::make_protect<callable>(cni(any_match), true));
        streams_ext.add_var("all_match", var::make_protect<callable>(cni(all_match), true));
        streams_ext.add_var("none_match", var::make_protect<callable>(cni(none_match), true));
        streams_ext.add_var("find_first", var::make_protect<callable>(cni(find_first), true));
        streams_ext.add_var("find_any", var::make_protect<callable>(cni(find_any), true));
        streams_ext.add_var("limit", var::make_protect<callable>(cni(limit), true));
        streams_ext.add_var("map", var::make_protect<callable>(cni(map), true));
        streams_ext.add_var("reduce", var::make_protect<callable>(cni(reduce), true));
        streams_ext.add_var("to_list", var::make_protect<callable>(cni(to_list), true));
    }
}

namespace cs_impl {
    template<>
    cs::extension_t &get_ext<streams::streams_holder>() {
        return streams_ext_shared;
    }

    template<>
    constexpr const char *get_name_of_type<streams::streams_holder>() {
        return "cs::streams";
    }
}

cs::extension *cs_extension() {
    streams::init();
    return &streams_ext;
}