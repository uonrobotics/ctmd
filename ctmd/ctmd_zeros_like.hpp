#pragma once

#include "ctmd_full_like.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr auto
zeros_like(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    return ctmd::full_like(std::forward<decltype(In)>(In), 0, mpmode);
}

template <typename dtype>
[[nodiscard]] inline constexpr auto
zeros_like(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    return ctmd::full_like<dtype>(std::forward<decltype(In)>(In), 0, mpmode);
}

} // namespace ctmd
