#pragma once

#include "linalg/ctmd_linalg_matmul.hpp"

namespace ctmd {

template <typename In1Type, typename In2Type, typename OutType>
inline constexpr void matmul(In1Type &&In1, In2Type &&In2, OutType &&Out,
                             const MPMode mpmode = MPMode::NONE) noexcept {
    ctmd::linalg::matmul(std::forward<In1Type>(In1), std::forward<In2Type>(In2),
                         std::forward<OutType>(Out), mpmode);
}

template <typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr auto
matmul(In1Type &&In1, In2Type &&In2,
       const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::linalg::matmul(std::forward<In1Type>(In1),
                                std::forward<In2Type>(In2), mpmode);
}

} // namespace ctmd
