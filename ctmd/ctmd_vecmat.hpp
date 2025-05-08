#pragma once

#include "linalg/ctmd_linalg_vecmat.hpp"

namespace ctmd {

template <md_c in1_t, md_c in2_t, md_c out_t>
    requires(in1_t::rank() >= 1 && in2_t::rank() >= 2 && out_t::rank() >= 1)
inline constexpr void vecmat(const in1_t &in1, const in2_t &in2, out_t &out,
                             const MPMode mpmode = MPMode::NONE) noexcept {
    linalg::vecmat(in1, in2, out, mpmode);
}

template <md_c in1_t, md_c in2_t>
    requires(in1_t::rank() >= 1 && in2_t::rank() >= 2)
[[nodiscard]] inline constexpr auto
vecmat(const in1_t &in1, const in2_t &in2,
       const MPMode mpmode = MPMode::NONE) noexcept {
    return linalg::vecmat(in1, in2, mpmode);
}

} // namespace ctmd
