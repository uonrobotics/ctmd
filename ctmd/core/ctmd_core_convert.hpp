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
[[nodiscard]] inline constexpr auto to_const(const in_t &in) noexcept {
    using const_element_t = const typename in_t::element_type;
    return mdspan<const_element_t, typename in_t::extents_type,
                  typename in_t::layout_type,
                  std::experimental::default_accessor<const_element_t>>(
        in.data_handle(), in.mapping(), {});
}

template <mdspan_c in_t>
[[nodiscard]] inline constexpr auto to_layout_stride(const in_t &in) noexcept {
    if constexpr (std::is_same_v<typename in_t::layout_type, layout_stride>) {
        return in;

    } else {
        const auto new_strides =
            [&in]<size_t... Is>(std::index_sequence<Is...>) {
                return std::array<typename in_t::size_type, in_t::rank()>{
                    in.stride(Is)...};
            }(std::make_index_sequence<in_t::rank()>{});

        return mdspan<typename in_t::element_type, typename in_t::extents_type,
                      layout_stride, typename in_t::accessor_type>{
            in.data_handle(),
            layout_stride::mapping{in.extents(), new_strides}};
    }
}

} // namespace core
} // namespace ctmd
