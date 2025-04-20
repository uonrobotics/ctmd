#pragma once

#include "ctmd_core_convert.hpp"
#include "ctmd_core_type.hpp"

namespace ctmd {
namespace core {
namespace detail {

template <size_t... Is>
[[nodiscard]] constexpr auto
full_extents_tuple(std::index_sequence<Is...>) noexcept {
    return std::make_tuple(((void)Is, full_extent)...);
}

template <size_t Count> [[nodiscard]] constexpr auto full_extents() noexcept {
    return full_extents_tuple(std::make_index_sequence<Count>{});
}

template <md_c in_t, typename Tuple, typename... SliceSpecifiers>
[[nodiscard]] constexpr auto
submdspan_from_start_impl(in_t &in, Tuple &&extents_tail,
                          SliceSpecifiers &&...slices) {
    return std::apply(
        [&](auto &&...tail_slices) {
            return submdspan(core::to_mdspan(in),
                             std::forward<SliceSpecifiers>(slices)...,
                             tail_slices...);
        },
        std::forward<Tuple>(extents_tail));
}

template <md_c in_t, typename Tuple, typename... SliceSpecifiers>
[[nodiscard]] constexpr auto
submdspan_from_last_impl(in_t &in, Tuple &&extents_head,
                         SliceSpecifiers &&...slices) {
    return std::apply(
        [&](auto &&...head_slices) {
            return submdspan(core::to_mdspan(in), head_slices...,
                             std::forward<SliceSpecifiers>(slices)...);
        },
        std::forward<Tuple>(extents_head));
}

} // namespace detail

template <md_c in_t, typename... SliceSpecifiers>
[[nodiscard]] inline constexpr auto
submdspan_from_start(in_t &in, SliceSpecifiers... slices) noexcept {
    constexpr size_t rest_rank = in_t::rank() - sizeof...(SliceSpecifiers);
    return detail::submdspan_from_start_impl(
        in, detail::full_extents<rest_rank>(),
        std::forward<SliceSpecifiers>(slices)...);
}

template <md_c in_t, typename... SliceSpecifiers>
[[nodiscard]] inline constexpr auto
submdspan_from_last(in_t &in, SliceSpecifiers... slices) noexcept {
    constexpr size_t rest_rank = in_t::rank() - sizeof...(SliceSpecifiers);
    return detail::submdspan_from_last_impl(
        in, detail::full_extents<rest_rank>(),
        std::forward<SliceSpecifiers>(slices)...);
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
