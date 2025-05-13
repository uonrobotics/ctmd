#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

template <int64_t Axis, md_c in_t>
[[nodiscard]] inline constexpr auto expand_dims(in_t &in) noexcept {
    constexpr size_t axis =
        static_cast<size_t>(((Axis % (in_t::rank() + 1)) + (in_t::rank() + 1)) %
                            (in_t::rank() + 1));

    const auto in_span = core::to_mdspan(in);
    using in_span_t = decltype(in_span);

    const auto new_extents =
        [&in_span]<size_t... Is>(std::index_sequence<Is...>) {
            return extents<
                typename in_span_t::index_type,
                (Is < axis
                     ? in_span_t::static_extent(Is)
                     : (Is == axis ? 1 : in_span_t::static_extent(Is - 1)))...>{
                (Is < axis ? in_span.extent(Is)
                           : (Is == axis ? 1 : in_span.extent(Is - 1)))...};
        }(std::make_index_sequence<in_span_t::rank() + 1>{});

    const auto new_strides = [&in_span]<size_t... Is>(
                                 std::index_sequence<Is...>) {
        return std::array<typename in_span_t::size_type, in_span_t::rank() + 1>{
            (Is < axis ? in_span.stride(Is)
                       : (Is == axis ? 1 : in_span.stride(Is - 1)))...};
    }(std::make_index_sequence<in_span_t::rank() + 1>{});

    return mdspan<typename in_span_t::element_type,
                  std::remove_const_t<decltype(new_extents)>, layout_stride,
                  typename in_span_t::accessor_type>{
        in_span.data_handle(),
        layout_stride::mapping{new_extents, new_strides}};
}

} // namespace ctmd
