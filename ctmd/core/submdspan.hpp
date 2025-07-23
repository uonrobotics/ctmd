#pragma once

#include "./convert.hpp"
#include "./type.hpp"

namespace ctmd {

template <typename InType, typename... slices_t>
[[nodiscard]] inline constexpr auto submdspan(InType &&In,
                                              slices_t &&...slices) noexcept {
    return std::experimental::submdspan(
        core::to_mdspan(std::forward<InType>(In)),
        std::forward<slices_t>(slices)...);
}

namespace core {

template <size_t lspace, size_t rspace, typename InType, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_with_space(InType &&In, slices_t &&...slices) noexcept {
    return [&]<size_t... Is, size_t... Js>(std::index_sequence<Is...>,
                                           std::index_sequence<Js...>) {
        return ctmd::submdspan(std::forward<InType>(In),
                               ((void)Is, ctmd::full_extent)...,
                               std::forward<slices_t>(slices)...,
                               ((void)Js, ctmd::full_extent)...);
    }(std::make_index_sequence<lspace>{}, std::make_index_sequence<rspace>{});
}

template <size_t lspace = 0, typename InType, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_left(InType &&In, slices_t &&...slices) noexcept {
    constexpr size_t rspace =
        to_mdspan_t<InType>::rank() - (lspace + sizeof...(slices_t));

    return submdspan_with_space<lspace, rspace>(
        std::forward<InType>(In), std::forward<slices_t>(slices)...);
}

template <size_t rspace = 0, typename InType, typename... slices_t>
[[nodiscard]] inline constexpr auto
submdspan_from_right(InType &&In, slices_t &&...slices) noexcept {
    constexpr size_t lspace =
        to_mdspan_t<InType>::rank() - (rspace + sizeof...(slices_t));

    return submdspan_with_space<lspace, rspace>(
        std::forward<InType>(In), std::forward<slices_t>(slices)...);
}

} // namespace core
} // namespace ctmd
