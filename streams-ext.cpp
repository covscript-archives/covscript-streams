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
* Copyright (C) 2019 Kiva
* Email: libkernelpanic@gmail.com
* Github: https://github.com/imkiva
*/
#include <covscript/dll.hpp>
#include "stream.hpp"

static cs::namespace_t streams_ext = cs::make_shared_namespace<cs::name_space>();

#define NO_LIMITS (-1)

namespace streams {
    using namespace cs;
    using namespace imkiva;

    using StreamType = imkiva::Stream<cs_impl::any>;

    StreamType of(const cs::list &list) {
        return StreamType::of(list);
    }

    StreamType repeat(const var &element) {
        return StreamType::repeat(element);
    }

    StreamType iterate(const var &head, const var &iterator) {
        return StreamType::iterate(head, [=](const var &v) {
            return invoke(iterator, v);
        });
    }

    void for_each(StreamType &holder, const var &predicate) {
        holder.forEach([&predicate](const var &item) {
            invoke(predicate, item);
        });
    }

    StreamType peek(StreamType &holder, const var &predicate) {
        return holder.peek([&predicate](const var &item) {
            invoke(predicate, item);
        });
    }

    StreamType filter(StreamType &holder, const var &predicate) {
        return holder.filter([predicate](const var &v) {
            return invoke(predicate, v).val<bool>();
        });
    }

    StreamType map(StreamType &holder, const var &mapper) {
        return holder.map([mapper](const var &v) {
            return invoke(mapper, v);
        });
    }

    cs::list to_list(StreamType &holder) {
        auto &&v = holder.collect();
        cs::list result;
        std::copy(v.begin(), v.end(), std::back_inserter(result));
        return std::move(result);
    }

    StreamType skip(StreamType &holder, number n) {
        return holder.drop(static_cast<int>(n));
    }

    number count(StreamType &holder) {
        return static_cast<number>(holder.count());
    }

    cs::boolean any_match(StreamType &holder, const var &predicate) {
        return holder.any([&predicate](const var &v) {
            return invoke(predicate, v).val<bool>();
        });
    }

    cs::boolean all_match(StreamType &holder, const var &predicate) {
        return holder.all([&predicate](const var &v) {
            return invoke(predicate, v).val<bool>();
        });
    }

    cs::boolean none_match(StreamType &holder, const var &predicate) {
        return holder.none([&predicate](const var &v) {
            return invoke(predicate, v).val<bool>();
        });
    }

    var find_first(StreamType &holder) {
        return holder.headOr(null_pointer);
    }

    var find_any(StreamType &holder) {
        return find_first(holder);
    }

    var reduce(StreamType &holder, const var &identity, const var &accumulator) {
        return holder.reduce<cs_impl::any>(identity,
            [&accumulator](const var &acc, const var &item) {
                return invoke(accumulator, acc, item);
            });
    }

    void init(name_space *ns) {
        (*ns)
            .add_var("of", make_cni(of, true))
            .add_var("for_each", make_cni(for_each, true))
            .add_var("peek", make_cni(peek, true))
            .add_var("count", make_cni(count, true))
            .add_var("skip", make_cni(skip, true))
            .add_var("filter", make_cni(filter, true))
            .add_var("any_match", make_cni(any_match, true))
            .add_var("all_match", make_cni(all_match, true))
            .add_var("none_match", make_cni(none_match, true))
            .add_var("find_first", make_cni(find_first, true))
            .add_var("find_any", make_cni(find_any, true))
            .add_var("map", make_cni(map, true))
            .add_var("reduce", make_cni(reduce, true))
            .add_var("to_list", make_cni(to_list, true));
        (*streams_ext)
            .add_var("for_each", make_cni(for_each, true))
            .add_var("peek", make_cni(peek, true))
            .add_var("count", make_cni(count, true))
            .add_var("skip", make_cni(skip, true))
            .add_var("filter", make_cni(filter, true))
            .add_var("any_match", make_cni(any_match, true))
            .add_var("all_match", make_cni(all_match, true))
            .add_var("none_match", make_cni(none_match, true))
            .add_var("find_first", make_cni(find_first, true))
            .add_var("find_any", make_cni(find_any, true))
            .add_var("map", make_cni(map, true))
            .add_var("reduce", make_cni(reduce, true))
            .add_var("to_list", make_cni(to_list, true));
    }
}

namespace cs_impl {
    template <>
    cs::namespace_t &get_ext<streams::StreamType>() {
        return streams_ext;
    }

    template <>
    constexpr const char *get_name_of_type<streams::StreamType>() {
        return "cs::lazy_stream";
    }
}

void cs_extension_main(cs::name_space *ns) {
    streams::init(ns);
}
