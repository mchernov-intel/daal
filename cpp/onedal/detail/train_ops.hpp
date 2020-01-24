/*******************************************************************************
 * Copyright 2020 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

#pragma once

#include "onedal/execution_context.hpp"

namespace dal {
namespace detail {

template <typename Params, typename Tag>
struct train_ops;

template <typename Context, typename Params, typename Head, typename... Tail>
auto train_dispatch_by_input(const Context& ctx,
                             const Params& params,
                             Head&& head, Tail&&... tail) {
    using tag_t = typename Params::tag_t;
    using ops_t = train_ops<Params, tag_t>;
    using input_t = typename ops_t::input_t;

    if constexpr (std::is_same_v<std::decay_t<Head>, input_t>) {
        return ops_t()(ctx, params, std::forward<Head>(head),
                                    std::forward<Tail>(tail)...);
    }
    else {
        const auto input = input_t { std::forward<Head>(head),
                                    std::forward<Tail>(tail)... };
        return ops_t()(ctx, params, input);
    }
};

template <typename Head, typename... Tail>
auto train_dispatch_by_ctx(Head&& head, Tail&&... tail) {
    using tag_t = typename std::decay_t<Head>::tag_t;
    if constexpr (std::is_same_v<tag_t, detail::execution_context_tag>) {
        return train_dispatch_by_input(head, std::forward<Tail>(tail)...);
    }
    else {
        return train_dispatch_by_input(default_execution_context(),
                                    std::forward<Head>(head),
                                    std::forward<Tail>(tail)...);
    }
};

} // namespace detail
} // namespace dal
