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

inline constexpr void
eye_to(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::eye_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<2>{}, mpmode,
        core::to_mdspan(std::forward<decltype(In)>(In)));
}

template <typename dtype, extents_c extents_t>
[[nodiscard]] inline constexpr auto
eye(const extents_t &extents = extents_t{},
    const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    auto out = ctmd::empty<dtype>(extents);
    ctmd::eye_to(out, mpmode);
    return out;
}

} // namespace ctmd
