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

#include "onedal/table_homogen.hpp"
#include "onedal/table_homogen_impl.hpp"

namespace dal {

table_homogen::table_homogen(const array<byte_t>& data,
                             detail::type_id type_id,
                             std::int64_t row_count,
                             std::int64_t column_count)
    : table(new dal::detail::table_homogen_impl{data, type_id, row_count, column_count}) {}

} // namespace dal