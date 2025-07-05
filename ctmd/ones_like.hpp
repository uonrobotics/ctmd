#pragma once

#include "full_like.hpp"

namespace ctmd {

template <typename InType>
[[nodiscard]] inline constexpr auto
ones_like(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::full_like(std::forward<InType>(In), 1, mpmode);
}

template <typename T, typename InType>
[[nodiscard]] inline constexpr auto
ones_like(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::full_like<T>(std::forward<InType>(In), 1, mpmode);
}

} // namespace ctmd
