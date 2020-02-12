/*******************************************************************************
 * Copyright 2014-2019 Intel Corporation
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

#include "onedal/backend/dispatcher_dp.hpp"
#include "onedal/decomposition/pca/train_types.hpp"

namespace dal {
namespace decomposition {
namespace pca {
namespace backend {

template <typename Float, typename Method>
struct train_kernel_gpu {
  train_result operator()(const dal::backend::context_gpu& ctx,
                          const descriptor_base& params,
                          const train_input& input) const;
};

} // namespace detail
} // namespace pca
} // namespace decomposition
} // namespace dal
