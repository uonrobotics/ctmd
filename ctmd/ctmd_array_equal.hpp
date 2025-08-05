#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr bool array_equal(auto &&In1,
                                                auto &&In2) noexcept {
    const auto in1 = core::to_const_mdspan(std::forward<decltype(In1)>(In1));
    const auto in2 = core::to_const_mdspan(std::forward<decltype(In2)>(In2));
    using in1_t = decltype(in1);
    using in2_t = decltype(in2);

    if constexpr (in1_t::rank() != in2_t::rank()) {
        return false;
    }

    for (size_t i = 0; i < in1_t::rank(); i++) {
        if (in1.extent(i) != in2.extent(i)) {
            return false;
        }
    }

    if constexpr (in1_t::rank() == 0) {
        return in1() == in2();

    } else {
        for (typename in1_t::index_type i = 0; i < in1.extent(0); i++) {
            if (!ctmd::array_equal(core::submdspan_from_left(in1, i),
                                   core::submdspan_from_left(in2, i))) {
                return false;
            }
        }
    }

    return true;
}

} // namespace ctmd
