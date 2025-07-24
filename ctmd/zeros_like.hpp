#pragma once

#include "full_like.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr auto
zeros_like(auto &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::full_like(std::forward<decltype(In)>(In), 0, mpmode);
}

template <typename dtype>
[[nodiscard]] inline constexpr auto
zeros_like(auto &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::full_like<dtype>(std::forward<decltype(In)>(In), 0, mpmode);
}

} // namespace ctmd
