#pragma once

#include "core/ctmd_core.hpp"
#include "ctmd_add.hpp"
#include "ctmd_fill.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() - out_t::rank() == 1)
inline constexpr void sum_impl(const in_t &in, const out_t &out) noexcept {
    ctmd::fill(out, 0);

    for (typename in_t::size_type i = 0; i < in.extent(0); i++) {
        ctmd::add(out, core::submdspan_from_start(in, i), out);
    }
}

} // namespace detail

template <int64_t Axis, typename in_t, typename out_t>
inline constexpr void sum(in_t &&in, out_t &&out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin = core::to_mdspan(std::forward<in_t>(in));
    const auto rout = core::to_mdspan(std::forward<out_t>(out));

    constexpr size_t urin_rank =
        rin.rank() -
        static_cast<size_t>(
            ((Axis % static_cast<int64_t>(rin.rank())) + (rin.rank())) %
            (rin.rank()));

    const auto urin_exts = core::slice_from_start<urin_rank>(rin.extents());
    const auto urout_exts =
        core::slice_from_start<urin_rank - 1>(rout.extents());

    core::batch(
        [](const auto &in, const auto &out) { detail::sum_impl(in, out); },
        std::tuple{rin, rout}, std::tuple{urin_exts, urout_exts}, std::tuple{},
        mpmode);
}

template <int64_t Axis, typename in_t>
[[nodiscard]] inline constexpr auto
sum(in_t &&in, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin = core::to_mdspan(std::forward<in_t>(in));

    constexpr size_t urin_rank =
        rin.rank() -
        static_cast<size_t>(
            ((Axis % static_cast<int64_t>(rin.rank())) + (rin.rank())) %
            (rin.rank()));

    const auto urin_exts = core::slice_from_start<urin_rank>(rin.extents());
    const auto urout_exts = core::slice_from_last<urin_rank - 1>(urin_exts);

    return core::batch(
        [](const auto &in, const auto &out) { detail::sum_impl(in, out); },
        std::tuple{rin}, std::tuple{urin_exts, urout_exts}, std::tuple{},
        mpmode);
}

} // namespace ctmd
