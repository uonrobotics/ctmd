#pragma once

#include "ctmd_all.hpp"
#include "ctmd_equal.hpp"

namespace ctmd {

template <typename in1_t, typename in2_t>
[[nodiscard]] inline constexpr bool
array_equiv(in1_t &&in1, in2_t &&in2,
            const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::all(ctmd::equal(std::forward<in1_t>(in1),
                                 std::forward<in2_t>(in2), mpmode));
}

} // namespace ctmd
