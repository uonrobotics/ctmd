#pragma once

#include <cmath>

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void sin_impl(const in_t &in, const out_t &out) noexcept {
    out() = std::sin(in());
}

} // namespace detail

template <typename in_t, typename out_t>
inline constexpr void sin(in_t &&in, out_t &&out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin = core::to_mdspan(std::forward<in_t>(in));
    const auto rout = core::to_mdspan(std::forward<out_t>(out));

    constexpr auto urin_exts = extents<typename decltype(rin)::index_type>{};
    constexpr auto urout_exts = extents<typename decltype(rout)::index_type>{};

    core::batch(
        [](const auto &in, const auto &out) { detail::sin_impl(in, out); },
        std::tuple{rin, rout}, std::tuple{urin_exts, urout_exts}, std::tuple{},
        mpmode);
}

template <typename in_t>
[[nodiscard]] inline constexpr auto
sin(in_t &&in, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin = core::to_mdspan(std::forward<in_t>(in));

    constexpr auto urin_exts = extents<typename decltype(rin)::index_type>{};
    constexpr auto urout_exts = extents<typename decltype(rin)::index_type>{};

    return core::batch(
        [](const auto &in, const auto &out) { detail::sin_impl(in, out); },
        std::tuple{rin}, std::tuple{urin_exts, urout_exts}, std::tuple{},
        mpmode);
}

} // namespace ctmd
