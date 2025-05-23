#pragma once

#include "ctmd_full.hpp"

namespace ctmd {

template <typename T, extents_c exts_t = extents<size_t>>
[[nodiscard]] inline constexpr auto
ones(const exts_t &exts = exts_t{},
     const MPMode mpmode = MPMode::NONE) noexcept {
    return ctmd::full<T>(1, exts, mpmode);
}

} // namespace ctmd
