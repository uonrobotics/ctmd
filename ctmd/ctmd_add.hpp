#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 0 && in2_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void add_impl(const in1_t &in1, const in2_t &in2,
                               const out_t &out) noexcept {
    out() = in1() + in2();
}

} // namespace detail

template <typename in1_t, typename in2_t, typename out_t>
inline constexpr void add(in1_t &&in1, in2_t &&in2, out_t &&out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin1 = core::to_mdspan(std::forward<in1_t>(in1));
    const auto rin2 = core::to_mdspan(std::forward<in2_t>(in2));
    const auto rout = core::to_mdspan(std::forward<out_t>(out));

    constexpr auto urin1_exts = extents<typename decltype(rin1)::index_type>{};
    constexpr auto urin2_exts = extents<typename decltype(rin2)::index_type>{};
    constexpr auto urout_exts = extents<typename decltype(rout)::index_type>{};

    core::batch([](const auto &in1, const auto &in2,
                   const auto &out) { detail::add_impl(in1, in2, out); },
                std::tuple{rin1, rin2, rout},
                std::tuple{urin1_exts, urin2_exts, urout_exts}, std::tuple{},
                mpmode);
}

template <typename in1_t, typename in2_t>
[[nodiscard]] inline constexpr auto
add(in1_t &&in1, in2_t &&in2, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin1 = core::to_mdspan(std::forward<in1_t>(in1));
    const auto rin2 = core::to_mdspan(std::forward<in2_t>(in2));

    constexpr auto urin1_exts = extents<typename decltype(rin1)::index_type>{};
    constexpr auto urin2_exts = extents<typename decltype(rin2)::index_type>{};
    constexpr auto urout_exts =
        extents<std::common_type_t<typename decltype(rin1)::index_type,
                                   typename decltype(rin2)::index_type>>{};

    return core::batch([](const auto &in1, const auto &in2,
                          const auto &out) { detail::add_impl(in1, in2, out); },
                       std::tuple{rin1, rin2},
                       std::tuple{urin1_exts, urin2_exts, urout_exts},
                       std::tuple{}, mpmode);
}

} // namespace ctmd
