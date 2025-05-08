#pragma once

#include "../core/ctmd_core.hpp"
#include "../ctmd_copy.hpp"

namespace ctmd {
namespace linalg {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 1 && in2_t::rank() == 2 && out_t::rank() == 1)
inline constexpr void vecmat_naive(const in1_t &in1, const in2_t &in2,
                                   const out_t &out) noexcept {
    if (core::to_mdspan(in1).data_handle() !=
            core::to_mdspan(out).data_handle() &&
        core::to_mdspan(in2).data_handle() !=
            core::to_mdspan(out).data_handle()) [[likely]] {
        for (typename out_t::size_type i = 0; i < out.extent(0); i++) {
            out[i] = 0;
            for (typename in1_t::size_type j = 0; j < in1.extent(0); j++) {
                out[i] += in1[j] * in2[j, i];
            }
        }

    } else [[unlikely]] {
        auto out_tmp =
            mdarray<typename out_t::element_type, typename out_t::extents_type>{
                out.extents()};
        vecmat_naive(in1, in2, out_tmp.to_mdspan());
        copy(out_tmp, out);
    }
}

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 1 && in2_t::rank() == 2 && out_t::rank() == 1)
inline constexpr void vecmat_impl(const in1_t &in1, const in2_t &in2,
                                  const out_t &out) noexcept {
    if constexpr (!core::eigen::can_map<in1_t>() ||
                  !core::eigen::can_map<in2_t>() ||
                  !core::eigen::can_map<out_t>()) {
        vecmat_naive(in1, in2, out);

    } else {
        if (std::is_constant_evaluated() || out.extent(0) <= 8) [[likely]] {
            vecmat_naive(in1, in2, out);

        } else [[unlikely]] {
            const auto A_eigen = core::eigen::to_eigen(in1);
            const auto B_eigen = core::eigen::to_eigen(in2);
            auto C_eigen = core::eigen::to_eigen(out);
            C_eigen = A_eigen * B_eigen;
        }
    }
}

} // namespace detail

template <md_c in1_t, md_c in2_t, md_c out_t>
    requires(in1_t::rank() >= 1 && in2_t::rank() >= 2 && out_t::rank() >= 1)
inline constexpr void vecmat(const in1_t &in1, const in2_t &in2, out_t &out,
                             const MPMode mpmode = MPMode::NONE) noexcept {
    const auto uin1_exts = core::slice_from_last<1>(in1.extents());
    const auto uin2_exts = core::slice_from_last<2>(in2.extents());
    const auto uout_exts = core::slice_from_last<1>(out.extents());

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
            detail::vecmat_impl<decltype(ubin1), decltype(ubin2),
                                decltype(ubout)>,
            std::tuple{bin1, bin2, bout}, std::tuple{});
        break;

    case MPMode::CPUMP:
        core::batch<decltype(bexts)::rank(), MPMode::CPUMP>(
            detail::vecmat_impl<decltype(ubin1), decltype(ubin2),
                                decltype(ubout)>,
            std::tuple{bin1, bin2, bout}, std::tuple{});
        break;

    default:
        assert(false);
        break;
    }
}

template <md_c in1_t, md_c in2_t>
    requires(in1_t::rank() >= 1 && in2_t::rank() >= 2)
[[nodiscard]] inline constexpr auto
vecmat(const in1_t &in1, const in2_t &in2,
       const MPMode mpmode = MPMode::NONE) noexcept {
    const auto uin1_exts = core::slice_from_last<1>(in1.extents());
    const auto uin2_exts = core::slice_from_last<2>(in2.extents());
    const auto uout_exts =
        extents<std::common_type_t<typename in1_t::index_type,
                                   typename in2_t::index_type>,
                decltype(uin2_exts)::static_extent(1)>{uin2_exts.extent(1)};

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
            detail::vecmat_impl<decltype(ubin1), decltype(ubin2),
                                decltype(ubout)>,
            std::tuple{bin1, bin2, bout}, std::tuple{});
        break;

    case MPMode::CPUMP:
        core::batch<decltype(bexts)::rank(), MPMode::CPUMP>(
            detail::vecmat_impl<decltype(ubin1), decltype(ubin2),
                                decltype(ubout)>,
            std::tuple{bin1, bin2, bout}, std::tuple{});
        break;

    default:
        assert(false);
        break;
    }

    return out;
}

} // namespace linalg
} // namespace ctmd
