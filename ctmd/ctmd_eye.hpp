#pragma once

#include "ctmd_empty.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t>
    requires(in_t::rank() == 2)
inline constexpr void eye_impl(const in_t &in) noexcept {
    for (size_t i = 0; i < in.extent(0); ++i) {
        for (size_t j = 0; j < in.extent(1); ++j) {
            in(i, j) = (i == j) ? 1 : 0;
        }
    }
}

} // namespace detail

template <typename InType>
inline constexpr void eye(InType &&In,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_mdspan(std::forward<InType>(In));

    static_assert(decltype(in)::rank() >= 2,
                  "eye function requires at least 2D input.");

    core::batch(
        [](auto &&...elems) {
            detail::eye_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in}, std::tuple{core::slice_from_last<2>(in.extents())},
        std::tuple{}, mpmode);
}

template <typename T, extents_c extents_t>
[[nodiscard]] inline constexpr auto
eye(const extents_t &extents = extents_t{},
    const MPMode mpmode = MPMode::NONE) noexcept {
    auto out = ctmd::empty<T>(extents);
    ctmd::eye(out, mpmode);
    return out;
}

} // namespace ctmd
