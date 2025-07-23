#pragma once

#include "./core/core.hpp"

namespace ctmd {

template <typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr bool array_equal(In1Type &&In1,
                                                In2Type &&In2) noexcept {
    const auto in1 = core::to_const_mdspan(std::forward<In1Type>(In1));
    const auto in2 = core::to_const_mdspan(std::forward<In2Type>(In2));
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
        for (typename in1_t::size_type i = 0; i < in1.extent(0); i++) {
            if (!ctmd::array_equal(core::submdspan_from_left(in1, i),
                                   core::submdspan_from_left(in2, i))) {
                return false;
            }
        }
    }

    return true;
}

} // namespace ctmd
