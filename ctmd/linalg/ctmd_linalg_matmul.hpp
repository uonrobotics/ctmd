#pragma once

#include "../core/ctmd_core.hpp"
#include "../ctmd_copy.hpp"

namespace ctmd {
namespace linalg {
namespace detail {

template <md_c A_t, md_c B_t, md_c C_t>
    requires(A_t::rank() == 2 && B_t::rank() == 2 && C_t::rank() == 2)
inline constexpr void matmul_naive(const A_t &A, const B_t &B,
                                   C_t &C) noexcept {
    if (core::to_mdspan(A).data_handle() != core::to_mdspan(C).data_handle() &&
        core::to_mdspan(B).data_handle() != core::to_mdspan(C).data_handle())
        [[likely]] {
        for (typename C_t::size_type i = 0; i < C.extent(0); i++) {
            for (typename C_t::size_type j = 0; j < C.extent(1); j++) {
                C[i, j] = 0;
                for (typename A_t::size_type k = 0; k < A.extent(1); k++) {
                    C[i, j] += A[i, k] * B[k, j];
                }
            }
        }

    } else [[unlikely]] {
        auto C_tmp =
            mdarray<typename C_t::element_type, typename C_t::extents_type>{
                C.extent(0), C.extent(1)};
        matmul_naive(A, B, C_tmp);
        copy(C_tmp, C);
    }
}

template <md_c A_t, md_c B_t, md_c C_t>
    requires(A_t::rank() == 2 && B_t::rank() == 2 && C_t::rank() == 2)
inline constexpr void matmul_impl(const A_t &A, const B_t &B, C_t &C) noexcept {
    if constexpr (!core::eigen::can_map<A_t>() ||
                  !core::eigen::can_map<B_t>() ||
                  !core::eigen::can_map<C_t>()) {
        detail::matmul_naive(A, B, C);

    } else {
        if (std::is_constant_evaluated() || C.extent(0) + C.extent(1) <= 8)
            [[likely]] {
            detail::matmul_naive(A, B, C);

        } else [[unlikely]] {
            const auto A_eigen = core::eigen::to_eigen(A);
            const auto B_eigen = core::eigen::to_eigen(B);
            auto C_eigen = core::eigen::to_eigen(C);
            C_eigen = A_eigen * B_eigen;
        }
    }
}

} // namespace detail

template <md_c A_t, md_c B_t, md_c C_t>
    requires(A_t::rank() >= 2 && B_t::rank() >= 2 && C_t::rank() >= 2)
inline constexpr void matmul(const A_t &A, const B_t &B, C_t &C,
                             const bool multi_process = false) noexcept {
    const auto uA = core::submdspan_unit<2>(A);
    const auto uB = core::submdspan_unit<2>(B);
    const auto uC = core::submdspan_unit<2>(C);

    core::batch(detail::matmul_impl<decltype(uA), decltype(uB), decltype(uC)>,
                std::make_tuple(uA, uB), std::make_tuple(uC),
                std::make_tuple(core::to_mdspan(A), core::to_mdspan(B)),
                std::make_tuple(core::to_mdspan(C)), std::tuple<>{},
                multi_process);
}

template <md_c A_t, md_c B_t>
    requires(A_t::rank() >= 2 && B_t::rank() >= 2)
[[nodiscard]] inline constexpr auto
matmul(const A_t &A, const B_t &B, const bool multi_process = false) noexcept {
    const auto uA = core::submdspan_unit<2>(A);
    const auto uB = core::submdspan_unit<2>(B);

    using uC_element_type =
        std::remove_const_t<std::common_type_t<typename A_t::element_type,
                                               typename B_t::element_type>>;
    using uC_index_type =
        std::common_type_t<typename A_t::index_type, typename B_t::index_type>;

    const auto uC =
        mdarray<uC_element_type,
                stdex::extents<uC_index_type, A_t::static_extent(0),
                               B_t::static_extent(1)>>{
            stdex::extents<uC_index_type, A_t::static_extent(0),
                           B_t::static_extent(1)>{A.extent(0), B.extent(1)}}
            .to_mdspan();

    const auto results = core::batch_out(
        detail::matmul_impl<decltype(uA), decltype(uB), decltype(uC)>,
        std::make_tuple(uA, uB), std::make_tuple(uC),
        std::make_tuple(core::to_mdspan(A), core::to_mdspan(B)), std::tuple<>{},
        multi_process);

    return std::get<0>(results);
}

} // namespace linalg
} // namespace ctmd
