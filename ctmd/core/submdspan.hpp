#pragma once

#include "convert.hpp"
#include "type.hpp"

namespace ctmd {
namespace core {

template <typename InType, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_left(InType &&In, slices_t &&...slices) noexcept {
    // NOTE: use non-const rin to avoid unnecessary copies
    auto in = to_mdspan(std::forward<InType>(In));

    static_assert(decltype(in)::rank() >= sizeof...(slices_t),
                  "The number of slices must not exceed the rank of the input "
                  "mdspan.");

    return [&in, &slices...]<size_t... Is>(std::index_sequence<Is...>) {
        return ctmd::submdspan(in, std::forward<slices_t>(slices)...,
                               ((void)Is, ctmd::full_extent)...);
    }(std::make_index_sequence<decltype(in)::rank() - sizeof...(slices_t)>{});
}

template <typename InType, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_right(InType &&In, slices_t &&...slices) noexcept {
    // NOTE: use non-const rin to avoid unnecessary copies
    auto in = to_mdspan(std::forward<InType>(In));

    static_assert(decltype(in)::rank() >= sizeof...(slices_t),
                  "The number of slices must not exceed the rank of the input "
                  "mdspan.");

    return [&in, &slices...]<size_t... Is>(std::index_sequence<Is...>) {
        return ctmd::submdspan(in, ((void)Is, ctmd::full_extent)...,
                               std::forward<slices_t>(slices)...);
    }(std::make_index_sequence<decltype(in)::rank() - sizeof...(slices_t)>{});
}

} // namespace core
} // namespace ctmd
