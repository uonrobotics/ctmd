#pragma once

#include "../ctmd_copy.hpp"

namespace ctmd {
namespace linalg {
namespace detail {

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 2 && in2_t::rank() == 1 && out_t::rank() == 1)
inline constexpr void matvec_naive(const in1_t &in1, const in2_t &in2,
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
            for (typename in1_t::size_type j = 0; j < in1.extent(1); j++) {
                out(i) += in1(i, j) * in2(j);
            }
        }

    } else [[unlikely]] {
        auto out_tmp =
            ctmd::mdarray<typename out_t::value_type,
                          typename out_t::extents_type>{out.extents()};
        matvec_naive(in1, in2, out_tmp.to_mdspan());
        ctmd::copy_to(out_tmp, out);
    }
}

template <mdspan_c in1_t, mdspan_c in2_t, mdspan_c out_t>
    requires(in1_t::rank() == 2 && in2_t::rank() == 1 && out_t::rank() == 1)
inline constexpr void matvec_impl(const in1_t &in1, const in2_t &in2,
                                  const out_t &out) noexcept {
#ifdef USE_EIGEN
    if constexpr (core::eigen::eigen_mappable_mdspan_c<in1_t> &&
                  core::eigen::eigen_mappable_mdspan_c<in2_t> &&
                  core::eigen::eigen_mappable_mdspan_c<out_t>) {
        if (!std::is_constant_evaluated() && 1 <= out.extent(0)) [[likely]] {
            using common_t = std::common_type_t<typename in1_t::value_type,
                                                typename in2_t::value_type>;
            const auto ein1 =
                core::eigen::to_eigen(in1).template cast<common_t>();
            const auto ein2 =
                core::eigen::to_eigen(in2).template cast<common_t>();
            auto eout = core::eigen::to_eigen(out);
            eout = (ein1 * ein2).template cast<typename out_t::value_type>();
            return;
        }
    }

#endif

    matvec_naive(in1, in2, out);
}

} // namespace detail

inline constexpr void
matvec_to(auto &&In1, auto &&In2, auto &&Out,
          const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::matvec_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<2, 1, 1>{}, mpmode,
        core::to_const_mdspan(std::forward<decltype(In1)>(In1)),
        core::to_const_mdspan(std::forward<decltype(In2)>(In2)),
        core::to_mdspan(std::forward<decltype(Out)>(Out)));
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto
matvec(auto &&In1, auto &&In2,
       const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    const auto in1 = core::to_const_mdspan(std::forward<decltype(In1)>(In1));
    const auto in2 = core::to_const_mdspan(std::forward<decltype(In2)>(In2));

    const auto uin1_exts = core::slice_from_right<2>(in1.extents());
    const auto uin2_exts = core::slice_from_right<1>(in2.extents());
    const auto uout_exts =
        extents<std::common_type_t<typename decltype(uin1_exts)::index_type,
                                   typename decltype(uin2_exts)::index_type>,
                decltype(uin1_exts)::static_extent(0)>{uin1_exts.extent(0)};

    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::matvec_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<2, 1>{}, uout_exts, mpmode, in1, in2);
}

} // namespace linalg
} // namespace ctmd
