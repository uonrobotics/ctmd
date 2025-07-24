#pragma once

#include "ctmd_empty_like.hpp"
#include "ctmd_fill.hpp"

namespace ctmd {

[[nodiscard]] inline constexpr auto
full_like(auto &&In, const auto &val,
          const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    auto out = ctmd::empty_like(std::forward<decltype(In)>(In));
    ctmd::fill(out, val, mpmode);
    return out;
}

template <typename dtype>
[[nodiscard]] inline constexpr auto
full_like(auto &&In, const auto &val,
          const ctmd::MPMode mpmode = ctmd::MPMode::NONE) noexcept {
    auto out = ctmd::empty_like<dtype>(std::forward<decltype(In)>(In));
    ctmd::fill(out, val, mpmode);
    return out;
}

} // namespace ctmd
