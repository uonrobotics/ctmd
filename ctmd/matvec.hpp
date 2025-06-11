#pragma once

#include "linalg/matvec.hpp"

namespace ctmd {

template <typename In1Type, typename In2Type, typename OutType>
inline constexpr void matvec(In1Type &&In1, In2Type &&In2, OutType &&Out,
                             const MPMode mpmode = MPMode::NONE) noexcept {
    ctmd::linalg::matvec(std::forward<In1Type>(In1), std::forward<In2Type>(In2),
                         std::forward<OutType>(Out), mpmode);
}

template <typename In1Type, typename In2Type>
[[nodiscard]] inline constexpr auto
matvec(In1Type &&In1, In2Type &&In2,
       const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::linalg::matvec(std::forward<In1Type>(In1),
                                std::forward<In2Type>(In2), mpmode);
}

} // namespace ctmd
