#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 0 && in2_t::rank() == 0 && out_t::rank() == 0)
inline constexpr void multiply_impl(const in1_t &in1, const in2_t &in2,
                                    const out_t &out) noexcept {
    out[] = in1[] * in2[];
}

} // namespace detail

template <md_c in1_t, md_c in2_t, md_c out_t>
inline constexpr void multiply(const in1_t &in1, const in2_t &in2, out_t &out,
                               const MPMode mpmode = MPMode::NONE) noexcept {
    constexpr auto uin1_exts = extents<typename in1_t::index_type>{};
    constexpr auto uin2_exts = extents<typename in2_t::index_type>{};
    constexpr auto uout_exts = extents<typename out_t::index_type>{};

    const auto bexts = core::broadcast(
        core::slice_from_start<in1_t::rank() - decltype(uin1_exts)::rank()>(
            in1.extents()),
        core::slice_from_start<in2_t::rank() - decltype(uin2_exts)::rank()>(
            in2.extents()),
        core::slice_from_start<out_t::rank() - decltype(uout_exts)::rank()>(
            out.extents()));

    auto bin1 = core::broadcast_to(core::to_mdspan(in1),
                                   core::concatenate(bexts, uin1_exts));
    auto bin2 = core::broadcast_to(core::to_mdspan(in2),
                                   core::concatenate(bexts, uin2_exts));
    auto bout = core::to_mdspan(out);

    const auto ubin1 = core::submdspan_unit<decltype(uin1_exts)::rank()>(bin1);
    const auto ubin2 = core::submdspan_unit<decltype(uin2_exts)::rank()>(bin2);
    const auto ubout = core::submdspan_unit<decltype(uout_exts)::rank()>(bout);

    switch (mpmode) {
    case MPMode::NONE:
        core::batch<decltype(bexts)::rank(), MPMode::NONE>(
            detail::multiply_impl<decltype(ubin1), decltype(ubin2),
                                  decltype(ubout)>,
            std::tuple{bin1, bin2, bout}, std::tuple{});
        break;

    case MPMode::CPUMP:
        core::batch<decltype(bexts)::rank(), MPMode::CPUMP>(
            detail::multiply_impl<decltype(ubin1), decltype(ubin2),
                                  decltype(ubout)>,
            std::tuple{bin1, bin2, bout}, std::tuple{});
        break;

    default:
        assert(false);
        break;
    }
}

template <md_c in1_t, md_c in2_t>
[[nodiscard]] inline constexpr auto
multiply(const in1_t &in1, const in2_t &in2,
         const MPMode mpmode = MPMode::NONE) noexcept {
    constexpr auto uin1_exts = extents<typename in1_t::index_type>{};
    constexpr auto uin2_exts = extents<typename in2_t::index_type>{};
    constexpr auto uout_exts =
        extents<std::common_type_t<typename in1_t::index_type,
                                   typename in2_t::index_type>>{};

    const auto bexts = core::broadcast(
        core::slice_from_start<in1_t::rank() - decltype(uin1_exts)::rank()>(
            in1.extents()),
        core::slice_from_start<in2_t::rank() - decltype(uin2_exts)::rank()>(
            in2.extents()));

    auto out_exts = core::concatenate(bexts, uout_exts);
    auto out = mdarray<std::common_type_t<typename in1_t::element_type,
                                          typename in2_t::element_type>,
                       decltype(out_exts)>{out_exts};

    auto bin1 = core::broadcast_to(core::to_mdspan(in1),
                                   core::concatenate(bexts, uin1_exts));
    auto bin2 = core::broadcast_to(core::to_mdspan(in2),
                                   core::concatenate(bexts, uin2_exts));
    auto bout = core::to_mdspan(out);

    const auto ubin1 = core::submdspan_unit<decltype(uin1_exts)::rank()>(bin1);
    const auto ubin2 = core::submdspan_unit<decltype(uin2_exts)::rank()>(bin2);
    const auto ubout = core::submdspan_unit<decltype(uout_exts)::rank()>(bout);

    switch (mpmode) {
    case MPMode::NONE:
        core::batch<decltype(bexts)::rank(), MPMode::NONE>(
            detail::multiply_impl<decltype(ubin1), decltype(ubin2),
                                  decltype(ubout)>,
            std::tuple{bin1, bin2, bout}, std::tuple{});
        break;

    case MPMode::CPUMP:
        core::batch<decltype(bexts)::rank(), MPMode::CPUMP>(
            detail::multiply_impl<decltype(ubin1), decltype(ubin2),
                                  decltype(ubout)>,
            std::tuple{bin1, bin2, bout}, std::tuple{});
        break;

    default:
        assert(false);
        break;
    }

    return out;
}

} // namespace ctmd
