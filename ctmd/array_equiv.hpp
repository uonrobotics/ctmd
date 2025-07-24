#pragma once

#include "all.hpp"
#include "equal.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr bool
array_equiv(auto &&In1, auto &&In2,
            const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::all(ctmd::equal(std::forward<decltype(In1)>(In1),
                                 std::forward<decltype(In2)>(In2), mpmode));
}

} // namespace ctmd
