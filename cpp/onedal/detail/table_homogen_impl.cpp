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

#include "onedal/detail/table_homogen_impl.hpp"

using std::int32_t;
using std::int64_t;

namespace dal {
namespace detail {

float* table_homogen_impl::get_data_ptr(const table_range& r, float*) const {
    return get_slice_impl<float>(r);
}

double* table_homogen_impl::get_data_ptr(const table_range& r, double*) const {
    return get_slice_impl<double>(r);
}

int32_t* table_homogen_impl::get_data_ptr(const table_range& r, int32_t*) const {
    return get_slice_impl<int32_t>(r);
}

void table_homogen_impl::release_data_ptr(const table_range& r, float* data, bool need_copy_ptr) {
    release_slice_impl(r, data, need_copy_ptr);
}

void table_homogen_impl::release_data_ptr(const table_range& r, double* data, bool need_copy_ptr) {
    release_slice_impl(r, data, need_copy_ptr);
}

void table_homogen_impl::release_data_ptr(const table_range& r, int32_t* data, bool need_copy_ptr) {
    release_slice_impl(r, data, need_copy_ptr);
}

struct index_pair {
    int64_t x;
    int64_t y;
};

struct slice_info {
    index_pair size;
    index_pair offset;

    int64_t ld_data;

    slice_info(const table_range& r, int64_t num_rows, int64_t num_cols)
        : size({ r.columns.get_num_of_elements(num_cols), r.rows.get_num_of_elements(num_rows) }),
          offset({ r.columns.start_idx, r.rows.start_idx }),
          ld_data(num_cols){
    }
};

bool need_allocate_ptr(const slice_info& info) {
    return (info.size.y > 1 && info.size.x != info.ld_data);
}

template <typename DataType>
DataType* table_homogen_impl::get_slice_impl(const table_range& r) const {
    slice_info info { r, get_num_rows(), get_num_cols() };

    if (type_rt_ == dal::detail::make_type_rt<DataType>()) {
        DataType* data = reinterpret_cast<DataType*>(data_bytes_);

        if (need_allocate_ptr(info) == true) {
            DataType* out_array = new DataType[info.size.x * info.size.y];
            for (int y = 0; y < info.size.y; y++) {
                for (int x = 0; x < info.size.x; x++) {
                    const int64_t data_x = x + info.offset.x;
                    const int64_t data_y = y + info.offset.y;
                    const int64_t ld_out = info.size.x;
                    const int64_t ld_data = info.ld_data;

                    out_array[y*ld_out + x] = data[data_y*ld_data + data_x];
                }
            }
            return out_array;
        }
        else {
            return data + info.offset.x + info.offset.y*info.ld_data;
        }
    }
    // TODO: implement conversion
    return nullptr;
}

template <typename DataType>
void table_homogen_impl::release_slice_impl(const table_range& r, DataType* data, bool need_copy_ptr) {
    slice_info info { r, get_num_rows(), get_num_cols() };

    if (type_rt_ == dal::detail::make_type_rt<DataType>()) {
        const bool need_release_ptr = need_allocate_ptr(info);

        if (need_copy_ptr && need_release_ptr) {
            DataType* dest = reinterpret_cast<DataType*>(data_bytes_);

            for (int y = 0; y < info.size.y; y++) {
                for (int x = 0; x < info.size.x; x++) {
                    const int64_t data_x = x + info.offset.x;
                    const int64_t data_y = y + info.offset.y;
                    const int64_t ld_out = info.size.x;
                    const int64_t ld_data = info.ld_data;

                    dest[data_y*ld_data + data_x] = data[y*ld_out + x];
                }
            }
        }

        if (need_release_ptr) {
            delete[] data;
            data = nullptr;
        }
    }
    // TODO: implement conversion
}

} // namespace detail
} // namespace dal
