#pragma once

#include "../core/ctmd_core.hpp"

namespace ctmd {
namespace linalg {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 1 && out_t::rank() == 0)
inline constexpr void norm_impl(const in_t &in, const out_t &out) noexcept {
    out[] = 0;
    for (typename in_t::size_type i = 0; i < in.extent(0); i++) {
        out[] += in[i] * in[i];
    }
    out[] = std::sqrt(out[]);
}

} // namespace detail

template <md_c in_t, typename out_t>
    requires(in_t::rank() >= 1)
inline constexpr void norm(const in_t &in, out_t &out,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    auto rin = core::to_mdspan(in);
    auto rout = core::to_mdspan(out);

    const auto urin_exts = core::slice_from_last<1>(rin.extents());
    constexpr auto urout_exts = extents<typename decltype(rout)::index_type>{};

    core::batch(
        [](const auto &in, const auto &out) { detail::norm_impl(in, out); },
        std::tuple{rin, rout}, std::tuple{urin_exts, urout_exts}, std::tuple{},
        mpmode);
}

template <md_c in_t>
    requires(in_t::rank() >= 1)
[[nodiscard]] inline constexpr auto
norm(const in_t &in, const MPMode mpmode = MPMode::NONE) noexcept {
    auto rin = core::to_mdspan(in);

    const auto urin_exts = core::slice_from_last<1>(rin.extents());
    constexpr auto urout_exts = extents<typename decltype(rin)::index_type>{};

    return core::batch(
        [](const auto &in, const auto &out) { detail::norm_impl(in, out); },
        std::tuple{rin}, std::tuple{urin_exts, urout_exts}, std::tuple{},
        mpmode);
}

} // namespace linalg
} // namespace ctmd
