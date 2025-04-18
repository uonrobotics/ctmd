#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

template <md_c in1_t, md_c in2_t>
[[nodiscard]] inline constexpr bool
allclose(const in1_t &in1, const in2_t &in2, const double rtol = 1e-05,
         const double atol = 1e-08) noexcept {
    if constexpr (in1_t::rank() != in2_t::rank()) {
        return false;
    }

    for (size_t i = 0; i < in1_t::rank(); ++i) {
        if (in1.extent(i) != in2.extent(i)) {
            return false;
        }
    }

    if constexpr (in1_t::rank() == 0) {
        return std::abs(in1[] - in2[]) <= (atol + rtol * std::abs(in2[]));

    } else if constexpr (in1_t::rank() == 1) {
        for (typename in1_t::size_type i = 0; i < in1.extent(0); i++) {
            if (std::abs(in1[i] - in2[i]) > (atol + rtol * std::abs(in2[i]))) {
                return false;
            }
        }

    } else {
        for (typename in1_t::size_type i = 0; i < in1.extent(0); i++) {
            const auto in1_slice = core::submdspan_from_start(in1, i);
            const auto in2_slice = core::submdspan_from_start(in2, i);
            if (!allclose(in1_slice, in2_slice, rtol, atol)) {
                return false;
            }
        }
    }

    return true;
}

} // namespace ctmd