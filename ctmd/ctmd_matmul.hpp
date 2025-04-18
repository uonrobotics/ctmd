#pragma once

#include "linalg/ctmd_linalg_matmul.hpp"

namespace ctmd {

template <md_c A_t, md_c B_t, md_c C_t>
    requires(A_t::rank() >= 2 && B_t::rank() >= 2 && C_t::rank() >= 2)
inline constexpr void matmul(const A_t &A, const B_t &B, C_t &C,
                             const bool multi_process = false) noexcept {
    linalg::matmul(A, B, C, multi_process);
}

template <md_c A_t, md_c B_t>
    requires(A_t::rank() >= 2 && B_t::rank() >= 2)
[[nodiscard]] inline constexpr auto
matmul(const A_t &A, const B_t &B, const bool multi_process = false) noexcept {
    return linalg::matmul(A, B, multi_process);
}

} // namespace ctmd
