#pragma once

#include "../copy.hpp"

namespace ctmd {
namespace linalg {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 1 && in2_t::rank() == 2 && out_t::rank() == 1)
inline constexpr void vecmat_naive(const in1_t &in1, const in2_t &in2,
                                   const out_t &out) noexcept {
    bool need_copy = false;

    if constexpr (requires {
                      core::to_const_mdspan(in1).data_handle() ==
                          core::to_mdspan(out).data_handle();
                  }) {
        if (core::to_const_mdspan(in1).data_handle() ==
            core::to_mdspan(out).data_handle()) [[unlikely]] {
            need_copy = true;
        }
    }

    if constexpr (requires {
                      core::to_const_mdspan(in2).data_handle() ==
                          core::to_mdspan(out).data_handle();
                  }) {
        if (core::to_const_mdspan(in2).data_handle() ==
            core::to_mdspan(out).data_handle()) [[unlikely]] {
            need_copy = true;
        }
    }

    if (!need_copy) [[likely]] {
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
        ctmd::copy(out_tmp, out);
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

template <typename In1Type, typename In2Type, typename OutType>
inline constexpr void vecmat(In1Type &&In1, In2Type &&In2, OutType &&Out,
                             const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in1 = core::to_const_mdspan(std::forward<In1Type>(In1));
    const auto in2 = core::to_const_mdspan(std::forward<In2Type>(In2));
    const auto out = core::to_mdspan(std::forward<OutType>(Out));

    core::batch(
        [](auto &&...elems) {
            detail::vecmat_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in1, in2, out},
        std::tuple{core::slice_from_last<1>(in1.extents()),
                   core::slice_from_last<2>(in2.extents()),
                   core::slice_from_last<1>(out.extents())},
        mpmode);
}

template <typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr auto
vecmat(In1Type &&In1, In2Type &&In2,
       const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in1 = core::to_const_mdspan(std::forward<In1Type>(In1));
    const auto in2 = core::to_mdspan(std::forward<In2Type>(In2));

    const auto uin1_exts = core::slice_from_last<1>(in1.extents());
    const auto uin2_exts = core::slice_from_last<2>(in2.extents());
    const auto uout_exts =
        extents<std::common_type_t<typename decltype(uin1_exts)::index_type,
                                   typename decltype(uin2_exts)::index_type>,
                decltype(uin2_exts)::static_extent(1)>{uin2_exts.extent(1)};

    return core::batch_out(
        [](auto &&...elems) {
            detail::vecmat_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::tuple{in1, in2}, std::tuple{uin1_exts, uin2_exts, uout_exts},
        mpmode);
}

} // namespace linalg
} // namespace ctmd
