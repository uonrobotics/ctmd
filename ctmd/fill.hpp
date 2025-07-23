#pragma once

#include "./core/core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, typename val_t>
    requires(in_t::rank() == 0)
inline constexpr void fill_impl(const in_t &in, const val_t &val) noexcept {
    in() = val;
}

} // namespace detail

template <typename InType, typename val_t>
inline constexpr void fill(InType &&In, const val_t &val,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [&](auto &&...elems) {
            detail::fill_impl(std::forward<decltype(elems)>(elems)..., val);
        },
        std::index_sequence<0>{}, mpmode,
        core::to_mdspan(std::forward<InType>(In)));
}

} // namespace ctmd
