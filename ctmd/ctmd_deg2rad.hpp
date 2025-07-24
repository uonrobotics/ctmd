#pragma once

#include "ctmd_multiply.hpp"

namespace ctmd {

inline constexpr void
deg2rad_to(auto &&In, auto &&Out,
           const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    using TI =
        decltype(core::to_mdspan(std::forward<decltype(In)>(In)))::value_type;
    constexpr TI D2R = static_cast<TI>(M_PI / 180.);

    ctmd::multiply(std::forward<decltype(In)>(In), D2R,
                   std::forward<decltype(Out)>(Out), mpmode);
}

template <typename dtype = void>
[[nodiscard]] inline constexpr auto
deg2rad(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    using TI =
        decltype(core::to_mdspan(std::forward<decltype(In)>(In)))::value_type;
    constexpr TI D2R = static_cast<TI>(M_PI / 180.);

    return ctmd::multiply<dtype>(std::forward<decltype(In)>(In), D2R, mpmode);
}

} // namespace ctmd
