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
        [&](auto &&...tail) {
            return submdspan(core::to_mdspan(in),
                             std::forward<slices_t>(slices)..., tail...);
        },
        std::forward<tails_t>(tails));
}

template <mdspan_c in_t, typename heads_t, typename... slices_t>
[[nodiscard]] constexpr auto submdspan_from_last_impl(const in_t &in,
                                                      heads_t &&heads,
                                                      slices_t &&...slices) {
    return std::apply(
        [&](auto &&...head) {
            return submdspan(core::to_mdspan(in), head...,
                             std::forward<slices_t>(slices)...);
        },
        std::forward<heads_t>(heads));
}

} // namespace detail

template <mdspan_c in_t, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_start(const in_t &in, slices_t... slices) noexcept {
    return detail::submdspan_from_start_impl(
        in, detail::full_extents_tuple<in_t::rank() - sizeof...(slices_t)>(),
        std::forward<slices_t>(slices)...);
}

template <mdspan_c in_t, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_last(const in_t &in, slices_t... slices) noexcept {
    return detail::submdspan_from_last_impl(
        in, detail::full_extents_tuple<in_t::rank() - sizeof...(slices_t)>(),
        std::forward<slices_t>(slices)...);
}

template <size_t URank, mdspan_c in_t>
[[nodiscard]] inline constexpr auto submdspan_unit(const in_t &in) {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        auto in_span = core::to_mdspan(in);
        return submdspan_from_start(in_span, ((void)Is, 0)...);
    }(std::make_index_sequence<std::remove_cvref_t<in_t>::rank() - URank>{});
}

} // namespace core
} // namespace ctmd
