#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

template <md_c in1_t, md_c in2_t>
[[nodiscard]] inline constexpr bool array_equal(const in1_t &in1,
                                                const in2_t &in2) noexcept {
    if constexpr (in1_t::rank() != in2_t::rank()) {
        return false;
    }

    for (size_t i = 0; i < in1_t::rank(); i++) {
        if (in1.extent(i) != in2.extent(i)) {
            return false;
        }
    }

    if constexpr (in1_t::rank() == 0) {
        return in1[] == in2[];

    } else {
        for (typename in1_t::size_type i = 0; i < in1.extent(0); i++) {
            if (!array_equal(core::submdspan_from_start(in1, i),
                             core::submdspan_from_start(in2, i))) {
                return false;
            }
        }
    }

    return true;
}

} // namespace ctmd
