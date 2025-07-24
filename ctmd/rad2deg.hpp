#pragma once

#include "multiply.hpp"

namespace ctmd {

inline constexpr void rad2deg_to(auto &&In, auto &&Out,
                                 const MPMode mpmode = MPMode::NONE) noexcept {
    using TI =
        decltype(core::to_mdspan(std::forward<decltype(In)>(In)))::value_type;
    constexpr TI R2D = static_cast<TI>(180. / M_PI);

    ctmd::multiply(std::forward<decltype(In)>(In), R2D,
                   std::forward<decltype(Out)>(Out), mpmode);
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto
rad2deg(auto &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    using TI =
        decltype(core::to_mdspan(std::forward<decltype(In)>(In)))::value_type;
    constexpr TI R2D = static_cast<TI>(180. / M_PI);

    return ctmd::multiply<dtype>(std::forward<decltype(In)>(In), R2D, mpmode);
}

} // namespace ctmd
