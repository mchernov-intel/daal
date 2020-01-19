#pragma once
#include "onedal/common.hpp"
#include "onedal/detail/common.hpp"

namespace dal {
namespace data_management {

/*template <typename T>
class Array {};*/

namespace detail {
class Slicable;
} // namespace detail

// TODO: think of how to semantically distinguish 
// "simple" and "complex" types
class Slice : public Base {
public:
    using PimplDataOwner = dal::detail::Pimpl<detail::Slicable>;

public:
    Slice(const PimplDataOwner& impl, Range rows_range, Range cols_range)
        : _data_owner(impl),
          _rows(rows_range),
          _cols(cols_range)
    { }

    std::int32_t get_num_rows() const noexcept;
    std::int32_t get_num_cols() const noexcept;

    Slice& row(std::int32_t idx) {
        _rows = Range{idx, idx+1}; 
        return *this; 
    }
    Slice& col(std::int32_t idx) {
        _cols = Range{idx, idx+1}; 
        return *this; 
    }

    Slice& rows(Range r) {
        _rows = r;
        return *this;
    }

    Slice& cols(Range r) {
        _cols = r;
        return *this;
    }

    /*template <typename T>
    Array<T> to_array() const;*/
private:
    PimplDataOwner _data_owner;
    Range _rows;
    Range _cols;
};

} // namespace data_management   
} // namespace dal
