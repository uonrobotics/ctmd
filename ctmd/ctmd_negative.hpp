#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void negative_impl(const in_t &in, const out_t &out) noexcept {
    out[] = -in[];
}

} // namespace detail

template <md_c in_t, md_c out_t>
inline constexpr void negative(const in_t &in, out_t &out,
                               const MPMode mpmode = MPMode::NONE) noexcept {
    constexpr auto uin_exts = extents<typename in_t::index_type>{};
    constexpr auto uout_exts = extents<typename out_t::index_type>{};

    const auto bexts = core::broadcast(
        core::slice_from_start<in_t::rank() - decltype(uin_exts)::rank()>(
            in.extents()),
        core::slice_from_start<out_t::rank() - decltype(uout_exts)::rank()>(
            out.extents()));

    auto bin = core::broadcast_to(core::to_mdspan(in),
                                  core::concatenate(bexts, uin_exts));
    auto bout = core::to_mdspan(out);

    const auto ubin = core::submdspan_unit<decltype(uin_exts)::rank()>(bin);
    const auto ubout = core::submdspan_unit<decltype(uout_exts)::rank()>(bout);

    switch (mpmode) {
    case MPMode::NONE:
        core::batch<decltype(bexts)::rank(), MPMode::NONE>(
            detail::negative_impl<decltype(ubin), decltype(ubout)>,
            std::tuple{bin, bout}, std::tuple{});
        break;

    case MPMode::CPUMP:
        core::batch<decltype(bexts)::rank(), MPMode::CPUMP>(
            detail::negative_impl<decltype(ubin), decltype(ubout)>,
            std::tuple{bin, bout}, std::tuple{});
        break;

    default:
        assert(false);
        break;
    }
}

template <md_c in_t>
[[nodiscard]] inline constexpr auto
negative(const in_t &in, const MPMode mpmode = MPMode::NONE) noexcept {
    auto out = ctmd::mdarray<std::remove_const_t<typename in_t::element_type>,
                             typename in_t::extents_type>{in.extents()};
    negative(in, out, mpmode);
    return out;
}

} // namespace ctmd
