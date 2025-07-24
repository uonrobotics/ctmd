#pragma once

#include "../copy.hpp"
#include "../core/eigen/eigen.hpp"

namespace ctmd {
namespace linalg {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 2 && out_t::rank() == 2)
inline constexpr void inv_naive(const in_t &in, const out_t &out) noexcept {
    using index_type = typename in_t::index_type;

    const index_type n = in.extent(0);

    // check in and out are same pointer
    auto in_copy = ctmd::copy(in);
    for (index_type i = 0; i < n; i++) {
        for (index_type j = 0; j < n; j++) {
            out(i, j) = (i == j) ? 1 : 0;
        }
    }

    for (index_type i = 0; i < n; i++) {
        const auto pivot = in_copy(i, i);

        if (pivot == 0) {
            // Handle error: singular matrix (no inverse)
            return;
        }

        // Normalize the pivot row
        for (index_type j = 0; j < n; j++) {
            in_copy(i, j) /= pivot;
            out(i, j) /= pivot;
        }

        // Eliminate other rows
        for (index_type j = 0; j < n; j++) {
            if (i == j)
                continue;

            const auto factor = in_copy(j, i);
            for (index_type k = 0; k < n; k++) {
                in_copy(j, k) -= factor * in_copy(i, k);
                out(j, k) -= factor * out(i, k);
            }
        }
    }
}

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 2 && out_t::rank() == 2)
inline constexpr void inv_impl(const in_t &in, const out_t &out) noexcept {
#ifdef USE_EIGEN
    if constexpr (core::eigen::eigen_mappable_mdspan_c<in_t> &&
                  core::eigen::eigen_mappable_mdspan_c<out_t>) {
        if (!std::is_constant_evaluated()) [[likely]] {
            const auto ein = core::eigen::to_eigen(in);
            auto eout = core::eigen::to_eigen(out);
            eout = ein.inverse();
            return;
        }
    }

#endif

    inv_naive(in, out);
}

} // namespace detail

template <typename InType, typename OutType>
inline constexpr void inv(InType &&In, OutType &&Out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    core::batch(
        [](auto &&...elems) {
            detail::inv_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<2, 2>{}, mpmode,
        core::to_const_mdspan(std::forward<InType>(In)),
        core::to_mdspan(std::forward<OutType>(Out)));
}

template <typename dtype = void, typename InType>
[[nodiscard]] inline constexpr auto
inv(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));

    return core::batch_out<dtype>(
        [](auto &&...elems) {
            detail::inv_impl(std::forward<decltype(elems)>(elems)...);
        },
        std::index_sequence<2>{}, core::slice_from_right<2>(in.extents()),
        mpmode, in);
}

} // namespace linalg
} // namespace ctmd
