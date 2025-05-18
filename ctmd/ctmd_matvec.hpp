#pragma once

#include "linalg/ctmd_linalg_matvec.hpp"

namespace ctmd {

template <typename in1_t, typename in2_t, typename out_t>
inline constexpr void matvec(in1_t &&in1, in2_t &&in2, out_t &&out,
                             const MPMode mpmode = MPMode::NONE) noexcept {
    linalg::matvec(std::forward<in1_t>(in1), std::forward<in2_t>(in2),
                   std::forward<out_t>(out), mpmode);
}

template <typename in1_t, typename in2_t>
[[nodiscard]] inline constexpr auto
matvec(in1_t &&in1, in2_t &&in2, const MPMode mpmode = MPMode::NONE) noexcept {
    return linalg::matvec(std::forward<in1_t>(in1), std::forward<in2_t>(in2),
                          mpmode);
}

} // namespace ctmd
