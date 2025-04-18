#pragma once

#include "../core/ctmd_core.hpp"
#include "../core/eigen/ctmd_core_eigen.hpp"
#include "../ctmd_copy.hpp"

namespace ctmd {
namespace linalg {
namespace detail {

template <md_c A_t, md_c A_inv_t>
    requires(A_t::rank() == 2 && A_inv_t::rank() == 2)
inline constexpr void inv_naive(const A_t &A, A_inv_t &A_inv) noexcept {
    using TO = typename A_inv_t::element_type;

    const size_t n = A.extent(0);

    // check A and A_inv are same pointer
    auto A_copy = copy(A);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            A_inv[i, j] = (i == j) ? 1 : 0;
        }
    }

    for (size_t i = 0; i < n; ++i) {
        const TO pivot = A_copy[i, i];

        if (pivot == TO(0)) {
            // Handle error: singular matrix (no inverse)
            return;
        }

        // Normalize the pivot row
        for (size_t j = 0; j < n; ++j) {
            A_copy[i, j] /= pivot;
            A_inv[i, j] /= pivot;
        }

        // Eliminate other rows
        for (size_t j = 0; j < n; ++j) {
            if (i == j)
                continue;

            TO factor = A_copy[j, i];
            for (size_t k = 0; k < n; ++k) {
                A_copy[j, k] -= factor * A_copy[i, k];
                A_inv[j, k] -= factor * A_inv[i, k];
            }
        }
    }
}

} // namespace detail

template <md_c A_t, md_c A_inv_t>
    requires(A_t::rank() == 2 && A_inv_t::rank() == 2)
inline constexpr void inv(const A_t &A, A_inv_t &A_inv) noexcept {
    if constexpr (!core::eigen::can_map<A_t>() ||
                  !core::eigen::can_map<A_inv_t>()) {
        detail::inv_naive(A, A_inv);

    } else {
        if (std::is_constant_evaluated()) [[unlikely]] {
            detail::inv_naive(A, A_inv);

        } else [[likely]] {
            const auto A_eigen = core::eigen::to_eigen(A);
            auto A_inv_eigen = core::eigen::to_eigen(A_inv);
            A_inv_eigen = A_eigen.inverse();
        }
    }
}

template <md_c A_t, md_c A_inv_t>
    requires(A_t::rank() >= 2 && A_inv_t::rank() >= 2 &&
             !(A_t::rank() == 2 && A_inv_t::rank() == 2))
inline constexpr void inv(const A_t &A, A_inv_t &A_inv,
                          const bool multi_process = false) noexcept {
    const auto uA = core::submdspan_unit<2>(A);
    const auto uA_inv = core::submdspan_unit<2>(A_inv);

    core::batch(inv<decltype(uA), decltype(uA_inv)>, std::make_tuple(uA),
                std::make_tuple(uA_inv), std::make_tuple(core::to_mdspan(A)),
                std::make_tuple(core::to_mdspan(A_inv)), std::tuple<>{},
                multi_process);
}

template <md_c A_t>
    requires(A_t::rank() >= 2)
[[nodiscard]] inline constexpr auto
inv(const A_t &A, const bool multi_process = false) noexcept {
    const auto uA = core::submdspan_unit<2>(A);

    const auto uA_inv =
        mdarray<std::remove_const_t<typename decltype(uA)::element_type>,
                typename decltype(uA)::extents_type>{uA.extents()}
            .to_mdspan();

    const auto results = core::batch_out(
        inv<decltype(uA), decltype(uA_inv)>, std::make_tuple(uA),
        std::make_tuple(uA_inv), std::make_tuple(core::to_mdspan(A)),
        std::tuple<>{}, multi_process);

    return std::get<0>(results);
}

} // namespace linalg
} // namespace ctmd
