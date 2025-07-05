#pragma once

#include "empty_like.hpp"
#include "fill.hpp"

namespace ctmd {

template <typename InType, typename val_t>
[[nodiscard]] inline constexpr auto
full_like(InType &&In, const val_t &val,
          const MPMode mpmode = MPMode::NONE) noexcept {
    auto out = ctmd::empty_like(std::forward<InType>(In));
    ctmd::fill(out, val, mpmode);
    return out;
}

template <typename T, typename InType, typename val_t>
[[nodiscard]] inline constexpr auto
full_like(InType &&In, const val_t &val,
          const MPMode mpmode = MPMode::NONE) noexcept {
    auto out = ctmd::empty_like<T>(std::forward<InType>(In));
    ctmd::fill(out, val, mpmode);
    return out;
}

} // namespace ctmd
