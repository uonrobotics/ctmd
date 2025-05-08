#pragma once

#include "../core/ctmd_core.hpp"

namespace ctmd {
namespace linalg {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 1 && out_t::rank() == 1)
inline constexpr void norm_impl(const in_t &in, const out_t &out) noexcept {
    out[0] = 0;
    for (typename in_t::size_type i = 0; i < in.extent(0); i++) {
        out[0] += in[i] * in[i];
    }
    out[0] = std::sqrt(out[0]);
}

} // namespace detail

template <md_c in_t, md_c out_t>
    requires(in_t::rank() >= 1 && out_t::rank() >= 1)
inline constexpr void norm(const in_t &in, out_t &out,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    const auto uin1_exts = core::slice_from_last<1>(in.extents());
    constexpr auto uout_exts = extents<typename out_t::index_type, 1>{1};

    const auto bexts = core::broadcast(
        core::slice_from_start<in_t::rank() - decltype(uin1_exts)::rank()>(
            in.extents()),
        core::slice_from_start<out_t::rank() - decltype(uout_exts)::rank()>(
            out.extents()));

    auto bin = core::broadcast_to(core::to_mdspan(in),
                                  core::concatenate(bexts, uin1_exts));
    auto bout = core::to_mdspan(out);

    const auto ubin = core::submdspan_unit<decltype(uin1_exts)::rank()>(bin);
    const auto ubout = core::submdspan_unit<decltype(uout_exts)::rank()>(bout);

    switch (mpmode) {
    case MPMode::NONE:
        core::batch<decltype(bexts)::rank(), MPMode::NONE>(
            detail::norm_impl<decltype(ubin), decltype(ubout)>,
            std::tuple{bin, bout}, std::tuple{});
        break;

    case MPMode::CPUMP:
        core::batch<decltype(bexts)::rank(), MPMode::CPUMP>(
            detail::norm_impl<decltype(ubin), decltype(ubout)>,
            std::tuple{bin, bout}, std::tuple{});
        break;

    default:
        assert(false);
        break;
    }
}

template <md_c in_t>
    requires(in_t::rank() >= 1)
[[nodiscard]] inline constexpr auto
norm(const in_t &in, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto uin_exts = core::slice_from_last<1>(in.extents());
    constexpr auto uout_exts = extents<typename in_t::index_type, 1>{1};

    const auto bexts =
        core::slice_from_start<in_t::rank() - decltype(uin_exts)::rank()>(
            in.extents());

    auto out_exts = core::concatenate(bexts, uout_exts);
    auto out = ctmd::mdarray<typename in_t::element_type, decltype(out_exts)>{
        out_exts};

    auto bin = core::to_mdspan(in);
    auto bout = core::to_mdspan(out);

    const auto ubin = core::submdspan_unit<decltype(uin_exts)::rank()>(bin);
    const auto ubout = core::submdspan_unit<decltype(uout_exts)::rank()>(bout);

    switch (mpmode) {
    case MPMode::NONE:
        core::batch<decltype(bexts)::rank(), MPMode::NONE>(
            detail::norm_impl<decltype(ubin), decltype(ubout)>,
            std::tuple{bin, bout}, std::tuple{});
        break;

    case MPMode::CPUMP:
        core::batch<decltype(bexts)::rank(), MPMode::CPUMP>(
            detail::norm_impl<decltype(ubin), decltype(ubout)>,
            std::tuple{bin, bout}, std::tuple{});
        break;

    default:
        assert(false);
        break;
    }

    return out;
}

} // namespace linalg
} // namespace ctmd
