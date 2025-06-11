#pragma once

#include "empty_like.hpp"
#include "fill.hpp"

namespace ctmd {

template <typename InType>
[[nodiscard]] inline constexpr auto
zeros_like(InType &&In, const MPMode mpmode = MPMode::NONE) noexcept {
    auto out = ctmd::empty_like(std::forward<InType>(In));
    ctmd::fill(out, 0, mpmode);
    return out;
}

} // namespace ctmd
