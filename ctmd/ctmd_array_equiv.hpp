#pragma once

#include "core/ctmd_core.hpp"

#include "ctmd_equal.hpp"

namespace ctmd {

template <typename in1_t, typename in2_t>
[[nodiscard]] inline constexpr bool
array_equiv(const in1_t &in1, const in2_t &in2,
            const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::all(ctmd::equal(in1, in2, mpmode));
}

} // namespace ctmd
