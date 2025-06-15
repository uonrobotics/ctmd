#pragma once

#include "convert.hpp"
#include "type.hpp"

namespace ctmd {
namespace core {

template <mdspan_c in_t, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_left(const in_t &in, slices_t &&...slices) noexcept {
    static_assert(in_t::rank() >= sizeof...(slices_t),
                  "The number of slices must not exceed the rank of the input "
                  "mdspan.");

    return [&in, &slices...]<size_t... Is>(std::index_sequence<Is...>) {
        return ctmd::submdspan(in, std::forward<slices_t>(slices)...,
                               ((void)Is, ctmd::full_extent)...);
    }(std::make_index_sequence<in_t::rank() - sizeof...(slices_t)>{});
}

template <mdspan_c in_t, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_right(const in_t &in, slices_t &&...slices) noexcept {
    static_assert(in_t::rank() >= sizeof...(slices_t),
                  "The number of slices must not exceed the rank of the input "
                  "mdspan.");

    return [&in, &slices...]<size_t... Is>(std::index_sequence<Is...>) {
        return ctmd::submdspan(in, ((void)Is, ctmd::full_extent)...,
                               std::forward<slices_t>(slices)...);
    }(std::make_index_sequence<in_t::rank() - sizeof...(slices_t)>{});
}

} // namespace core
} // namespace ctmd
