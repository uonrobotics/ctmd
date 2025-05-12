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
            out[i, j] = (i == j) ? 1 : 0;
        }
    }

    for (size_t i = 0; i < n; i++) {
        const TO pivot = in_copy[i, i];

        if (pivot == TO(0)) {
            // Handle error: singular matrix (no inverse)
            return;
        }

        // Normalize the pivot row
        for (size_t j = 0; j < n; j++) {
            in_copy[i, j] /= pivot;
            out[i, j] /= pivot;
        }

        // Eliminate other rows
        for (size_t j = 0; j < n; j++) {
            if (i == j)
                continue;

            TO factor = in_copy[j, i];
            for (size_t k = 0; k < n; k++) {
                in_copy[j, k] -= factor * in_copy[i, k];
                out[j, k] -= factor * out[i, k];
            }
        }
    }
}

template <mdspan_c in_t, mdspan_c out_t>
    requires(in_t::rank() == 2 && out_t::rank() == 2)
inline constexpr void inv_impl(const in_t &in, const out_t &out) noexcept {
    if constexpr (!core::eigen::can_map<in_t>() ||
                  !core::eigen::can_map<out_t>()) {
        inv_naive(in, out);

    } else {
        if (std::is_constant_evaluated()) [[unlikely]] {
            inv_naive(in, out);

        } else [[likely]] {
            const auto A_eigen = core::eigen::to_eigen(in);
            auto A_inv_eigen = core::eigen::to_eigen(out);
            A_inv_eigen = A_eigen.inverse();
        }
    }
}

} // namespace detail

template <md_c in_t, md_c out_t>
    requires(in_t::rank() >= 2 && out_t::rank() >= 2)
inline constexpr void inv(const in_t &in, out_t &out,
                          const MPMode mpmode = MPMode::NONE) noexcept {
    auto rin = core::to_mdspan(in);
    auto rout = core::to_mdspan(out);

    const auto urin_exts = core::slice_from_last<2>(rin.extents());
    const auto urout_exts = core::slice_from_last<2>(rout.extents());

    core::batch(
        [](const auto &in, const auto &out) { detail::inv_impl(in, out); },
        std::tuple{rin, rout}, std::tuple{urin_exts, urout_exts}, std::tuple{},
        mpmode);
}

template <md_c in_t>
    requires(in_t::rank() >= 2)
[[nodiscard]] inline constexpr auto
inv(const in_t &in, const MPMode mpmode = MPMode::NONE) noexcept {
    auto rin = core::to_mdspan(in);

    const auto urin_exts = core::slice_from_last<2>(rin.extents());
    const auto urout_exts = urin_exts;

    return core::batch(
        [](const auto &in, const auto &out) { detail::inv_impl(in, out); },
        std::tuple{rin}, std::tuple{urin_exts, urout_exts}, std::tuple{},
        mpmode);
}

} // namespace linalg
} // namespace ctmd
