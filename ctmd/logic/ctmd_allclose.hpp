#pragma once

#include "ctmd_all.hpp"
#include "ctmd_isclose.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr bool
allclose(auto &&In1, auto &&In2, const double &rtol = 1e-05,
         const double &atol = 1e-08,
         const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    return ctmd::all(ctmd::isclose(std::forward<decltype(In1)>(In1),
                                   std::forward<decltype(In2)>(In2), rtol, atol,
                                   mpmode));
}

} // namespace ctmd
