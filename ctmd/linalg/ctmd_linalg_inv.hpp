#pragma once

#include "../core/ctmd_core.hpp"
#include "../core/eigen/ctmd_core_eigen.hpp"
#include "../ctmd_copy.hpp"

namespace ctmd {
namespace linalg {
namespace detail {

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 2 && out_t::rank() == 2)
inline constexpr void inv_naive(const in_t &in, const out_t &out) noexcept {
    using TO = typename out_t::element_type;

    const size_t n = in.extent(0);

    // check in and out are same pointer
    auto in_copy = copy(in);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            out(i, j) = (i == j) ? 1 : 0;
        }
    }

    for (size_t i = 0; i < n; i++) {
        const TO pivot = in_copy(i, i);

        if (pivot == TO(0)) {
            // Handle error: singular matrix (no inverse)
            return;
        }

        // Normalize the pivot row
        for (size_t j = 0; j < n; j++) {
            in_copy(i, j) /= pivot;
            out(i, j) /= pivot;
        }

        // Eliminate other rows
        for (size_t j = 0; j < n; j++) {
            if (i == j)
                continue;

            TO factor = in_copy(j, i);
            for (size_t k = 0; k < n; k++) {
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

template <typename in_t, typename out_t>
inline constexpr void inv(in_t &&in, out_t &&out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin = core::to_mdspan(std::forward<in_t>(in));
    const auto rout = core::to_mdspan(std::forward<out_t>(out));

    const auto urin_exts = core::slice_from_last<2>(rin.extents());
    const auto urout_exts = core::slice_from_last<2>(rout.extents());

    core::batch(
        [](const auto &in, const auto &out) { detail::inv_impl(in, out); },
        std::tuple{rin, rout}, std::tuple{urin_exts, urout_exts}, std::tuple{},
        mpmode);
}

template <typename in_t>
[[nodiscard]] inline constexpr auto
inv(in_t &&in, const MPMode mpmode = MPMode::NONE) noexcept {
    const auto rin = core::to_mdspan(std::forward<in_t>(in));

    const auto urin_exts = core::slice_from_last<2>(rin.extents());
    const auto urout_exts = urin_exts;

    return core::batch(
        [](const auto &in, const auto &out) { detail::inv_impl(in, out); },
        std::tuple{rin}, std::tuple{urin_exts, urout_exts}, std::tuple{},
        mpmode);
}

} // namespace linalg
} // namespace ctmd
