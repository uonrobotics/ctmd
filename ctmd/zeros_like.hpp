#pragma once

#include "./full_like.hpp"

namespace ctmd {

template <typename InType>
[[nodiscard]] inline constexpr auto
zeros_like(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::full_like(std::forward<InType>(In), 0, mpmode);
}

template <typename dtype, typename InType>
[[nodiscard]] inline constexpr auto
zeros_like(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::full_like<dtype>(std::forward<InType>(In), 0, mpmode);
}

} // namespace ctmd
