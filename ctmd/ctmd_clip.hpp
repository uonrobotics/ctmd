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

template <mdspan_c in_t, mdspan_c out_t, typename min_t, typename max_t>
    requires(in_t::rank() == 0 && out_t::rank() == 0 &&
             std::is_arithmetic_v<min_t> && std::is_arithmetic_v<max_t>)
inline constexpr void clip_impl(const in_t &in, const out_t &out,
                                const min_t &min, const max_t &max) noexcept {
    out[] = std::clamp(in[], min, max);
}

} // namespace detail

template <md_c in_t, md_c min_t, md_c max_t, md_c out_t>
inline constexpr void clip(const in_t &in, const min_t &min, const max_t &max,
                           out_t &out,
                           const MPMode mpmode = MPMode::NONE) noexcept {
    constexpr auto uin_exts = extents<typename in_t::index_type>{};
    constexpr auto umin_exts = extents<typename min_t::index_type>{};
    constexpr auto umax_exts = extents<typename max_t::index_type>{};
    constexpr auto uout_exts = extents<typename out_t::index_type>{};

    const auto bexts = core::broadcast(
        core::slice_from_start<in_t::rank() - decltype(uin_exts)::rank()>(
            in.extents()),
        core::slice_from_start<min_t::rank() - decltype(umin_exts)::rank()>(
            min.extents()),
        core::slice_from_start<max_t::rank() - decltype(umax_exts)::rank()>(
            max.extents()),
        core::slice_from_start<out_t::rank() - decltype(uout_exts)::rank()>(
            out.extents()));

    auto bin = core::broadcast_to(core::to_mdspan(in),
                                  core::concatenate(bexts, uin_exts));
    auto bmin = core::broadcast_to(core::to_mdspan(min),
                                   core::concatenate(bexts, umin_exts));
    auto bmax = core::broadcast_to(core::to_mdspan(max),
                                   core::concatenate(bexts, umax_exts));
    auto bout = core::to_mdspan(out);

    const auto ubin = core::submdspan_unit<decltype(uin_exts)::rank()>(bin);
    const auto ubmin = core::submdspan_unit<decltype(umin_exts)::rank()>(bmin);
    const auto ubmax = core::submdspan_unit<decltype(umax_exts)::rank()>(bmax);
    const auto ubout = core::submdspan_unit<decltype(uout_exts)::rank()>(bout);

    switch (mpmode) {
    case MPMode::NONE:
        core::batch<decltype(bexts)::rank(), MPMode::NONE>(
            detail::clip_impl<decltype(ubin), decltype(ubmin), decltype(ubmax),
                              decltype(ubout)>,
            std::tuple{bin, bmin, bmax, bout}, std::tuple{});
        break;

    case MPMode::CPUMP:
        core::batch<decltype(bexts)::rank(), MPMode::CPUMP>(
            detail::clip_impl<decltype(ubin), decltype(ubmin), decltype(ubmax),
                              decltype(ubout)>,
            std::tuple{bin, bmin, bmax, bout}, std::tuple{});
        break;

    default:
        assert(false);
        break;
    }
}

template <md_c in_t, md_c out_t, typename min_t, typename max_t>
    requires(std::is_arithmetic_v<min_t> && std::is_arithmetic_v<max_t>)
inline constexpr void clip(const in_t &in, const min_t &min, const max_t &max,
                           out_t &out,
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
            detail::clip_impl<decltype(ubin), decltype(ubout), min_t, max_t>,
            std::tuple{bin, bout}, std::tuple{min, max});
        break;

    case MPMode::CPUMP:
        core::batch<decltype(bexts)::rank(), MPMode::CPUMP>(
            detail::clip_impl<decltype(ubin), decltype(ubout), min_t, max_t>,
            std::tuple{bin, bout}, std::tuple{min, max});
        break;

    default:
        assert(false);
        break;
    }
}

template <md_c in_t, md_c min_t, md_c max_t>
[[nodiscard]] inline constexpr auto
clip(const in_t &in, const min_t &min, const max_t &max,
     const MPMode mpmode = MPMode::NONE) noexcept {
    constexpr auto uin_exts = extents<typename in_t::index_type>{};
    constexpr auto umin_exts = extents<typename min_t::index_type>{};
    constexpr auto umax_exts = extents<typename max_t::index_type>{};
    constexpr auto uout_exts =
        extents<std::common_type_t<typename in_t::index_type,
                                   typename min_t::index_type,
                                   typename max_t::index_type>>{};

    const auto bexts = core::broadcast(
        core::slice_from_start<in_t::rank() - decltype(uin_exts)::rank()>(
            in.extents()),
        core::slice_from_start<min_t::rank() - decltype(umin_exts)::rank()>(
            min.extents()),
        core::slice_from_start<max_t::rank() - decltype(umax_exts)::rank()>(
            max.extents()));

    auto out_exts = core::concatenate(bexts, uout_exts);
    auto out = ctmd::mdarray<std::common_type_t<typename in_t::element_type,
                                                typename min_t::element_type,
                                                typename max_t::element_type>,
                             decltype(out_exts)>{out_exts};

    auto bin = core::broadcast_to(core::to_mdspan(in),
                                  core::concatenate(bexts, uin_exts));
    auto bmin = core::broadcast_to(core::to_mdspan(min),
                                   core::concatenate(bexts, umin_exts));
    auto bmax = core::broadcast_to(core::to_mdspan(max),
                                   core::concatenate(bexts, umax_exts));
    auto bout = core::to_mdspan(out);

    const auto ubin = core::submdspan_unit<decltype(uin_exts)::rank()>(bin);
    const auto ubmin = core::submdspan_unit<decltype(umin_exts)::rank()>(bmin);
    const auto ubmax = core::submdspan_unit<decltype(umax_exts)::rank()>(bmax);
    const auto ubout = core::submdspan_unit<decltype(uout_exts)::rank()>(bout);

    switch (mpmode) {
    case MPMode::NONE:
        core::batch<decltype(bexts)::rank(), MPMode::NONE>(
            detail::clip_impl<decltype(ubin), decltype(ubmin), decltype(ubmax),
                              decltype(ubout)>,
            std::tuple{bin, bmin, bmax, bout}, std::tuple{});
        break;

    case MPMode::CPUMP:
        core::batch<decltype(bexts)::rank(), MPMode::CPUMP>(
            detail::clip_impl<decltype(ubin), decltype(ubmin), decltype(ubmax),
                              decltype(ubout)>,
            std::tuple{bin, bmin, bmax, bout}, std::tuple{});
        break;

    default:
        assert(false);
        break;
    }

    return out;
}

template <md_c in_t, typename min_t, typename max_t>
    requires(std::is_arithmetic_v<min_t> && std::is_arithmetic_v<max_t>)
[[nodiscard]] inline constexpr auto
clip(const in_t &in, const min_t &min, const max_t &max,
     const MPMode mpmode = MPMode::NONE) noexcept {
    constexpr auto uin_exts = extents<typename in_t::index_type>{};
    constexpr auto uout_exts = extents<typename in_t::index_type>{};

    const auto bexts =
        core::slice_from_start<in_t::rank() - decltype(uin_exts)::rank()>(
            in.extents());

    auto out_exts = core::concatenate(bexts, uout_exts);
    auto out = ctmd::mdarray<
        std::common_type_t<typename in_t::element_type, min_t, max_t>,
        decltype(out_exts)>{out_exts};

    auto bin = core::broadcast_to(core::to_mdspan(in),
                                  core::concatenate(bexts, uin_exts));
    auto bout = core::to_mdspan(out);

    const auto ubin = core::submdspan_unit<decltype(uin_exts)::rank()>(bin);
    const auto ubout = core::submdspan_unit<decltype(uout_exts)::rank()>(bout);

    switch (mpmode) {
    case MPMode::NONE:
        core::batch<decltype(bexts)::rank(), MPMode::NONE>(
            detail::clip_impl<decltype(ubin), decltype(ubout), min_t, max_t>,
            std::tuple{bin, bout}, std::tuple{min, max});
        break;

    case MPMode::CPUMP:
        core::batch<decltype(bexts)::rank(), MPMode::CPUMP>(
            detail::clip_impl<decltype(ubin), decltype(ubout), min_t, max_t>,
            std::tuple{bin, bout}, std::tuple{min, max});
        break;

    default:
        assert(false);
        break;
    }

    return out;
}

} // namespace ctmd
