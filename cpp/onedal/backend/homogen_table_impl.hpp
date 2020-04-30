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

#include "onedal/common_helpers.hpp"
#include "onedal/table_metadata.hpp"

namespace dal::backend {

class homogen_table_impl {
public:
    template <typename DataType>
    homogen_table_impl(std::int64_t N, std::int64_t p, const DataType* data_pointer, data_layout layout)
        : row_count_(N),
          column_count_(p),
          finfo_(feature_info{ make_data_type<DataType>() }),
          meta_(table_metadata{ p, finfo_, layout }) {
        data_.reset_not_owning(reinterpret_cast<const byte_t*>(data_pointer),
                               N * p * sizeof(DataType));
    }

    std::int64_t get_column_count() const {
        return column_count_;
    }

    std::int64_t get_row_count() const {
        return row_count_;
    }

    const table_metadata& get_metadata() const {
        return meta_;
    }

    template <typename DataType>
    const DataType* get_data() const {
        return reinterpret_cast<const DataType*>(data_.get_data());
    }

    void pull_rows(array<float>& a, const range& r) const {
        pull_rows_impl(a, r);
    }

    void pull_rows(array<double>& a, const range& r) const {
        pull_rows_impl(a, r);
    }

    void pull_rows(array<std::int32_t>& a, const range& r) const {
        pull_rows_impl(a, r);
    }

    void push_back_rows(const array<float>&, const range&) {

    }

    void push_back_rows(const array<double>&, const range&) {

    }

    void push_back_rows(const array<std::int32_t>&, const range&) {

    }

private:
    template <typename T>
    void pull_rows_impl(array<T>&, const range&) const;

private:
    std::int64_t row_count_;
    std::int64_t column_count_;
    feature_info finfo_;
    table_metadata meta_;
    array<byte_t> data_;
};

} // namespace dal::backend
