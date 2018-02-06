/*
 * Covariant Script Streams API Extension
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2018 Kiva
 * Email: libkernelpanic@gmail.com
 * Github: https://github.com/imkiva
 */
#include <covscript/cni.hpp>
#include <covscript/extension.hpp>

static cs::extension streams_ext;
static cs::extension_t streams_ext_shared = cs::make_shared_extension(streams_ext);

namespace streams {
    using namespace cs;

    struct streams_holder {
        cs::list collection;
    };

    streams_holder of(const cs::list &list) {
        return streams_holder{list};
    }

    streams_holder for_each(streams_holder &holder, callable &callable) {
        std::for_each(holder.collection.begin(), holder.collection.end(),
            [&callable](cs_impl::any &item) {
                vector args = {item};
                callable.call(args);
            });
        return holder;
    }

    streams_holder filter(streams_holder &holder, callable &predicate) {
        cs::list new_list;
        std::for_each(holder.collection.begin(), holder.collection.end(),
                      [&predicate, &new_list](cs_impl::any &item) {
                          vector args = {item};
                          if (predicate.call(args).const_val<boolean>()) {
                              new_list.push_back(item);
                          }
                      });
        holder.collection = new_list;
        return holder;
    }

    streams_holder limit(streams_holder &holder) {
        return holder;
    }

    cs::list to_list(streams_holder &holder) {
        return holder.collection;
    }

    streams_holder skip(streams_holder &holder, number n) {
        auto count = std::min(static_cast<int>(n),
                              static_cast<int>(holder.collection.size()));
        for (int i = 0; i < count; ++i) {
            holder.collection.pop_front();
        }
        return holder;
    }

    streams_holder reverse(streams_holder &holder) {
        holder.collection.reverse();
        return holder;
    }

    number count(const streams_holder &holder) {
        return number(holder.collection.size());
    }

    boolean any_match(streams_holder &holder) {
        return true;
    }

    boolean all_match(streams_holder &holder) {
        return true;
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

    void init() {
        streams_ext.add_var("of", var::make_protect<callable>(cni(of), true));
        streams_ext.add_var("for_each", var::make_protect<callable>(cni(for_each), true));
        streams_ext.add_var("count", var::make_protect<callable>(cni(count), true));
        streams_ext.add_var("skip", var::make_protect<callable>(cni(skip), true));
        streams_ext.add_var("reverse", var::make_protect<callable>(cni(reverse), true));
        streams_ext.add_var("filter", var::make_protect<callable>(cni(filter), true));
        streams_ext.add_var("any_match", var::make_protect<callable>(cni(any_match), true));
        streams_ext.add_var("all_match", var::make_protect<callable>(cni(all_match), true));
        streams_ext.add_var("find_first", var::make_protect<callable>(cni(find_first), true));
        streams_ext.add_var("find_any", var::make_protect<callable>(cni(find_any), true));
        streams_ext.add_var("limit", var::make_protect<callable>(cni(limit), true));
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

