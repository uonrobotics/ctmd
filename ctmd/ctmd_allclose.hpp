#pragma once

#include "core/ctmd_core.hpp"

#include "ctmd_all.hpp"
#include "ctmd_isclose.hpp"

namespace ctmd {

template <typename in1_t, typename in2_t>
[[nodiscard]] inline constexpr bool
allclose(const in1_t &in1, const in2_t &in2, const double rtol = 1e-05,
         const double atol = 1e-08,
         const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::all(ctmd::isclose(in1, in2, rtol, atol, mpmode));
}

} // namespace ctmd
