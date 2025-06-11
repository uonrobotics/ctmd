#pragma once

#include "linalg/vecmat.hpp"

namespace ctmd {

template <typename In1Type, typename In2Type, typename OutType>
inline constexpr void vecmat(In1Type &&In1, In2Type &&In2, OutType &&Out,
                             const MPMode mpmode = MPMode::NONE) noexcept {
    ctmd::linalg::vecmat(std::forward<In1Type>(In1), std::forward<In2Type>(In2),
                         std::forward<OutType>(Out), mpmode);
}

template <typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr auto
vecmat(In1Type &&In1, In2Type &&In2,
       const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::linalg::vecmat(std::forward<In1Type>(In1),
                                std::forward<In2Type>(In2), mpmode);
}

} // namespace ctmd
