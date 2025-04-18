#pragma once

#include "ctmd_core_type.hpp"

namespace ctmd {
namespace core {

template <md_c in_t>
[[nodiscard]] inline constexpr auto to_mdspan(in_t &in) noexcept {
    if constexpr (mdspan_c<in_t>) {
        return in;

    } else if constexpr (mdarray_c<in_t>) {
        return in.to_mdspan();

    } else {
        static_assert(false, "Invalid type");
    }
}

template <mdspan_c in_t>
[[nodiscard]] inline constexpr auto to_const(const in_t &in = in_t{}) noexcept {
    using const_element_t = const typename in_t::element_type;
    return mdspan<const_element_t, typename in_t::extents_type,
                  typename in_t::layout_type,
                  std::experimental::default_accessor<const_element_t>>(
        in.data_handle(), in.mapping(), {});
}

} // namespace core
} // namespace ctmd
