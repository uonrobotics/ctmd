#pragma once

#include "convert.hpp"
#include "type.hpp"

namespace ctmd {
namespace core {

template <size_t lspace, size_t rspace, mdspan_c in_t, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_with_space(const in_t &in, slices_t &&...slices) noexcept {
    static_assert(in_t::rank() == lspace + sizeof...(slices_t) + rspace,
                  "The number of slices must match the rank of the input "
                  "mdspan.");

    return [&]<size_t... Is, size_t... Js>(std::index_sequence<Is...>,
                                           std::index_sequence<Js...>) {
        return ctmd::submdspan(in, ((void)Is, ctmd::full_extent)...,
                               std::forward<slices_t>(slices)...,
                               ((void)Js, ctmd::full_extent)...);
    }(std::make_index_sequence<lspace>{}, std::make_index_sequence<rspace>{});
}

template <size_t lspace = 0, mdspan_c in_t, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_left(const in_t &in, slices_t &&...slices) noexcept {
    static_assert(
        in_t::rank() >= lspace + sizeof...(slices_t),
        "The number of slices must not exceed the rank of the input mdspan.");

    constexpr size_t rspace = in_t::rank() - (lspace + sizeof...(slices_t));
    return submdspan_with_space<lspace, rspace>(
        in, std::forward<slices_t>(slices)...);
}

template <size_t rspace = 0, mdspan_c in_t, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_right(const in_t &in, slices_t &&...slices) noexcept {
    static_assert(
        in_t::rank() >= rspace + sizeof...(slices_t),
        "The number of slices must not exceed the rank of the input mdspan.");

    constexpr size_t lspace = in_t::rank() - (rspace + sizeof...(slices_t));
    return submdspan_with_space<lspace, rspace>(
        in, std::forward<slices_t>(slices)...);
}

} // namespace core
} // namespace ctmd
