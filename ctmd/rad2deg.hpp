#pragma once

#include <cmath>

#include "multiply.hpp"

namespace ctmd {

template <typename InType, typename OutType>
inline constexpr void rad2deg(InType &&In, OutType &&Out,
                              const MPMode mpmode = MPMode::NONE) noexcept {
    using TI = decltype(core::to_mdspan(std::forward<InType>(In)))::value_type;
    constexpr TI R2D = static_cast<TI>(180. / M_PI);

    ctmd::multiply(std::forward<InType>(In), R2D, std::forward<OutType>(Out),
                   mpmode);
}

template <typename InType>
[[nodiscard]] inline constexpr auto
rad2deg(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    using TI = decltype(core::to_mdspan(std::forward<InType>(In)))::value_type;
    constexpr TI R2D = static_cast<TI>(180. / M_PI);

    return ctmd::multiply(std::forward<InType>(In), R2D, mpmode);
}

} // namespace ctmd
