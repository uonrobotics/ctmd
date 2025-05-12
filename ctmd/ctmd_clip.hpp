#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c min_t, mdspan_c max_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && min_t::rank() == 0 && max_t::rank() == 0 &&
             out_t::rank() == 0)
inline constexpr void clip_impl(const in_t &in, const min_t &min,
                                const max_t &max, const out_t &out) noexcept {
    out[] = std::clamp(in[], min[], max[]);
}

} // namespace detail

template <typename in_t, typename min_t, typename max_t, typename out_t>
inline constexpr void clip(const in_t &in, const min_t &min, const max_t &max,
                           out_t &out,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin = core::to_mdspan(in);
    const auto rmin = core::to_mdspan(min);
    const auto rmax = core::to_mdspan(max);
    const auto rout = core::to_mdspan(out);

    constexpr auto urin_exts = extents<typename decltype(rin)::index_type>{};
    constexpr auto urmin_exts = extents<typename decltype(rmin)::index_type>{};
    constexpr auto urmax_exts = extents<typename decltype(rmax)::index_type>{};
    constexpr auto urout_exts = extents<typename decltype(rout)::index_type>{};

    core::batch([](const auto &in, const auto &min, const auto &max,
                   const auto &out) { detail::clip_impl(in, min, max, out); },
                std::tuple{rin, rmin, rmax, rout},
                std::tuple{urin_exts, urmin_exts, urmax_exts, urout_exts},
                std::tuple{}, mpmode);
}

template <typename in_t, typename min_t, typename max_t>
[[nodiscard]] inline constexpr auto
clip(const in_t &in, const min_t &min, const max_t &max,
     const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin = core::to_mdspan(in);
    const auto rmin = core::to_mdspan(min);
    const auto rmax = core::to_mdspan(max);

    constexpr auto urin_exts = extents<typename decltype(rin)::index_type>{};
    constexpr auto urmin_exts = extents<typename decltype(rmin)::index_type>{};
    constexpr auto urmax_exts = extents<typename decltype(rmax)::index_type>{};
    constexpr auto urout_exts =
        extents<std::common_type_t<typename decltype(rin)::index_type,
                                   typename decltype(rmin)::index_type,
                                   typename decltype(rmax)::index_type>>{};

    return core::batch(
        [](const auto &in, const auto &min, const auto &max, const auto &out) {
            detail::clip_impl(in, min, max, out);
        },
        std::tuple{rin, rmin, rmax},
        std::tuple{urin_exts, urmin_exts, urmax_exts, urout_exts}, std::tuple{},
        mpmode);
}

} // namespace ctmd
