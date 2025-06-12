#pragma once

#include <cmath>

#include "multiply.hpp"

namespace ctmd {

template <typename InType, typename OutType>
inline constexpr void deg2rad(InType &&In, OutType &&Out,
                              const MPMode mpmode = MPMode::NONE) noexcept {
    using TI =
        decltype(core::to_mdspan(std::forward<InType>(In)))::element_type;
    constexpr TI D2R = static_cast<TI>(M_PI / 180.);

    ctmd::multiply(std::forward<InType>(In), D2R, std::forward<OutType>(Out),
                   mpmode);
}

template <typename InType>
[[nodiscard]] inline constexpr auto
deg2rad(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    using TI =
        decltype(core::to_mdspan(std::forward<InType>(In)))::element_type;
    constexpr TI D2R = static_cast<TI>(M_PI / 180.);

    return ctmd::multiply(std::forward<InType>(In), D2R, mpmode);
}

} // namespace ctmd
