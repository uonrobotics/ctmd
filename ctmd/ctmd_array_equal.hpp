#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

template <typename in1_t, typename in2_t>
[[nodiscard]] inline constexpr bool array_equal(in1_t &&in1,
                                                in2_t &&in2) noexcept {
    const auto rin1 = core::to_mdspan(std::forward<in1_t>(in1));
    const auto rin2 = core::to_mdspan(std::forward<in2_t>(in2));
    using rint1_t = decltype(rin1);
    using rint2_t = decltype(rin2);

    if constexpr (rint1_t::rank() != rint2_t::rank()) {
        return false;
    }

    for (size_t i = 0; i < rint1_t::rank(); i++) {
        if (rin1.extent(i) != rin2.extent(i)) {
            return false;
        }
    }

    if constexpr (rint1_t::rank() == 0) {
        return rin1() == rin2();

    } else {
        for (typename rint1_t::size_type i = 0; i < rin1.extent(0); i++) {
            if (!array_equal(core::submdspan_from_start(rin1, i),
                             core::submdspan_from_start(rin2, i))) {
                return false;
            }
        }
    }

    return true;
}

} // namespace ctmd
