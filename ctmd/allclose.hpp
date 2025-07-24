#pragma once

#include "all.hpp"
#include "isclose.hpp"

namespace ctmd {

template <typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr bool
allclose(In1Type &&In1, In2Type &&In2, const double &rtol = 1e-05,
         const double &atol = 1e-08,
         const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::all(ctmd::isclose(std::forward<In1Type>(In1),
                                   std::forward<In2Type>(In2), rtol, atol,
                                   mpmode));
}

} // namespace ctmd
