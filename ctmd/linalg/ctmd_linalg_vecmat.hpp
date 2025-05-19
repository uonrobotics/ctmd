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
            out(i) = 0;
            for (typename in1_t::size_type j = 0; j < in1.extent(0); j++) {
                out(i) += in1(j) * in2(j, i);
            }
        }

    } else [[unlikely]] {
        auto out_tmp =
            ctmd::mdarray<typename out_t::element_type,
                          typename out_t::extents_type>{out.extents()};
        vecmat_naive(in1, in2, out_tmp.to_mdspan());
        copy(out_tmp, out);
    }
}

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 1 && in2_t::rank() == 2 && out_t::rank() == 1)
inline constexpr void vecmat_impl(const in1_t &in1, const in2_t &in2,
                                  const out_t &out) noexcept {
#ifdef USE_EIGEN
    if constexpr (core::eigen::eigen_mappable_mdspan_c<in1_t> &&
                  core::eigen::eigen_mappable_mdspan_c<in2_t> &&
                  core::eigen::eigen_mappable_mdspan_c<out_t>) {
        if (!std::is_constant_evaluated() && 1 <= out.extent(0)) [[likely]] {
            const auto ein1 = core::eigen::to_eigen(in1);
            const auto ein2 = core::eigen::to_eigen(in2);
            auto eout = core::eigen::to_eigen(out);
            eout = ein1 * ein2;
            return;
        }
    }

#endif

    vecmat_naive(in1, in2, out);
}

} // namespace detail

template <typename in1_t, typename in2_t, typename out_t>
inline constexpr void vecmat(in1_t &&in1, in2_t &&in2, out_t &&out,
                             const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin1 = core::to_mdspan(std::forward<in1_t>(in1));
    const auto rin2 = core::to_mdspan(std::forward<in2_t>(in2));
    const auto rout = core::to_mdspan(std::forward<out_t>(out));

    const auto urin1_exts = core::slice_from_last<1>(rin1.extents());
    const auto urin2_exts = core::slice_from_last<2>(rin2.extents());
    const auto urout_exts = core::slice_from_last<1>(rout.extents());

    core::batch([](const auto &in1, const auto &in2,
                   const auto &out) { detail::vecmat_impl(in1, in2, out); },
                std::tuple{rin1, rin2, rout},
                std::tuple{urin1_exts, urin2_exts, urout_exts}, std::tuple{},
                mpmode);
}

template <typename in1_t, typename in2_t>
[[nodiscard]] inline constexpr auto
vecmat(in1_t &&in1, in2_t &&in2, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin1 = core::to_mdspan(std::forward<in1_t>(in1));
    const auto rin2 = core::to_mdspan(std::forward<in2_t>(in2));

    const auto urin1_exts = core::slice_from_last<1>(rin1.extents());
    const auto urin2_exts = core::slice_from_last<2>(rin2.extents());
    const auto urout_exts =
        extents<std::common_type_t<typename decltype(urin1_exts)::index_type,
                                   typename decltype(urin2_exts)::index_type>,
                decltype(urin2_exts)::static_extent(1)>{urin2_exts.extent(1)};

    return core::batch(
        [](const auto &in1, const auto &in2, const auto &out) {
            detail::vecmat_impl(in1, in2, out);
        },
        std::tuple{rin1, rin2}, std::tuple{urin1_exts, urin2_exts, urout_exts},
        std::tuple{}, mpmode);
}

} // namespace linalg
} // namespace ctmd
