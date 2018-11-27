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
#include <covscript/extension.hpp>

static cs::namespace_t streams_ext=cs::make_shared_namespace<cs::name_space>();

#define NO_LIMITS (-1)

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

    void init(name_space* ns) {
        (*ns)
        .add_var("of", make_cni(of, true))
        .add_var("for_each", make_cni(for_each, true))
        .add_var("peek", make_cni(peek, true))
        .add_var("count", make_cni(count, true))
        .add_var("skip", make_cni(skip, true))
        .add_var("reverse", make_cni(reverse, true))
        .add_var("filter", make_cni(filter, true))
        .add_var("any_match", make_cni(any_match, true))
        .add_var("all_match", make_cni(all_match, true))
        .add_var("none_match", make_cni(none_match, true))
        .add_var("find_first", make_cni(find_first, true))
        .add_var("find_any", make_cni(find_any, true))
        .add_var("limit", make_cni(limit, true))
        .add_var("map", make_cni(map, true))
        .add_var("reduce", make_cni(reduce, true))
        .add_var("to_list", make_cni(to_list, true));
        (*streams_ext)
        .add_var("for_each", make_cni(for_each, true))
        .add_var("peek", make_cni(peek, true))
        .add_var("count", make_cni(count, true))
        .add_var("skip", make_cni(skip, true))
        .add_var("reverse", make_cni(reverse, true))
        .add_var("filter", make_cni(filter, true))
        .add_var("any_match", make_cni(any_match, true))
        .add_var("all_match", make_cni(all_match, true))
        .add_var("none_match", make_cni(none_match, true))
        .add_var("find_first", make_cni(find_first, true))
        .add_var("find_any", make_cni(find_any, true))
        .add_var("limit", make_cni(limit, true))
        .add_var("map", make_cni(map, true))
        .add_var("reduce", make_cni(reduce, true))
        .add_var("to_list", make_cni(to_list, true));
    }
}

namespace cs_impl {
    template<>
    cs::namespace_t &get_ext<streams::streams_holder>() {
        return streams_ext;
    }

    template<>
    constexpr const char *get_name_of_type<streams::streams_holder>() {
        return "cs::streams";
    }
}

void cs_extension_main(cs::name_space* ns) {
    streams::init(ns);
}