#pragma once

#include "ctmd_core_convert.hpp"
#include "ctmd_core_type.hpp"

namespace ctmd {
namespace core {

namespace detail {

template <size_t Count> [[nodiscard]] constexpr auto full_extents() noexcept {
    return []<size_t... Is>(std::index_sequence<Is...>) {
        return std::integer_sequence<full_extent_t,
                                     ((void)Is, full_extent)...>{};
    }(std::make_index_sequence<Count>{});
}

template <md_c in_t, full_extent_t... Extents, typename... SliceSpecifiers>
[[nodiscard]] inline constexpr auto
submdspan_from_start_impl(in_t &in,
                          std::integer_sequence<full_extent_t, Extents...>,
                          SliceSpecifiers... slices) noexcept {
    return submdspan(core::to_mdspan(in), slices..., Extents...);
}

template <md_c in_t, full_extent_t... Extents, typename... SliceSpecifiers>
[[nodiscard]] inline constexpr auto
submdspan_from_last_impl(in_t &in,
                         std::integer_sequence<full_extent_t, Extents...>,
                         SliceSpecifiers... slices) noexcept {
    return submdspan(core::to_mdspan(in), Extents..., slices...);
}

} // namespace detail

template <md_c in_t, typename... SliceSpecifiers>
[[nodiscard]] inline constexpr auto
submdspan_from_start(in_t &in, SliceSpecifiers... slices) noexcept {
    return detail::submdspan_from_start_impl(
        in, detail::full_extents<in_t::rank() - sizeof...(SliceSpecifiers)>(),
        slices...);
}

template <md_c in_t, typename... SliceSpecifiers>
[[nodiscard]] inline constexpr auto
submdspan_from_last(in_t &in, SliceSpecifiers... slices) noexcept {
    return detail::submdspan_from_last_impl(
        in, detail::full_extents<in_t::rank() - sizeof...(SliceSpecifiers)>(),
        slices...);
}

template <size_t URank, md_c in_t>
[[nodiscard]] inline constexpr auto submdspan_unit(in_t &in) {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        auto in_span = core::to_mdspan(in);
        return submdspan_from_start(in_span, ((void)Is, 0)...);
    }(std::make_index_sequence<std::remove_cvref_t<in_t>::rank() - URank>{});
}

} // namespace core
} // namespace ctmd
