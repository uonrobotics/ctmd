#pragma once

#include "ctmd_full_like.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr auto
ones_like(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    return ctmd::full_like(std::forward<decltype(In)>(In), 1, mpmode);
}

template <typename dtype>
[[nodiscard]] inline constexpr auto
ones_like(auto &&In, const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    return ctmd::full_like<dtype>(std::forward<decltype(In)>(In), 1, mpmode);
}

} // namespace ctmd
