#pragma once

#include "core/core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t>
    requires(in_t::rank() == 0)
inline constexpr void fill_impl(const in_t &in, const auto &val) noexcept {
    in() = val;
}

} // namespace detail

inline constexpr void fill(auto &&In, const auto &val,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [&](auto &&...elems) {
            detail::fill_impl(std::forward<decltype(elems)>(elems)..., val);
        },
        std::index_sequence<0>{}, mpmode,
        core::to_mdspan(std::forward<decltype(In)>(In)));
}

} // namespace ctmd
