#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, arithmetic_c val_t>
    requires(in_t::rank() == 0)
inline constexpr void fill_impl(const in_t &in, const val_t &val) noexcept {
    in() = val;
}

} // namespace detail

template <typename in_t, arithmetic_c val_t>
inline constexpr void fill(in_t &&in, const val_t &val,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin = core::to_mdspan(std::forward<in_t>(in));

    constexpr auto urin_exts = extents<typename decltype(rin)::index_type>{};

    core::batch(
        [](const auto &in, const auto &val) { detail::fill_impl(in, val); },
        std::tuple{rin}, std::tuple{urin_exts}, std::tuple{val}, mpmode);
}

} // namespace ctmd
