#pragma once

#include "ctmd_all.hpp"
#include "ctmd_equal.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr bool
array_equiv(auto &&In1, auto &&In2,
            const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    return ctmd::all(ctmd::equal(std::forward<decltype(In1)>(In1),
                                 std::forward<decltype(In2)>(In2), mpmode));
}

} // namespace ctmd
