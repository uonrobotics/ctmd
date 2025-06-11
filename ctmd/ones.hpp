#pragma once

#include "full.hpp"

namespace ctmd {

template <typename T, extents_c exts_t = extents<size_t>>
[[nodiscard]] inline constexpr auto
zeros(const exts_t &exts = exts_t{},
      const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::full<T>(0, exts, mpmode);
}

} // namespace ctmd
