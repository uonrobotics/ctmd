#pragma once

#include "ctmd_all.hpp"
#include "ctmd_equal.hpp"

namespace ctmd {

template <typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr bool
array_equiv(In1Type &&In1, In2Type &&In2,
            const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::all(ctmd::equal(std::forward<In1Type>(In1),
                                 std::forward<In2Type>(In2), mpmode));
}

} // namespace ctmd
