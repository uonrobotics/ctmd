#pragma once

#include "all.hpp"
#include "isclose.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr bool
allclose(auto &&In1, auto &&In2, const double &rtol = 1e-05,
         const double &atol = 1e-08,
         const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::all(ctmd::isclose(std::forward<decltype(In1)>(In1),
                                   std::forward<decltype(In2)>(In2), rtol, atol,
                                   mpmode));
}

} // namespace ctmd
