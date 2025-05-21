#pragma once

#include "ctmd_core_convert.hpp"
#include "ctmd_core_type.hpp"

namespace ctmd {
namespace core {
namespace detail {

template <size_t... Is>
[[nodiscard]] constexpr auto
full_extents_tuple(std::index_sequence<Is...>) noexcept {
    return std::tuple{((void)Is, full_extent)...};
}

template <size_t Rank>
[[nodiscard]] constexpr auto full_extents_tuple() noexcept {
    return full_extents_tuple(std::make_index_sequence<Rank>{});
}

template <mdspan_c in_t, typename tails_t, typename... slices_t>
[[nodiscard]] constexpr auto submdspan_from_start_impl(const in_t &in,
                                                       tails_t &&tails,
                                                       slices_t &&...slices) {
    return std::apply(
        [&](auto &&...tails) {
            return submdspan(in, std::forward<slices_t>(slices)..., tails...);
        },
        std::forward<tails_t>(tails));
}

template <mdspan_c in_t, typename heads_t, typename... slices_t>
[[nodiscard]] constexpr auto submdspan_from_last_impl(const in_t &in,
                                                      heads_t &&heads,
                                                      slices_t &&...slices) {
    return std::apply(
        [&](auto &&...heads) {
            return submdspan(in, heads..., std::forward<slices_t>(slices)...);
        },
        std::forward<heads_t>(heads));
}

} // namespace detail

template <typename in_t, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_start(in_t &&in, slices_t &&...slices) noexcept {
    // NOTE: use non-const rin to avoid unnecessary copies
    auto rin = to_mdspan(std::forward<in_t>(in));

    constexpr auto full_exts =
        detail::full_extents_tuple<decltype(rin)::rank() -
                                   sizeof...(slices_t)>();

    return detail::submdspan_from_start_impl(rin, full_exts,
                                             std::forward<slices_t>(slices)...);
}

template <typename in_t, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_last(in_t &&in, slices_t &&...slices) noexcept {
    // NOTE: use non-const rin to avoid unnecessary copies
    auto rin = to_mdspan(std::forward<in_t>(in));

    constexpr auto full_exts =
        detail::full_extents_tuple<decltype(rin)::rank() -
                                   sizeof...(slices_t)>();

    return detail::submdspan_from_last_impl(rin, full_exts,
                                            std::forward<slices_t>(slices)...);
}

} // namespace core
} // namespace ctmd
